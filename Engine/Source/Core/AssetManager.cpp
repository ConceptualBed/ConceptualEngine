#include "Core/AssetManager.h"
#include <iostream>
#include <stdexcept>

AssetManager& AssetManager::GetInstance()
{
    static AssetManager instance;
    return instance;
}

AssetManager::AssetManager()
{
    asyncWorker = std::thread(&AssetManager::AsyncWorkerThread, this);
}

AssetManager::~AssetManager()
{
    stopWorker = true;
    taskCondition.notify_one();
    if (asyncWorker.joinable())
    {
        asyncWorker.join();
    }
}

template<typename T, typename... Args>
std::shared_ptr<T> AssetManager::FindOrLoad(const std::string& name, const std::string& path, Args&&... args)
{
    std::lock_guard<std::mutex> lock(assetsMutex);

    if (assets.find(name) != assets.end())
    {
        return std::static_pointer_cast<T>(assets.at(name));
    }

    std::shared_ptr<T> asset = std::make_shared<T>(path, std::forward<Args>(args)...);
    assets[name] = asset;

    return asset;
}

std::shared_ptr<Shader> AssetManager::GetShader(const std::string& name, const std::map<unsigned int, std::string>& shaderPaths)
{
    std::lock_guard<std::mutex> lock(assetsMutex);
    if (assets.find(name) != assets.end())
    {
        return std::static_pointer_cast<Shader>(assets.at(name));
    }

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(shaderPaths);
    assets[name] = shader;
    return shader;
}

std::shared_ptr<Texture> AssetManager::GetTexture(const std::string& name, const std::string& path, TextureFilter filter)
{
    return FindOrLoad<Texture>(name, path, filter);
}

void AssetManager::GarbageCollect()
{
    std::lock_guard<std::mutex> lock(assetsMutex);

    for (auto it = assets.begin(); it != assets.end();)
    {
        if (it->second.use_count() == 1)
        {
            std::cout << "Garbage collecting: " << it->first << std::endl;
            it = assets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void AssetManager::LoadAssetAsync(const std::string& name, const std::string& path, const std::function<std::shared_ptr<Asset>()>& loadFunction)
{
    {
        std::lock_guard<std::mutex> lock(taskMutex);
        asyncTasks.push_back({ name, path, loadFunction });
        activeTasks++;
    }
    taskCondition.notify_one();
}

void AssetManager::WaitForAllLoads()
{
    while (activeTasks > 0)
    {
        std::this_thread::yield();
    }
}

void AssetManager::AsyncWorkerThread()
{
    while (!stopWorker)
    {
        std::unique_lock<std::mutex> lock(taskMutex);
        taskCondition.wait(lock, [this]()
            {
                return !asyncTasks.empty() || stopWorker;
            });

        if (stopWorker)
        {
            return;
        }

        AsyncLoadTask task = asyncTasks.front();
        asyncTasks.erase(asyncTasks.begin());

        lock.unlock();

        try
        {
            std::shared_ptr<Asset> loadedAsset = task.loadFunction();
            std::lock_guard<std::mutex> assetLock(assetsMutex);
            assets[task.name] = loadedAsset;
            std::cout << "Successfully loaded asset: " << task.name << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to load async asset: " << task.name << " | Error: " << e.what() << std::endl;
        }

        activeTasks--;
    }
}
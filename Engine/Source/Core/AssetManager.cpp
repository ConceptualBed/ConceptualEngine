#include "Core/AssetManager.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp>

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

std::shared_ptr<MaterialAsset> AssetManager::GetMaterialAsset(const std::string& path)
{
    std::lock_guard<std::mutex> lock(assetsMutex);

    if (assets.count(path))
    {
        return std::static_pointer_cast<MaterialAsset>(assets.at(path));
    }

    std::shared_ptr<MaterialAsset> materialAsset;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ERROR: Failed to open material asset file: " << path << std::endl;
        return nullptr;
    }

    nlohmann::json data;
    file >> data;

    if (data.contains("parent"))
    {
        std::string parentPath = data.at("parent");
        std::shared_ptr<MaterialAsset> parentAsset = GetMaterialAsset(parentPath);
        if (parentAsset)
        {
            materialAsset = std::make_shared<MaterialInstanceAsset>(path, parentAsset);
        }
        else
        {
            std::cerr << "ERROR: Failed to load parent material asset: " << parentPath << std::endl;
            return nullptr;
        }
    }
    else
    {
        materialAsset = std::make_shared<MaterialAsset>(path);
    }

    assets[path] = materialAsset;
    return materialAsset;
}

std::shared_ptr<Material> AssetManager::CreateMaterialFromAsset(const std::shared_ptr<MaterialAsset>& materialAsset)
{
    if (!materialAsset)
    {
        return nullptr;
    }

    // Ottieni lo shader
    std::map<unsigned int, std::string> shaderPaths;
    const nlohmann::json& paths = materialAsset->GetShaderPaths();
    if (paths.contains("vertex"))
    {
        shaderPaths[GL_VERTEX_SHADER] = paths.at("vertex");
    }
    if (paths.contains("fragment"))
    {
        shaderPaths[GL_FRAGMENT_SHADER] = paths.at("fragment");
    }

    std::shared_ptr<Shader> shader = GetShader(paths.at("vertex"), shaderPaths);
    if (!shader)
    {
        return nullptr;
    }

    // Crea e configura l'istanza del Material
    std::shared_ptr<Material> material = std::make_shared<Material>(shader);

    const nlohmann::json& uniforms = materialAsset->GetUniforms();
    for (auto const& [key, val] : uniforms.items())
    {
        if (val.is_number())
        {
            material->SetFloat(key, val.get<float>());
        }
        else if (val.is_array() && val.size() == 3)
        {
            material->SetVec3(key, glm::vec3(val[0], val[1], val[2]));
        }
        else if (val.is_array() && val.size() == 4)
        {
            material->SetVec4(key, glm::vec4(val[0], val[1], val[2], val[3]));
        }
        else if (val.is_object() && val.contains("path") && val.contains("filter"))
        {
            std::string texturePath = val.at("path").get<std::string>();
            std::string filterStr = val.at("filter").get<std::string>();

            TextureFilter filter = TextureFilter::SMOOTH;
            if (filterStr == "pixel_perfect")
            {
                filter = TextureFilter::PIXEL_PERFECT;
            }

            std::shared_ptr<Texture> texture = GetTexture(texturePath, texturePath, filter);
            if (texture)
            {
                material->SetTexture(key, texture);
            }
        }
    }
    return material;
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
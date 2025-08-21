#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <functional>
#include <vector>

#include "Core/Assets/Asset.h"
#include "Core/Assets/Shader.h"
#include "Core/Assets/Texture.h"

// Task per il caricamento asincrono
struct AsyncLoadTask
{
    std::string name;
    std::string path;
    std::function<std::shared_ptr<Asset>()> loadFunction;
};

class AssetManager
{
public:
    static AssetManager& GetInstance();

    // Metodi per ottenere asset
    std::shared_ptr<Shader> GetShader(const std::string& name, const std::map<unsigned int, std::string>& shaderPaths);
    std::shared_ptr<Texture> GetTexture(const std::string& name, const std::string& path, TextureFilter filter = TextureFilter::SMOOTH);

    // Metodi per il caricamento asincrono
    void LoadAssetAsync(const std::string& name, const std::string& path, const std::function<std::shared_ptr<Asset>()>& loadFunction);
    void WaitForAllLoads();

    // Clean up all unused assets
    void GarbageCollect();

private:
    AssetManager();
    ~AssetManager();

    // Mappa per memorizzare gli asset caricati
    std::unordered_map<std::string, std::shared_ptr<Asset>> assets;
    std::mutex assetsMutex;

    // Per il caricamento asincrono
    std::vector<AsyncLoadTask> asyncTasks;
    std::thread asyncWorker;
    std::mutex taskMutex;
    std::condition_variable taskCondition;
    std::atomic<bool> stopWorker = false;
    std::atomic<int> activeTasks = 0;

    void AsyncWorkerThread();

    template<typename T, typename... Args>
    std::shared_ptr<T> FindOrLoad(const std::string& name, const std::string& path, Args&&... args);
};
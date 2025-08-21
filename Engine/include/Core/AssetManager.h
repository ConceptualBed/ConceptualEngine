#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include <glad/gl.h>
#include <flecs.h>

// Definizione delle risorse
//struct Texture
//{
//    unsigned int id;
//};

//struct Shader
//{
//    unsigned int id;
//};

struct SpriteData
{
    unsigned int textureID;
    float uvX, uvY, uvWidth, uvHeight;
};

class AssetManager
{
public:
    static unsigned int LoadTexture(const std::string& path);
    static unsigned int LoadShader(const std::string& vertexPath, const std::string& fragmentPath);
    static unsigned int RegisterSpriteData(const SpriteData& data);

    static Texture* GetTexture(unsigned int id);
    static Shader* GetShader(unsigned int id);
    static SpriteData* GetSpriteData(unsigned int id);

private:
    static std::map<std::string, unsigned int> textureCache;
    static std::map<std::string, unsigned int> shaderCache;
    static std::vector<std::unique_ptr<Texture>> textures;
    static std::vector<std::unique_ptr<Shader>> shaders;
    static std::vector<SpriteData> sprites;

    // Non permettiamo l'istanziazione
    AssetManager() = delete;
};

// Modulo Flecs per l'AssetManager
struct AssetManagerModule {
    AssetManagerModule(flecs::world& world)
    {
        world.module<AssetManagerModule>();
    }
};
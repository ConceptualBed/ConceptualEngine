#include "Core/AssetManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Definizione e inizializzazione dei membri statici
std::map<std::string, unsigned int> AssetManager::textureCache;
std::map<std::string, unsigned int> AssetManager::shaderCache;
std::vector<std::unique_ptr<Texture>> AssetManager::textures;
std::vector<std::unique_ptr<Shader>> AssetManager::shaders;
std::vector<SpriteData> AssetManager::sprites;

// Funzione statica helper per la compilazione degli shader
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Gestione degli errori di compilazione
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

// Implementazione del modulo Flecs
AssetManagerModule::AssetManagerModule(flecs::world& world)
{
    // Inizializza i sistemi dell'asset manager, se necessario
}

unsigned int AssetManager::LoadTexture(const std::string& path)
{
    // 1. Controlla la cache per evitare di caricare la stessa texture due volte
    if (textureCache.count(path))
    {
        return textureCache.at(path);
    }

    // 2. Carica i dati dell'immagine con STB Image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD_IMAGE: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    // 3. Genera e configura la texture OpenGL
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    // Impostazioni per la pixel art
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Carica i dati nell'oggetto texture
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // 4. Memorizza la risorsa e l'ID
    unsigned int id = textures.size();
    textures.push_back(std::make_unique<Texture>(Texture{ textureID }));
    textureCache[path] = id;

    return id;
}

unsigned int AssetManager::LoadShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. Controlla la cache
    std::string cacheKey = vertexPath + "|" + fragmentPath;
    if (shaderCache.count(cacheKey))
    {
        return shaderCache.at(cacheKey);
    }

    // 2. Carica i file di testo degli shader
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        return 0;
    }

    // 3. Compila e collega gli shader
    unsigned int vertex, fragment;
    vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
    fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    unsigned int programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);

    // Gestione degli errori di link
    int success;
    char infoLog[512];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // 4. Memorizza la risorsa e l'ID
    unsigned int id = shaders.size();
    shaders.push_back(std::make_unique<Shader>(Shader{ programID }));
    shaderCache[cacheKey] = id;

    return id;
}

unsigned int AssetManager::RegisterSpriteData(const SpriteData& data)
{
    unsigned int id = sprites.size();
    sprites.push_back(data);
    return id;
}

Texture* AssetManager::GetTexture(unsigned int id)
{
    if (id >= textures.size()) return nullptr;
    return textures[id].get();
}

Shader* AssetManager::GetShader(unsigned int id)
{
    if (id >= shaders.size()) return nullptr;
    return shaders[id].get();
}

SpriteData* AssetManager::GetSpriteData(unsigned int id)
{
    if (id >= sprites.size()) return nullptr;
    return &sprites[id];
}
#include "Core/Assets/MaterialAsset.h"
#include <fstream>
#include <iostream>

MaterialAsset::MaterialAsset(const std::string& path)
{
    this->path = path;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ERROR: Failed to open material asset file: " << path << std::endl;
        return;
    }
    nlohmann::json data;
    file >> data;

    LoadShaderPathsFromJson(data);
    LoadUniformsFromJson(data);
}

void MaterialAsset::LoadShaderPathsFromJson(const nlohmann::json& data)
{
    if (data.contains("shader_paths"))
    {
        shaderPaths = data.at("shader_paths");
    }
}

void MaterialAsset::LoadUniformsFromJson(const nlohmann::json& data)
{
    if (data.contains("uniforms"))
    {
        uniforms = data.at("uniforms");
    }
}

// Implementazione di MaterialInstanceAsset
MaterialInstanceAsset::MaterialInstanceAsset(const std::string& path, const std::shared_ptr<MaterialAsset>& parent) : MaterialAsset(path)
{
    if (!parent)
    {
        std::cerr << "ERROR: Material instance has no valid parent." << std::endl;
        return;
    }
    // Eredita tutti i parametri dal genitore
    shaderPaths = parent->GetShaderPaths();
    uniforms = parent->GetUniforms();

    // Sovrascrivi i parametri con quelli specifici dell'istanza
    std::ifstream file(path);
    if (file.is_open())
    {
        nlohmann::json data;
        file >> data;
        if (data.contains("uniforms"))
        {
            for (auto const& [key, val] : data.at("uniforms").items())
            {
                uniforms[key] = val;
            }
        }
    }
}
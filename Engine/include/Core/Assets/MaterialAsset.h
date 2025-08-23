#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include "Asset.h"

// MaterialAsset è la classe base che rappresenta un Master Material
class MaterialAsset : public Asset
{
public:
    MaterialAsset(const std::string& path);

    const nlohmann::json& GetShaderPaths() const
    {
        return shaderPaths;
    }
    const nlohmann::json& GetUniforms() const
    {
        return uniforms;
    }

protected:
    nlohmann::json shaderPaths;
    nlohmann::json uniforms;
    nlohmann::json textureInfo;

    void LoadUniformsFromJson(const nlohmann::json& data);
    void LoadShaderPathsFromJson(const nlohmann::json& data);
};

// MaterialInstanceAsset è un'istanza che eredita da un MaterialAsset
class MaterialInstanceAsset : public MaterialAsset
{
public:
    MaterialInstanceAsset(const std::string& path, const std::shared_ptr<MaterialAsset>& parent);
};
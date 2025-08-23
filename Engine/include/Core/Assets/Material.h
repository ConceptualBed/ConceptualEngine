#pragma once

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <memory>
#include "Core/Assets/Shader.h"
#include "Core/Assets/Texture.h"

class Material
{
public:
    // Costruttore che accetta un shared_ptr allo shader
    Material(const std::shared_ptr<Shader>& shader);

    // Distruttore di default. I shared_ptr si gestiscono da soli.
    ~Material() = default;

    // Metodi per impostare i parametri del materiale
    void SetTexture(const std::string& uniformName, const std::shared_ptr<Texture>& texture);
    void SetFloat(const std::string& uniformName, float value);
    void SetVec3(const std::string& uniformName, const glm::vec3& value);
    void SetVec4(const std::string& uniformName, const glm::vec4& value);
    void SetMat4(const std::string& uniformName, const glm::mat4& value);

    // Metodo che applica lo stato del materiale per il rendering
    void Use() const;

private:
    std::shared_ptr<Shader> shader;

    // Mappe per memorizzare i parametri
    std::map<std::string, std::shared_ptr<Texture>> textures;
    std::map<std::string, float> floatUniforms;
    std::map<std::string, glm::vec3> vec3Uniforms;
    std::map<std::string, glm::vec4> vec4Uniforms;
    std::map<std::string, glm::mat4> mat4Uniforms;
};
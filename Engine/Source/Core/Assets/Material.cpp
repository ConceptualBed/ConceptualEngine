#include "Core/Assets/Material.h"
#include <glad/gl.h>
#include <iostream>

Material::Material(const std::shared_ptr<Shader>& shader) : shader(shader)
{
}

void Material::SetTexture(const std::string& uniformName, const std::shared_ptr<Texture>& texture)
{
    textures[uniformName] = texture;
}

void Material::SetFloat(const std::string& uniformName, float value)
{
    floatUniforms[uniformName] = value;
}

void Material::SetVec3(const std::string& uniformName, const glm::vec3& value)
{
    vec3Uniforms[uniformName] = value;
}

void Material::SetVec4(const std::string& uniformName, const glm::vec4& value)
{
    vec4Uniforms[uniformName] = value;
}

void Material::SetMat4(const std::string& uniformName, const glm::mat4& value)
{
    mat4Uniforms[uniformName] = value;
}

void Material::Use() const
{
    if (!shader)
    {
        std::cerr << "ERROR: Material does not have a valid shader." << std::endl;
        return;
    }

    // Attiva lo shader
    shader->Use();

    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);

    /*if (shader->GetUniformLocation("texture_diffuse") != -1)
    {
        glEnableVertexAttribArray(1);
    }
    else
    {
        glDisableVertexAttribArray(1);
    }*/


    // Imposta le texture
    int textureUnit = 0;
    for (const auto& pair : textures)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, pair.second->GetID());
        shader->SetInt(pair.first, textureUnit);
        textureUnit++;
    }

    // Imposta tutti gli altri uniforms
    for (const auto& pair : floatUniforms)
    {
        shader->SetFloat(pair.first, pair.second);
    }
    for (const auto& pair : vec3Uniforms)
    {
        shader->SetVec3(pair.first, pair.second);
    }
    for (const auto& pair : vec4Uniforms)
    {
        shader->SetVec4(pair.first, pair.second);
    }
    for (const auto& pair : mat4Uniforms)
    {
        shader->SetMat4(pair.first, pair.second);
    }
}
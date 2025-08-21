#pragma once

#include <glad/gl.h>
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

class Shader
{
public:
    // Maps shader paths per type (ie. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)
    Shader(const std::map<unsigned int, std::string>& shaderPaths);
    ~Shader();

    // Activates the shader
    void Use() const;
    // Getter function for shader ID
    unsigned int GetID() const;

    // API to set the uniforms

    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& mat) const;

private:
    unsigned int id;

    std::string LoadShaderSource(const std::string& path) const;
    unsigned int CompileShader(unsigned int type, const std::string& source) const;
    void CheckErrors(unsigned int shader, const std::string& type) const;

    // Caches the location of uniforms for performance optimization
    mutable std::map<std::string, GLint> uniformCache;
    GLint GetUniformLocation(const std::string& name) const;
};
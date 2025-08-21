#include "Core/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::map<unsigned int, std::string>& shaderPaths)
{
    std::vector<unsigned int> attachedShaders;

    // Load and complile a shader
    for (const auto& pair : shaderPaths)
    {
        unsigned int type = pair.first;
        const std::string& path = pair.second;

        try
        {
            std::string source = LoadShaderSource(path);
            unsigned int shaderID = CompileShader(type, source);
            attachedShaders.push_back(shaderID);
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR::SHADER_LOADING_FAILED: " << e.what() << std::endl;
            // Clean already compiled shaders in case of error
            for (unsigned int shaderID : attachedShaders)
            {
                glDeleteShader(shaderID);
            }
            throw; // throw and exception
        }
    }

    // Build and link the shader program
    id = glCreateProgram();
    for (unsigned int shaderID : attachedShaders)
    {
        glAttachShader(id, shaderID);
    }
    glLinkProgram(id);

    // Check linking errors
    try
    {
        CheckErrors(id, "PROGRAM");
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::SHADER_LINKING_FAILED: " << e.what() << std::endl;
        // Clean the shaders in case of linking error
        for (unsigned int shaderID : attachedShaders)
        {
            glDeleteShader(shaderID);
        }
        glDeleteProgram(id);
        throw;
    }

    // Delete the single shaders after the linking process. They're not necessary anymore.
    for (unsigned int shaderID : attachedShaders)
    {
        glDeleteShader(shaderID);
    }
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::Use() const
{
    glUseProgram(id);
}

unsigned int Shader::GetID() const
{
    return id;
}

// API to set the uniforms

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

// Caching of the uniforms for performance optimization
GLint Shader::GetUniformLocation(const std::string& name) const
{
    if (uniformCache.count(name))
    {
        return uniformCache.at(name);
    }

    GLint location = glGetUniformLocation(id, name.c_str());
    if (location == -1)
    {

#ifdef _DEBUG
        std::cerr << "WARNING: Uniform '" << name << "' not found in shader program " << id << std::endl;
#endif
    }
    uniformCache[name] = location;
    return location;
}

// Load a shader from source file
std::string Shader::LoadShaderSource(const std::string& path) const
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    }
    catch (std::ifstream::failure& e)
    {
        throw std::runtime_error("File not successfully read: " + path);
    }
}

// Compile a shader
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) const
{
    unsigned int shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    CheckErrors(shaderID, (type == GL_VERTEX_SHADER) ? "VERTEX" : (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : (type == GL_GEOMETRY_SHADER) ? "GEOMETRY" : "UNKNOWN");
    return shaderID;
}

// Helper function to check errors
void Shader::CheckErrors(unsigned int shader, const std::string& type) const
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error("Shader compilation failed.");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error("Program linking failed.");
        }
    }
}
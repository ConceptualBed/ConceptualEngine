#pragma once

#include <glad/gl.h>
#include <memory>
#include "Assets/Shader.h"
#include "Assets/Texture.h"

// Basic class that manages rendering pipeline
class Renderer
{
public:
    Renderer();
    ~Renderer();

    void DrawSingleColoredQuad(const glm::vec2& position, float scale);

private:
    unsigned int quadVAO, quadVBO, EBO;
    std::shared_ptr<Shader> simpleShader;
    std::shared_ptr<Texture> simpleTexture;

    void InitBuffers();
};
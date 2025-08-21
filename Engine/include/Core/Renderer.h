#pragma once

#include <glad/gl.h>
#include "Shader.h"
#include "Texture.h"

// Basic class that manages rendering pipeline
class Renderer
{
public:
    Renderer();
    ~Renderer();

    void DrawSingleColoredQuad(const glm::vec2& position, float scale);

private:
    unsigned int quadVAO, quadVBO, EBO;
    Shader* simpleShader;
    Texture* simpleTexture;

    void InitBuffers();
};
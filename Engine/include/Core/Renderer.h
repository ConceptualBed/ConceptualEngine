#pragma once

#include <glad/gl.h>
#include <memory>
#include "Assets/Shader.h"
#include "Assets/Texture.h"
#include "Assets/MaterialAsset.h"
#include "AssetManager.h"

// Basic class that manages rendering pipeline
class Renderer
{
public:
    Renderer();
    ~Renderer();

    void DrawSingleColoredQuad(const std::shared_ptr<MaterialAsset>& materialAsset, const glm::vec2& position, float scale);

private:
    unsigned int quadVAO, quadVBO, EBO;

    void InitBuffers();
};
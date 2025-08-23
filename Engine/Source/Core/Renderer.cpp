#include "Core/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <map>
#include "Core/AssetManager.h"
#include <iostream>

const float WORLD_WIDTH = 800.0f;
const float WORLD_HEIGHT = 600.0f;

Renderer::Renderer()
{
    InitBuffers();
    /*std::map<unsigned int, std::string> shaderPaths;
    shaderPaths[GL_VERTEX_SHADER] = "Resources/Assets/Shaders/Basic/Textured.vert";
    shaderPaths[GL_FRAGMENT_SHADER] = "Resources/Assets/Shaders/Basic/Textured.frag";
    testShader = AssetManager::GetInstance().GetShader("test_shader", shaderPaths);

    testTexture = AssetManager::GetInstance().GetTexture("simple_texture", "Resources/Assets/Textures/sampleTexture.png", TextureFilter::PIXEL_PERFECT);*/
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &EBO);
}

void Renderer::DrawSingleColoredQuad(const std::shared_ptr<MaterialAsset>& materialAsset, const glm::vec2& position, float scale)
{
    if (!materialAsset)
    {
        std::cerr << "ERROR: Cannot draw with a null MaterialAsset." << std::endl;
        return;
    }

    // Crea l'istanza Material "al volo" basata sull'asset
    std::shared_ptr<Material> material = AssetManager::GetInstance().CreateMaterialFromAsset(materialAsset);

    if (!material)
    {
        std::cerr << "ERROR: Failed to create material from asset." << std::endl;
        return;
    }

    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

    float aspectRatio = static_cast<float>(width) / height;

    // 1. Calculate orthographic projection matrix
    // L'origine (0,0) sarà in basso a sinistra e le coordinate del mondo andranno fino a (WORLD_WIDTH, WORLD_HEIGHT).
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

    // 2. Calculate model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

    material->SetMat4("projection", projection);
    material->SetMat4("model", model);
    material->SetFloat("time", (float)glfwGetTime());

    material->Use();

    // 4. Disegna il quadrato
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // Sconnetti il VAO per evitare modifiche accidentali
}

void Renderer::InitBuffers()
{
    float vertices[] = {
        // positions         // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right (0)
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right (1)
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // top left (2)
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f  // bottom left (3)
    };

    unsigned int indices[] = {
        0, 1, 2, // Primo triangolo
        1, 3, 2  // Secondo triangolo
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Sconnetti il VAO
}
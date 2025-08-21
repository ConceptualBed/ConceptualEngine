#include "Core/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <map>

const float WORLD_WIDTH = 800.0f;
const float WORLD_HEIGHT = 600.0f;

Renderer::Renderer()
{
    InitBuffers();

    // Crea una mappa per i percorsi degli shader
    std::map<unsigned int, std::string> shaderPaths;
    shaderPaths[GL_VERTEX_SHADER] = "Resources/Assets/Shaders/Basic/Textured.vert";
    shaderPaths[GL_FRAGMENT_SHADER] = "Resources/Assets/Shaders/Basic/Textured.frag";

    // Crea un'istanza della classe Shader
    simpleShader = new Shader(shaderPaths);
    
    simpleTexture = new Texture("Resources/Assets/Textures/sampleTexture.png", TextureFilter::PIXEL_PERFECT);
}

Renderer::~Renderer()
{
    delete simpleShader;
    delete simpleTexture;
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &EBO);
}

void Renderer::DrawSingleColoredQuad(const glm::vec2& position, float scale)
{
    // 1. Attiva lo shader
    simpleShader->Use();

    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

    float aspectRatio = static_cast<float>(width) / height;

    // 1. Calculate orthographic projection matrix
    // L'origine (0,0) sarà in basso a sinistra e le coordinate del mondo andranno fino a (WORLD_WIDTH, WORLD_HEIGHT).
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

    // 2. Passa la matrice allo shader.
    simpleShader->SetMat4("projection", projection);

    // 2. Calculate model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

    // 3. Passa la matrice allo shader
    simpleShader->SetMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, simpleTexture->GetID());

    simpleShader->SetInt("texture_diffuse", 0);

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
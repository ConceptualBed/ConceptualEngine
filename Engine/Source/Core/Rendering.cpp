#include "Core/Rendering.h"
#include "Core/AssetManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// VBO e VAO globali per il rendering instanced
static unsigned int quadVAO, quadVBO;
static unsigned int instanceVBO;

// Mappa per il batching
static std::map<unsigned int, std::vector<glm::mat4>> batches;

// Shader source (estremamente semplificato)
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"layout (location = 2) in mat4 aModel;\n"
"void main()\n"
"{\n"
"   gl_Position = aModel * vec4(aPos, 0.0, 1.0);\n"
"}\0";

// Funzione di inizializzazione per il quad e i buffer per l'instancing
void InitRenderingSystem()
{
    // Definizione dei vertici del quad
    float quadVertices[] = {
        // pos      // texcoords
        -0.5f,  0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f
    };

    // Configurazione VAO e VBO per il quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Attributi dei vertici
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // VBO per i dati instanced
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // Verrà riempito ogni frame, non ha bisogno di dati iniziali

    // Attributi per la matrice di trasformazione (divisori a 1 per instancing)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glVertexAttribDivisor(2, 1);

    // ... e così via per gli altri 3 vettori della matrice
    glBindVertexArray(0);
}

// Il sistema di rendering di Flecs
void RenderingSystem(flecs::iter& it, Position* p, Rotation* r, Scale* s, SpriteRef* spr, MaterialRef* mat)
{
    // 1. Raccogli i dati e li raggruppa per materiale
    for (int i = 0; i < it.count(); ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(p[i].x, p[i].y, 0.0f));
        model = glm::rotate(model, glm::radians(r[i].value), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(s[i].x, s[i].y, 1.0f));
        batches[mat[i].materialID].push_back(model);
    }

    // 2. Disegna i batch
    for (auto const& [materialID, models] : batches)
    {
        if (models.empty()) continue;

        // Attiva lo shader e la texture del materiale
        Shader* shader = AssetManager::GetShader(materialID);
        shader->use();

        // Collega i dati delle trasformazioni
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_DYNAMIC_DRAW);

        // Draw call instanced
        glBindVertexArray(quadVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, models.size());
        glBindVertexArray(0);
    }

    // 3. Pulisci i batch per il prossimo frame
    batches.clear();
}
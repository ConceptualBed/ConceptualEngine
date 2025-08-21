#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "Engine.h"

// Struttura dei dati per l'instancing
struct InstancedData
{
    glm::mat4 model;
    unsigned int textureID;
};

// Sistemi di rendering
void RenderingSystem(flecs::iter& it, Position* p, Rotation* r, Scale* s, SpriteRef* spr, MaterialRef* mat);
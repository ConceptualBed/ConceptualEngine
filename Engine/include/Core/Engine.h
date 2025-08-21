#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <flecs.h>
#include "Core/Renderer.h"

#include <iostream>

// Componenti di base per la trasformazione e la grafica
struct Position
{
    float x, y;
};
struct Rotation
{
    float value;
};
struct Scale
{
    float x, y;
};

// Componenti che definiscono la risorsa grafica
struct MaterialRef
{
    unsigned int materialID;
};
struct SpriteRef
{
    unsigned int spriteID;
};

class Engine
{
public:
    Engine(int width, int height, const char* title);
    ~Engine();

    void Run();
    flecs::world& GetWorld();

private:
    GLFWwindow* window;
    flecs::world world;
    Renderer* renderer;

    double lastFrameTime = 0.0;
    int frameCount = 0;
    double frameRate = 0.0;
    
    void MainLoop();
    void UpdateStats();
    void RegisterEngineComponents();
};
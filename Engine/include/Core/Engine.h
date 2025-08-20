#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <flecs.h>

#include <iostream>

// Componenti del motore per il rendering data-oriented
struct Position
{
    float x, y;
};
struct Sprite
{
    int textureID;
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

    void InitSubsystems();
    void MainLoop();
    void RegisterEngineComponents();
    void RegisterEngineSystems();
};
#include "Core/Engine.h"
#include <iostream>
#include <sstream>
//#include "Core/Rendering.h"
#include "Core/AssetManager.h"

//// Hint per NVIDIA: forza l'uso della GPU dedicata
//extern "C" {
//    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
//}
//
//// Hint per AMD: forza l'uso della GPU dedicata
//extern "C" {
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}

// Funzione statica per il callback del ridimensionamento della finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Dichiarazione del sistema di rendering per Flecs
//void RenderingSystem(flecs::iter& it, Position* p, Rotation* r, Scale* s, SpriteRef* spr, MaterialRef* mat);

Engine::Engine(int width, int height, const char* title)
{
    // 1. Inizializza GLFW (gestione della finestra)
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Crea la finestra e rendi il contesto OpenGL attuale
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Inizializza i sottosistemi del motore (Flecs, rendering, ecc.)
    RegisterEngineComponents();

    world.import<AssetManagerModule>();
    //world.system<Position, Rotation, Scale, SpriteRef, MaterialRef>("RenderingSystem")
    //    .each(RenderingSystem);

    renderer = new Renderer();

    // V-Sync
    glfwSwapInterval(0);
}

Engine::~Engine()
{
    delete renderer;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::Run()
{
#ifdef _DEBUG
    lastFrameTime = glfwGetTime();
#endif // _DEBUG

    MainLoop();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        // Pulisci i buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        world.progress();
        renderer->DrawSingleColoredQuad(glm::vec2(0.0f, 0.0f), 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();

#ifdef _DEBUG
        UpdateStats();
#endif // _DEBUG

    }
}

void Engine::UpdateStats()
{
    double currentFrameTime = glfwGetTime();
    double deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    // Aggiorna il contatore dei frame e il tempo totale
    static double totalTime = 0.0;
    totalTime += deltaTime;
    static int frameCounter = 0;
    frameCounter++;

    // Aggiorna il titolo ogni 0.5 secondi per evitare di rallentare
    if (totalTime >= 1.0f)
    {
        // Calcola gli FPS medi in questo intervallo
        double fps = frameCounter / totalTime;
        double frameTimeMs = (totalTime / frameCounter) * 1000.0;

        // Crea una stringa formattata per il titolo
        std::stringstream ss;
        ss.precision(2);
        ss << std::fixed << "My Game Engine | FPS: " << fps << " | " << frameTimeMs << " ms";

        // Imposta il titolo della finestra
        glfwSetWindowTitle(window, ss.str().c_str());

        // Resetta i contatori per il prossimo intervallo
        totalTime = 0.0;
        frameCounter = 0;
    }
}

void Engine::RegisterEngineComponents()
{
    world.component<Position>();
    world.component<Rotation>();
    world.component<Scale>();
    world.component<MaterialRef>();
    world.component<SpriteRef>();
}

flecs::world& Engine::GetWorld()
{
    return world;
}
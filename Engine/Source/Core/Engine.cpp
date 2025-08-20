#include "Core/Engine.h"
#include <iostream>

// Funzione statica per il callback del ridimensionamento della finestra
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Dichiarazione del sistema di rendering per Flecs
//void RenderingSystem(flecs::iter& it, Position* p, Sprite* s)
//{
//    // Logica di rendering ottimizzata per i dati
//    // I dati sono già in un array contiguo, pronti per il batching/instancing.
//    for (int i = 0; i < it.count(); ++i)
//    {
//        // Esempio: invia la posizione p[i] e la sprite s[i] alla GPU
//        // In un motore 2D professionale, raccoglieresti i dati di tutte le entità
//        // con la stessa texture per una singola "draw call".
//    }
//}

Engine::Engine(int width, int height, const char* title)
{
    // 1. Inizializza GLFW (gestione della finestra)
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    InitSubsystems();
}

Engine::~Engine()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::InitSubsystems()
{
    // Registra i componenti e i sistemi del motore nel mondo Flecs
    RegisterEngineComponents();
    RegisterEngineSystems();
}

void Engine::Run()
{
    MainLoop();
}

void Engine::MainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        // Pulisci i buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Aggiorna il mondo Flecs, che esegue tutti i sistemi
        world.progress();

        // Scambia i buffer e gestisci gli eventi
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::RegisterEngineComponents()
{
    // Il motore registra i componenti di cui ha bisogno
    world.component<Position>();
    world.component<Sprite>();
}

void Engine::RegisterEngineSystems()
{
    // Il motore registra i suoi sistemi di base
    //world.system<Position, Sprite>().each(RenderingSystem);
}

flecs::world& Engine::GetWorld()
{
    return world;
}
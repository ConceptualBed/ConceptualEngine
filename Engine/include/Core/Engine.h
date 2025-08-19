//#include <GLFW/glfw3.h>
#include <iostream>

class Engine
{
public:
    // Costruttore: si occupa di tutte le inizializzazioni
    Engine();

    // Distruttore: si occupa di tutte le operazioni di pulizia
    ~Engine();

    // Avvia il game loop principale
    void Run();

    //// Restituisce il mondo di Flecs per permettere al gioco di gestirlo
    //flecs::world& GetWorld();

private:
    // Puntamento alla finestra di GLFW
    //GLFWwindow* window;

    // Il mondo di Flecs, il cuore del sistema data-oriented
    //flecs::world world;

    // Funzioni private per l'inizializzazione e il rendering
    void InitGLFW();
    void InitGLEW();
    void SetupRenderingSystem();
};
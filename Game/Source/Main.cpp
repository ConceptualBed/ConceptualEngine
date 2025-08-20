#include "Core/Engine.h"

int main()
{
    try
    {
        Engine engine(1920, 1080, "My Game");

        flecs::world& world = engine.GetWorld();

        world.import<flecs::stats>();
        world.set<flecs::Rest>({});

        world.entity("Player")
            .set<Position>({ 640.0f, 360.0f });

        engine.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
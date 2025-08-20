add_executable(MyGame Source/Main.cpp)

target_link_libraries(MyGame PRIVATE
    Engine
    flecs::flecs_static
)

target_include_directories(MyGame PRIVATE
    "../libraries/glm"
)
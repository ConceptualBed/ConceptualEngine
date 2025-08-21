add_library(Engine STATIC
    "${CMAKE_SOURCE_DIR}/libraries/glad/src/gl.c"
    Source/Core/Engine.cpp
    Source/Core/Renderer.cpp
    Source/Core/Shader.cpp
 "Source/Core/Texture.cpp")

# Rendi visibili gli header del motore al progetto del gioco
target_include_directories(Engine PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/flecs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/glm"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/stb"
)

# Collega il tuo motore alle librerie esterne
target_link_libraries(Engine PRIVATE
    glfw
    flecs::flecs_static
    glm::glm
)

set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/Resources/Assets/Shaders/Basic/Textured.vert
    ${CMAKE_SOURCE_DIR}/Resources/Assets/Shaders/Basic/Textured.frag
    PROPERTIES GENERATED true
)
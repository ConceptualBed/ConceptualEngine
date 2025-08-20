add_library(Engine STATIC
    Source/Core/Engine.cpp
    "${CMAKE_SOURCE_DIR}/libraries/glad/src/gl.c"
    # Aggiungi qui tutti i file .cpp del tuo motore
)

# Rendi visibili gli header del motore al progetto del gioco
target_include_directories(Engine PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/glfw/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/flecs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../libraries/glm"
)

# Collega il tuo motore alle librerie esterne
target_link_libraries(Engine PRIVATE
    glfw
    flecs::flecs_static
)
add_library(Engine STATIC
    Source/Core/Engine.cpp
    # Aggiungi qui tutti i file .cpp del tuo motore
)

# Rendi visibili gli header del motore al progetto del gioco
target_include_directories(Engine PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

#find_package(glfw3 REQUIRED)

# Collega il tuo motore alle librerie esterne
target_link_libraries(Engine PRIVATE
    glfw
)
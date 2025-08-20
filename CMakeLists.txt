cmake_minimum_required( VERSION 3.31 )

project( ConceptualEngine )

find_package( OpenGL REQUIRED )

include_directories(
	${OPENGL_INCLUDE_DIRS}
	libraries/glad/include
)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

set(EXTERNAL_LIBRARIES_DIR "${CMAKE_SOURCE_DIR}/libraries")

add_subdirectory( ${EXTERNAL_LIBRARIES_DIR}/glfw )
add_subdirectory( ${EXTERNAL_LIBRARIES_DIR}/flecs )

add_executable(ConceptualEngine libraries/glad/src/gl.c)

add_subdirectory( Engine )
add_subdirectory( Game )
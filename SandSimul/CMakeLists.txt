cmake_minimum_required(VERSION 3.15)
project(SandSimul)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SANDSIMUL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c"
)

add_executable(SandSimul ${SANDSIMUL_SOURCES})


target_include_directories(SandSimul PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"    
)

target_link_directories(SandSimul PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/lib"
)

target_link_libraries(SandSimul PRIVATE
    glfw3
    opengl32
)

# Copy shaders to the build directory
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}")
file(COPY ${SHADER_SOURCE_DIR} DESTINATION ${SHADER_OUTPUT_DIR})
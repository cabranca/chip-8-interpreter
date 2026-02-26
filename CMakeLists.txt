cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Chip-8-Interpreter)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(external/SDL2)

add_executable(chip-8-interpreter src/Main.cpp src/Chip8.cpp src/Renderer.cpp)

target_link_libraries(chip-8-interpreter PRIVATE SDL3::SDL3-static)

cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(Chip-8-Interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

add_subdirectory(external/SDL2)

add_executable(chip-8-interpreter src/Main.cpp src/Chip8.cpp src/Renderer.cpp src/AudioEngine.cpp)

target_compile_features(chip-8-interpreter PRIVATE cxx_std_23)

target_sources(chip-8-interpreter PUBLIC FILE_SET CXX_MODULES 
    FILES src/chip8.ixx src/chip8-interpreter.ixx src/chip8-renderer.ixx src/chip8-audio.ixx)

target_link_libraries(chip-8-interpreter PRIVATE SDL3::SDL3-static)

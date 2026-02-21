#include <cstdio>
#include <cstdlib>
#include <print>
#include <vector>

#include "Chip8.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::print(stderr, "Usage: {} <rom-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::FILE *romFile = std::fopen(argv[1], "rb");
    if (!romFile)
    {
        std::print(stderr, "Failed to open ROM: {}\n", argv[1]);
        return EXIT_FAILURE;
    }

    std::fseek(romFile, 0, SEEK_END);
    const auto romSize = std::ftell(romFile);
    std::fseek(romFile, 0, SEEK_SET);

    std::vector<uint8_t> romData(romSize);
    std::fread(romData.data(), 1, romSize, romFile);
    std::fclose(romFile);

    bool success = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (!success)
    {
        auto error = SDL_GetError();
        std::print("Failed to initialized SDL: {0}", error);
    }

    auto window = SDL_CreateWindow("CHIP-8", 800, 600, 0);
    if (!success)
    {
        auto error = SDL_GetError();
        std::print("Failed to create SDL Window: {0}", error);
    }

    auto renderer = SDL_CreateRenderer(window, nullptr);
    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    chip8::Chip8 interpreter{renderer, texture};
    interpreter.loadProgram(romData.data(), romData.size());
    interpreter.run();

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

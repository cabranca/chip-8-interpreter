#include <print>
#include "Chip8.h"


int main(int argc, char** argv) {

    bool success = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (!success) {
        auto error = SDL_GetError();
        std::print("Failed to initialized SDL: {0}", error);
    }

    auto window = SDL_CreateWindow("CHIP-8", 800, 600, 0);
    if (!success) {
        auto error = SDL_GetError();
        std::print("Failed to create SDL Window: {0}", error);
    }

    auto renderer = SDL_CreateRenderer(window, nullptr);
    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    chip8::Chip8 interpreter{texture};
    interpreter.run();
    

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

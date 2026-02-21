#include <SDL3/SDL.h>
#include <print>


int main(int argc, char** argv) {

    bool res = SDL_Init(SDL_INIT_VIDEO);


    std::print("{2} {0}{1}", "World", "!", "Hello");

    return 0;
}

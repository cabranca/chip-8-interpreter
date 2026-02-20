#include <SDL3/SDL.h>
#include <iostream>


int main(int argc, char** argv) {
    int a = 0;
    a++;

    bool res = SDL_Init(SDL_INIT_VIDEO);

    std::cout << "Hello World!" << a << std::endl;

    return 0;
}

#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <cstdint>

namespace chip8
{
    class Renderer
    {
      public:
        Renderer();
        ~Renderer();

        void update(const std::array<uint8_t, 2048> frameBuffer);

      private:
        SDL_Window *m_Window;
        SDL_Renderer *m_Renderer;
        SDL_Texture *m_Texture;
    };
} // namespace chip8
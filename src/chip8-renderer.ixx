module;

#include <array>
#include <cstdint>
#include <SDL3/SDL.h>

export module chip8:renderer;

namespace chip8
{
    export class Renderer
    {
      public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

        void update(const std::array<uint8_t, 2048>& frameBuffer);

      private:
        SDL_Window *m_Window;
        SDL_Renderer *m_Renderer;
        SDL_Texture *m_Texture;
    };
} // namespace chip8
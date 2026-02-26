module;

#include <cstdint>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

export module chip8:renderer;

import std;

namespace chip8
{
    export class Renderer
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
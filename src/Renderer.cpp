module;

#include <print>
#include <SDL3/SDL.h>

module chip8;

namespace chip8
{
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
    constexpr int TEXTURE_WIDTH = 64;
    constexpr int TEXTURE_HEIGHT = 32;

    Renderer::Renderer()
    {
        std::println("Initializing SDL Video Subsystem");
        bool success = SDL_InitSubSystem(SDL_INIT_VIDEO);
        if (!success)
        {
            auto error = SDL_GetError();
            std::println("Failed to initialized SDL: {0}", error);
        }

        std::println("Initializing SDL Window");
        m_Window = SDL_CreateWindow("CHIP-8", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
        if (!m_Window)
        {
            auto error = SDL_GetError();
            std::println("Failed to create SDL Window: {0}", error);
        }

        std::println("Initializing SDL Renderer");
        m_Renderer = SDL_CreateRenderer(m_Window, nullptr);
        if (!m_Renderer)
        {
            auto error = SDL_GetError();
            std::println("Failed to create SDL Renderer: {0}", error);
        }

        std::println("Initializing SDL Texture");
        m_Texture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TEXTURE_WIDTH, TEXTURE_HEIGHT);
        if (!m_Texture)
        {
            auto error = SDL_GetError();
            std::println("Failed to create SDL Texture: {0}", error);
        }
    }

    Renderer::~Renderer()
    {
        if (m_Texture)
            SDL_DestroyTexture(m_Texture);
        if (m_Renderer)
            SDL_DestroyRenderer(m_Renderer);
        if (m_Window)
            SDL_DestroyWindow(m_Window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void Renderer::update(const std::array<uint8_t, 2048>& frameBuffer)
    {
        uint32_t* pixels;
        int pitch;
        SDL_LockTexture(m_Texture, nullptr, reinterpret_cast<void **>(&pixels), &pitch);

        const int stride = pitch / sizeof(uint32_t);

        for (int y = 0; y < 32; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                const uint8_t pixel = frameBuffer.at(y * 64 + x);
                pixels[y * stride + x] = pixel ? 0xFFFFFFFF : 0x000000FF;
            }
            
        }

        SDL_UnlockTexture(m_Texture);

        SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(m_Renderer);
        SDL_RenderTexture(m_Renderer, m_Texture, nullptr, nullptr);
        SDL_RenderPresent(m_Renderer);
    }
}
#pragma once

#include <SDL3/SDL.h>

namespace chip8
{
    class AudioEngine
    {
      public:
        AudioEngine();
        ~AudioEngine();

        void play();
        void update();
        void stop();

      private:
        SDL_AudioStream *m_AudioStream = nullptr;

        int m_CurrentSineSample = 0.f;
    };
}

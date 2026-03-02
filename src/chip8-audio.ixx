module;

#include <SDL3/SDL.h>

export module chip8:audio;

namespace chip8
{
    export class AudioEngine
    {
      public:
        AudioEngine();
        ~AudioEngine();

        AudioEngine(const AudioEngine &) = delete;
        AudioEngine &operator=(const AudioEngine &) = delete;

        void play();
        void update();
        void stop();

      private:
        SDL_AudioStream *m_AudioStream = nullptr;

        int m_CurrentSineSample = 0;
    };
}

#include "AudioEngine.h"

#include <print>

namespace chip8
{
    AudioEngine::AudioEngine()
    {
        std::println("Initializing SDL Audio Subsystem");
        bool success = SDL_InitSubSystem(SDL_INIT_AUDIO);
        if (!success)
        {
            auto error = SDL_GetError();
            std::println("Failed to initialized SDL: {0}", error);
        }

        std::println("Initializing SDL Audio Subsystem");
        SDL_AudioSpec audioSpecs;
        audioSpecs.format = SDL_AUDIO_S8;
        audioSpecs.channels = 1;
        audioSpecs.freq = 8000;

        m_AudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpecs,
                                                  nullptr, nullptr);
        if (!m_AudioStream)
        {
            auto error = SDL_GetError();
            std::println("Failed to create SDL Audio: {0}", error);
        }
    }

    AudioEngine::~AudioEngine()
    {
        if (m_AudioStream)
            SDL_DestroyAudioStream(m_AudioStream);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    void AudioEngine::play()
    {
        SDL_ResumeAudioStreamDevice(m_AudioStream);
    }

    void AudioEngine::update()
    {
        SDL_ResumeAudioStreamDevice(m_AudioStream);
        const int minimum_audio = (8000 * sizeof(float)) / 2;
        if (SDL_GetAudioStreamQueued(m_AudioStream) < minimum_audio)
        {
            static float samples[512]; /* this will feed 512 samples each frame
                                                      until we get to our maximum. */
            int i;

            /* generate a 440Hz pure tone */
            for (i = 0; i < SDL_arraysize(samples); i++)
            {
                const int freq = 440;
                const float phase = m_CurrentSineSample * freq / 8000.0f;
                samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
                m_CurrentSineSample++;
            }

            /* wrapping around to avoid floating-point errors */
            m_CurrentSineSample %= 8000;

            /* feed the new data to the stream. It will queue at the end, and
             * trickle out as the hardware needs more data. */
            SDL_PutAudioStreamData(m_AudioStream, samples, sizeof(samples));
        }
    }

    void AudioEngine::stop()
    {
        SDL_PauseAudioStreamDevice(m_AudioStream);
    }
}

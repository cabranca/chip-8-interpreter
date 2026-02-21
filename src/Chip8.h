#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <cstdint>

namespace chip8
{

    class Chip8
    {
      public:
        Chip8(SDL_Texture *texture);

        void run();

      private:
        uint16_t m_PC = 0x200;

        std::array<uint8_t, 4096> m_Memory;
        std::array<uint8_t, 16> m_Reg;
        uint16_t m_IReg = 0;

        std::array<uint16_t, 16> m_Stack;
        uint8_t m_StackPtr = 0;

        std::array<uint8_t, 64 * 32> m_Video;

        uint8_t m_Delay = 0;
        uint8_t m_Sound = 0;

        SDL_Texture *m_Texture;

        uint16_t fetch();
        void decodeExecute(uint16_t instruction);
        void clearScreen();
        void jump(uint16_t address);
        void setRegister(uint8_t index, uint8_t value);
        void addRegister(uint8_t index, uint8_t value);
        void setIndexRegister(uint16_t value);
        void draw(uint8_t regx, uint8_t regy, uint8_t height);
    };
} // namespace chip8
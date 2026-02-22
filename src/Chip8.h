#pragma once

#include <stack>

#include "Renderer.h"

namespace chip8
{

    class Chip8
    {
      public:
        void loadProgram(uint8_t *data, size_t size);
        void run();

      private:
        uint16_t m_PC = 0x200;

        std::array<uint8_t, 4096> m_Memory;
        std::array<uint8_t, 16> m_Reg;
        uint16_t m_IReg = 0;

        std::stack<uint16_t> m_Stack;

        std::array<uint8_t, 64 * 32> m_Video;

        uint8_t m_Delay = 0;
        uint8_t m_Sound = 0;

        Renderer m_Renderer;

        uint16_t fetch();
        void decodeExecute(uint16_t instruction);

        void clearScreen();

        void jump(uint16_t address);
        
        void callSubroutine(uint16_t address);
        void returnSubroutine();

        void jumpEqual(uint8_t x, uint8_t nn);
        void jumpNotEqual(uint8_t x, uint8_t nn);
        void jumpRegEqual(uint8_t x, uint8_t y);
        void jumpRegNotEqual(uint8_t x, uint8_t y);
        
        void setRegister(uint8_t index, uint8_t value);
        
        void addRegister(uint8_t index, uint8_t value);
        
        void setIndexRegister(uint16_t value);
        
        void draw(uint8_t regx, uint8_t regy, uint8_t height);
    };
} // namespace chip8
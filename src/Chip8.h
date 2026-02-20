#pragma once

#include <array>
#include <cstdint>

namespace chip8 {

    class Chip8 {
        public:
            Chip8();

        private:
            std::array<uint8_t, 16> m_Reg;
            uint16_t m_IReg = 0;
            uint8_t m_Delay = 0;
            uint8_t m_Sound = 0;
            uint16_t m_PC = 0;
            uint8_t m_StackPtr = 0;
            std::array<uint16_t, 16> m_Stack;
    };
}
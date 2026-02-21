#include "Chip8.h"

namespace chip8
{
    constexpr uint16_t X_MASK = 0x0F00;
    constexpr uint16_t Y_MASK = 0x00F0;
    constexpr uint16_t N_MASK = 0x000F;
    constexpr uint16_t NN_MASK = 0x00FF;
    constexpr uint16_t NNN_MASK = 0x0FFF;

    Chip8::Chip8(SDL_Texture *texture) : m_Texture(texture)
    {
        m_Memory[0x200] = 1;
        m_Memory[0x201] = 2;
    }

    void Chip8::run()
    {
        bool running = true;
        while (running)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                default:
                    break;
                }
            }

            auto instruction = fetch();
            decodeExecute(instruction);
        }
    }

    uint16_t Chip8::fetch()
    {
        const uint16_t high = m_Memory.at(m_PC);
        const uint16_t low = m_Memory.at(m_PC + 1);

        // Only increment now to ensure the read hasn't thrown exception
        m_PC += 2;
        return (high << 8 | low);
    }

    void Chip8::decodeExecute(uint16_t instruction)
    {
        const uint8_t opcode = static_cast<uint8_t>(instruction >> 12);
        uint8_t vx = static_cast<uint8_t>((instruction & X_MASK) >> 8);
        uint8_t vy = static_cast<uint8_t>((instruction & Y_MASK) >> 4);
        uint8_t n = static_cast<uint8_t>(instruction & N_MASK);
        uint8_t nn = static_cast<uint8_t>(instruction & NN_MASK);
        uint16_t nnn = instruction & NNN_MASK;
        switch (opcode)
        {
        case 0:
            // Clear Screen
            break;
        case 1:
            jump(instruction & NNN_MASK);
            break;
        case 6:
            setRegister(vx, nn);
            break;
        case 7:
            addRegister(vx, nn);
            break;
        case 0xA:
            setIndexRegister(instruction & NNN_MASK);
            break;
        case 0xD:
            draw(vx, vy, n);
            break;
        }
    }

    void Chip8::clearScreen()
    {
    }

    void Chip8::jump(uint16_t address)
    {
        m_PC = address;
    }

    void Chip8::setRegister(uint8_t index, uint8_t value)
    {
        m_Reg.at(index) = value;
    }

    void Chip8::addRegister(uint8_t index, uint8_t value)
    {
        m_Reg.at(index) += value;
    }

    void Chip8::setIndexRegister(uint16_t value)
    {
        m_IReg = value;
    }

    void Chip8::draw(uint8_t regx, uint8_t regy, uint8_t height)
    {
    }
} // namespace chip8
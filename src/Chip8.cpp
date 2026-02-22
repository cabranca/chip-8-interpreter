#include "Chip8.h"

namespace chip8
{
    constexpr uint16_t X_MASK = 0x0F00;
    constexpr uint16_t Y_MASK = 0x00F0;
    constexpr uint16_t N_MASK = 0x000F;
    constexpr uint16_t NN_MASK = 0x00FF;
    constexpr uint16_t NNN_MASK = 0x0FFF;

    void Chip8::loadProgram(uint8_t *data, size_t size)
    {
        memcpy(&m_Memory.at(0x200), data, size);
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
                switch (n)
                {
                    case 0:
                        clearScreen();
                        break;
                    case 0xE:
                        returnSubroutine();
                        break;
                }
                break;
            case 1:
                jump(instruction & NNN_MASK);
                break;
            case 2:
                callSubroutine(instruction & NNN_MASK);
                break;
            case 3:
                jumpEqual(vx, nn);
                break;
            case 4:
                jumpNotEqual(vx, nn);
                break;
            case 5:
                jumpRegEqual(vx, vy);
                break;
            case 9:
                jumpRegNotEqual(vx, vy);
                break;
            case 6:
                setRegister(vx, nn);
                break;
            case 7:
                addRegister(vx, nn);
                break;
            case 8:
                switch (n)
                {
                    case 0:
                        setXtoY(vx, vy);
                        break;
                    case 1:
                        binaryOR(vx, vy);
                        break;
                    case 2:
                        binaryAND(vx, vy);
                        break;
                    case 3:
                        binaryXOR(vx, vy);
                        break;
                    case 4:
                        add(vx, vy);
                        break;
                    case 5:
                        substractYtoX(vx, vy);
                        break;
                    case 7:
                        substractXtoY(vx, vy);
                        break;
                    case 6:
                        shiftR(vx, vy);
                        break;
                    case 0xE:
                        shiftL(vx, vy);
                        break;
                }
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
        for (auto &pixel : m_Video)
            pixel = 0;
        m_Reg.at(0xF) = 0;
        m_Renderer.update(m_Video);
    }

    void Chip8::jump(uint16_t address)
    {
        m_PC = address;
    }

    void Chip8::callSubroutine(uint16_t address)
    {
        m_Stack.push(m_PC);
        m_PC = address;
    }

    void Chip8::returnSubroutine()
    {
        m_PC = m_Stack.top();
        m_Stack.pop();
    }

    void Chip8::jumpEqual(uint8_t x, uint8_t nn)
    {
        m_PC += m_Reg.at(x) == nn ? m_PC + 2 : m_PC;
    }

    void Chip8::jumpNotEqual(uint8_t x, uint8_t nn)
    {
        m_PC += m_Reg.at(x) != nn ? m_PC + 2 : m_PC;
    }

    void Chip8::jumpRegEqual(uint8_t x, uint8_t y)
    {
        m_PC += m_Reg.at(x) == m_Reg.at(y) ? m_PC + 2 : m_PC;
    }

    void Chip8::jumpRegNotEqual(uint8_t x, uint8_t y)
    {
        m_PC += m_Reg.at(x) != m_Reg.at(y) ? m_PC + 2 : m_PC;
    }

    void Chip8::setXtoY(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) = m_Reg.at(y);
    }

    void Chip8::binaryOR(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) |= m_Reg.at(y);
    }

    void Chip8::binaryAND(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) &= m_Reg.at(y);
    }

    void Chip8::binaryXOR(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) ^= m_Reg.at(y);
    }

    void Chip8::add(uint8_t x, uint8_t y)
    {
        if (m_Reg.at(x) + m_Reg.at(y) > 255)
            m_Reg.at(0xF) = 1;
        else
            m_Reg.at(0xF) = 0;
        m_Reg.at(x) += m_Reg.at(y);
    }

    void Chip8::substractYtoX(uint8_t x, uint8_t y)
    {
        if (m_Reg.at(x) > m_Reg.at(y))
            m_Reg.at(0xF) = 1;
        else
            m_Reg.at(0xF) = 0;
        m_Reg.at(x) -= m_Reg.at(y);
    }

    void Chip8::substractXtoY(uint8_t x, uint8_t y)
    {
        if (m_Reg.at(y) > m_Reg.at(x))
            m_Reg.at(0xF) = 1;
        else
            m_Reg.at(0xF) = 0;
        m_Reg.at(x) = m_Reg.at(y) - m_Reg.at(x);
    }

    void Chip8::shiftL(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) = m_Reg.at(y);
        m_Reg.at(x) <<= 1;
    }

    void Chip8::shiftR(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) = m_Reg.at(y);
        m_Reg.at(x) >>= 1;
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
        const uint8_t xPos = m_Reg.at(regx) % 64;
        const uint8_t yPos = m_Reg.at(regy) % 32;

        m_Reg.at(0xF) = 0;

        for (uint8_t row = 0; row < height; row++)
        {
            if (yPos + row >= 32)
                break;

            const uint8_t spriteByte = m_Memory.at(m_IReg + row);

            for (uint8_t col = 0; col < 8; col++)
            {
                if (xPos + col >= 64)
                    break;

                const uint8_t spritePixel = (spriteByte >> (7 - col)) & 1;
                if (spritePixel == 0)
                    continue;

                const uint16_t screenIndex = (yPos + row) * 64 + (xPos + col);

                if (m_Video.at(screenIndex) == 1)
                    m_Reg.at(0xF) = 1;

                m_Video.at(screenIndex) ^= 1;
            }
        }

        m_Renderer.update(m_Video);
    }
} // namespace chip8
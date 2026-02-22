#include "Chip8.h"

namespace chip8
{
    constexpr uint16_t X_MASK = 0x0F00;
    constexpr uint16_t Y_MASK = 0x00F0;
    constexpr uint16_t N_MASK = 0x000F;
    constexpr uint16_t NN_MASK = 0x00FF;
    constexpr uint16_t NNN_MASK = 0x0FFF;

    constexpr uint16_t FONT_START_ADDRESS = 0x050;

    // Each character is a 4x5 pixel sprite stored as 5 bytes.
    // Characters: 0 1 2 3 4 5 6 7 8 9 A B C D E F
    constexpr std::array<uint8_t, 80> FONT_SET = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    constexpr std::array<std::pair<SDL_Scancode, uint8_t>, 16> SDL_TO_CHIP8_KEYS = {{
        {SDL_SCANCODE_1, 0x1},
        {SDL_SCANCODE_2, 0x2},
        {SDL_SCANCODE_3, 0x3},
        {SDL_SCANCODE_4, 0xC},
        {SDL_SCANCODE_Q, 0x4},
        {SDL_SCANCODE_W, 0x5},
        {SDL_SCANCODE_E, 0x6},
        {SDL_SCANCODE_R, 0xD},
        {SDL_SCANCODE_A, 0x7},
        {SDL_SCANCODE_S, 0x8},
        {SDL_SCANCODE_D, 0x9},
        {SDL_SCANCODE_F, 0xE},
        {SDL_SCANCODE_Z, 0xA},
        {SDL_SCANCODE_X, 0x0},
        {SDL_SCANCODE_C, 0xB},
        {SDL_SCANCODE_V, 0xF},
    }};

    constexpr std::optional<uint8_t> findChip8Key(SDL_Scancode scancode)
    {
        for (const auto& [sc, key] : SDL_TO_CHIP8_KEYS)
        {
            if (sc == scancode)
                return key;
        }
        return std::nullopt;
    }

    Chip8::Chip8()
    {
        m_Memory.fill(0);
        m_Reg.fill(0);
        m_Video.fill(0);
        m_KeyMap.fill(false);

        // Load font set into memory at 0x050
        std::copy(FONT_SET.begin(), FONT_SET.end(), m_Memory.begin() + FONT_START_ADDRESS);
    }

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
                case SDL_EVENT_KEY_DOWN:
                    if (auto key = findChip8Key(event.key.scancode))
                        m_KeyMap[*key] = true;
                    break;
                case SDL_EVENT_KEY_UP:
                    if (auto key = findChip8Key(event.key.scancode))
                        m_KeyMap[*key] = false;
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
            case 0xB:
                jumpOffset(instruction & NNN_MASK);
                break;
            case 0xC:
                random(vx, nn);
                break;
            case 0xD:
                draw(vx, vy, n);
                break;
            case 0xE:
                switch (vy)
                {
                    case 9:
                        jumpKey(vx);
                        break;
                    case 0xA:
                        jumpNotKey(vx);
                        break;
                }
                break;
            case 0xF:
                switch (nn) {
                    case 0x07:
                        setXToDelay(vx);
                        break;
                    case 0x15:
                        setDelay(vx);
                        break;
                    case 0x18:
                        setSound(vx);
                        break;
                    case 0x1E:
                        addToIndex(vx);
                        break;
                    case 0x0A:
                        getKey(vx);
                        break;
                    case 0x29:
                        setIndexToFont(vx);
                        break;
                    case 0x33:
                        splitValue(vx);
                        break;
                    case 0x55:
                        storeMemory(vx);
                        break;
                    case 0x65:
                        loadMemory(vx);
                        break;
                }
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
        m_PC = m_Reg.at(x) == nn ? m_PC + 2 : m_PC;
    }

    void Chip8::jumpNotEqual(uint8_t x, uint8_t nn)
    {
        m_PC = m_Reg.at(x) != nn ? m_PC + 2 : m_PC;
    }

    void Chip8::jumpRegEqual(uint8_t x, uint8_t y)
    {
        m_PC = m_Reg.at(x) == m_Reg.at(y) ? m_PC + 2 : m_PC;
    }

    void Chip8::jumpRegNotEqual(uint8_t x, uint8_t y)
    {
        m_PC = m_Reg.at(x) != m_Reg.at(y) ? m_PC + 2 : m_PC;
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
        m_Reg.at(0xF) = (m_Reg.at(x) >> 7) & 1;
        m_Reg.at(x) <<= 1;
    }

    void Chip8::shiftR(uint8_t x, uint8_t y)
    {
        m_Reg.at(x) = m_Reg.at(y);
        m_Reg.at(0xF) = m_Reg.at(x) & 1;
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

    void Chip8::jumpOffset(uint16_t address)
    {
        m_PC = address + m_Reg.at(0);
    }

    void Chip8::random(uint8_t x, uint8_t nn)
    {
        m_Reg.at(x) = m_RngDist(m_Rng) & nn;
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
    
    void Chip8::jumpKey(uint8_t x)
    {
        m_PC = m_KeyMap.at(m_Reg.at(x)) ? m_PC + 2 : m_PC;
    }
    
    void Chip8::jumpNotKey(uint8_t x)
    {
        m_PC = !m_KeyMap.at(m_Reg.at(x)) ? m_PC + 2 : m_PC;
    }
    
    void Chip8::setXToDelay(uint8_t x)
    {
        m_Reg.at(x) = m_DelayTimer;
    }
    
    void Chip8::setDelay(uint8_t x)
    {
        m_DelayTimer = m_Reg.at(x);
    }
    
    void Chip8::setSound(uint8_t x)
    {
        m_SoundTimer = m_Reg.at(x);
    }
    
    void Chip8::addToIndex(uint8_t x)
    {
        if (m_IReg + m_Reg.at(x) > 0x0FFF)
            m_Reg.at(0xF) = 1;
        else
            m_Reg.at(0xF) = 0;
        m_IReg += m_Reg.at(x);
    }

    void Chip8::getKey(uint8_t x)
    {
        for (uint8_t i = 0; i <= 0xF; i++)
        {
            if (m_KeyMap.at(i))
            {
                m_Reg.at(x) = i;
                return;
            }
        }
        m_PC -= 2;
    }

    void Chip8::setIndexToFont(uint8_t x)
    {
        m_IReg = FONT_START_ADDRESS + (m_Reg.at(x) & 0x0F) * 5;
    }

    void Chip8::splitValue(uint8_t x)
    {
        uint8_t value = m_Reg.at(x);
        m_Memory.at(m_IReg)     = value / 100;
        m_Memory.at(m_IReg + 1) = (value / 10) % 10;
        m_Memory.at(m_IReg + 2) = value % 10;
    }

    void Chip8::storeMemory(uint8_t x)
    {
        for (uint8_t i = 0; i <= x; i++)
            m_Memory.at(m_IReg +  i) = m_Reg.at(i);
    }

    void Chip8::loadMemory(uint8_t x)
    {
        for (uint8_t i = 0; i <= x; i++)
            m_Reg.at(i) = m_Memory.at(m_IReg +  i);
    }

} // namespace chip8
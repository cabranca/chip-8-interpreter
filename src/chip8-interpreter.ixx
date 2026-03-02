module;

#include <cstdint>
#include <random>
#include <span>
#include <stack>
#include <vector>

export module chip8:interpreter;
import :renderer;
import :audio;

namespace chip8
{

    export class Chip8
    {
      public:
        Chip8();
        void loadProgram(std::span<const uint8_t> program);
        void run();

      private:
        uint16_t m_PC = 0x200;

        std::array<uint8_t, 4096> m_Memory;
        std::array<uint8_t, 16> m_Reg;
        uint16_t m_IReg = 0;

        std::stack<uint16_t> m_Stack;

        std::array<uint8_t, 64 * 32> m_Video;

        uint8_t m_DelayTimer = 0;
        uint8_t m_SoundTimer = 0;

        std::mt19937 m_Rng{std::random_device{}()};
        std::uniform_int_distribution<unsigned short> m_RngDist{0, 255};

        Renderer m_Renderer;
        AudioEngine m_Audio;

        std::array<bool, 16> m_KeyMap;

        bool m_Running = true;
        bool m_Beeping = false;

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

        void setXtoY(uint8_t x, uint8_t y);
        void binaryOR(uint8_t x, uint8_t y);
        void binaryAND(uint8_t x, uint8_t y);
        void binaryXOR(uint8_t x, uint8_t y);
        void add(uint8_t x, uint8_t y);
        void substractYtoX(uint8_t x, uint8_t y);
        void substractXtoY(uint8_t x, uint8_t y);
        void shiftL(uint8_t x, uint8_t y);
        void shiftR(uint8_t x, uint8_t y);
        
        void setRegister(uint8_t index, uint8_t value);
        
        void addRegister(uint8_t index, uint8_t value);
        
        void setIndexRegister(uint16_t value);

        void jumpOffset(uint16_t address);

        void random(uint8_t x, uint8_t nn);
        
        void draw(uint8_t regx, uint8_t regy, uint8_t height);

        void jumpKey(uint8_t x);
        void jumpNotKey(uint8_t x);

        void setXToDelay(uint8_t x);
        void setDelay(uint8_t x);
        void setSound(uint8_t x);

        void addToIndex(uint8_t x);
        void getKey(uint8_t x);
        void setIndexToFont(uint8_t x);
        void splitValue(uint8_t x);

        void storeMemory(uint8_t x);
        void loadMemory(uint8_t x);
    };
} // namespace chip8
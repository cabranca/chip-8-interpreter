#include "Chip8.h"

namespace chip8 {

    Chip8::Chip8(SDL_Texture *texture) : m_Texture(texture)
    {

    }

    void Chip8::run()
    {
        bool running = true;
        while (running) {
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                switch(event.type){
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
        uint16_t instruction = m_Memory.at(m_PC++) << 8;
        instruction |= m_Memory.at(m_PC++);
        return instruction;
    }

    void Chip8::decodeExecute(uint16_t instruction)
    {
        uint8_t opcode = instruction & 15 << 12;
        switch (opcode)
        {
            case 0:
                // Clear Screen
                break;
            case 1:
                // Jump
                break;
            case 6:
                // Set register Vx
                break;
            case 7:
                // Add value to Vx
                break;
            case 0xA:
                // Set index register I
                break;
            case 0xD:
                // Display/Draw
                break;
        }
    }
}
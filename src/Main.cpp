#include <cstdio>
#include <cstdlib>
#include <print>
#include <vector>

#include "Chip8.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::println(stderr, "Usage: {} <rom-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::FILE *romFile = std::fopen(argv[1], "rb");
    if (!romFile)
    {
        std::println(stderr, "Failed to open ROM: {}\n", argv[1]);
        return EXIT_FAILURE;
    }

    std::fseek(romFile, 0, SEEK_END);
    const auto romSize = std::ftell(romFile);
    std::fseek(romFile, 0, SEEK_SET);

    std::vector<uint8_t> romData(romSize);
    std::fread(romData.data(), 1, romSize, romFile);
    std::fclose(romFile);

    std::println("Initializing CHIP-8 INTERPRETER");
    chip8::Chip8 interpreter;
    std::println("Finished CHIP-8 INTERPRETER initialization");
    std::println("Loading program into memory");
    interpreter.loadProgram(romData.data(), romData.size());
    std::println("Executing...");
    interpreter.run();

    return 0;
}

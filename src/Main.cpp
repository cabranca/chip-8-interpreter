#include <cstdio>
#include <cstdlib>
#include <print>
#include <vector>

#include "Chip8.h"

std::optional<std::vector<uint8_t>> openROM(int argc, char** argv)
{
    if (argc < 2)
    {
        std::println(stderr, "Usage: {} <rom-file>\n", argv[0]);
        return std::nullopt;
    }

    std::FILE *romFile = std::fopen(argv[1], "rb");
    if (!romFile)
    {
        std::println(stderr, "Failed to open ROM: {}\n", argv[1]);
        return std::nullopt;
    }

    std::fseek(romFile, 0, SEEK_END);
    const auto romSize = std::ftell(romFile);
    std::fseek(romFile, 0, SEEK_SET);

    std::vector<uint8_t> romData(romSize);
    const auto bytesRead = std::fread(romData.data(), 1, romSize, romFile);
    std::fclose(romFile);

    if (bytesRead != static_cast<size_t>(romSize))
    {
        std::println(stderr, "Failed to read ROM: expected {} bytes, got {}", romSize, bytesRead);
        return std::nullopt;
    }

    return romData;
}

int main(int argc, char **argv)
{
    auto rom = openROM(argc, argv);
    if (!rom)
        return EXIT_FAILURE;

    std::println("Initializing CHIP-8 INTERPRETER");
    chip8::Chip8 interpreter;
    std::println("Finished CHIP-8 INTERPRETER initialization");
    std::println("Loading program into memory");
    interpreter.loadProgram((*rom).data(), (*rom).size());
    std::println("Executing...");
    interpreter.run();

    return 0;
}

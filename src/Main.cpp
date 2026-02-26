#include <expected>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <vector>

#include "Chip8.h"

enum class ROMLoadError
{
    ROMNotSelected, 
    FileNotFound,
    NotAFile,
    SizeCheckFailure,
    PermisisonDenied,
    ReadFailure
};

struct ROMLoadErrorData
{
    ROMLoadError Type;
    std::filesystem::path Path;
    std::string Message;
};

static std::expected<std::vector<uint8_t>, ROMLoadErrorData> openROM(int argc, char** argv)
{
    std::error_code errorCode;
    if (argc < 2)
    {
        std::println(stderr, "Usage: {} <rom-file>\n", argv[0]);
        return std::unexpected(
            ROMLoadErrorData{ROMLoadError::ROMNotSelected, "", errorCode.message()});
    }

    const std::filesystem::path path{argv[1]};
    if (!std::filesystem::exists(path, errorCode))
        return std::unexpected(
            ROMLoadErrorData{ROMLoadError::FileNotFound, path, errorCode.message()});

    if (!std::filesystem::is_regular_file(path, errorCode))
        return std::unexpected(ROMLoadErrorData{ROMLoadError::NotAFile, path, errorCode.message()});

    const auto size = std::filesystem::file_size(path, errorCode);
    if (errorCode)
        return std::unexpected(
            ROMLoadErrorData{ROMLoadError::SizeCheckFailure, path, errorCode.message()});

    std::ifstream file(path, std::ios::binary);
    if (!file)
        return std::unexpected(ROMLoadErrorData{
            ROMLoadError::PermisisonDenied, "Cannot open the file"
        });

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char *>(buffer.data()), buffer.size()))
        return std::unexpected(
            ROMLoadErrorData{ROMLoadError::ReadFailure, path, "Cannot read file"});

    return buffer;
}

template <>
struct std::formatter<ROMLoadErrorData> : std::formatter<std::string>
{
    auto format(const ROMLoadErrorData& e, std::format_context& ctx) const
    {
        std::string description;
        switch (e.Type)
        {
        case ROMLoadError::ROMNotSelected:
            description = "ROM path not selected";
            break;
        case ROMLoadError::FileNotFound:
            description = "File not found";
            break;
        case ROMLoadError::NotAFile:
            description = "The path doesn't correspond to a file";
            break;
        case ROMLoadError::SizeCheckFailure:
            description = "Failed checking the file size";
            break;
        case ROMLoadError::PermisisonDenied:
            description = "Permission denied to read the file";
            break;
        case ROMLoadError::ReadFailure:
            description = "Failed to read the file";
            break;
        }

        std::string result = std::format("Error loading the ROM {}: {}", e.Path.string(), description);
        if (!e.Message.empty())
            result += std::format(" ({})", e.Message);

        return std::formatter<std::string>::format(result, ctx);
    }
};

int main(int argc, char **argv)
{
    auto result = openROM(argc, argv)
        .transform( [](const auto &rom)
        {
            std::println("Initializing CHIP-8 INTERPRETER");
            chip8::Chip8 interpreter;
            std::println("Finished CHIP-8 INTERPRETER initialization");
            std::println("Loading program into memory");
            interpreter.loadProgram(rom);
            std::println("Executing...");
            interpreter.run();
            return rom;
        })
        .or_else([](const auto& error)
            -> std::expected<std::vector<uint8_t>, ROMLoadErrorData>
        {
            std::println(stderr, "Error: {}", error);
            return std::unexpected(error);
        });

    return 0;
}

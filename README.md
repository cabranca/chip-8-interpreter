# CHIP-8 Interpreter

A CHIP-8 interpreter written in C++23, using SDL3 for display, input, and audio.

## Overview

CHIP-8 is an interpreted programming language developed in the mid-1970s, originally designed to run on the COSMAC VIP and Telmac 1800 microcomputers. Programs run on a virtual machine with the following specifications:

- 4 KB of RAM (0x000 - 0xFFF)
- 16 general-purpose 8-bit registers (V0 - VF)
- A 16-bit index register (I)
- A program counter starting at 0x200
- A 64x32 monochrome display
- A 16-key hexadecimal keypad
- Two 8-bit timers (delay and sound) counting down at 60 Hz
- A call stack for subroutine support

This implementation targets the standard CHIP-8 specification and executes approximately 700 instructions per second, with display and timer updates at 60 Hz.

## Requirements

- CMake 3.28 or later
- GCC 14 or later (C++23 with module support required)
- Git (for submodule initialization)

The SDL3 library is included as a Git submodule and is built as a static library. No separate SDL installation is required.

## Building

Initialize the SDL3 submodule before building:

```bash
git submodule update --init --recursive
```

### Debug build

```bash
mkdir -p build/Debug
cd build/Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build . -j $(nproc)
```

### Release build

```bash
mkdir -p build/Release
cd build/Release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . -j $(nproc)
```

The resulting binary is placed at `build/<BuildType>/chip-8-interpreter`.

## Usage

```
chip-8-interpreter <path-to-rom>
```

Example:

```bash
./chip-8-interpreter IBM_Logo.ch8
```

The interpreter loads the ROM into memory starting at address 0x200 and begins execution. Close the window to exit.

## Keyboard Mapping

The original CHIP-8 keypad is a 4x4 hexadecimal grid. The mapping to a standard QWERTY keyboard is as follows:

```
CHIP-8 keypad     Keyboard
-----------------+------------------
1  2  3  C       |  1  2  3  4
4  5  6  D       |  Q  W  E  R
7  8  9  E       |  A  S  D  F
A  0  B  F       |  Z  X  C  V
```

## Architecture

### Source files

| File | Description |
|---|---|
| `src/Main.cpp` | Entry point. Handles ROM loading and error reporting using `std::expected`. |
| `src/Chip8.cpp` | Core interpreter. Implements the fetch-decode-execute cycle, all 35 opcodes, timers, and the main run loop. |
| `src/Renderer.cpp` | SDL3-based renderer. Displays the 64x32 framebuffer scaled to an 800x600 window. |
| `src/AudioEngine.cpp` | SDL3-based audio engine. Generates a 440 Hz sine wave when the sound timer is active. |
| `src/chip8-interpreter.ixx` | Module interface unit for the `Chip8` class. |
| `src/chip8-renderer.ixx` | Module interface unit for the `Renderer` class. |
| `src/chip8-audio.ixx` | Module interface unit for the `AudioEngine` class. |
| `src/chip8.ixx` | Module re-export unit. |

### Memory layout

| Range | Contents |
|---|---|
| 0x000 - 0x04F | Reserved |
| 0x050 - 0x09F | Built-in font sprites (16 characters, 5 bytes each) |
| 0x200 - 0xFFF | ROM and working RAM |

### Display

The renderer creates an SDL3 window at 800x600 pixels. The 64x32 CHIP-8 framebuffer is stored as a flat array of `uint32_t` pixels in RGBA8888 format and uploaded each frame to a streaming texture. Active pixels are white (0xFFFFFFFF) and inactive pixels are black (0x000000FF).

### Audio

The audio engine opens an SDL3 audio stream at 8000 Hz, mono, using 32-bit floating-point samples. When the sound timer is non-zero, it generates and queues 440 Hz sine wave samples. Audio is paused when the sound timer reaches zero.

### Timing

- Instructions execute at approximately 700 Hz (one instruction every ~1.43 ms).
- Display updates and timer decrements occur at 60 Hz (every ~16.67 ms).
- Timing is driven by `SDL_GetTicks()` inside the main run loop.

## Dependencies

| Dependency | Version | Purpose |
|---|---|---|
| SDL3 | submodule (libsdl-org/SDL) | Window, rendering, audio, input |

SDL3 is built from source as a static library. No shared library installation is required on the target system.

## Language Features

The project uses several C++23 features:

- `std::expected<T, E>` for error propagation in ROM loading
- `std::mdspan` for multi-dimensional views over the framebuffer
- `std::print` / `std::println` for formatted console output
- C++20 named modules (`.ixx` files) for interface separation

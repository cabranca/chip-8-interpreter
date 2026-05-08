# CHIP-8 Interpreter

A CHIP-8 interpreter written in modern C++23 with SDL3 for display, input, and audio.

CHIP-8 is an interpreted language from the mid-1970s, originally designed for the COSMAC VIP microcomputer. It runs on a simple virtual machine: 4 KB of RAM, 16 registers, a 64×32 monochrome display, a 16-key hexadecimal keypad, and delay/sound timers. This implementation executes ~700 instructions/sec with display and timer updates at 60 Hz.

## Stack

| Layer | Technology |
|-------|------------|
| Language | C++23 |
| Build | CMake 3.28+, GCC 14+ |
| Display | SDL3 (streaming texture, 64×32 → 800×600) |
| Audio | SDL3 audio stream (440 Hz sine wave, 8 kHz mono f32) |
| Input | SDL3 keyboard events |
| Modules | C++20 named modules (`.ixx` interface units) |
| Error handling | `std::expected<T, E>` |

SDL3 is included as a Git submodule and built as a static library — no separate installation needed.

## Building

```bash
git submodule update --init --recursive

mkdir -p build/Debug && cd build/Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cmake --build . -j $(nproc)
```

## Usage

```bash
./build/Debug/chip-8-interpreter <path-to-rom>
```

## Keyboard Mapping

```
CHIP-8: 1 2 3 C     →   Keyboard: 1 2 3 4
        4 5 6 D                    Q W E R
        7 8 9 E                    A S D F
        A 0 B F                    Z X C V
```

# Game Boy (DMG-01) Emulator

A cycle-stepped original Game Boy (DMG-01) emulator written from scratch in C++ using SDL3 for rendering and input.

## Implemented Hardware Components

- **CPU (`cpuCore.h` / `cpuCore.cpp`)**: Full Sharp LR35902 instruction set (base opcodes and `0xCB` bit-manipulation prefix table), accurate flag handling, and hardware interrupt dispatch (`VBlank`, `LCD STAT`, `Timer`, `Serial`, `Joypad`). Passes all 11 of Blargg's `cpu_instrs` individual test ROMs and `instr_timing`.
- **Memory Management Unit (`mmu.h` / `mmu.cpp`)**: Full 16-bit (`0x0000 - 0xFFFF`) memory map routing across ROM banks, VRAM, External RAM, WRAM, Echo RAM, OAM, I/O registers, and HRAM, including OAM DMA transfers (`0xFF46`).
- **Cartridge & MBC (`cartridge.h` / `cartridge.cpp`)**: Polymorphic cartridge interface supporting `ROM ONLY` (32 KB) and `MBC1` ROM/RAM bank switching up to 2 MB ROM / 32 KB RAM.
- **PPU (`ppu.h` / `ppu.cpp`)**: Dot-accurate state machine cycling through OAM Scan (Mode 2), Pixel Transfer (Mode 3), H-Blank (Mode 0), and V-Blank (Mode 1). Supports Background scrolling (`SCX`/`SCY`), Window overlay (`WX`/`WY`), and OAM Sprites (`8x8` and `8x16` modes, X/Y flipping, transparency, priority, and palette remapping).
- **Timers (`timers.h` / `timers.cpp`)**: Cycle-accurate `DIV` (`0xFF04`) internal counter and configurable `TIMA`/`TMA`/`TAC` timer with falling-edge detection and overflow interrupts.
- **Display & Input (`renderer.cpp` / `joypad.cpp`)**: SDL3 streaming texture output (`160x144` integer scaling) paced to 59.7275 Hz (`70224` T-cycles per frame, `16742 us` frame time) and active-low matrix joypad multiplexing (`0xFF00`).

## Controls

| Game Boy Button | Keyboard Key |
| :--- | :--- |
| **Up / Left / Down / Right** | `W` / `A` / `S` / `D` |
| **A / B** | `K` / `J` |
| **Start / Select** | `T` / `Y` |

## Usage

```bash
./gb_emu <path_to_rom.gb>
```

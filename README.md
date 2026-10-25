# Game Boy (DMG-01) Emulator

A Game Boy emulator written in C++ using SDL3 for graphics and input handling.

## Features

- **CPU**: Sharp LR35902 core supporting all base and `0xCB` prefix instructions. Passes Blargg's `cpu_instrs` and `instr_timing` test ROMs.
- **Memory & Cartridges**: Basic memory bus (MMU) with support for `ROM ONLY` and `MBC1` cartridge bank switching.
- **Graphics (PPU)**: Renders background tiles, window layer, and sprites (8x8 and 8x16 modes with priority and palette mapping).
- **Timers & Interrupts**: Internal divider (`DIV`), configurable timer (`TIMA`), and hardware interrupt handling.
- **Display & Timing**: 160x144 frame rendering paced to original Game Boy timing (~59.7 Hz) using SDL3.

## Controls

| Game Boy | Keyboard |
| :--- | :--- |
| **D-Pad** | `W`, `A`, `S`, `D` |
| **A / B** | `K` / `J` |
| **Start / Select** | `T` / `Y` |

## Running

```bash
./gb_emu <path_to_rom.gb>
```

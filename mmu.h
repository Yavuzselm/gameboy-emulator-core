#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "cartridge.h"
#include "ppu.h"
#include "timers.h"

class Timers;
class PPU;

class MMU {
   private:
    uint8_t m_wram[8192]{};
    uint8_t m_hram[128]{};

    uint8_t m_ie{};
    uint8_t m_if{};

    Timers& m_timer;
    PPU& m_ppu;
    std::unique_ptr<Cartridge> m_cartridge;

   public:
    MMU(Timers& timer, PPU& ppu, const std::string& path);

    uint8_t read(const uint16_t adress);
    void write(const uint16_t adress, uint8_t data);
    void request_interrupt(const uint8_t interrupt);
};
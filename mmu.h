#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

class Timers;

class MMU {
   private:
    uint8_t m_wram[8192]{};
    uint8_t m_hram[128]{};

    uint8_t m_rom_0[16384]{};
    uint8_t m_rom_switchable[16384]{};
    uint8_t m_rom_save[8192]{};

    uint8_t m_ie{};
    uint8_t m_if{};

    Timers& m_timer;

   public:
    MMU(Timers& timer) : m_timer{timer} {};

    uint8_t read(const uint16_t adress);
    void write(const uint16_t adress, uint8_t data);
    void load_rom(const std::string& rom_name);
    void request_interrupt(const uint8_t interrupt);
};
#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

class MMU {
   private:
    uint8_t m_wram[8192]{};
    uint8_t m_hram[128]{};

    uint8_t m_rom_0[16384]{};
    uint8_t m_rom_switchable[16384]{};
    uint8_t m_rom_save[8192]{};

    uint8_t m_ie{};
    uint8_t m_if{};

   public:
    uint8_t read(uint16_t adress);
    void write(uint16_t adress, uint8_t data);
    void load_rom(const std::string& rom_name);
};
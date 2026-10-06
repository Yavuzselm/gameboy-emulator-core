#pragma once

#include <cstdint>

class MMU {
   private:
    uint8_t m_wram[8192];
    uint8_t m_hram[128];

   public:
    uint8_t read(uint16_t adress);
    void write(uint16_t adress, uint8_t data);
};
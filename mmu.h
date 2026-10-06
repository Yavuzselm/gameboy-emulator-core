#pragma once

#include <cstdint>

class MMU {
   private:
    uint8_t m_ram[8192];

   public:
    uint8_t read(uint16_t adress);
    void write(uint16_t adress, uint8_t data);
};
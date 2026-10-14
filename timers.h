#pragma once

#include <cstdint>

class Timers {
   private:
    uint16_t m_internal_timer{};
    uint8_t m_tac{};
    uint8_t m_tima{};
    uint8_t m_tma{};
    ;

    uint8_t bit_index();

   public:
    void reset();
    bool step_timers(const unsigned int& cycle);

    uint8_t get_div();

    uint8_t read(const uint16_t adress);
    void write(const uint16_t adres, const uint8_t data);
};
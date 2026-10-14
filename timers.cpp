#include "timers.h"

void Timers::reset() { m_internal_timer = 0; }

bool Timers::step_timers(const unsigned int& cycle) {
    bool interrupt = false;
    uint8_t selected_bit = bit_index();
    bool enable = (m_tac & 0x04) != 0;

    for (unsigned int i = 0; i < cycle; i++) {
        bool current_signal = ((m_internal_timer >> selected_bit) & 1) && enable;
        m_internal_timer++;
        bool next_signal = ((m_internal_timer >> selected_bit) & 1) && enable;

        if (current_signal && !next_signal) {
            m_tima++;
            if (m_tima == 0) {
                m_tima = m_tma;
                interrupt = true;
            }
        }
    }

    return interrupt;
}

uint8_t Timers::get_div() { return static_cast<uint8_t>(m_internal_timer >> 8); }

uint8_t Timers::bit_index() {
    switch (m_tac & 0x03) {
        case 0b00:
            return 9;
        case 0b01:
            return 3;
        case 0b10:
            return 5;
        case 0b11:
            return 7;
        default:
            return 9;
    }
}

uint8_t Timers::read(const uint16_t adress) {
    switch (adress) {
        case 0xFF04:
            return get_div();
        case 0xFF05:
            return m_tima;
        case 0xFF06:
            return m_tma;
        case 0xFF07:
            return m_tac | 0xF8;
        default:
            return get_div();
    }
}

void Timers::write(const uint16_t adress, const uint8_t data) {
    switch (adress) {
        case 0xFF04:
            m_internal_timer = 0;
            break;
        case 0xFF05:
            m_tima = data;
            break;
        case 0xFF06:
            m_tma = data;
            break;
        case 0xFF07:
            m_tac = data & 0x07;
            break;
    }
}
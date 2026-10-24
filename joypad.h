#pragma once

#include <SDL3/SDL.h>

class MMU;

class Joypad {
   private:
    uint8_t m_dpad{0xFF};
    uint8_t m_button{0xFF};
    uint8_t m_select{0xFF};

   public:
    void handle_input(bool& is_running);
    uint8_t read();
    void write(uint8_t data);
};

#include "joypad.h"

#include "mmu.h"

void Joypad::handle_input(bool& is_running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                is_running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_W:
                        m_dpad &= ~0x04;
                        break;
                    case SDLK_A:
                        m_dpad &= ~0x02;
                        break;
                    case SDLK_S:
                        m_dpad &= ~0x08;
                        break;
                    case SDLK_D:
                        m_dpad &= ~0x01;
                        break;
                    case SDLK_J:
                        m_button &= ~0x02;
                        break;
                    case SDLK_K:
                        m_button &= ~0x01;
                        break;
                    case SDLK_T:
                        m_button &= ~0x08;
                        break;
                    case SDLK_Y:
                        m_button &= ~0x04;
                        break;
                }
                break;

            case SDL_EVENT_KEY_UP:
                switch (event.key.key) {
                    case SDLK_W:
                        m_dpad |= 0x04;
                        break;
                    case SDLK_A:
                        m_dpad |= 0x02;
                        break;
                    case SDLK_S:
                        m_dpad |= 0x08;
                        break;
                    case SDLK_D:
                        m_dpad |= 0x01;
                        break;
                    case SDLK_J:
                        m_button |= 0x02;
                        break;
                    case SDLK_K:
                        m_button |= 0x01;
                        break;
                    case SDLK_T:
                        m_button |= 0x08;
                        break;
                    case SDLK_Y:
                        m_button |= 0x04;
                        break;
                }
                break;
        }
    }
}

uint8_t Joypad::read() {
    if (m_select == 0x20) {
        return (m_select | (m_dpad & 0x0F)) | 0xC0;
    } else if (m_select == 0x10) {
        return (m_select | (m_button & 0x0F)) | 0xC0;
    } else {
        return m_select | 0x0F | 0xC0;
    }
}

void Joypad::write(uint8_t data) { m_select = data & 0x30; }
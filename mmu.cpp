#include "mmu.h"

uint8_t MMU::read(uint16_t adress) {
    if (adress == 0xFF44) {
        return 0x90;
    }

    if (adress < 0x4000) {
        return m_rom_0[adress];
    }

    else if (adress < 0x8000) {
        return m_rom_switchable[adress - 0x4000];
    }

    else if (adress < 0xA000) {
        return 0xFF;  // vram will implement later
    }

    else if (adress < 0xC000) {
        return m_rom_save[adress - 0xA000];
    }

    else if (adress < 0xE000) {
        return m_wram[adress - 0xC000];
    }

    else if (adress < 0xFE00) {
        return m_wram[adress - 0xE000];
    }

    else if (adress < 0xFEA0) {
        return 0xFF;  // oam -- will implement later
    }

    else if (adress < 0xFF00) {
        return 0xFF;  // unusuable area
    }

    else if (adress < 0xFF80) {  // io registers -- will complete later
        switch (adress) {
            case 0xFF0F:
                return m_if;
                break;
            default:
                return 0xFF;
                break;
        }
    }

    else if (adress < 0xFFFF) {
        return m_hram[adress - 0xFF80];
    }

    else {
        return m_ie;
    }
}

void MMU::write(uint16_t adress, uint8_t data) {
    if (adress == 0xFF01) {
        std::cout << static_cast<char>(data) << std::flush;
    }

    if (adress < 0x4000) {
        return;
    }

    else if (adress < 0x8000) {
        return;
    }

    else if (adress < 0xA000) {
        return;  // vram will implement later
    }

    else if (adress < 0xC000) {
        m_rom_save[adress - 0xA000] = data;
    }

    else if (adress < 0xE000) {
        m_wram[adress - 0xC000] = data;
    }

    else if (adress < 0xFE00) {
        m_wram[adress - 0xE000] = data;
    }

    else if (adress < 0xFEA0) {
        return;  // oam -- will implement later
    }

    else if (adress < 0xFF00) {
        return;  // unusuable area
    }

    else if (adress < 0xFF80) {  // io registers -- will complete later
        switch (adress) {
            case 0xFF0F:
                m_if = data;
                break;
            default:
                return;
                break;
        }
    }

    else if (adress < 0xFFFF) {
        m_hram[adress - 0xFF80] = data;
    }

    else {
        m_ie = data;
    }
}

void MMU::load_rom(const std::string& rom_name) {
    std::ifstream rom(rom_name, std::ios::binary);

    if (!rom) {
        std::cerr << "unable to load the rom";

        return;
    }

    rom.read(reinterpret_cast<char*>(m_rom_0), 16384);

    rom.read(reinterpret_cast<char*>(m_rom_switchable), 16384);

    rom.close();
}
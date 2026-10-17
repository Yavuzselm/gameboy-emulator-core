#include "mmu.h"

#include "ppu.h"
#include "timers.h"

MMU::MMU(Timers& timer, PPU& ppu) : m_timer{timer}, m_ppu{ppu} {};

uint8_t MMU::read(const uint16_t address) {
    if (address < 0x4000) {
        return m_rom_0[address];
    }

    else if (address < 0x8000) {
        return m_rom_switchable[address - 0x4000];
    }

    else if (address < 0xA000) {
        return m_ppu.read_vram(address - 0x8000);
    }

    else if (address < 0xC000) {
        return m_rom_save[address - 0xA000];
    }

    else if (address < 0xE000) {
        return m_wram[address - 0xC000];
    }

    else if (address < 0xFE00) {
        return m_wram[address - 0xE000];
    }

    else if (address < 0xFEA0) {
        return m_ppu.read_oam(address - 0xFE00);
    }

    else if (address < 0xFF00) {
        return 0xFF;  // unusuable area
    }

    else if (address < 0xFF80) {  // io registers -- will complete later
        switch (address) {
            case 0xFF04:
            case 0xFF05:
            case 0xFF06:
            case 0xFF07:
                return m_timer.read(address);

            case 0xFF0F:
                return m_if;  // interrupt rgister

            case 0xFF40:
            case 0xFF41:
            case 0xFF42:
            case 0xFF43:
            case 0xFF44:
            case 0xFF45:
            case 0xFF46:
            case 0xFF47:
            case 0xFF48:
            case 0xFF49:
            case 0xFF4A:
            case 0xFF4B:
                return m_ppu.read_reg(address);
            default:
                return 0xFF;
        }
    }

    else if (address < 0xFFFF) {
        return m_hram[address - 0xFF80];
    }

    else {
        return m_ie;
    }
}

void MMU::write(const uint16_t address, uint8_t data) {
    if (address == 0xFF01) {
        std::cout << static_cast<char>(data) << std::flush;
    }

    if (address < 0x4000) {
        return;
    }

    else if (address < 0x8000) {
        return;
    }

    else if (address < 0xA000) {
        m_ppu.write_vram(address - 0x8000, data);
    }

    else if (address < 0xC000) {
        m_rom_save[address - 0xA000] = data;
    }

    else if (address < 0xE000) {
        m_wram[address - 0xC000] = data;
    }

    else if (address < 0xFE00) {
        m_wram[address - 0xE000] = data;
    }

    else if (address < 0xFEA0) {
        m_ppu.write_oam(address - 0xFE00, data);
    }

    else if (address < 0xFF00) {
        return;  // unusuable area
    }

    else if (address < 0xFF80) {  // io registers -- will complete later
        switch (address) {
            case 0xFF04:
            case 0xFF05:
            case 0xFF06:
            case 0xFF07:
                m_timer.write(address, data);
                break;
            case 0xFF0F:
                m_if = data;
                break;

            case 0xFF40:
            case 0xFF41:
            case 0xFF42:
            case 0xFF43:
            case 0xFF44:
            case 0xFF45:
            case 0xFF47:
            case 0xFF48:
            case 0xFF49:
            case 0xFF4A:
            case 0xFF4B:
                m_ppu.write_reg(address, data);
                break;

            case 0xFF46: {
                m_ppu.write_reg(address, data);

                uint16_t source = data << 8;
                for (uint8_t i{0}; i < 160; i++) {
                    m_ppu.write_oam(i, read(source + i));
                }
                break;
            }

            default:
                return;
                break;
        }
    }

    else if (address < 0xFFFF) {
        m_hram[address - 0xFF80] = data;
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

void MMU::request_interrupt(const uint8_t interrupt) { m_if |= interrupt; }
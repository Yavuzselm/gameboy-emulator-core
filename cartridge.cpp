#include "cartridge.h"

std::unique_ptr<Cartridge> createCartridge(const std::string& path) {
    std::ifstream rom(path, std::ios::binary | std::ios::ate);

    if (!rom) {
        std::cerr << "unable to load the rom";
        return nullptr;
    }

    std::streamsize size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> buffer(size);

    rom.read(reinterpret_cast<char*>(buffer.data()), size);

    if (buffer[0x0147] == 0x00) {
        return std::make_unique<RomOnly>(std::move(buffer));
    }

    else if (buffer[0x0147] == 0x01 || buffer[0x0147] == 0x02 || buffer[0x0147] == 0x03) {
        size_t ram_size = 0;

        switch (buffer[0x0149]) {
            case 0x02:
                ram_size = 8192;
                break;
            case 0x03:
                ram_size = 32768;
                break;
            case 0x04:
                ram_size = 131072;
                break;
            default:
                ram_size = 0;
                break;
        }

        return std::make_unique<Mbc1>(std::move(buffer), ram_size);
    }

    else {
        std::cerr << "Unsupported cartridge type: " << std::hex << static_cast<int>(buffer[0x0147]) << "\n";
        return nullptr;
    }
}

RomOnly::RomOnly(std::vector<uint8_t> rom_data) : m_rom{std::move(rom_data)} {}

uint8_t RomOnly::read_rom(uint16_t address) { return m_rom[address]; }

void RomOnly::write_rom(uint16_t address, uint8_t data) { return; }

uint8_t RomOnly::read_ram(uint16_t address) { return 0xFF; }

void RomOnly::write_ram(uint16_t address, uint8_t data) { return; }

Mbc1::Mbc1(std::vector<uint8_t> rom_data, size_t ram_size) : m_rom{std::move(rom_data)}, m_ram(ram_size, 0) {};

void Mbc1::write_rom(uint16_t address, uint8_t data) {
    if (address < 0x2000) {
        if ((data & 0x0F) == 0x0A) {
            m_ram_enabled = true;
        } else {
            m_ram_enabled = false;
        }
    }

    else if (address < 0x4000) {
        m_rom_bank = data & 0x1F;

        if (m_rom_bank == 0x00) {
            m_rom_bank = 0x01;
        }
    }

    else if (address < 0x6000) {
        m_ram_bank = data & 0x03;

    }

    else {
        m_banking_mode = data & 0x01;
    }
}

uint8_t Mbc1::read_rom(uint16_t address) {
    if (address < 0x4000) {
        return m_rom[address];
    }

    else if (address < 0x8000) {
        uint8_t bank = (m_ram_bank << 5) | m_rom_bank;
        uint32_t offset = (bank * 0x4000) + (address - 0x4000);
        return m_rom[offset % m_rom.size()];
    }

    return 0xFF;
}

uint8_t Mbc1::read_ram(uint16_t address) {
    if (!m_ram_enabled || m_ram.empty()) {
        return 0xFF;
    }

    uint8_t bank = (m_banking_mode == 1) ? m_ram_bank : 0;
    uint32_t offset = (bank * 0x2000) + (address - 0xA000);
    return m_ram[offset % m_ram.size()];
}

void Mbc1::write_ram(uint16_t address, uint8_t data) {
    if (!m_ram_enabled || m_ram.empty()) {
        return;
    }

    uint8_t bank = (m_banking_mode == 1) ? m_ram_bank : 0;
    uint32_t offset = (bank * 0x2000) + (address - 0xA000);
    m_ram[offset % m_ram.size()] = data;
}
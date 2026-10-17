#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Cartridge {
   public:
    virtual ~Cartridge() = default;

    virtual uint8_t read_rom(uint16_t address) = 0;
    virtual void write_rom(uint16_t address, uint8_t data) = 0;
    virtual uint8_t read_ram(uint16_t address) = 0;
    virtual void write_ram(uint16_t address, uint8_t data) = 0;
};

class RomOnly : public Cartridge {
   private:
    std::vector<uint8_t> m_rom;

   public:
    RomOnly(std::vector<uint8_t> rom_data);

    uint8_t read_rom(uint16_t address) override;
    void write_rom(uint16_t address, uint8_t data) override;
    uint8_t read_ram(uint16_t address) override;
    void write_ram(uint16_t address, uint8_t data) override;
};

class Mbc1 : public Cartridge {
   private:
    std::vector<uint8_t> m_rom;
    std::vector<uint8_t> m_ram;

    bool m_ram_enabled = false;
    uint8_t m_rom_bank = 1;
    uint8_t m_ram_bank = 0;
    uint8_t m_banking_mode = 0;

   public:
    Mbc1(std::vector<uint8_t> rom_data, size_t ram_size);
    uint8_t read_rom(uint16_t address) override;
    void write_rom(uint16_t address, uint8_t data) override;
    uint8_t read_ram(uint16_t address) override;
    void write_ram(uint16_t address, uint8_t data) override;
};

std::unique_ptr<Cartridge> createCartridge(const std::string& path);
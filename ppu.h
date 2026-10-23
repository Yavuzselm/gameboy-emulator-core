#pragma once

#include <cstdint>
#include <iostream>

class PPU {
   private:
    uint8_t m_vram[8192]{};
    uint8_t m_oam[160]{};
    unsigned int m_cycle_counter{};
    uint8_t m_ly{};
    uint8_t m_lcdc{};
    uint8_t m_stat{};
    uint8_t m_scy{};
    uint8_t m_scx{};
    uint8_t m_lyc{};
    uint8_t m_bgp{};
    uint8_t m_obp0{};
    uint8_t m_obp1{};
    uint8_t m_wy{};
    uint8_t m_wx{};
    uint8_t m_dma{};

    uint32_t m_framebuffer[23040]{};

    void set_mode();

   public:
    uint8_t read_vram(const uint16_t address);
    void write_vram(const uint16_t address, const uint8_t data);
    uint8_t read_oam(const uint16_t address);
    void write_oam(const uint16_t address, const uint8_t data);
    uint8_t read_reg(const uint16_t address);
    void write_reg(const uint16_t address, const uint8_t data);
    uint8_t step(unsigned int& cycle);
    const uint32_t* get_buffer() const;
    void scan_line();
};
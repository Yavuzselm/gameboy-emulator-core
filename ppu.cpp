#include "ppu.h"

uint8_t PPU::read_vram(const uint16_t address) { return m_vram[address]; }
void PPU::write_vram(const uint16_t address, uint8_t data) { m_vram[address] = data; }

uint8_t PPU::read_oam(const uint16_t address) { return m_oam[address]; }

void PPU::write_oam(const uint16_t address, uint8_t data) { m_oam[address] = data; }

uint8_t PPU::read_reg(const uint16_t address) {
    switch (address) {
        case 0xFF40:
            return m_lcdc;
        case 0xFF41:
            return m_stat;
        case 0xFF42:
            return m_scy;
        case 0xFF43:
            return m_scx;
        case 0xFF44:
            return m_ly;
        case 0xFF45:
            return m_lyc;
        case 0xFF46:
            return m_dma;
        case 0xFF47:
            return m_bgp;
        case 0xFF48:
            return m_obp0;
        case 0xFF49:
            return m_obp1;
        case 0xFF4A:
            return m_wy;
        case 0xFF4B:
            return m_wx;
        default:
            return 0xFF;
    }
}
void PPU::write_reg(const uint16_t address, const uint8_t data) {
    switch (address) {
        case 0xFF40:
            m_lcdc = data;
            break;
        case 0xFF41:
            m_stat = (m_stat & 0x07) | (data & 0xF8);
            break;
        case 0xFF42:
            m_scy = data;
            break;
        case 0xFF43:
            m_scx = data;
            break;
        case 0xFF44:
            m_ly = 0;
            break;
        case 0xFF45:
            m_lyc = data;
            break;
        case 0xFF46:
            m_dma = data;
            break;
        case 0xFF47:
            m_bgp = data;
            break;
        case 0xFF48:
            m_obp0 = data;
            break;
        case 0xFF49:
            m_obp1 = data;
            break;
        case 0xFF4A:
            m_wy = data;
            break;
        case 0xFF4B:
            m_wx = data;
            break;
    }
    return;
}

uint8_t PPU::step(unsigned int& cycle) {
    uint8_t interrupts = 0;

    if ((m_lcdc & 0x80) == 0) {
        m_ly = 0;
        m_cycle_counter = 0;
        return 0;
    }

    m_cycle_counter += cycle;
    set_mode();

    if (m_cycle_counter >= 456) {
        m_cycle_counter -= 456;

        if (m_ly < 144) {
            scan_line();
        }

        m_ly++;

        if (m_ly == 144) {
            interrupts |= 0x01;
        }

        if (m_ly == m_lyc && (m_stat & 0x40)) {
            interrupts |= 0x02;
        }

        if (m_ly == 154) {
            m_ly = 0;
        }
    }

    return interrupts;
}
void PPU::set_mode() {
    uint8_t mode{};

    if (m_ly >= 144) {
        mode = 0b01;
    } else if (m_cycle_counter < 80) {
        mode = 0b10;
    } else if (m_cycle_counter < 252) {
        mode = 0b11;
    } else {
        mode = 0b00;
    }

    m_stat &= 0xF8;
    m_stat |= mode;
}

void PPU::scan_line() {
    bool window_on_this_line = (m_lcdc & 0x20) && (m_ly >= m_wy);

    uint32_t colors[4] = {
        0xFFFFFFFF,  // 0: White
        0xAAAAAAFF,  // 1: Light Gray
        0x555555FF,  // 2: Dark Gray
        0x000000FF   // 3: Black
    };

    for (uint8_t x{}; x < 160; x++) {
        uint8_t tile_col, tile_row, pixel_x, pixel_y;
        uint16_t map_offset;

        if (window_on_this_line && (x + 7 >= m_wx)) {
            uint8_t win_x = x - (m_wx - 7);
            uint8_t win_y = m_ly - m_wy;
            map_offset = (m_lcdc & 0x40) ? 0x1C00 : 0x1800;

            tile_col = win_x / 8;
            tile_row = win_y / 8;
            pixel_x = win_x % 8;
            pixel_y = win_y % 8;
        } else {
            uint8_t bg_x = x + m_scx;
            uint8_t bg_y = m_ly + m_scy;
            map_offset = (m_lcdc & 0x08) ? 0x1C00 : 0x1800;

            tile_col = bg_x / 8;
            tile_row = bg_y / 8;
            pixel_x = bg_x % 8;
            pixel_y = bg_y % 8;
        }

        uint16_t map_index = (tile_row * 32) + tile_col;
        uint8_t tile_id = m_vram[map_offset + map_index];
        uint16_t tile_base_address = 0;

        if (m_lcdc & 0x10) {
            tile_base_address = tile_id * 16;
        } else {
            int8_t signed_id = static_cast<int8_t>(tile_id);
            tile_base_address = 0x1000 + (signed_id * 16);
        }

        uint16_t tile_line_address = tile_base_address + (pixel_y * 2);

        uint8_t color_lsb = m_vram[tile_line_address];
        uint8_t color_msb = m_vram[tile_line_address + 1];

        uint8_t bit_pos = 7 - pixel_x;
        uint8_t low_bit = (color_lsb >> bit_pos) & 1;
        uint8_t high_bit = (color_msb >> bit_pos) & 1;

        uint8_t color_id = (high_bit << 1) | low_bit;

        uint8_t shade = (m_bgp >> (color_id * 2)) & 0x03;

        m_framebuffer[(m_ly * 160) + x] = colors[shade];
    }
    if (!(m_lcdc & 0x02)) {
        return;
    }

    for (int8_t i = 39; i >= 0; i--) {
        uint8_t y_pos = m_oam[(i * 4)];
        uint8_t x_pos = m_oam[(i * 4) + 1];
        uint8_t title_id = m_oam[(i * 4) + 2];
        uint8_t flags = m_oam[(i * 4) + 3];

        int16_t screen_y = y_pos - 16;
        int16_t screen_x = x_pos - 8;

        uint8_t sprite_height = (m_lcdc & 0x04) == 0 ? 8 : 16;

        int ly = m_ly;
        if (ly < screen_y || ly >= (screen_y + sprite_height)) {
            continue;
        }

        uint8_t line_in_sprite = ly - screen_y;

        if (flags & 0x40) {
            line_in_sprite = sprite_height - 1 - line_in_sprite;
        }

        if (sprite_height == 16) {
            title_id &= 0xFE;
        }

        uint16_t tile_adress = (title_id * 16) + (line_in_sprite * 2);

        uint8_t lsb = m_vram[tile_adress];
        uint8_t msb = m_vram[tile_adress + 1];

        for (uint8_t px = 0; px < 8; px++) {
            int16_t pixel_screen_x = screen_x + px;

            if (pixel_screen_x < 0 || pixel_screen_x >= 160) {
                continue;
            }

            uint8_t bit_pos = (flags & 0x20) ? px : 7 - px;

            uint8_t color_id = (((msb >> bit_pos) & 1) << 1) | ((lsb >> bit_pos) & 1);

            if (color_id == 0) {
                continue;
            }

            uint8_t palette = (flags & 0x10) ? m_obp1 : m_obp0;
            uint8_t shade = (palette >> (color_id * 2)) & 0x03;

            m_framebuffer[m_ly * 160 + pixel_screen_x] = colors[shade];
        }
    }
}

const uint32_t* PPU::get_buffer() const { return m_framebuffer; }
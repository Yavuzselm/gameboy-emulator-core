#include "cpuCore.h"

#include "mmu.h"

CpuCore::CpuCore(MMU& mmu) : m_mmu{mmu} {}

uint16_t CpuCore::getAF() const { return (uint16_t(m_registerFile.A) << 8) | m_registerFile.F; }

uint16_t CpuCore::getBC() const { return (uint16_t(m_registerFile.B) << 8) | m_registerFile.C; }

uint16_t CpuCore::getDE() const { return (uint16_t(m_registerFile.D) << 8) | m_registerFile.E; }

uint16_t CpuCore::getHL() const { return (uint16_t(m_registerFile.H) << 8) | m_registerFile.L; }

void CpuCore::setAF(uint16_t value) {
    m_registerFile.A = value >> 8;
    m_registerFile.F = value & 0xF0;
}

void CpuCore::setBC(uint16_t value) {
    m_registerFile.B = value >> 8;
    m_registerFile.C = value & 0xFF;
}

void CpuCore::setDE(uint16_t value) {
    m_registerFile.D = value >> 8;
    m_registerFile.E = value & 0xFF;
}

void CpuCore::setHL(uint16_t value) {
    m_registerFile.H = value >> 8;
    m_registerFile.L = value & 0xFF;
}

uint8_t CpuCore::readReg(uint8_t index) const {
    switch (index) {
        case 0b000:
            return m_registerFile.B;
        case 0b001:
            return m_registerFile.C;
        case 0b010:
            return m_registerFile.D;
        case 0b011:
            return m_registerFile.E;
        case 0b100:
            return m_registerFile.H;
        case 0b101:
            return m_registerFile.L;
        case 0b110:
            return m_mmu.read(getHL());
        case 0b111:
            return m_registerFile.A;
    }
}

void CpuCore::writeReg(uint8_t index, uint8_t value) {
    switch (index) {
        case 0b000:
            m_registerFile.B = value;
            break;
        case 0b001:
            m_registerFile.C = value;
            break;
        case 0b010:
            m_registerFile.D = value;
            break;
        case 0b011:
            m_registerFile.E = value;
            break;
        case 0b100:
            m_registerFile.H = value;
            break;
        case 0b101:
            m_registerFile.L = value;
            break;
        case 0b110:
            m_mmu.write(getHL(), value);
            break;
        case 0b111:
            m_registerFile.A = value;
            break;
    }
}

uint16_t CpuCore::readRP(uint8_t index, bool flag_SP) const {
    switch (index) {
        case 0:
            return getBC();
        case 1:
            return getDE();
        case 2:
            return getHL();
        case 3:
            if (flag_SP) {
                return m_registerFile.SP;
            } else {
                return getAF();
            }
    }
}

void CpuCore::writeRP(uint8_t index, uint16_t value, bool flag_SP) {
    switch (index) {
        case 0:
            setBC(value);
            break;
        case 1:
            setDE(value);
            break;
        case 2:
            setHL(value);
            break;
        case 3:
            if (flag_SP) {
                m_registerFile.SP = value;
            } else {
                setAF(value);
            }
    }
}

void CpuCore::incRP(uint8_t index) {
    switch (index) {
        case 0:
            setBC(getBC() + 1);
            break;
        case 1:
            setDE(getDE() + 1);
            break;
        case 2:
            setHL(getHL() + 1);
            break;
        default:
            m_registerFile.SP++;
    }
}

void CpuCore::decRP(uint8_t index) {
    switch (index) {
        case 0:
            setBC(getBC() - 1);
            break;
        case 1:
            setDE(getDE() - 1);
            break;
        case 2:
            setHL(getHL() - 1);
            break;
        default:
            m_registerFile.SP--;
    }
}

void CpuCore::push(uint16_t value) {
    uint8_t high = (value >> 8) & 0xFF;
    uint8_t low = value & 0xFF;

    m_registerFile.SP--;
    m_mmu.write(m_registerFile.SP, high);
    m_registerFile.SP--;
    m_mmu.write(m_registerFile.SP, low);
}

void CpuCore::pop(uint8_t index) {
    uint16_t low = m_mmu.read(m_registerFile.SP);
    m_registerFile.SP++;
    uint16_t high = m_mmu.read(m_registerFile.SP);
    m_registerFile.SP++;
    writeRP(index, (high << 8) | low, false);
}

bool CpuCore::get_flag_z() const { return (m_registerFile.F & 0x80) != 0; }

bool CpuCore::get_flag_n() const { return (m_registerFile.F & 0x40) != 0; }

bool CpuCore::get_flag_h() const { return (m_registerFile.F & 0x20) != 0; }

bool CpuCore::get_flag_c() const { return (m_registerFile.F & 0x10) != 0; }

void CpuCore::set_flag_z(bool val) {
    if (val)
        m_registerFile.F |= 0x80;
    else
        m_registerFile.F &= ~0x80;
}

void CpuCore::set_flag_n(bool val) {
    if (val)
        m_registerFile.F |= 0x40;
    else
        m_registerFile.F &= ~0x40;
}

void CpuCore::set_flag_h(bool val) {
    if (val)
        m_registerFile.F |= 0x20;
    else
        m_registerFile.F &= ~0x20;
}

void CpuCore::set_flag_c(bool val) {
    if (val)
        m_registerFile.F |= 0x10;
    else
        m_registerFile.F &= ~0x10;
}

void CpuCore::add_a(uint8_t value, bool use_carry) {
    uint8_t carry = (use_carry && get_flag_c()) ? 1 : 0;

    uint16_t result = m_registerFile.A + value + carry;

    set_flag_z((result && 0xFF) == 0);
    set_flag_n(false);
    set_flag_c(result > 0xFF);
    set_flag_h(((m_registerFile.A & 0x0F) + (value & 0x0F) + carry) > 15);

    m_registerFile.A = static_cast<uint8_t>(result);
}

void CpuCore::sub_a(uint8_t value, bool use_carry) {
    uint8_t carry = (use_carry && get_flag_c()) ? 1 : 0;

    int16_t result = m_registerFile.A - value - carry;

    set_flag_z((result & 0xFF) == 0);
    set_flag_n(true);
    set_flag_c(result < 0);
    set_flag_h(((m_registerFile.A & 0x0F) - (value & 0x0F) - carry) < 0);

    m_registerFile.A = static_cast<uint8_t>(result);
}

void CpuCore::cp_a(uint8_t value) {
    int16_t result = m_registerFile.A - value;

    set_flag_z((result & 0xFF) == 0);
    set_flag_n(true);
    set_flag_c(result < 0);
    set_flag_h(((m_registerFile.A & 0x0F) - (value & 0x0F)) < 0);
}

void CpuCore::inc(uint8_t index) {
    uint8_t val = readReg(index);
    uint8_t result = val + 1;

    set_flag_z(result == 0);
    set_flag_n(false);
    set_flag_h((val & 0x0F) == 0x0F);

    writeReg(index, static_cast<uint8_t>(result));
}

void CpuCore::dec(uint8_t index) {
    uint8_t val = readReg(index);
    uint8_t result = val - 1;

    set_flag_z(result == 0);
    set_flag_n(true);
    set_flag_h((val & 0x0F) == 0);

    writeReg(index, static_cast<uint8_t>(result));
}

void CpuCore::and_a(uint8_t value) {
    uint8_t result = m_registerFile.A & value;

    set_flag_z(result == 0);
    set_flag_n(false);
    set_flag_h(true);
    set_flag_c(false);

    m_registerFile.A = result;
}

void CpuCore::or_a(uint8_t value) {
    uint8_t result = m_registerFile.A | value;

    set_flag_z(result == 0);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(0);

    m_registerFile.A = result;
}

void CpuCore::xor_a(uint8_t value) {
    uint8_t result = m_registerFile.A ^ value;

    set_flag_z(result == 0);
    set_flag_n(false);
    set_flag_h(false);
    set_flag_c(false);

    m_registerFile.A = result;
}

bool CpuCore::cc_check(uint8_t index) {
    switch (index) {
        case 0b00:
            return get_flag_z() == false;
        case 0b01:
            return get_flag_z() == true;
        case 0b10:
            return get_flag_c() == false;
        case 0b11:
            return get_flag_c() == true;
    }
}

uint16_t CpuCore::fetch16() {
    uint16_t low = m_mmu.read(m_registerFile.PC);
    m_registerFile.PC++;
    uint16_t high = m_mmu.read(m_registerFile.PC);
    m_registerFile.PC++;

    return (high << 8) | low;
}

void CpuCore::ret_f() {
    uint16_t lsb = m_mmu.read(m_registerFile.SP);
    m_registerFile.SP++;
    uint16_t msb = m_mmu.read(m_registerFile.SP);
    m_registerFile.SP++;
    m_registerFile.PC = (msb << 8) | lsb;
}

void CpuCore::step(unsigned int& cycle) {
    if (m_halted) {
        cycle = 4;
        return;
    }

    m_opCode = m_mmu.read(m_registerFile.PC);
    m_registerFile.PC++;

    uint8_t op_xx{(m_opCode >> 6) & 0x3};   //  6. and 7. bit
    uint8_t op_yyy{(m_opCode >> 3) & 0x7};  // 3. 4. and 5. bit
    uint8_t op_zzz{m_opCode & 0x7};         // 0. 1. and 2. bit
    uint8_t op_q{op_yyy & 0b1};             // 3. bit
    uint8_t op_pp{op_yyy >> 1};             // 4. and 5. bit

    switch (op_xx) {
        case 0b00:
            switch (op_zzz) {
                case 0b000:  // completed except stop instruction
                    switch (op_yyy) {
                        case 0:  // NOP no operation
                            cycle = 4;
                            break;

                        case 1:  // LD [a16], SP
                            m_mmu.write(fetch16(), m_registerFile.SP);
                            cycle = 20;
                            break;

                        case 2:  // STOP
                            m_registerFile.PC++;
                            cycle = 4;
                            break;

                        case 3: {  // JR r8 relative jump
                            int8_t offset = m_mmu.read(m_registerFile.PC);
                            m_registerFile.PC++;
                            m_registerFile.PC += offset;
                            cycle = 12;
                            break;
                        }
                        case 4:
                        case 5:
                        case 6:
                        case 7: {  // JR cc r8 relative jump conditional
                            int8_t offset = static_cast<int8_t>(m_mmu.read(m_registerFile.PC));
                            m_registerFile.PC++;

                            if (cc_check(op_yyy & 0b011)) {
                                m_registerFile.PC += offset;
                                cycle = 12;
                            } else {
                                cycle = 8;
                            }
                            break;
                        }
                    }
                    break;

                case 0b001:
                    if (op_q == 0) {  // q == 0
                        writeRP(op_pp, fetch16(), true);
                        cycle = 12;
                    } else {  // q == 1
                        uint16_t val = readRP(op_pp, true);
                        uint16_t hl = getHL();
                        uint32_t result = static_cast<uint32_t>(val) + static_cast<uint32_t>(hl);

                        set_flag_n(false);
                        set_flag_h(((val & 0x0FFF) + (hl & 0x0FFF)) > 0x0FFF);
                        set_flag_c(result > 0xFFFF);

                        setHL(static_cast<uint16_t>(result));
                        cycle = 8;
                    }
                    break;

                case 0b010:
                    cycle = 8;

                    if (op_q == 1) {  // Read from memory into A (LD A, [..])
                        switch (op_pp) {
                            case 0:
                            case 1:
                                m_registerFile.A = m_mmu.read(readRP(op_pp, true));
                                break;
                            case 2:
                                m_registerFile.A = m_mmu.read(getHL());
                                incRP(2);
                                break;
                            case 3:
                                m_registerFile.A = m_mmu.read(getHL());
                                decRP(2);
                                break;
                        }
                    } else {  // Write A into memory (LD [..], A)
                        switch (op_pp) {
                            case 0:
                            case 1:
                                m_mmu.write(readRP(op_pp, true), m_registerFile.A);
                                break;
                            case 2:
                                m_mmu.write(getHL(), m_registerFile.A);
                                incRP(2);
                                break;
                            case 3:
                                m_mmu.write(getHL(), m_registerFile.A);
                                decRP(2);
                                break;
                        }
                    }
                    break;

                case 0b011:
                    cycle = 8;
                    if (op_q) {
                        incRP(op_pp);
                    } else {
                        decRP(op_pp);
                    }
                    break;

                case 0b100:
                    cycle = (op_yyy == 0b110) ? 12 : 4;
                    inc(op_yyy);
                    break;

                case 0b101:
                    cycle = (op_yyy == 0b110) ? 12 : 4;
                    dec(op_yyy);
                    break;

                case 0b110:  // ld r, n
                    writeReg(op_yyy, m_mmu.read(m_registerFile.PC));
                    m_registerFile.PC++;

                    if (op_yyy == 0b110)
                        cycle = 12;
                    else
                        cycle = 8;
                    break;

                case 0b111:
                    cycle = 4;
                    switch (op_yyy) {
                        case 0b000: {  // RLCA
                            uint8_t b7 = m_registerFile.A >> 7;
                            m_registerFile.A = (m_registerFile.A << 1) | b7;

                            set_flag_z(false);
                            set_flag_n(false);
                            set_flag_h(false);
                            set_flag_c(b7 != 0);
                            break;
                        }

                        case 0b001: {  // RRCA

                            uint8_t b0 = m_registerFile.A & 0x01;
                            m_registerFile.A = (m_registerFile.A >> 1) | (b0 << 7);

                            set_flag_z(false);
                            set_flag_n(false);
                            set_flag_h(false);
                            set_flag_c(b0 != 0);
                            break;
                        }

                        case 0b010: {  // RLA
                            uint8_t carry = get_flag_c() ? 1 : 0;
                            uint8_t b7 = m_registerFile.A >> 7;
                            m_registerFile.A = (m_registerFile.A << 1) | carry;

                            set_flag_z(false);
                            set_flag_n(false);
                            set_flag_h(false);
                            set_flag_c(b7 != 0);
                            break;
                        }

                        case 0b011: {  // RRA
                            uint8_t carry = get_flag_c() ? 1 : 0;
                            uint8_t b0 = m_registerFile.A & 0x01;
                            m_registerFile.A = (m_registerFile.A >> 1) | (carry << 7);

                            set_flag_z(false);
                            set_flag_n(false);
                            set_flag_h(false);
                            set_flag_c(b0 != 0);
                            break;
                        }

                        case 0b100:  // DAA
                            if (!get_flag_n()) {
                                if (get_flag_c() || m_registerFile.A > 0x99) {
                                    m_registerFile.A += 0x60;
                                    set_flag_c(true);
                                }
                                if (get_flag_h() || (m_registerFile.A & 0x0F) > 0x09) {
                                    m_registerFile.A += 0x06;
                                }
                            } else {
                                if (get_flag_c()) {
                                    m_registerFile.A -= 0x60;
                                }
                                if (get_flag_h()) {
                                    m_registerFile.A -= 0x06;
                                }
                            }

                            set_flag_z(m_registerFile.A == 0);
                            set_flag_h(false);
                            break;

                        case 0b101:  // CPL
                            m_registerFile.A ^= 0xFF;
                            set_flag_n(true);
                            set_flag_h(true);
                            break;

                        case 0b110:  // SCF
                            set_flag_n(false);
                            set_flag_h(false);
                            set_flag_c(true);
                            break;

                        case 0b111:  // CCF
                            set_flag_n(false);
                            set_flag_h(false);
                            set_flag_c(!get_flag_c());
                            break;
                    }
                    break;
            }
            break;

        case 0b01:
            if (op_yyy == 0b110 && op_zzz == 0b110) {  // HALT
                m_halted = true;
                cycle = 4;
                break;
            }

            // ld r , r'
            writeReg(op_yyy, readReg(op_zzz));
            if (op_zzz == 0b110 || op_yyy == 0b110)  // hl access takes 2 m cycle
                cycle = 8;
            else
                cycle = 4;
            break;

        case 0b10:  // 8 bit alu instrucitons

            uint8_t alu_val = readReg(op_zzz);
            cycle = (op_zzz == 0b110) ? 8 : 4;

            switch (op_yyy) {
                case 0:
                    add_a(alu_val, false);
                    break;
                case 1:
                    add_a(alu_val, true);
                    break;
                case 2:
                    sub_a(alu_val, false);
                    break;
                case 3:
                    sub_a(alu_val, true);
                    break;
                case 4:
                    and_a(alu_val);
                    break;
                case 5:
                    xor_a(alu_val);
                    break;
                case 6:
                    or_a(alu_val);
                    break;
                case 7:
                    cp_a(alu_val);
                    break;
            }
            break;

        case 0b11:
            switch (op_zzz) {
                case 0b000:
                    switch (op_yyy) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            if (cc_check(op_yyy & 0b011)) {  // RET cc[y]
                                ret_f();
                                cycle = 20;
                            } else {
                                cycle = 8;
                            }
                            break;

                        case 4: {  // LDH (n), A: Load from accumulator
                            uint16_t adress = 0xFF00 | static_cast<uint16_t>(m_mmu.read(m_registerFile.PC));
                            m_registerFile.PC++;
                            m_mmu.write(adress, m_registerFile.A);
                            cycle = 12;
                            break;
                        }

                        case 5: {  // ADD SP, e: Add to stack pointer (relative)
                            uint8_t raw_byte = m_mmu.read(m_registerFile.PC);
                            int8_t offset = static_cast<int8_t>(raw_byte);
                            m_registerFile.PC++;

                            set_flag_z(false);
                            set_flag_n(false);
                            set_flag_h(((m_registerFile.SP & 0x0F) + (raw_byte & 0x0F)) > 0x0F);
                            set_flag_c(((m_registerFile.SP & 0xFF) + (raw_byte & 0xFF)) > 0xFF);

                            m_registerFile.SP = static_cast<uint16_t>(m_registerFile.SP + offset);

                            cycle = 16;
                            break;
                        }
                        case 6:  // LDH A, (n): Load accumulator (direct 0xFF00+n)
                            writeReg(0b111, m_mmu.read(0xFF00 + m_mmu.read(m_registerFile.PC)));
                            m_registerFile.PC++;
                            cycle = 12;
                            break;

                        case 7: {  // LD HL, SP+e: Load HL from adjusted stack pointer
                            uint8_t raw_byte = m_mmu.read(m_registerFile.PC);
                            int8_t offset = static_cast<int8_t>(raw_byte);
                            m_registerFile.PC++;
                            setHL(static_cast<uint16_t>(m_registerFile.SP + offset));

                            set_flag_z(false);
                            set_flag_n(false);
                            set_flag_h(((m_registerFile.SP & 0x0F) + (raw_byte & 0x0F)) > 0x0F);
                            set_flag_c(((m_registerFile.SP & 0xFF) + (raw_byte & 0xFF)) > 0xFF);

                            cycle = 12;
                            break;
                        }
                    }
                    break;
                case 0b001:
                    if (op_q == 0) {
                        pop(op_pp);
                        cycle = 12;

                    } else {
                        switch (op_pp) {
                            case 0: {  // RET
                                ret_f();
                                cycle = 16;
                                break;
                            }

                            case 1: {  // RETI
                                ret_f();
                                m_ime = true;
                                cycle = 16;
                                break;
                            }

                            case 2: {  // JP HL
                                m_registerFile.PC = getHL();
                                cycle = 4;
                                break;
                            }

                            case 3: {  // LD SP, HL
                                m_registerFile.SP = getHL();
                                cycle = 8;
                                break;
                            }
                        }
                    }
                    break;

                case 0b010:
                    switch (op_yyy) {
                        case 0:
                        case 1:
                        case 2:
                        case 3: {  // JP cc[y], a16
                            uint16_t nn = fetch16();

                            if (cc_check(op_yyy & 0b011)) {
                                m_registerFile.PC = nn;
                                cycle = 16;
                            } else {
                                cycle = 12;
                            }
                            break;
                        }

                        case 4:
                            m_mmu.write(0xFF00 + m_registerFile.C, m_registerFile.A);
                            cycle = 8;
                            break;

                        case 5:
                            m_mmu.write(fetch16(), m_registerFile.A);
                            cycle = 16;
                            break;
                        case 6:
                            writeReg(0b111, m_mmu.read(0xFF00 + m_registerFile.C));
                            cycle = 8;
                            break;
                        case 7:
                            writeReg(0b111, m_mmu.read(fetch16()));
                            cycle = 16;
                            break;
                    }
                    break;

                case 0b011:
                    switch (op_yyy) {
                        case 0:
                            m_registerFile.PC = fetch16();
                            cycle = 16;
                            break;
                        case 1:
                            step_CB(cycle);
                            break;
                        case 2:  // illegal unmapped
                        case 3:
                        case 4:
                        case 5:
                            break;
                        case 6:
                            m_ime = false;
                            cycle = 4;
                            break;
                        case 7:
                            m_ime = true;
                            cycle = 4;
                            break;
                    }
                    break;

                case 0b100:
                    switch (op_yyy) {
                        case 0:
                        case 1:
                        case 2:
                        case 3: {
                            uint16_t nn = fetch16();

                            if (cc_check(op_yyy & 0b011)) {
                                push(m_registerFile.PC);
                                m_registerFile.PC = nn;
                                cycle = 24;
                            } else {
                                cycle = 12;
                            }
                            break;
                        }
                        case 4:
                        case 5:
                        case 6:
                        case 7:  // illegal unmapped
                            break;
                    }
                    break;

                case 0b101:
                    if (op_q == 0) {
                        push(readRP(op_pp, false));
                        cycle = 16;
                        break;
                    } else {
                        switch (op_pp) {
                            case 0: {
                                uint16_t nn = fetch16();
                                push(m_registerFile.PC);
                                m_registerFile.PC = nn;
                                cycle = 24;
                                break;
                            }
                            case 1:  // unmapped -- illegal
                            case 2:
                            case 3:
                                break;
                        }
                    }
                    break;

                case 0b110: {
                    uint8_t alu_imm_val = m_mmu.read(m_registerFile.PC);
                    m_registerFile.PC++;
                    cycle = 8;

                    switch (op_yyy) {
                        case 0:
                            add_a(alu_imm_val, false);
                            break;
                        case 1:
                            add_a(alu_imm_val, true);
                            break;
                        case 2:
                            sub_a(alu_imm_val, false);
                            break;
                        case 3:
                            sub_a(alu_imm_val, true);
                            break;
                        case 4:
                            and_a(alu_imm_val);
                            break;
                        case 5:
                            xor_a(alu_imm_val);
                            break;
                        case 6:
                            or_a(alu_imm_val);
                            break;
                        case 7:
                            cp_a(alu_imm_val);
                            break;
                    }
                } break;

                case 0b111:
                    push(m_registerFile.PC);
                    m_registerFile.PC = op_yyy * 8;
                    cycle = 16;
                    break;
            }
            break;
    }
}

void CpuCore::step_CB(unsigned int& cycle) {
    m_opCode = m_mmu.read(m_registerFile.PC);
    m_registerFile.PC++;

    uint8_t op_xx{(m_opCode >> 6) & 0x3};   //  6. and 7. bit
    uint8_t op_yyy{(m_opCode >> 3) & 0x7};  // 3. 4. and 5. bit
    uint8_t op_zzz{m_opCode & 0x7};

    switch (op_xx) {
        uint8_t val = readReg(op_zzz);

        case 0:
            uint8_t result{};
            cycle = (op_zzz == 6) ? 16 : 8;
            switch (op_yyy) {
                case 0: {  // RLC
                    uint8_t b7 = val >> 7;
                    result = (val << 1) | b7;
                    set_flag_c(b7 != 0);
                    break;
                }
                case 1: {  // RRC
                    uint8_t b0 = val & 0x01;
                    result = (val >> 1) | (b0 << 7);
                    set_flag_c(b0 != 0);
                    break;
                }
                case 2: {  // RL
                    uint8_t b7 = val >> 7;
                    uint8_t old_carry = get_flag_c() ? 1 : 0;
                    result = (val << 1) | old_carry;
                    set_flag_c(b7 != 0);
                    break;
                }
                case 3: {  // RR
                    uint8_t b0 = val & 0x01;
                    uint8_t old_carry = get_flag_c() ? 1 : 0;
                    result = (val >> 1) | (old_carry << 7);
                    set_flag_c(b0 != 0);
                    break;
                }
                case 4: {  // SLA
                    uint8_t b7 = val >> 7;
                    result = (val << 1);
                    set_flag_c(b7 != 0);
                    break;
                }
                case 5: {  // SRA
                    uint8_t b0 = val & 0x01;
                    uint8_t b7 = val & 0x80;
                    result = (val >> 1) | (b7);
                    set_flag_c(b0 != 0);
                    break;
                }
                case 6: {  // SWAP
                    uint8_t high = (val & 0xF0) >> 4;
                    uint8_t low = (val & 0x0F) << 4;
                    result = low | high;
                    set_flag_c(false);
                    break;
                }
                case 7: {  // SRL
                    uint8_t b0 = val & 0x01;
                    result = (val >> 1);
                    set_flag_c(b0 != 0);
                    break;
                }
            }
            set_flag_z(result == 0);
            set_flag_n(false);
            set_flag_h(false);
            writeReg(op_zzz, result);
            break;
        case 1:  // BIT
            set_flag_z((val & (1 << op_yyy)) == 0);
            set_flag_n(false);
            set_flag_h(true);
            cycle = (op_zzz == 6) ? 12 : 8;
            break;
        case 2:  // RES
            writeReg(op_zzz, val & ~(1 << op_yyy));
            cycle = (op_zzz == 6) ? 16 : 8;
            break;
        case 3:  // SET
            writeReg(op_zzz, val | (1 << op_yyy));
            cycle = (op_zzz == 6) ? 16 : 8;
            break;
    }
}
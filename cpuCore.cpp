#include "cpuCore.h"

#include "mmu.h"

CpuCore::CpuCore(MMU& mmu) : m_mmu{mmu} {}

uint16_t CpuCore::getAF() const {
    return (uint16_t(m_registerFile.A) << 8) | m_registerFile.F;
}

uint16_t CpuCore::getBC() const {
    return (uint16_t(m_registerFile.B) << 8) | m_registerFile.C;
}

uint16_t CpuCore::getDE() const {
    return (uint16_t(m_registerFile.D) << 8) | m_registerFile.E;
}

uint16_t CpuCore::getHL() const {
    return (uint16_t(m_registerFile.H) << 8) | m_registerFile.L;
}

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

uint16_t CpuCore::readRP(uint8_t index) const {
    switch (index) {
        case 0:
            return getBC();
        case 1:
            return getDE();
        case 2:
            return getHL();
        default:
            return m_registerFile.SP;
    }
}

void CpuCore::writeRP(uint8_t index, uint16_t value) {
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
        default:
            m_registerFile.SP = value;
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

uint16_t CpuCore::fetch16() {
    uint16_t low = m_mmu.read(m_registerFile.PC++);
    uint16_t high = m_mmu.read(m_registerFile.PC++);
    return (high << 8) | low;
}

void CpuCore::step(unsigned int& cycle) {
    m_opCode = m_mmu.read(m_registerFile.PC);
    m_registerFile.PC++;

    uint8_t op_xx{(m_opCode >> 6) && 0x3};   //  6. and 7. bit
    uint8_t op_yyy{(m_opCode >> 3) && 0x7};  // 3. 4. and 5. bit
    uint8_t op_zzz{m_opCode && 0x7};         // 0. 1. and 2. bit
    uint8_t op_q{op_yyy & 0b1};              // 3. bit
    uint8_t op_pp{op_yyy >> 1};              // 4. and 5. bit

    switch (op_xx) {
        case 0b01:  // ld r , r'
            writeReg(op_yyy, readReg(op_zzz));
            if (op_zzz == 0b110 ||
                op_yyy == 0b110)  // hl access takes 2 m cycle
                cycle = 8;
            else
                cycle = 4;
            break;

        case 0b00:
            switch (op_zzz) {
                case 0b110:  // ld r, n
                    writeReg(op_yyy, m_mmu.read(m_registerFile.PC));
                    m_registerFile.PC++;

                    if (op_yyy == 0b110)
                        cycle = 12;
                    else
                        cycle = 8;
                    break;
                case 0b010:
                    if (op_q) {  // load A, registerPair
                        switch (op_pp) {
                            case 0:
                            case 1:
                                writeReg(0b111, m_mmu.read(readRP(op_pp)));
                                break;
                            case 2:
                                writeReg(0b111, m_mmu.read(readRP(0b10)));
                                incRP(2);
                                break;
                            case 3:
                                writeReg(0b111, m_mmu.read(readRP(0b10)));
                                decRP(2);
                                break;
                        }
                    } else {  // load registerpair, A
                        // m_mmu.write(readRP(op_yy >> 1), m_registerFile.A);
                        switch (op_pp) {
                            case 0:
                            case 1:
                                m_mmu.write(readRP(op_pp), m_registerFile.A);
                                break;
                            case 2:
                                m_mmu.write(readRP(0b10), m_registerFile.A);
                                incRP(2);
                                break;
                            case 3:
                                m_mmu.write(readRP(0b10), m_registerFile.A);
                                decRP(2);
                                break;
                        }
                    }
                    cycle = 8;
                    break;
            }
            break;
        case 0b11:
            switch (m_opCode) {
                case 0xFA:
                    writeReg(0b111, m_mmu.read(fetch16()));
                    cycle = 16;
                    break;
                case 0xEA:
                    m_mmu.write(fetch16(), m_registerFile.A);
                    cycle = 16;
                    break;
                case 0xF2:
                    writeReg(0b111, m_mmu.read(0xFF00 + m_registerFile.C));
                    m_registerFile.PC++;
                    cycle = 8;
                    break;
                case 0xE2:
                    m_mmu.write(0xFF00 + m_registerFile.C, m_registerFile.A);
                    m_registerFile.PC++;
                    cycle = 8;
                    break;
                case 0xF0:
                    writeReg(0b111, m_mmu.read(0xFF00 +
                                               m_mmu.read(m_registerFile.PC)));
                    m_registerFile.PC++;
                    cycle = 12;
                    break;
                case 0xE0:
                    m_mmu.write(0xFF00 + m_registerFile.C, m_registerFile.A);
                    m_registerFile.PC++;
                    cycle = 12;
            }
            break;
    }
}
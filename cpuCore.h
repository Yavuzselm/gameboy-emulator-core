#pragma once

#include <cstdint>

class MMU;  // forward declaration for MMU class to pass it to step function

struct RegisterFile {
    uint16_t PC{};
    uint16_t SP{};
    uint8_t A{};
    uint8_t B{};
    uint8_t C{};
    uint8_t D{};
    uint8_t E{};
    uint8_t F{};
    uint8_t H{};
    uint8_t L{};
    uint8_t IR{};
    uint8_t IE{};

    // bool Z{};  maybe i can use them instead of F register
    // bool N{};
    // bool H{};
    // bool C{};
};

class CpuCore {
   private:
    RegisterFile m_registerFile{};
    uint8_t m_opCode{};
    MMU& m_mmu;

   public:
    CpuCore(MMU& mmu);

    uint16_t getBC() const;
    uint16_t getDE() const;
    uint16_t getHL() const;
    uint16_t getAF() const;

    void setBC(uint16_t value);
    void setDE(uint16_t value);
    void setHL(uint16_t value);
    void setAF(uint16_t value);

    uint8_t readReg(uint8_t index) const;
    uint16_t readRP(uint8_t index) const;
    void writeReg(uint8_t index, uint8_t value);
    void writeRP(uint8_t index, uint16_t value);
    void incRP(uint8_t index);
    void decRP(uint8_t index);

    uint16_t fetch16();

    void step(unsigned int& cycle);
};
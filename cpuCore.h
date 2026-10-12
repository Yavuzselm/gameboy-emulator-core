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
};

class CpuCore {
   private:
    RegisterFile m_registerFile{};
    bool m_ime{};
    bool m_halted{};
    uint8_t m_opCode{};
    MMU& m_mmu;

   public:
    CpuCore(MMU& mmu);

    // register file functions
    uint16_t getBC() const;
    uint16_t getDE() const;
    uint16_t getHL() const;
    uint16_t getAF() const;

    void setBC(uint16_t value);
    void setDE(uint16_t value);
    void setHL(uint16_t value);
    void setAF(uint16_t value);

    uint8_t readReg(uint8_t index) const;
    uint16_t readRP(uint8_t index, bool flag_SP) const;
    void writeReg(uint8_t index, uint8_t value);
    void writeRP(uint8_t index, uint16_t value, bool flag_SP);

    void push(uint16_t value);
    void pop(uint8_t index);

    // flag functions
    bool get_flag_z() const;
    bool get_flag_n() const;
    bool get_flag_h() const;
    bool get_flag_c() const;

    void set_flag_z(bool val);
    void set_flag_n(bool val);
    void set_flag_h(bool val);
    void set_flag_c(bool val);

    bool cc_check(uint8_t index);

    // 8 bit arithmetic functions

    void incRP(uint8_t index);
    void decRP(uint8_t index);
    void add_a(uint8_t value, bool use_carry);
    void sub_a(uint8_t vale, bool use_carry);
    void cp_a(uint8_t value);
    void inc(uint8_t index);
    void dec(uint8_t index);
    void and_a(uint8_t value);
    void or_a(uint8_t value);
    void xor_a(uint8_t value);

    uint16_t fetch16();
    void ret_f();

    void step(unsigned int& cycle);
    void step_CB(unsigned int& cycle);
    void handle_interrupt(unsigned int& cycle);
};
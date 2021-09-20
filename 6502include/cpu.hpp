#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <iostream>
#include "mem.hpp"

struct CPU {
    Word PC;    // Program counter - 16 bit register that points to next
                // address on the stack to be executed.
    
    Byte SP;    // Stack pointer - 8 bit register that points to next
                // free location on the stack.

    Byte A;     // Accululator - 8 bit register used for all arithmetic
                // and logic operations.

    Byte X;     // Index Register X - 8 bit register for offsets or counters
                // for accessing memory.

    Byte Y;     // Index Reigster Y - 8 bit register for counter or offset
                // memory addresses

    // Processor status flags: only 1 bit long and are technically supposed to
    // be in a single byte. In order these flags are: the carry flag, the zero
    // flag, the interrupt disable flag, the decimal mode flag, the break command
    // flag, the overflow flag, and the negative flag.
    Byte C : 1; 
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    // Read byte from memory, increment program counter and decrement nCycles
    Byte FetchByte(unsigned int &nCycles, Mem &mem);

    // Same thing as FetchByte but don't increment program counter
    Byte ReadByte(unsigned int &nCycles, Word addr, Mem &mem);

    // Fetch word from memory, increment program counter and decrement cycles
    Word FetchWord(unsigned int &nCycles, Mem &mem);

    // Same thing as FetchWord but don't increment program counter
    Word ReadWord(unsigned int &nCycles, Word addr, Mem &mem);

    // Write word and byte
    void WriteWord(Word dta, unsigned int addr, unsigned int &cycles, Mem &mem);
    void WriteByte(Byte data, unsigned int addr, unsigned int &nCycles, Mem &mem);

    // Write register to memory
    void WriteToMemFromRegister(Byte &reg, Word addr, unsigned int& nCycles, Mem &mem);

    // Write to register using value
    void WriteRegister(Byte &reg, Byte value);

    // Execute instruction based on PC location
    void Execute(unsigned int nCycles, Mem &mem);

    // Reset everything to default status
    void Reset(Mem &mem);

    // Debugging function
    void debugReport();

    // Status flag update after function call (where necessary)
    void UpdateZeroAndNegativeFlags(Byte &reg);

    // Addressing modes
    Byte AddressingZeroPage(unsigned int &nCycles, Mem &mem);
    Byte AddressingZeroPageX(unsigned int &nCycles, Mem &mem);
    Byte AddressingZeroPageY(unsigned int &nCycles, Mem &mem);
    Word AddressingAbsolute(unsigned int &nCycles, Mem &mem);
    Word AddressingAbsoluteX(unsigned int &nCycles, Mem &mem);
    Word AddressingAbsoluteY(unsigned int &nCycles, Mem &mem);
    Word AddressingIndirect(unsigned int &nCycles, Mem &mem);
    Word AddressingIndexedIndirect(unsigned int &nCycles, Mem &mem);
    Word AddressingIndirectIndexed(unsigned int &nCycles, Mem &mem);

    // Stack operations
    Word SPToAddr();
    Byte PopByte(unsigned int &nCycles, Mem &mem);
    Word PopWord(unsigned int &nCycles, Mem &mem);
    void PushByte(unsigned int &nCycles, Byte val, Mem &mem);
    void PushWord(unsigned int &nCycles, Word value, Mem &mem);

    void TransferRegister(Byte &src, Byte &dest);

    // Arithmetic instructions
    void lAND(Byte val);
    void EOR(Byte val);

    /*
     * Immediate: load next byte as the "argument" to the instruction.
     * Zero Page: load next byte as the address (in the zero page) to the
     *            "argument" of the instruction (a.k.a next byte = address
     *            to read from for the argument.)
     */
    static const Byte
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_AB = 0xAD,
        INS_LDA_ABX = 0xBD,
        INS_LDA_ABY = 0xB9,
        INS_LDA_IDX = 0xA1,
        INS_LDA_IDY = 0xB1,

        INS_LDX_IM = 0xA2,
        INS_LDX_ZP = 0xA6,
        INS_LDX_ZPY = 0xB6,
        INS_LDX_AB = 0xAE,
        INS_LDX_ABY = 0xBE,

        INS_STA_ZP = 0x85,
        INS_STA_ZPX = 0x95,
        INS_STA_AB = 0x8D,
        INS_STA_ABX = 0x9D,
        INS_STA_ABY = 0x99,
        INS_STA_IDX = 0x81,
        INS_STA_IDY = 0x91,

        INS_STX_ZP = 0x86,
        INS_STX_ZPY = 0x96,
        INS_STX_AB = 0x8E,

        INS_STY_ZP = 0x84,
        INS_STY_ZPX = 0x94,
        INS_STY_AB = 0x8C,

        INS_JSR = 0x20,
        INS_RTS = 0x60,

        INS_ADC_IM = 0x69,
        INS_ADC_ZP = 0x65,
        INS_ADC_ZPX = 0x75,
        INS_ADC_AB = 0x6D,
        INS_ADC_ABX = 0x7D,
        INS_ADC_ABY = 0x79,
        INS_ADC_IDX = 0x61,
        INS_ADC_IDY = 0x71,

        INS_TAX = 0xAA,
        INS_TAY = 0xA8,
        INS_TSX = 0xBA,
        INS_TXA = 0x8A,
        INS_TXS = 0x9A,
        INS_TYA = 0x98,

        INS_AND_IM = 0x29,
        INS_AND_ZP = 0x25,
        INS_AND_ZPX = 0x35,
        INS_AND_AB = 0x2D,
        INS_AND_ABX = 0x3D,
        INS_AND_ABY = 0x39,
        INS_AND_IDX = 0x21,
        INS_AND_IDY = 0x31,

        INS_EOR_IM = 0x49,
        INS_EOR_ZP = 0x45,
        INS_EOR_ZPX = 0x55,
        INS_EOR_AB = 0x4D,
        INS_EOR_ABX = 0x5D,
        INS_EOR_ABY = 0x59,
        INS_EOR_IDX = 0x41,
        INS_EOR_IDY = 0x51,

        INS_ORA_IM = 0x09,
        INS_ORA_ZP = 0x05,
        INS_ORA_ZPX = 0x15,
        INS_ORA_AB = 0x0D,
        INS_ORA_ABX = 0x1D,
        INS_ORA_ABY = 0x19,
        INS_ORA_IDX = 0x01,
        INS_ORA_IDY = 0x11,

        INS_BIT_ZP = 0x24,
        INS_BIT_AB = 0x2C,

        INS_INC_ZP = 0xE6,
        INS_INC_ZPX = 0xF6,
        INS_INC_AB = 0xEE,
        INS_INC_ABX = 0xFE,

        INS_INX = 0xE8,

        INS_INY = 0xC8,

        INS_DEC_ZP = 0xC6,
        INS_DEC_ZPX = 0xD6,
        INS_DEC_AB = 0xCE,
        INS_DEC_ABX = 0xDE,
        INS_DEX = 0xCA,
        INS_DEY = 0x88,

        INS_ASL_ACC = 0x0A,
        INS_ASL_ZP = 0x06,
        INS_ASL_ZPX = 0x16,
        INS_ASL_AB = 0x0E,
        INS_ASL_ABX = 0x1E,

        INS_LSR_ACC = 0x4A,
        INS_LSR_ZP = 0x46,
        INS_LSR_ZPX = 0x56,
        INS_LSR_AB = 0x4E,
        INS_LSR_ABX = 0x5E,

        INS_ROL_ACC = 0x2A,
        INS_ROL_ZP = 0x26,
        INS_ROL_ZPX = 0x36,
        INS_ROL_AB = 0x2E,
        INS_ROL_ABX = 0x3E,

        INS_ROR_ACC = 0x6A,
        INS_ROR_ZP = 0x66,
        INS_ROR_ZPX = 0x76,
        INS_ROR_AB = 0x6E,
        INS_ROR_ABX = 0x7E,

        INS_CLC = 0x18,
        INS_CLD = 0xD8,
        INS_CLI = 0x58,
        INS_CLV = 0xB8,
        INS_SEC = 0x38,
        INS_SED = 0xF8,
        INS_SEI = 0x78,


        INS_JMP_AB = 0x4C,
        INS_JMP_ID = 0x6C
        ;

};

#endif

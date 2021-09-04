#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <iostream>
#include "mem.hpp"

struct CPU {
    Word PC;    // Program counter - 16 bit register that points to next
                // address on the stack to be executed.
    
    Word SP;    // Stack pointer - 8 bit register that points to next
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

    // Execute instruction based on PC location
    void Execute(unsigned int nCycles, Mem &mem);

    // Reset everything to default status
    void Reset(Mem &mem);

    // Debugging function
    void debugReport();

    // Status flag update after function call (where necessary)
    void LDAStatusUpdate();
    void LDXStatusUpdate();

    // Addressing modes
    Byte AddressingZeroPage(unsigned int &nCycles, Mem &mem);
    Byte AddressingZeroPageX(unsigned int &nCycles, Mem &mem);
    Byte AddressingZeroPageY(unsigned int &nCycles, Mem &mem);
    Byte AddressingRelative(unsigned int &nCycles, Mem &mem);
    Word AddressingAbsolute(unsigned int &nCycles, Mem &mem);
    Word AddressingAbsoluteX(unsigned int &nCycles, Mem &mem);
    Word AddressingAbsoluteY(unsigned int &nCycles, Mem &mem);
    Word AddressingIndirect(unsigned int &nCycles, Mem &mem);
    Word AddressingIndexedIndirect(unsigned int &nCycles, Mem &mem);
    Word AddressingIndirectIndexed(unsigned int &nCycles, Mem &mem);

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

        INS_RTS = 0x60
        ;

};

#endif
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
    Byte O : 1;
    Byte N : 1;

    // Read byte from memory, increment program counter and decrement nCycles
    Byte FetchByte(unsigned int &nCycles, Mem &mem) {
        Byte ins = mem[PC];
        PC++;
        nCycles--;
        return ins;
    }

    // Same thing as FetchByte but don't increment program counter
    Byte ReadByte(unsigned int &nCycles, Byte addr, Mem &mem) {
        Byte ins = mem[addr];
        nCycles--;
        return ins;
    }

    // Fetch word from memory, increment program counter and decrement cycles
    Word FetchWord(unsigned int &nCycles, Mem &mem) {
        // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
        // BIT COMES FIRST.
        Word Data = mem[PC];
        PC++;
        nCycles--;

        Data |= (mem[PC] << 8);
        PC++;
        nCycles--;

        // Swap lower and higher bits if your platform is not low endian
        // x86_64 is little endian, so I don't need to swap the bytes around
        // luckily, but eventually add test here to make this platform agnostic
        // Note to future self: you can just do the previous steps in reverse 
        // for big endian systems.

        return Data;
    }

    // Same thing as FetchWord but don't increment program counter
    Word ReadWord(unsigned int &nCycles, Byte addr, Mem &mem) {
        // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
        // BIT COMES FIRST.
        Word Data = mem[addr];

        Data |= (mem[addr] << 8);
        nCycles-=2;

        // Swap lower and higher bits if your platform is not low endian
        // x86_64 is little endian, so I don't need to swap the bytes around
        // luckily, but eventually add test here to make this platform agnostic
        // Note to future self: you can just do the previous steps in reverse 
        // for big endian systems.

        return Data;
    }

    // Update the status flags for the LDA instruction
    void LDAStatusUpdate() {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void LDXStatusUpdate() {
        Z = (X == 0);
        N = (A & 0b10000000) > 0;
    }

    // Debugging step to print out value of each register
    void debugReport() {
        // Registers
        std::cout << "PC: " << std::hex << unsigned(PC) << ", ";
        std::cout << "SP: " << std::hex << unsigned(SP) << ", ";
        std::cout << "A: " << std::hex << unsigned(A) << ", ";
        std::cout << "X: " << std::hex << unsigned(X) << ", ";
        std::cout << "Y: " << std::hex << unsigned(Y) << "\n";
        // Status flags
        std::cout << "C: " << std::hex << unsigned(C) << ", ";
        std::cout << "Z: " << std::hex << unsigned(Z) << ", ";
        std::cout << "I: " << std::hex << unsigned(I) << ", ";
        std::cout << "D: " << std::hex << unsigned(D) << ", ";
        std::cout << "B: " << std::hex << unsigned(B) << ", ";
        std::cout << "O: " << std::hex << unsigned(O) << ", ";
        std::cout << "N: " << std::hex << unsigned(N) << "\n";
    }

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
        INS_LDX_ABS = 0xAE,
        INS_LDX_ABY = 0xBE,

        INS_JSR = 0x20
        ;

    void Execute(unsigned int nCycles, Mem &mem) {
        while (nCycles > 0) {
            Byte instruction = FetchByte(nCycles, mem);
            /* 
             * FOR DEBUGGING ONLY DELETE ME WHEN THIS WORKS!!!
             */
            std::cout << "Instruction: " << std::hex << unsigned(instruction) << "\n";
            switch (instruction) {
                // LDA instruction
                case INS_LDA_IM: {
                    Byte val = FetchByte(nCycles, mem); // Get byte at PC
                    A = val;            // Set accululator to the value
                    LDAStatusUpdate();  // Set status flags
                    debugReport();
                } break;
                case INS_LDA_ZP: {
                    Byte zpAddress = FetchByte(nCycles, mem); // Get argument from PC
                    A = ReadByte(nCycles, zpAddress, mem); // Read from address
                    LDAStatusUpdate(); 
                    debugReport();
                } break;
                case INS_LDA_ZPX: {
                    Byte zpAddress = FetchByte(nCycles, mem);
                    zpAddress += X; // Add x register to zpAddress 
                    nCycles--;      // Add operation takes one clock cycle so decrement 
                                    // nCycles
                    /* Check if address is greater than 1 byte. if it is, "overflow" */
                    if(zpAddress >= 0xFF) { zpAddress -= 0x100; } 
                    A = ReadByte(nCycles, zpAddress, mem);
                    LDAStatusUpdate();
                    debugReport();
                    //std::cout << "nCycles: " << nCycles << std::endl;
                } break;
                case INS_LDA_AB: {
                    Word addr = FetchWord(nCycles, mem);
                    A = ReadByte(nCycles, addr, mem);
                    LDAStatusUpdate();
                    debugReport();
                }
                case INS_LDA_ABX: {
                    Word addr = FetchWord(nCycles, mem);
                    addr += X; // Add X register to the fetched address
                    nCycles--; // Addition takes one cycle
                    A = ReadByte(nCycles, addr, mem); // Read the value and put into A
                    LDAStatusUpdate();
                    debugReport();
                } break;
                case INS_LDA_ABY: {
                    Word addr = FetchWord(nCycles, mem);
                    addr += Y; // Add X register to the fetched address
                    nCycles--; // Addition takes one cycle
                    A = ReadByte(nCycles, addr, mem); // Read the value and put into A
                    LDAStatusUpdate();
                    debugReport();
                } break;
                case INS_LDA_IDX: {
                    Byte a = FetchByte(nCycles, mem); // Get address of table
                    a += X; // Add x to address of table
                    if (a > 0xFF) { a -= 0x100; } // Can't be out of the zero page
                    nCycles--; // a+=X takes 1 clock cycle
                    A = ReadByte(nCycles, a, mem); // Read the value at the address
                    LDAStatusUpdate();
                    debugReport();
                } break;
                case INS_LDA_IDY: { // Same as above but with Y register
                    Byte a = FetchByte(nCycles, mem); // Get address of table
                    a += Y; // Add x to address of table
                    if (a > 0xFF) { a -= 0x100; } // Can't be out of the zero page
                    nCycles--; // a+=X takes 1 clock cycle
                    A = ReadByte(nCycles, a, mem); // Read the value at the address
                    LDAStatusUpdate();
                    debugReport();
                } break;
                // LDX instruction
                case INS_LDX_IM: {
                    Byte val = FetchByte(nCycles, mem);
                    X = val;
                    LDXStatusUpdate();
                    debugReport();
                } break;
                case INS_LDX_ZP: {
                    Byte zpAddress = FetchByte(nCycles, mem); // Get argument from PC
                    X = ReadByte(nCycles, zpAddress, mem); // Read from address
                    LDAStatusUpdate(); 
                    debugReport();
                } break;
                case INS_LDX_ZPY: {
                    Byte zpAddress = FetchByte(nCycles, mem);
                    zpAddress += Y; // Add x register to zpAddress 
                    nCycles--;      // Add operation takes one clock cycle so decrement 
                                    // nCycles
                    /* Check if address is greater than 1 byte. if it is, "overflow" */
                    if(zpAddress >= 0xFF) { zpAddress -= 0x100; } 
                    X = ReadByte(nCycles, zpAddress, mem);
                    LDAStatusUpdate();
                    debugReport();
                } break;
                case INS_LDX_ABS: {
                    Word addr = FetchWord(nCycles, mem);
                    X = ReadByte(nCycles, addr, mem);
                    LDXStatusUpdate();
                    debugReport();
                } break;
                case INS_LDX_ABY: {
                    Word addr = FetchWord(nCycles, mem);
                    addr += Y;
                    nCycles--;
                    X = ReadByte(nCycles, addr, mem); 
                } break;

                // JSR instruction
                case INS_JSR: {
                    Word jmpAddr = FetchWord(nCycles, mem); // We first get the address by fetching the next word
                    mem.WriteWord(PC - 1, SP, nCycles);     // Then write the current program counter - 1 (as per the 6502 instruction set spec.)
                    SP++;                                   // Increment stack pointer cuz we just wrote to the stack

                    PC = jmpAddr;                           // New program counter is the address we just read
                    nCycles--;                              // We used up a cycle so decrement
                    debugReport();
                } break;
                default:
                    std::cout << "Instruction not handled!\n";     
            }break;
        }
    }

    void Reset(Mem &mem) {
        PC = 0xFFFC;             // Initialize program counter to 0xFFC
        SP = 0x0100;            // Inititalize stack pointer to 0x01FF
        C = Z = I = D = B = O = N = 0; // Reset status flags
        A = X = Y = 0;          // Reset registers
        mem.Init();             // Reset memory
    }

};

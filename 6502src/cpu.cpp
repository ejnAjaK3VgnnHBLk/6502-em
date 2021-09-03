#include "cpu.hpp"

void CPU::Execute(unsigned int nCycles, Mem &mem) {
    while (nCycles > 0) {
        Byte instruction = FetchByte(nCycles, mem);
        switch (instruction) {
            // LDA instruction
            case INS_LDA_IM: {
                A = FetchByte(nCycles, mem);            // Set accululator to the value
                LDAStatusUpdate();  // Set status flags
            } break;
            case INS_LDA_ZP: {
                A = ReadByte(nCycles, FetchByte(nCycles, mem), mem); // Read from address
                LDAStatusUpdate(); 
            } break;
            case INS_LDA_ZPX: {
                A = ReadByte(nCycles, GetZeroPageAddrX(nCycles, mem), mem);
                LDAStatusUpdate();
            } break;
            case INS_LDA_AB: {
                A = ReadByte(nCycles, FetchWord(nCycles, mem), mem);
                LDAStatusUpdate();
            } break;
            case INS_LDA_ABX: {
                A = ReadByte(nCycles, GetAddrX(nCycles, mem), mem); // Read the value and put into A
                LDAStatusUpdate();
            } break;
            case INS_LDA_ABY: {
                A = ReadByte(nCycles, GetAddrY(nCycles, mem), mem); // Read the value and put into A
                LDAStatusUpdate();
            } break;
            case INS_LDA_IDX: {
                A = ReadByte(nCycles, GetZeroPageAddrX(nCycles, mem), mem); // Read the value at the address
                LDAStatusUpdate();
            } break;
            case INS_LDA_IDY: { // Same as above but with Y register
                A = ReadByte(nCycles, GetZeroPageAddrY(nCycles, mem), mem); // Read the value at the address
                LDAStatusUpdate();
            } break;
            // LDX instruction
            case INS_LDX_IM: {
                X = FetchByte(nCycles, mem);
                LDXStatusUpdate();
            } break;
            case INS_LDX_ZP: {
                X = ReadByte(nCycles, FetchByte(nCycles, mem), mem); // Read from address
                LDXStatusUpdate(); 
            } break;
            case INS_LDX_ZPY: {
                Byte addr = GetZeroPageAddrY(nCycles, mem);
                X = ReadByte(nCycles, addr, mem);
                LDXStatusUpdate();
            } break;
            case INS_LDX_AB: {
                X = ReadByte(nCycles, FetchWord(nCycles, mem), mem);
                LDXStatusUpdate();
            } break;
            case INS_LDX_ABY: {
                X = ReadByte(nCycles, GetAddrY(nCycles, mem), mem); 
                LDXStatusUpdate();
            } break;

            // JSR instruction
            case INS_JSR: {
                /* WIP:
                The JSR instruction pushes the address (minus one) of the return point on to the stack and then sets the program counter to the target memory address.
                */

                Word jmpAddr = FetchWord(nCycles, mem); // We first get the address by fetching the next word
                mem.WriteWord(PC - 1, SP, nCycles);     // Then write the current program counter - 1 (as per the 6502 instruction set spec.)
                SP++;                                   // Increment stack pointer cuz we just wrote to the stack

                PC = jmpAddr;                           // New program counter is the address we just read
                nCycles--;                              // We used up a cycle so decrement
            } break;
            default:
                std::cout << "Instruction not handled!\n";     
        }break;
    }
}

void CPU::debugReport() {
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
    std::cout << "V: " << std::hex << unsigned(V) << ", ";
    std::cout << "N: " << std::hex << unsigned(N) << "\n";
}

void CPU::LDAStatusUpdate() {
    Z = (A == 0);
    N = (A & 0b10000000) > 0;
}

void CPU::LDXStatusUpdate() {
    Z = (X == 0);
    N = (A & 0b10000000) > 0;
}

Byte CPU::FetchByte(unsigned int &nCycles, Mem &mem) {
    Byte ins = mem[PC];
    PC++;
    nCycles--;
    return ins;
}

Byte CPU::ReadByte(unsigned int &nCycles, Word addr, Mem &mem) {
    Byte ins = mem[addr];
    nCycles--;
    return ins;
}

Word CPU::FetchWord(unsigned int &nCycles, Mem &mem) {
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

Word CPU::ReadWord(unsigned int &nCycles, Word addr, Mem &mem) {
    // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
    // BIT COMES FIRST.
    Word Data = mem[addr];
    addr++;
    Data |= (mem[addr] << 8);
    nCycles-=2;

    // Swap lower and higher bits if your platform is not low endian
    // x86_64 is little endian, so I don't need to swap the bytes around
    // luckily, but eventually add test here to make this platform agnostic
    // Note to future self: you can just do the previous steps in reverse 
    // for big endian systems.

    return Data;
}

Byte CPU::GetZeroPageAddrY(unsigned int& nCycles, Mem &mem) {
    Byte zpAddress = FetchByte(nCycles, mem);
    zpAddress += Y; // Add x register to zpAddress 
    nCycles--;      // Add operation takes one clock cycle so decrement 
                    // nCycles
    /* Check if address is greater than 1 byte. if it is, "overflow" */
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; nCycles--; } 
    return zpAddress;
}

Byte CPU::GetZeroPageAddrX(unsigned int& nCycles, Mem &mem) {
    Byte zpAddress = FetchByte(nCycles, mem);
    zpAddress += X; // Add x register to zpAddress 
    nCycles--;      // Add operation takes one clock cycle so decrement 
                    // nCycles
    /* Check if address is greater than 1 byte. if it is, "overflow" */
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; nCycles--; } 
    return zpAddress;
}

Word CPU::GetAddrY(unsigned int &nCycles, Mem &mem) {
    Word addr = FetchWord(nCycles, mem);
    addr += Y; // Add X register to the fetched address
    nCycles--; // Addition takes one cycle
    return addr;
}

Word CPU::GetAddrX(unsigned int &nCycles, Mem &mem) {
    Word addr = FetchWord(nCycles, mem);
    addr += X; // Add X register to the fetched address
    nCycles--; // Addition takes one cycle
    return addr;
}

void CPU::Reset(Mem &mem) {
    PC = 0xFFFC;             // Initialize program counter to 0xFFC
    SP = 0x0100;            // Inititalize stack pointer to 0x01FF
    C = Z = I = D = B = V = N = 0; // Reset status flags
    A = X = Y = 0;          // Reset registers
    mem.Init();             // Reset memory
}
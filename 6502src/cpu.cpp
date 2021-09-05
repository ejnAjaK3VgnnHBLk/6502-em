#include "cpu.hpp"
#include <iostream> 

void CPU::Execute(unsigned int nCycles, Mem &mem) {
    while (nCycles > 0) {
        Byte instruction = FetchByte(nCycles, mem);
        switch (instruction) {
            // LDA instruction
            case INS_LDA_IM:
                WriteRegister(A, FetchByte(nCycles, mem));
            break;
            case INS_LDA_ZP:
                WriteRegister(A, ReadByte(nCycles, AddressingZeroPage(nCycles, mem), mem));
            break;
            case INS_LDA_ZPX:
                WriteRegister(A, ReadByte(nCycles, AddressingZeroPageX(nCycles, mem), mem));
            break;
            case INS_LDA_AB:
                WriteRegister(A, ReadByte(nCycles, AddressingAbsolute(nCycles, mem), mem));
            break;
            case INS_LDA_ABX:
                WriteRegister(A, ReadByte(nCycles, AddressingAbsoluteX(nCycles, mem), mem));
            break;
            case INS_LDA_ABY:
                WriteRegister(A, ReadByte(nCycles, AddressingAbsoluteY(nCycles, mem), mem));
            break;
            case INS_LDA_IDX:
                WriteRegister(A, ReadByte(nCycles, AddressingIndexedIndirect(nCycles, mem), mem));
            break;
            case INS_LDA_IDY:
                WriteRegister(A, ReadByte(nCycles, AddressingIndirectIndexed(nCycles, mem), mem));
            break;
            // LDX instruction
            case INS_LDX_IM:
                WriteRegister(X, FetchByte(nCycles, mem));
            break;
            case INS_LDX_ZP:
                WriteRegister(X, ReadByte(nCycles, FetchByte(nCycles, mem), mem));
            break;
            case INS_LDX_ZPY:
                WriteRegister(X, ReadByte(nCycles, AddressingZeroPageY(nCycles, mem), mem));
            break;
            case INS_LDX_AB:
                WriteRegister(X, ReadByte(nCycles, AddressingAbsolute(nCycles, mem), mem));
            break;
            case INS_LDX_ABY:
                WriteRegister(X, ReadByte(nCycles, AddressingAbsoluteY(nCycles, mem), mem)); 
            break;
            // STA Instruction
            case INS_STA_ZP:
                WriteToMemFromRegister(A, FetchByte(nCycles, mem), nCycles, mem);
            break;
            case INS_STA_ZPX:
                WriteToMemFromRegister(A, AddressingZeroPageX(nCycles, mem), nCycles, mem);
            break;
            case INS_STA_AB:
                WriteToMemFromRegister(A, FetchWord(nCycles, mem), nCycles, mem);
            break;
            case INS_STA_ABX:
                WriteToMemFromRegister(A, AddressingAbsoluteX(nCycles, mem), nCycles, mem);
            break;
            case INS_STA_ABY:
                WriteToMemFromRegister(A, AddressingAbsoluteY(nCycles, mem), nCycles, mem);
            break;
            case INS_STA_IDX:
                WriteToMemFromRegister(A, AddressingIndexedIndirect(nCycles, mem), nCycles, mem);
            break;
             case INS_STA_IDY:
                WriteToMemFromRegister(A, AddressingIndirectIndexed(nCycles, mem), nCycles, mem);
            break;
            // STX instruction
            case INS_STX_ZP:
                WriteToMemFromRegister(X, FetchByte(nCycles, mem), nCycles, mem);  
            break;
            case INS_STX_ZPY:
                WriteToMemFromRegister(X, AddressingZeroPageY(nCycles, mem), nCycles, mem);
            break;
            case INS_STX_AB:
                WriteToMemFromRegister(X, FetchWord(nCycles, mem), nCycles, mem);
            break;
            // STY instruction
            case INS_STY_ZP:
                WriteToMemFromRegister(Y, FetchByte(nCycles, mem), nCycles, mem);
            break;
            case INS_STY_ZPX:
                WriteToMemFromRegister(Y, AddressingZeroPageX(nCycles, mem), nCycles, mem);
            break;
            case INS_STY_AB:
                WriteToMemFromRegister(Y, FetchWord(nCycles, mem), nCycles, mem);
            break;

            // JSR instruction
            case INS_JSR: {
                /* WIP:
                The JSR instruction pushes the address (minus one) of the return point on to the stack and then sets the program counter to the target memory address.
                */

                Word jmpAddr = FetchWord(nCycles, mem); // We first get the address by fetching the next word
                WriteWord(PC - 1, SP, nCycles, mem);         // Then write the current program counter - 1 (as per the 6502 instruction set spec.)
                SP+=2;                                  // Increment stack pointer cuz we just wrote to the stack

                PC = jmpAddr;                           // New program counter is the address we just read
                nCycles--;                              // We used up a cycle so decrement
            } break;
            default:
                std::cout << "Instruction: " << std::hex << unsigned(instruction) << " not handled!\n" ;     
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

void CPU::UpdateZeroAndNegativeFlags(Byte &reg) {
    Z = (reg == 0);
    N = (reg & 0b10000000) > 0;
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

    Data |= (mem[PC] << 8);
    PC++;
    nCycles-=2;

    return Data;
}

Word CPU::ReadWord(unsigned int &nCycles, Word addr, Mem &mem) {
    // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
    // BIT COMES FIRST.
    Word Data = mem[addr];
    addr++;
    Data |= (mem[addr] << 8);
    nCycles-=2;

    return Data;
}

void CPU::WriteWord(Word dta, unsigned int addr, unsigned int &cycles, Mem &mem) {
    mem[addr] = dta & 0xFF;
    mem[addr+1] = (dta >> 8);
    cycles -= 2;
}

void CPU::WriteByte(Byte data, unsigned int addr, unsigned int &nCycles, Mem &mem) {
    mem[addr] = data;
    nCycles--;
}

void CPU::WriteToMemFromRegister(Byte &reg, Word addr, unsigned int& nCycles, Mem &mem) {
    mem[addr] = reg;
    nCycles--;
}

void CPU::WriteRegister(Byte &reg, Byte value) {
    reg = value;
    UpdateZeroAndNegativeFlags(reg);
}

// Addressing Mode Functions
// See (https://github.com/ejnAjaK3VgnnHBLk/6502-em/pull/20#issue-988360270) for why I don't implement
//      some addressing modes here!
Byte CPU::AddressingZeroPage(unsigned int &nCycles, Mem &mem) {
    // zero page addressing mode has only an 8 bit address operand
    return FetchByte(nCycles, mem);
}

Byte CPU::AddressingZeroPageX(unsigned int &nCycles, Mem &mem) {
    // taking the 8 bit zero page address from the instruction and adding the current value of 
    // the X register to it
    Byte zpAddress = FetchByte(nCycles, mem);
    zpAddress += X;
    nCycles--; 
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; nCycles--; } 
    return zpAddress;
}

Byte CPU::AddressingZeroPageY(unsigned int &nCycles, Mem &mem) {
    // taking the 8 bit zero page address from the instruction and adding the current value of 
    // the Y register to it
    Byte zpAddress = FetchByte(nCycles, mem);
    zpAddress += Y; 
    nCycles--; 
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; nCycles--; } 
    return zpAddress;
}

Word CPU::AddressingAbsolute(unsigned int &nCycles, Mem &mem) {
    // contain a full 16 bit address to identify the target location
    return FetchWord(nCycles, mem);
}

Word CPU::AddressingAbsoluteX(unsigned int &nCycles, Mem &mem) {
    // taking the 16 bit address from the instruction and added the contents of the X register
    Word addr = FetchWord(nCycles, mem);
    addr += X; // Add X register to the fetched address
    nCycles--; // Addition takes one cycle
    return addr;
}

Word CPU::AddressingAbsoluteY(unsigned int &nCycles, Mem &mem) {
    // same as the previous mode only with the contents of the Y register
    Word addr = FetchWord(nCycles, mem);
    addr += Y; // Add X register to the fetched address
    nCycles--; // Addition takes one cycle
    return addr;
}

Word CPU::AddressingIndirect(unsigned int &nCycles, Mem &mem) {
    // The instruction contains a 16 bit address which identifies the location of the least 
    //significant byte of another 16 bit memory address which is the real target of the instruction
    Word addr = FetchWord(nCycles, mem);
    return ReadWord(nCycles, addr, mem);

}

Word CPU::AddressingIndexedIndirect(unsigned int &nCycles, Mem &mem) {
    // The address of the table is taken from the instruction and the X register added to it (with 
    // zero page wrap around) to give the location of the least significant byte of the target address.
    Byte addr = FetchByte(nCycles, mem);
    addr += X;
    nCycles--;
    return ReadWord(nCycles, addr, mem);
}

Word CPU::AddressingIndirectIndexed(unsigned int &nCycles, Mem &mem) {
    // In instruction contains the zero page location of the least significant byte of 16 bit address. 
    // The Y register is dynamically added to this value to generated the actual target address for operation.
    Byte addr = FetchByte(nCycles, mem);
    addr += Y;
    nCycles--;
    return ReadWord(nCycles, addr, mem);
}   

void CPU::Reset(Mem &mem) {
    PC = 0xFFFC;             // Initialize program counter to 0xFFC
    SP = 0x0100;            // Inititalize stack pointer to 0x01FF
    C = Z = I = D = B = V = N = 0; // Reset status flags
    A = X = Y = 0;          // Reset registers
    mem.Init();             // Reset memory
}
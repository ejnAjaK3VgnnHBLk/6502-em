#include "../6502include/cpu.hpp"

void CPU::Reset(Mem &mem) {
    PC = 0xFFFC;             // Initialize program counter to 0xFFC
    SP = 0x0100;            // Inititalize stack pointer to 0x01FF
    C = Z = I = D = B = V = N = 0; // Reset status flags
    A = X = Y = 0;          // Reset registers
    mem.Init();             // Reset memory
}

void CPU::Execute(unsigned int nCycles, Mem &mem) {
    while (nCycles > 0) {
        Byte instruction = FetchByte(nCycles, mem);
        switch (instruction) {
            // LDA instruction
            case INS_LDA_IM: {
                Byte val = FetchByte(nCycles, mem);
                A = val;            // Set accululator to the value
                LDAStatusUpdate();  // Set status flags
            } break;
            case INS_LDA_ZP: {
                Byte zpAddress = FetchByte(nCycles, mem); // Get argument from PC
                A = ReadByte(nCycles, zpAddress, mem); // Read from address
                LDAStatusUpdate(); 
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
            } break;
            case INS_LDA_AB: {
                Word addr = FetchWord(nCycles, mem);
                A = ReadByte(nCycles, addr, mem);
                LDAStatusUpdate();
            } break;
            case INS_LDA_ABX: {
                Word addr = FetchWord(nCycles, mem);
                addr += X; // Add X register to the fetched address
                nCycles--; // Addition takes one cycle
                A = ReadByte(nCycles, addr, mem); // Read the value and put into A
                LDAStatusUpdate();
            } break;
            case INS_LDA_ABY: {
                Word addr = FetchWord(nCycles, mem);
                addr += Y; // Add X register to the fetched address
                nCycles--; // Addition takes one cycle
                A = ReadByte(nCycles, addr, mem); // Read the value and put into A
                LDAStatusUpdate();
            } break;
            case INS_LDA_IDX: {
                Byte a = FetchByte(nCycles, mem); // Get address of table
                a += X; // Add x to address of table
                if (a > 0xFF) { a -= 0x100; } // Can't be out of the zero page
                nCycles--; // a+=X takes 1 clock cycle
                A = ReadByte(nCycles, a, mem); // Read the value at the address
                LDAStatusUpdate();
            } break;
            case INS_LDA_IDY: { // Same as above but with Y register
                Byte a = FetchByte(nCycles, mem); // Get address of table
                a += Y; // Add x to address of table
                if (a > 0xFF) { a -= 0x100; } // Can't be out of the zero page
                nCycles--; // a+=X takes 1 clock cycle
                A = ReadByte(nCycles, a, mem); // Read the value at the address
                LDAStatusUpdate();
            } break;
            // LDX instruction
            case INS_LDX_IM: {
                Byte val = FetchByte(nCycles, mem);
                X = val;
                LDXStatusUpdate();
            } break;
            case INS_LDX_ZP: {
                Byte zpAddress = FetchByte(nCycles, mem); // Get argument from PC
                X = ReadByte(nCycles, zpAddress, mem); // Read from address
                LDXStatusUpdate(); 
            } break;
            case INS_LDX_ZPY: {
                Byte zpAddress = FetchByte(nCycles, mem);
                zpAddress += Y; // Add x register to zpAddress 
                nCycles--;      // Add operation takes one clock cycle so decrement 
                                // nCycles
                /* Check if address is greater than 1 byte. if it is, "overflow" */
                if(zpAddress >= 0xFF) { zpAddress -= 0x100; } 
                X = ReadByte(nCycles, zpAddress, mem);
                LDXStatusUpdate();
            } break;
            case INS_LDX_AB: {
                Word addr = FetchWord(nCycles, mem);
                X = ReadByte(nCycles, addr, mem);
                LDXStatusUpdate();
            } break;
            case INS_LDX_ABY: {
                Word addr = FetchWord(nCycles, mem);
                addr += Y;
                nCycles--;
                X = ReadByte(nCycles, addr, mem); 
                LDXStatusUpdate();
            } break;

            // JSR instruction
            case INS_JSR: {
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
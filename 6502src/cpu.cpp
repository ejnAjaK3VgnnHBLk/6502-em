#include "cpu.hpp"

/*
 *ADC AND ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC
 *CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP
 *JSR LDA LDX LDY LSR NOP ORA PHA PHP PLA PLP ROL ROR RTI
 *RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA
 */


/*
 * Not yet implemented instructions:
 *ADC BCC BCS BEQ BIT BMI BNE BPL BVC BVS CMP CPX CPY
 *SBC
 */

void CPU::Execute(unsigned int nCycles, Mem &mem) {
    auto lAND = [&nCycles, &mem, this](Word addr) {
        A &= ReadByte(nCycles, addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto EOR = [&nCycles, &mem, this](Word addr) {
        A ^= ReadByte(nCycles, addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto ORA = [&nCycles, &mem, this](Word addr) {
        A |= ReadByte(nCycles, addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto BIT = [&nCycles, &mem, this](Word addr) {
        Byte val = ReadByte(nCycles, addr, mem);
        SF.V = (val & 0b1000000) > 0;
        SF.N = (val & 0b10000000) > 0;
        SF.Z = (A & val) == 0;
    };

    auto IncrementMem = [&nCycles, &mem, this](Word addr) {
        Byte xd = ReadByte(nCycles, addr, mem);
        WriteByte(++xd, addr, nCycles, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto IncrementReg = [&nCycles, &mem, this](Byte &reg) {
        reg += 1;
        UpdateZeroAndNegativeFlags(reg);
    };

    auto DecrementMem = [&nCycles, &mem, this](Word addr) {
        Byte xd = ReadByte(nCycles, addr, mem);
        WriteByte(--xd, addr, nCycles, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto DecrementReg = [&nCycles, &mem, this](Byte &reg) {
        reg -= 1;
        UpdateZeroAndNegativeFlags(reg);
    };

    auto ASL = [&nCycles, &mem, this](Word addr) {
        Byte val = ReadByte(nCycles, addr, mem);
        SF.C = (val & 0b10000000) > 0;
        SF.Z = (val == 0);
        WriteByte(val << 1, addr, nCycles, mem);
        SF.N = (mem[addr] & 0b10000000) > 0;
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto LSR = [&nCycles, &mem, this](Word addr) {
        SF.C = (mem[addr] & 0b1);
        WriteByte(mem[addr] >> 1, addr, nCycles, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto ROL = [&nCycles, &mem, this](Word addr) {
        Byte old = ReadByte(nCycles, addr, mem);
        WriteByte(mem[addr] << 1, addr, nCycles, mem);
        Byte val = ReadByte(nCycles, addr, mem) | SF.C << 0;
        WriteByte(val, addr, nCycles, mem);
        SF.C = (old & 0b10000000) > 0;
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto ROR = [&nCycles, &mem, this](Word addr) {
        Byte old = ReadByte(nCycles, addr, mem);
        WriteByte(mem[addr] >> 1, addr, nCycles, mem); // Right shift everything
        Byte asdf = ReadByte(nCycles, addr, mem) ;
        asdf |= asdf << 7;
        WriteByte(asdf, addr, nCycles, mem);
        SF.C = (old & 0b1);
        UpdateZeroAndNegativeFlags(asdf);
    };

    auto Branch = [&nCycles, &mem, this](bool test, bool val) {
        Byte displacement = FetchByte(nCycles, mem);
        if (test == val) {
            Word oldPC = PC;

            PC+=displacement;
            nCycles--;

            if ((oldPC >> 8) != (PC >> 8))
                nCycles--;
        }
    };

    while (nCycles > 0) {
        Byte instruction = FetchByte(nCycles, mem);
        switch (instruction) {
            // Branch Functions
            case INS_BCC: {
                Branch(SF.C, false);
            } break;
            case INS_BCS:
                Branch(SF.C, true);
            break;
            case INS_BEQ:
                Branch(SF.Z, true);
            break;
            case INS_BMI:
                Branch(SF.N, true);
            break;
            case INS_BNE:
                Branch(SF.Z, false);
            break;
            case INS_BPL:
                Branch(SF.N, false);
            break;
            case INS_BVC:
                Branch(SF.V, false);
            break;
            case INS_BVS:
                Branch(SF.V, true);
            break;
            // System Functions
            case INS_NOP:
                nCycles -= 2;
            break;
            case INS_BRK: {
                PushWord(nCycles, PC + 1, mem);
                PushStatusFlagsToStack(nCycles, mem);
                PC = ReadWord(nCycles, 0xFFFE, mem);
                SF.B = 1;
            } break;
            case INS_RTI: {
                PopStatusFlagsFromStack(nCycles, mem);
                PC = PopWord(nCycles, mem);
                SF.B = 0;
                SF.na = 0;
            } break;
            // Status flag changes
            case INS_CLC:
                SF.C = 0;
            break;
            case INS_CLD:
                SF.D = 0;
            break;
            case INS_CLI:
                SF.I = 0;
            break;
            case INS_CLV:
                SF.V = 0;
            break;
            case INS_SEC:
                SF.C = 1;
            break;
            case INS_SED:
                SF.D = 1;
            break;
            case INS_SEI:
                SF.I = 1;
            break;
            // Rotate Right ---------------------------------------------
            case INS_ROR_ACC: {
                Byte old = A;
                A = A >> 1; // Right shift everything
                A |= SF.C << 7; // Set 7th bit to C
                SF.C = (old & 0b1); // First bit of old is new C
            } break;
            case INS_ROR_ZP:
                ROR(AddressingZeroPage(nCycles, mem));
            break;
            case INS_ROR_ZPX:
                ROR(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_ROR_AB:
                ROR(AddressingAbsolute(nCycles, mem));
            break;
            case INS_ROR_ABX:
                ROR(AddressingAbsoluteX(nCycles, mem));
            break;
            // Arithmetic Shift Left -------------------------------------------------
            case INS_ASL_ACC: {
                SF.C = (A & 0b10000000) > 0;
                SF.Z = (A == 0);
                A = A << 1;
                SF.N = (A & 0b10000000) > 0;
            } break;
            case INS_ASL_ZP:
                ASL(AddressingZeroPage(nCycles, mem));
            break;
            case INS_ASL_ZPX:
                ASL(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_ASL_AB:
                ASL(AddressingAbsolute(nCycles, mem));
            break;
            case INS_ASL_ABX:
                ASL(AddressingAbsoluteX(nCycles, mem));
            break;
            // Logical Shift Right ---------------------------------------------------
            case INS_LSR_ACC: {
                SF.C = (A & 0b1);
                A = A >> 1;
                UpdateZeroAndNegativeFlags(A);
            } break;
            case INS_LSR_ZP:
                LSR(AddressingZeroPage(nCycles, mem));
            break;
            case INS_LSR_ZPX:
                LSR(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_LSR_AB:
                LSR(AddressingAbsolute(nCycles, mem));
            break;
            case INS_LSR_ABX:
                LSR(AddressingAbsoluteX(nCycles, mem));
            break;
            // Rotate left ---------------------------------------------
            case INS_ROL_ACC: {
                Byte old = A;
                A = A << 1;
                A |= SF.C << 0;
                SF.C = (old & 0b10000000) > 0;
                UpdateZeroAndNegativeFlags(A);

            } break;
            case INS_ROL_ZP:
                ROL(AddressingZeroPage(nCycles, mem));
            break;
            case INS_ROL_ZPX:
                ROL(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_ROL_AB:
                ROL(AddressingAbsolute(nCycles, mem));
            break;
            case INS_ROL_ABX:
                ROL(AddressingAbsoluteX(nCycles, mem));
            break;
            // Increment memory location ---------------------------------------------
            case INS_INC_ZP:
                IncrementMem(AddressingZeroPage(nCycles, mem));
            break;
            case INS_INC_ZPX:
                IncrementMem(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_INC_AB:
                IncrementMem(AddressingAbsolute(nCycles, mem));
            break;
            case INS_INC_ABX:
                IncrementMem(AddressingAbsoluteX(nCycles, mem));
            break;
            // Decrement memory location
            case INS_DEC_ZP:
                DecrementMem(AddressingZeroPage(nCycles, mem));
            break;
            case INS_DEC_ZPX:
                DecrementMem(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_DEC_AB:
                DecrementMem(AddressingAbsolute(nCycles, mem));
            break;
            case INS_DEC_ABX:
                DecrementMem(AddressingAbsoluteX(nCycles, mem));
            break;
            // Increment and decrement register
            case INS_INX:
                IncrementReg(X);
            break;
            case INS_INY:
                IncrementReg(Y);
            break;
            case INS_DEX:
                DecrementReg(X);
            break;
            case INS_DEY:
                DecrementReg(Y);
            break;

            // BIT test ---------------------------------------------------------------
            case INS_BIT_ZP: {
                BIT(AddressingZeroPage(nCycles, mem));
            } break;
            case INS_BIT_AB: {
                BIT(AddressingAbsolute(nCycles, mem));
            } break;
            // Logical, inclusive OR ---------------------------------------------------
            case INS_ORA_IM: {
                A |= FetchByte(nCycles, mem);
                UpdateZeroAndNegativeFlags(A);
            } break;
            case INS_ORA_ZP:
                ORA(AddressingZeroPage(nCycles, mem));
            break;
            case INS_ORA_ZPX:
                ORA(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_ORA_AB:
                ORA(AddressingAbsolute(nCycles, mem));
            break;
            case INS_ORA_ABX:
                ORA(AddressingAbsoluteX(nCycles, mem));
            break;
            case INS_ORA_ABY:
                ORA(AddressingAbsoluteY(nCycles, mem));
            break;
            case INS_ORA_IDX:
                ORA(AddressingIndexedIndirect(nCycles, mem));
            break;
            case INS_ORA_IDY:
                ORA(AddressingIndirectIndexed(nCycles, mem));
            break;
            // Exclusive OR instruction ---------------------------------------------------
            case INS_EOR_IM: {
                A ^= FetchByte(nCycles, mem);
                UpdateZeroAndNegativeFlags(A);
            } break;
            case INS_EOR_ZP: {
                EOR(AddressingZeroPage(nCycles, mem));
            } break;
            case INS_EOR_ZPX: {
                EOR(AddressingZeroPageX(nCycles, mem));
            } break;
            case INS_EOR_AB: {
                EOR(AddressingAbsolute(nCycles, mem));
            } break;
            case INS_EOR_ABX: {
                EOR(AddressingAbsoluteX(nCycles, mem));
            } break;
            case INS_EOR_ABY: {
                EOR(AddressingAbsoluteY(nCycles, mem));
            } break;
            case INS_EOR_IDX: {
                EOR(AddressingIndexedIndirect(nCycles, mem));
            } break;
            case INS_EOR_IDY: {
                EOR(AddressingIndirectIndexed(nCycles, mem));
            } break;
            // Logical AND instruction ------------------------------------------------------
            case INS_AND_IM: {
                A &= FetchByte(nCycles, mem);
                UpdateZeroAndNegativeFlags(A);
            } break;
            case INS_AND_ZP: {
                lAND(AddressingZeroPage(nCycles, mem));
            } break;
            case INS_AND_ZPX: {
                lAND(AddressingZeroPageX(nCycles, mem));
            } break;
            case INS_AND_AB: {
                lAND(AddressingAbsolute(nCycles, mem));
            } break;
            case INS_AND_ABX: {
                lAND(AddressingAbsoluteX(nCycles, mem));
            } break;
            case INS_AND_ABY: {
                lAND(AddressingAbsoluteY(nCycles, mem));
            } break;
            case INS_AND_IDX: {
                lAND(AddressingIndexedIndirect(nCycles, mem));
            } break;
            case INS_AND_IDY: {
                lAND(AddressingIndirectIndexed(nCycles, mem));
            } break;
            // Transfer Instructions ----------------------------------------------------------
            case INS_TAX:{
                TransferRegister(A, X);
            } break;
            case INS_TAY: {
                TransferRegister(A, Y);
            } break;
            case INS_TSX:
                TransferRegister(SP, X);
            break;
            case INS_TXA:
                TransferRegister(X, A);
            break;
            case INS_TXS:
                TransferRegister(X, SP);
            break;
            case INS_TYA:
                TransferRegister(Y, A);
            break;
            // Various stack operations
            case INS_PHA:
                PushByte(nCycles, A, mem);
            break;
            case INS_PHP:
                PushStatusFlagsToStack(nCycles, mem);
            break;
            case INS_PLA:
                A = PopByte(nCycles, mem);
                UpdateZeroAndNegativeFlags(A);
            break;
            case INS_PLP:
                PopStatusFlagsFromStack(nCycles, mem);
            break;
            // LDA instruction ----------------------------------------------------------------
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
            // LDX instruction ----------------------------------------------------------------
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
            // LDY instruction ----------------------------------------------------------------
            case INS_LDY_IM:
                WriteRegister(Y, FetchByte(nCycles, mem));
            break;
            case INS_LDY_ZP:
                WriteRegister(Y, ReadByte(nCycles, FetchByte(nCycles, mem), mem));
            break;
            case INS_LDY_ZPX:
                WriteRegister(Y, ReadByte(nCycles, AddressingZeroPageY(nCycles, mem), mem));
            break;
            case INS_LDY_AB:
                WriteRegister(Y, ReadByte(nCycles, AddressingAbsolute(nCycles, mem), mem));
            break;
            case INS_LDY_ABX:
                WriteRegister(Y, ReadByte(nCycles, AddressingAbsoluteY(nCycles, mem), mem));
            break;
            // STA Instruction -----------------------------------------------------------------
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
            // STX instruction ------------------------------------------------------------------
            case INS_STX_ZP:
                WriteToMemFromRegister(X, FetchByte(nCycles, mem), nCycles, mem);  
            break;
            case INS_STX_ZPY:
                WriteToMemFromRegister(X, AddressingZeroPageY(nCycles, mem), nCycles, mem);
            break;
            case INS_STX_AB:
                WriteToMemFromRegister(X, FetchWord(nCycles, mem), nCycles, mem);
            break;
            // STY instruction ------------------------------------------------------------------
            case INS_STY_ZP:
                WriteToMemFromRegister(Y, FetchByte(nCycles, mem), nCycles, mem);
            break;
            case INS_STY_ZPX:
                WriteToMemFromRegister(Y, AddressingZeroPageX(nCycles, mem), nCycles, mem);
            break;
            case INS_STY_AB:
                WriteToMemFromRegister(Y, FetchWord(nCycles, mem), nCycles, mem);
            break;
            // JSR instruction ------------------------------------------------------------------
            case INS_JSR: {
                Word target = FetchWord(nCycles, mem);
                Word pcMinusOne = PC - 1;
                PushWord(nCycles, pcMinusOne, mem);
                PC = target;
            } break;
            case INS_RTS: {
                // It pulls the program counter (minus one) from the stack.
                Word PCPlusOne = PopWord(nCycles, mem) + 1;
                PC = PCPlusOne;
                nCycles -= 2;
            } break;
            case INS_JMP_AB: 
                PC = AddressingAbsolute(nCycles, mem);
            break;
            case INS_JMP_ID:
                PC = AddressingIndirect(nCycles, mem);
            break;
            default:
                std::cout << "Instruction: " << std::hex << unsigned(instruction) << " not handled!\n" ;     
        }break;
    }
}

void CPU::PushStatusFlagsToStack(unsigned int &nCycles, Mem &mem) {
    PushByte(nCycles, PSF, mem);
}

void CPU::PopStatusFlagsFromStack(unsigned int &nCycles, Mem &mem) {
    PSF = PopByte(nCycles, mem);
}

void CPU::debugReport() {
    // Registers
    std::cout << "PC: " << std::hex << unsigned(PC) << ", ";
    std::cout << "SP: " << std::hex << unsigned(SP) << ", ";
    std::cout << "A: " << std::hex << unsigned(A) << ", ";
    std::cout << "X: " << std::hex << unsigned(X) << ", ";
    std::cout << "Y: " << std::hex << unsigned(Y) << "\n";
    // Status flags
    std::cout << "C: " << std::hex << unsigned(SF.C) << ", ";
    std::cout << "Z: " << std::hex << unsigned(SF.Z) << ", ";
    std::cout << "I: " << std::hex << unsigned(SF.I) << ", ";
    std::cout << "D: " << std::hex << unsigned(SF.D) << ", ";
    std::cout << "B: " << std::hex << unsigned(SF.B) << ", ";
    std::cout << "V: " << std::hex << unsigned(SF.V) << ", ";
    std::cout << "N: " << std::hex << unsigned(SF.N) << "\n";
}

void CPU::UpdateZeroAndNegativeFlags(Byte &reg) {
    SF.Z = (reg == 0);
    SF.N = (reg & 0b10000000) > 0;
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

Word CPU::SPToAddr() { return SP + 0x100; }

Byte CPU::PopByte(unsigned int &nCycles, Mem &mem) {
    SP++;
    Byte value = ReadByte(nCycles, SPToAddr(), mem);
    nCycles--;
    return value;
}

Word CPU::PopWord(unsigned int &nCycles, Mem &mem) {
    Word value = mem[SPToAddr()+1];
    SP++;
    value |= (mem[SPToAddr()+1] << 8);
    nCycles-=2;

    SP++;
    nCycles -= 2;
    return value;
}

void CPU::PushByte(unsigned int &nCycles, Byte value, Mem &mem) {
    mem[SPToAddr()] = value;
    SP--;
    nCycles--;
}

void CPU::PushWord(unsigned int &nCycles, Word value, Mem &mem) {
    mem[SPToAddr()] = value >> 8;
    SP--;
    mem[SPToAddr()] = value & 0xFF;
    SP--;
    nCycles -= 2;
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
    SP = 0xFF;            // Inititalize stack pointer to 0x01FF
    SF.C = SF.Z = SF.I = SF.D = SF.B = SF.V = SF.N = 0; // Reset status flags
    A = X = Y = 0;          // Reset registers
    mem.Init();             // Reset memory
}

void CPU::TransferRegister(Byte &src, Byte &dest) {
    dest = src;
    UpdateZeroAndNegativeFlags(dest);
}

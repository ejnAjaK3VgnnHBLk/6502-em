#include "cpu_6502.hpp"

/*
 *ADC AND ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC
 *CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP
 *JSR LDA LDX LDY LSR NOP ORA PHA PHP PLA PLP ROL ROR RTI
 *RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA
 */

void cpu_6502::CPU::Execute(unsigned int nCycles, cpu_6502::Mem &mem) {
    auto lAND = [&mem, this](cpu_6502::Word addr) {
        A &= ReadByte(addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto EOR = [&mem, this](cpu_6502::Word addr) {
        A ^= ReadByte(addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto ORA = [&mem, this](cpu_6502::Word addr) {
        A |= ReadByte(addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto BIT = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(addr, mem);
        SF.V = (val & 0b1000000) > 0;
        SF.N = (val & 0b10000000) > 0;
        SF.Z = (A & val) == 0;
    };

    auto IncrementMem = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte xd = ReadByte(addr, mem);
        WriteByte(++xd, addr, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto IncrementReg = [&mem, this](cpu_6502::Byte &reg) {
        reg += 1;
        UpdateZeroAndNegativeFlags(reg);
    };

    auto DecrementMem = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte xd = ReadByte(addr, mem);
        WriteByte(--xd, addr, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto DecrementReg = [&mem, this](cpu_6502::Byte &reg) {
        reg -= 1;
        UpdateZeroAndNegativeFlags(reg);
    };

    auto ASL = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(addr, mem);
        SF.C = (val & 0b10000000) > 0;
        SF.Z = (val == 0);
        WriteByte(val << 1, addr, mem);
        SF.N = (mem[addr] & 0b10000000) > 0;
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto LSR = [&mem, this](cpu_6502::Word addr) {
        SF.C = (mem[addr] & 0b1);
        WriteByte(mem[addr] >> 1, addr, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto ROL = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte old = ReadByte(addr, mem);
        WriteByte(mem[addr] << 1, addr, mem);
        cpu_6502::Byte val = ReadByte(addr, mem) | SF.C << 0;
        WriteByte(val, addr, mem);
        SF.C = (old & 0b10000000) > 0;
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto ROR = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte old = ReadByte(addr, mem);
        WriteByte(mem[addr] >> 1, addr, mem); // Right shift everything
        cpu_6502::Byte asdf = ReadByte(addr, mem) ;
        asdf |= asdf << 7;
        WriteByte(asdf, addr, mem);
        SF.C = (old & 0b1);
        UpdateZeroAndNegativeFlags(asdf);
    };

    auto Branch = [&nCycles, &mem, this](bool test, bool val) {
        cpu_6502::Byte displacement = FetchByte(mem);
        if (test == val) {
            PC+=displacement;
            return true;
        }
        return false;
    };

    auto Compare = [&mem, this](cpu_6502::Byte &reg, cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(addr, mem);
        SF.C = reg >= val;
        SF.Z = (reg == val);
        SF.N = ((reg - val) & 0b10000000) > 0;
    };

    auto ADC = [&mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(addr, mem);
        cpu_6502::Byte origA = A;

        cpu_6502::Word sum = A + val + SF.C;
        A = sum & 0xFF;
        UpdateZeroAndNegativeFlags(A);
        SF.C = sum > 0xFF;

        // Most complicated flag to set
        // First, check if sign bits are the same. We exclusive or both values to figure
        //    out what bits have changed. Then AND it with the negative flag bit (0b10000000)
        //    to figure out if that bit was *not* set, so invert it.
        // Secondly, check if the sign bits have changed after the addition, and do the same as
        //    above, except don't negate the bits.
        // Lastly, and the two values as booleans, and set that to the value of V
        SF.V = !((origA ^ val) & 0b10000000) && ((A ^ val) & 0b10000000);
    };

    auto SBC = [&mem, this] (cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(addr, mem);
        cpu_6502::Byte origA = A;

        cpu_6502::Word sum = A - val - SF.C;
        A = sum & 0xFF;
        UpdateZeroAndNegativeFlags(A);
        SF.C = sum > 0xFF;
        SF.V = !((origA ^ val) & 0b10000000) && ((A ^ val) & 0b10000000);
    };

    auto CheckPCCrossedPageBoundary = [&nCycles, this](cpu_6502::Word OldPC) {
        return ((OldPC >> 8) != (PC >> 8)) ? true : false;
    };

    using namespace cpu_6502;

    while (nCycles > 0) {
        cpu_6502::Byte instruction = FetchByte(mem);
        switch (instruction) {
            // Add and subtract
            case INS_ADC_IM: {
                cpu_6502::Byte val = FetchByte(mem);
                cpu_6502::Byte origA = A;

                cpu_6502::Word sum = A + val + SF.C;
                A = sum & 0xFF;
                UpdateZeroAndNegativeFlags(A);
                SF.C = sum > 0xFF;
                SF.V = !((origA ^ val) & 0b10000000) && ((A ^ val) & 0b10000000);

                nCycles -= 2;
            } break;
            case INS_ADC_ZP: {
                ADC(AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_ADC_ZPX: {
                ADC(AddressingZeroPageX(mem));
                nCycles -= 4;
            } break;
            case INS_ADC_AB:
                ADC(AddressingAbsolute(mem));
                nCycles -= 4;
            break;
            case INS_ADC_ABX: {
                Word oldPC = PC;
                ADC(AddressingAbsoluteX(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_ADC_ABY: {
                Word oldPC = PC;
                ADC(AddressingAbsoluteY(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_ADC_IDX: {
                ADC(AddressingIndexedIndirect(mem));
                nCycles -= 6;
            } break;
            case INS_ADC_IDY: {
                Word oldPC = PC;
                ADC(AddressingIndirectIndexed(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            case INS_SBC_IM: {
                cpu_6502::Byte val = FetchByte(mem);
                cpu_6502::Byte origA = A;

                cpu_6502::Word sum = A - val - SF.C;
                A = sum & 0xFF;
                UpdateZeroAndNegativeFlags(A);
                SF.C = sum > 0xFF;
                SF.V = !((origA ^ val) & 0b10000000) && ((A ^ val) & 0b10000000);
                nCycles -= 2;
            } break;
            case INS_SBC_ZP: {
                SBC(AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_SBC_ZPX: {
                SBC(AddressingZeroPageX(mem));
                nCycles -= 4;
            } break;
            case INS_SBC_AB: {
                SBC(AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_SBC_ABX: {
                Word oldPC = PC;
                SBC(AddressingAbsoluteX(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_SBC_ABY: {
                Word oldPC = PC;
                SBC(AddressingAbsoluteY(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_SBC_IDX: {
                SBC(AddressingIndexedIndirect(mem));
                nCycles -= 6;
            } break;
            case INS_SBC_IDY: {
                Word oldPC = PC;
                SBC(AddressingIndirectIndexed(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            //Compare instructions
            case INS_CPX_IM: {
                cpu_6502::Byte val = FetchByte(mem);
                SF.C = X >= val;
                SF.Z = (X == val);
                SF.N = ((X - val) & 0b10000000) > 0;
                nCycles -= 2;
            } break;
            case INS_CPX_ZP: {
                Compare(X, AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_CPX_AB: {
                Compare(X, AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_CPY_IM: {
                cpu_6502::Byte val = FetchByte(mem);
                SF.C = Y >= val;
                SF.Z = (Y == val);
                SF.N = ((Y - val) & 0b10000000) > 0;
                nCycles -= 2;
            } break;
            case INS_CPY_ZP: {
                Compare(Y, AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_CPY_AB: {
                Compare(Y, AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_CMP_IM: {
                cpu_6502::Byte val = FetchByte(mem);
                SF.C = A >= val;
                SF.Z = (A == val);
                SF.N = ((A - val) & 0b10000000) > 0;
                nCycles -= 2;
            } break;
            case INS_CMP_ZP: {
                Compare(A, AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_CMP_ZPX: {
                Compare(A, AddressingZeroPageX(mem));
                nCycles -= 4;
            } break;
            case INS_CMP_AB: {
                Compare(A, AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_CMP_ABX: {
                Word oldPC = PC;
                Compare(A, AddressingAbsoluteX(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_CMP_ABY: {
                Word oldPC = PC;
                Compare(A, AddressingAbsoluteY(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_CMP_IDX: {
                Compare(A, AddressingIndexedIndirect(mem));
                nCycles -= 6;
            } break;
            case INS_CMP_IDY: {
                Word oldPC = PC;
                Compare(A, AddressingIndirectIndexed(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            // Branch Functions
            case INS_BCC: {
                Word oldPC = PC;
                if(Branch(SF.C, false)) 
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BCS: {
                Word oldPC = PC;
                if(Branch(SF.C, true))
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BEQ: {
                Word oldPC = PC;
                if(Branch(SF.Z, true))
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BMI: {
                Word oldPC = PC;
                if(Branch(SF.N, true))
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BNE: {
                Word oldPC = PC;
                if(Branch(SF.Z, false))
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BPL: {
                Word oldPC = PC;
                if(Branch(SF.N, false))
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BVC: {
                Word oldPC = PC;
                if(Branch(SF.V, false)) 
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            case INS_BVS: {
                Word oldPC = PC;
                if(Branch(SF.V, true)) 
                    nCycles -= 1;
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 2;
                nCycles -= 2;
            } break;
            // System Functions
            case INS_NOP:
                nCycles -= 2;
            break;
            case INS_BRK: {
                PushWord(PC + 1, mem);
                PushStatusFlagsToStack(mem);
                PC = ReadWord(0xFFFE, mem);
                SF.B = 1;
                nCycles -= 7;
            } break;
            case INS_RTI: {
                PopStatusFlagsFromStack(mem);
                PC = PopWord(mem);
                SF.B = 0;
                SF.na = 0;
                nCycles -= 6;
            } break;
            // Status flag changes
            case INS_CLC: {
                SF.C = 0;
                nCycles -= 2;
            } break;
            case INS_CLD: {
                SF.D = 0;
                nCycles -= 2;
            } break;
            case INS_CLI: {
                SF.I = 0;
                nCycles -= 2;
            } break;
            case INS_CLV: {
                SF.V = 0;
                nCycles -= 2;
            } break;
            case INS_SEC: {
                SF.C = 1;
                nCycles -= 2;
            } break;
            case INS_SED: {
                SF.D = 1;
                nCycles -= 2;
            } break;
            case INS_SEI: {
                SF.I = 1;
                nCycles -= 2;
            } break;
            // Rotate Right ---------------------------------------------
            case INS_ROR_ACC: {
                cpu_6502::Byte old = A;
                A = A >> 1; // Right shift everything
                A |= SF.C << 7; // Set 7th bit to C
                SF.C = (old & 0b1); // First bit of old is new C
                nCycles -= 2;
            } break;
            case INS_ROR_ZP: {
                ROR(AddressingZeroPage(mem));
                nCycles -= 5;
            } break;
            case INS_ROR_ZPX: {
                ROR(AddressingZeroPageX(mem));
                nCycles -= 6;
            } break;
            case INS_ROR_AB: {
                ROR(AddressingAbsolute(mem));
                nCycles -= 6;
            } break;
            case INS_ROR_ABX: {
                ROR(AddressingAbsoluteX(mem));
                nCycles -= 7;
            } break;
            // Arithmetic Shift Left -------------------------------------------------
            case INS_ASL_ACC: {
                SF.C = (A & 0b10000000) > 0;
                SF.Z = (A == 0);
                A = A << 1;
                SF.N = (A & 0b10000000) > 0;
                nCycles -= 2;
            } break;
            case INS_ASL_ZP: {
                ASL(AddressingZeroPage(mem));
                nCycles -= 5;
            } break;
            case INS_ASL_ZPX: {
                ASL(AddressingZeroPageX(mem));
                nCycles -= 6;
            } break;
            case INS_ASL_AB: {
                ASL(AddressingAbsolute(mem));
                nCycles -= 6;
            } break;
            case INS_ASL_ABX: {
                ASL(AddressingAbsoluteX(mem));
                nCycles -= 7;
            } break;
            // Logical Shift Right ---------------------------------------------------
            case INS_LSR_ACC: {
                SF.C = (A & 0b1);
                A = A >> 1;
                UpdateZeroAndNegativeFlags(A);
                nCycles -= 2;
            } break;
            case INS_LSR_ZP: {
                LSR(AddressingZeroPage(mem));
                nCycles -= 5;
            } break;
            case INS_LSR_ZPX: {
                LSR(AddressingZeroPageX(mem));
                nCycles -= 6;
            } break;
            case INS_LSR_AB: {
                LSR(AddressingAbsolute(mem));
                nCycles -= 6;
            } break;
            case INS_LSR_ABX: {
                LSR(AddressingAbsoluteX(mem));
                nCycles -= 7;
            } break;
            // Rotate left ---------------------------------------------
            case INS_ROL_ACC: {
                cpu_6502::Byte old = A;
                A = A << 1;
                A |= SF.C << 0;
                SF.C = (old & 0b10000000) > 0;
                UpdateZeroAndNegativeFlags(A);
                nCycles -= 2;
            } break;
            case INS_ROL_ZP: {
                ROL(AddressingZeroPage(mem));
                nCycles -= 5;
            } break;
            case INS_ROL_ZPX: {
                ROL(AddressingZeroPageX(mem));
                nCycles -= 6;
            } break;
            case INS_ROL_AB: {
                ROL(AddressingAbsolute(mem));
                nCycles -= 6;
            } break;
            case INS_ROL_ABX: {
                ROL(AddressingAbsoluteX(mem));
                nCycles -= 7;
            } break;
            // Increment memory location ---------------------------------------------
            case INS_INC_ZP: {
                IncrementMem(AddressingZeroPage(mem));
                nCycles -= 5;
            } break;
            case INS_INC_ZPX: {
                IncrementMem(AddressingZeroPageX(mem));
                nCycles -= 6;
            } break;
            case INS_INC_AB: {
                IncrementMem(AddressingAbsolute(mem));
                nCycles -= 6;
            } break;
            case INS_INC_ABX: {
                IncrementMem(AddressingAbsoluteX(mem));
                nCycles -= 7;
            } break;
            // Decrement memory location
            case INS_DEC_ZP: {
                DecrementMem(AddressingZeroPage(mem));
                nCycles -= 5;
            } break;
            case INS_DEC_ZPX: {
                DecrementMem(AddressingZeroPageX(mem));
                nCycles -= 6;
            } break;
            case INS_DEC_AB: {
                DecrementMem(AddressingAbsolute(mem));
                nCycles -= 6;
            } break;
            case INS_DEC_ABX: {
                DecrementMem(AddressingAbsoluteX(mem));
                nCycles -= 7;
            } break;
            // Increment and decrement register
            case INS_INX: {
                IncrementReg(X);
                nCycles -= 2;
            } break;
            case INS_INY: {
                IncrementReg(Y);
                nCycles -= 2;
            } break;
            case INS_DEX: {
                DecrementReg(X);
                nCycles -= 2;
            } break;
            case INS_DEY: {
                DecrementReg(Y);
                nCycles -= 2;
            } break;

            // BIT test ---------------------------------------------------------------
            case INS_BIT_ZP: {
                BIT(AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_BIT_AB: {
                BIT(AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            // Logical, inclusive OR ---------------------------------------------------
            case INS_ORA_IM: {
                A |= FetchByte(mem);
                UpdateZeroAndNegativeFlags(A);
                nCycles -= 2;
            } break;
            case INS_ORA_ZP: {
                ORA(AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_ORA_ZPX: {
                ORA(AddressingZeroPageX(mem));
                nCycles -= 4;
            } break;
            case INS_ORA_AB: {
                ORA(AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_ORA_ABX: {
                Word oldPC = PC;
                ORA(AddressingAbsoluteX(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_ORA_ABY: {
                Word oldPC = PC;
                ORA(AddressingAbsoluteY(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_ORA_IDX: {
                ORA(AddressingIndexedIndirect(mem));
                nCycles -= 6;
            } break;
            case INS_ORA_IDY: {
                Word oldPC = PC;
                ORA(AddressingIndirectIndexed(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            // Exclusive OR instruction ---------------------------------------------------
            case INS_EOR_IM: {
                A ^= FetchByte(mem);
                UpdateZeroAndNegativeFlags(A);
                nCycles -= 2;
            } break;
            case INS_EOR_ZP: {
                EOR(AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_EOR_ZPX: {
                EOR(AddressingZeroPageX(mem));
                nCycles -= 4;
            } break;
            case INS_EOR_AB: {
                EOR(AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_EOR_ABX: {
                Word oldPC = PC;
                EOR(AddressingAbsoluteX(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_EOR_ABY: {
                Word oldPC = PC;
                EOR(AddressingAbsoluteY(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_EOR_IDX: {
                EOR(AddressingIndexedIndirect(mem));
                nCycles -= 6;
            } break;
            case INS_EOR_IDY: {
                Word oldPC = PC;
                EOR(AddressingIndirectIndexed(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            // Logical AND instruction ------------------------------------------------------
            case INS_AND_IM: {
                A &= FetchByte(mem);
                UpdateZeroAndNegativeFlags(A);
                nCycles -= 2;
            } break;
            case INS_AND_ZP: {
                lAND(AddressingZeroPage(mem));
                nCycles -= 3;
            } break;
            case INS_AND_ZPX: {
                lAND(AddressingZeroPageX(mem));
                nCycles -= 4;
            } break;
            case INS_AND_AB: {
                lAND(AddressingAbsolute(mem));
                nCycles -= 4;
            } break;
            case INS_AND_ABX: {
                Word oldPC = PC;
                lAND(AddressingAbsoluteX(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_AND_ABY: {
                Word oldPC = PC;
                lAND(AddressingAbsoluteY(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_AND_IDX: {
                lAND(AddressingIndexedIndirect(mem));
                nCycles -= 6;
            } break;
            case INS_AND_IDY: {
                Word oldPC = PC;
                lAND(AddressingIndirectIndexed(mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            // Transfer Instructions ----------------------------------------------------------
            case INS_TAX:{
                TransferRegister(A, X);
                nCycles -= 2;
            } break;
            case INS_TAY: {
                TransferRegister(A, Y);
                nCycles -= 2;
            } break;
            case INS_TSX: {
                TransferRegister(SP, X);
                nCycles -= 2;
            } break;
            case INS_TXA: {
                TransferRegister(X, A);
                nCycles -= 2;
            } break;
            case INS_TXS: {
                TransferRegister(X, SP);
                nCycles -= 2;
            } break;
            case INS_TYA: {
                TransferRegister(Y, A);
                nCycles -= 2;
            } break;
            // Various stack operations
            case INS_PHA: {
                PushByte(A, mem);
                nCycles -= 3;
            } break;
            case INS_PHP: {
                PushStatusFlagsToStack(mem);
                nCycles -= 3;
            } break;
            case INS_PLA: {
                A = PopByte(mem);
                UpdateZeroAndNegativeFlags(A);
                nCycles -= 4;
            } break;
            case INS_PLP: {
                PopStatusFlagsFromStack(mem);
                nCycles -= 4;
            } break;
            // LDA instruction ----------------------------------------------------------------
            case INS_LDA_IM: {
                WriteRegister(A, FetchByte(mem));
                nCycles -= 2;
            } break;
            case INS_LDA_ZP: {
                WriteRegister(A, ReadByte(AddressingZeroPage(mem), mem));
                nCycles -= 3;
            } break;
            case INS_LDA_ZPX: {
                WriteRegister(A, ReadByte(AddressingZeroPageX(mem), mem));
                nCycles -= 4;
            } break;
            case INS_LDA_AB: {
                WriteRegister(A, ReadByte(AddressingAbsolute(mem), mem));
                nCycles -= 4;
            } break;
            case INS_LDA_ABX: {
                Word oldPC = PC;
                WriteRegister(A, ReadByte(AddressingAbsoluteX(mem), mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_LDA_ABY: {
                Word oldPC = PC;
                WriteRegister(A, ReadByte(AddressingAbsoluteY(mem), mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            case INS_LDA_IDX: {
                WriteRegister(A, ReadByte(AddressingIndexedIndirect(mem), mem));
                nCycles -= 6;
            } break;
            case INS_LDA_IDY: {
                Word oldPC = PC;
                WriteRegister(A, ReadByte(AddressingIndirectIndexed(mem), mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 5;
            } break;
            // LDX instruction ----------------------------------------------------------------
            case INS_LDX_IM: {
                WriteRegister(X, FetchByte(mem));
                nCycles -= 2;
            } break;
            case INS_LDX_ZP: {
                WriteRegister(X, ReadByte(FetchByte(mem), mem));
                nCycles -= 3;
            } break;
            case INS_LDX_ZPY: {
                WriteRegister(X, ReadByte(AddressingZeroPageY(mem), mem));
                nCycles -= 4;
            } break;
            case INS_LDX_AB: {
                WriteRegister(X, ReadByte(AddressingAbsolute(mem), mem));
                nCycles -= 4;
            } break;
            case INS_LDX_ABY: {
                Word oldPC = PC;
                WriteRegister(X, ReadByte(AddressingAbsoluteY(mem), mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            // LDY instruction ----------------------------------------------------------------
            case INS_LDY_IM: {
                WriteRegister(Y, FetchByte(mem));
                nCycles -= 2;
            } break;
            case INS_LDY_ZP: {
                WriteRegister(Y, ReadByte(FetchByte(mem), mem));
                nCycles -= 3;
            } break;
            case INS_LDY_ZPX: {
                WriteRegister(Y, ReadByte(AddressingZeroPageY(mem), mem));
                nCycles -= 4;
            } break;
            case INS_LDY_AB: {
                WriteRegister(Y, ReadByte(AddressingAbsolute(mem), mem));
                nCycles -= 4;
            } break;
            case INS_LDY_ABX: {
                Word oldPC = PC;
                WriteRegister(Y, ReadByte(AddressingAbsoluteY(mem), mem));
                if(CheckPCCrossedPageBoundary(oldPC))
                    nCycles -= 1;
                nCycles -= 4;
            } break;
            // STA Instruction -----------------------------------------------------------------
            case INS_STA_ZP: {
                WriteToMemFromRegister(A, FetchByte(mem), mem);
                nCycles -= 3;
            } break;
            case INS_STA_ZPX: {
                WriteToMemFromRegister(A, AddressingZeroPageX(mem), mem);
                nCycles -= 4;
            } break;
            case INS_STA_AB: {
                WriteToMemFromRegister(A, FetchWord(mem), mem);
                nCycles -= 4;
            } break;
            case INS_STA_ABX: {
                WriteToMemFromRegister(A, AddressingAbsoluteX(mem), mem);
                nCycles -= 5;
            } break;
            case INS_STA_ABY: {
                WriteToMemFromRegister(A, AddressingAbsoluteY(mem), mem);
                nCycles -= 5;
            } break;
            case INS_STA_IDX:{ 
                WriteToMemFromRegister(A, AddressingIndexedIndirect(mem), mem);
                nCycles -= 6;
            } break;
             case INS_STA_IDY: {
                WriteToMemFromRegister(A, AddressingIndirectIndexed(mem), mem);
                nCycles -= 6;
             } break;
            // STX instruction ------------------------------------------------------------------
            case INS_STX_ZP: {
                WriteToMemFromRegister(X, FetchByte(mem), mem);
                nCycles -= 3;
            } break;
            case INS_STX_ZPY: {
                WriteToMemFromRegister(X, AddressingZeroPageY(mem), mem);
                nCycles -= 4;
            } break;
            case INS_STX_AB: {
                WriteToMemFromRegister(X, FetchWord(mem), mem);
                nCycles -= 4;
            } break;
            // STY instruction ------------------------------------------------------------------
            case INS_STY_ZP: {
                WriteToMemFromRegister(Y, FetchByte(mem), mem);
                nCycles -= 3;
            } break;
            case INS_STY_ZPX: {
                WriteToMemFromRegister(Y, AddressingZeroPageX(mem), mem);
                nCycles -= 4;
            } break;
            case INS_STY_AB: {
                WriteToMemFromRegister(Y, FetchWord(mem), mem);
                nCycles -= 4;
            } break;
            // JSR instruction ------------------------------------------------------------------
            case INS_JSR: {
                cpu_6502::Word target = FetchWord(mem);
                cpu_6502::Word pcMinusOne = PC - 1;
                PushWord(pcMinusOne, mem);
                PC = target;
                nCycles -= 6;
            } break;
            case INS_RTS: {
                // It pulls the program counter (minus one) from the stack.
                cpu_6502::Word PCPlusOne = PopWord(mem) + 1;
                PC = PCPlusOne;
                nCycles -= 6;
            } break;
            case INS_JMP_AB: 
                PC = AddressingAbsolute(mem);
                nCycles -= 3;
            break;
            case INS_JMP_ID:
                PC = AddressingIndirect(mem);
                nCycles -= 5;
            break;
            default:
                std::cout << "Instruction: " << std::hex << unsigned(instruction) << " not handled!\n" ;     
        };
    }
}

void cpu_6502::CPU::PushStatusFlagsToStack(cpu_6502::Mem &mem) {
    PushByte(PSF, mem);
}

void cpu_6502::CPU::PopStatusFlagsFromStack(cpu_6502::Mem &mem) {
    PSF = PopByte(mem);
}

void cpu_6502::CPU::debugReport() {
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

void cpu_6502::CPU::UpdateZeroAndNegativeFlags(cpu_6502::Byte &reg) {
    SF.Z = (reg == 0);
    SF.N = (reg & 0b10000000) > 0;
}

cpu_6502::Byte cpu_6502::CPU::FetchByte(cpu_6502::Mem &mem) {
    cpu_6502::Byte ins = mem[PC];
    PC++;
    return ins;
}

cpu_6502::Byte cpu_6502::CPU::ReadByte(cpu_6502::Word addr, cpu_6502::Mem &mem) {
    cpu_6502::Byte ins = mem[addr];
    return ins;
}

cpu_6502::Word cpu_6502::CPU::FetchWord(cpu_6502::Mem &mem) {
    // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
    // BIT COMES FIRST.
    cpu_6502::Word Data = mem[PC];
    PC++;

    Data |= (mem[PC] << 8);
    PC++;

    return Data;
}

cpu_6502::Word cpu_6502::CPU::ReadWord(cpu_6502::Word addr, cpu_6502::Mem &mem) {
    // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
    // BIT COMES FIRST.
    cpu_6502::Word Data = mem[addr];
    addr++;
    Data |= (mem[addr] << 8);

    return Data;
}

void cpu_6502::CPU::WriteWord(cpu_6502::Word dta, unsigned int addr, cpu_6502::Mem &mem) {
    mem[addr] = dta & 0xFF;
    mem[addr+1] = (dta >> 8);
}

void cpu_6502::CPU::WriteByte(cpu_6502::Byte data, unsigned int addr, cpu_6502::Mem &mem) {
    mem[addr] = data;
}

void cpu_6502::CPU::WriteToMemFromRegister(cpu_6502::Byte &reg, cpu_6502::Word addr, cpu_6502::Mem &mem) {
    mem[addr] = reg;
}

void cpu_6502::CPU::WriteRegister(cpu_6502::Byte &reg, cpu_6502::Byte value) {
    reg = value;
    UpdateZeroAndNegativeFlags(reg);
}

cpu_6502::Word cpu_6502::CPU::SPToAddr() { return SP + 0x100; }

cpu_6502::Byte cpu_6502::CPU::PopByte(cpu_6502::Mem &mem) {
    SP++;
    cpu_6502::Byte value = ReadByte(SPToAddr(), mem);
    return value;
}

cpu_6502::Word cpu_6502::CPU::PopWord(cpu_6502::Mem &mem) {
    cpu_6502::Word value = mem[SPToAddr()+1];
    SP++;
    value |= (mem[SPToAddr()+1] << 8);

    SP++;
    return value;
}

void cpu_6502::CPU::PushByte(cpu_6502::Byte value, cpu_6502::Mem &mem) {
    mem[SPToAddr()] = value;
    SP--;
}

void cpu_6502::CPU::PushWord(cpu_6502::Word value, cpu_6502::Mem &mem) {
    mem[SPToAddr()] = value >> 8;
    SP--;
    mem[SPToAddr()] = value & 0xFF;
    SP--;
}

// Addressing Mode Functions
// See (https://github.com/ejnAjaK3VgnnHBLk/6502-em/pull/20#issue-988360270) for why I don't implement
//      some addressing modes here!
cpu_6502::Byte cpu_6502::CPU::AddressingZeroPage(cpu_6502::Mem &mem) {
    // zero page addressing mode has only an 8 bit address operand
    return FetchByte(mem);
}

cpu_6502::Byte cpu_6502::CPU::AddressingZeroPageX(cpu_6502::Mem &mem) {
    // taking the 8 bit zero page address from the instruction and adding the current value of 
    // the X register to it
    cpu_6502::Byte zpAddress = FetchByte(mem);
    zpAddress += X;
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; }
    return zpAddress;
}

cpu_6502::Byte cpu_6502::CPU::AddressingZeroPageY(cpu_6502::Mem &mem) {
    // taking the 8 bit zero page address from the instruction and adding the current value of 
    // the Y register to it
    cpu_6502::Byte zpAddress = FetchByte(mem);
    zpAddress += Y; 
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; }
    return zpAddress;
}

cpu_6502::Word cpu_6502::CPU::AddressingAbsolute(cpu_6502::Mem &mem) {
    // contain a full 16 bit address to identify the target location
    return FetchWord(mem);
}

cpu_6502::Word cpu_6502::CPU::AddressingAbsoluteX(cpu_6502::Mem &mem) {
    // taking the 16 bit address from the instruction and added the contents of the X register
    cpu_6502::Word addr = FetchWord(mem);
    addr += X; // Add X register to the fetched address
    return addr;
}

cpu_6502::Word cpu_6502::CPU::AddressingAbsoluteY(cpu_6502::Mem &mem) {
    // same as the previous mode only with the contents of the Y register
    cpu_6502::Word addr = FetchWord(mem);
    addr += Y; // Add X register to the fetched address
    return addr;
}

cpu_6502::Word cpu_6502::CPU::AddressingIndirect(cpu_6502::Mem &mem) {
    // The instruction contains a 16 bit address which identifies the location of the least 
    //significant byte of another 16 bit memory address which is the real target of the instruction
    cpu_6502::Word addr = FetchWord(mem);
    return ReadWord(addr, mem);

}

cpu_6502::Word cpu_6502::CPU::AddressingIndexedIndirect(cpu_6502::Mem &mem) {
    // The address of the table is taken from the instruction and the X register added to it (with 
    // zero page wrap around) to give the location of the least significant byte of the target address.
    cpu_6502::Byte addr = FetchByte(mem);
    addr += X;
    return ReadWord(addr, mem);
}

cpu_6502::Word cpu_6502::CPU::AddressingIndirectIndexed(cpu_6502::Mem &mem) {
    // In instruction contains the zero page location of the least significant byte of 16 bit address. 
    // The Y register is dynamically added to this value to generated the actual target address for operation.
    cpu_6502::Byte addr = FetchByte(mem);
    addr += Y;
    return ReadWord(addr, mem);
}   

void cpu_6502::CPU::Reset(cpu_6502::Mem &mem) {
    PC = 0xFFFC;             // Initialize program counter to 0xFFC
    SP = 0xFF;            // Inititalize stack pointer to 0x01FF
    SF.C = SF.Z = SF.I = SF.D = SF.B = SF.V = SF.N = 0; // Reset status flags
    A = X = Y = 0;          // Reset registers
    mem.Init();             // Reset memory
}

void cpu_6502::CPU::TransferRegister(cpu_6502::Byte &src, cpu_6502::Byte &dest) {
    dest = src;
    UpdateZeroAndNegativeFlags(dest);
}

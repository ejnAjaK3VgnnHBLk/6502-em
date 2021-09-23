#include "cpu_6502.hpp"

/*
 *ADC AND ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC
 *CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP
 *JSR LDA LDX LDY LSR NOP ORA PHA PHP PLA PLP ROL ROR RTI
 *RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA
 */

void cpu_6502::CPU::Execute(unsigned int nCycles, cpu_6502::Mem &mem) {
    auto lAND = [&nCycles, &mem, this](cpu_6502::Word addr) {
        A &= ReadByte(nCycles, addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto EOR = [&nCycles, &mem, this](cpu_6502::Word addr) {
        A ^= ReadByte(nCycles, addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto ORA = [&nCycles, &mem, this](cpu_6502::Word addr) {
        A |= ReadByte(nCycles, addr, mem);
        UpdateZeroAndNegativeFlags(A);
    };

    auto BIT = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(nCycles, addr, mem);
        SF.V = (val & 0b1000000) > 0;
        SF.N = (val & 0b10000000) > 0;
        SF.Z = (A & val) == 0;
    };

    auto IncrementMem = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte xd = ReadByte(nCycles, addr, mem);
        WriteByte(++xd, addr, nCycles, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto IncrementReg = [&nCycles, &mem, this](cpu_6502::Byte &reg) {
        reg += 1;
        UpdateZeroAndNegativeFlags(reg);
    };

    auto DecrementMem = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte xd = ReadByte(nCycles, addr, mem);
        WriteByte(--xd, addr, nCycles, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto DecrementReg = [&nCycles, &mem, this](cpu_6502::Byte &reg) {
        reg -= 1;
        UpdateZeroAndNegativeFlags(reg);
    };

    auto ASL = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(nCycles, addr, mem);
        SF.C = (val & 0b10000000) > 0;
        SF.Z = (val == 0);
        WriteByte(val << 1, addr, nCycles, mem);
        SF.N = (mem[addr] & 0b10000000) > 0;
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto LSR = [&nCycles, &mem, this](cpu_6502::Word addr) {
        SF.C = (mem[addr] & 0b1);
        WriteByte(mem[addr] >> 1, addr, nCycles, mem);
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto ROL = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte old = ReadByte(nCycles, addr, mem);
        WriteByte(mem[addr] << 1, addr, nCycles, mem);
        cpu_6502::Byte val = ReadByte(nCycles, addr, mem) | SF.C << 0;
        WriteByte(val, addr, nCycles, mem);
        SF.C = (old & 0b10000000) > 0;
        UpdateZeroAndNegativeFlags(mem[addr]);
    };

    auto ROR = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte old = ReadByte(nCycles, addr, mem);
        WriteByte(mem[addr] >> 1, addr, nCycles, mem); // Right shift everything
        cpu_6502::Byte asdf = ReadByte(nCycles, addr, mem) ;
        asdf |= asdf << 7;
        WriteByte(asdf, addr, nCycles, mem);
        SF.C = (old & 0b1);
        UpdateZeroAndNegativeFlags(asdf);
    };

    auto Branch = [&nCycles, &mem, this](bool test, bool val) {
        cpu_6502::Byte displacement = FetchByte(nCycles, mem);
        if (test == val) {
            cpu_6502::Word oldPC = PC;

            PC+=displacement;
            nCycles--;

            if ((oldPC >> 8) != (PC >> 8))
                nCycles--;
        }
    };

    auto Compare = [&nCycles, &mem, this](cpu_6502::Byte &reg, cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(nCycles, addr, mem);
        SF.C = reg >= val;
        SF.Z = (reg == val);
        SF.N = ((reg - val) & 0b10000000) > 0;
    };

    auto ADC = [&nCycles, &mem, this](cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(nCycles, addr, mem);
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

    auto SBC = [&nCycles, &mem, this] (cpu_6502::Word addr) {
        cpu_6502::Byte val = ReadByte(nCycles, addr, mem);
        cpu_6502::Byte origA = A;

        cpu_6502::Word sum = A - val - SF.C;
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

    while (nCycles > 0) {
        cpu_6502::Byte instruction = FetchByte(nCycles, mem);
        switch (instruction) {
            // Add and subtract
            case INS_ADC_IM: {
                cpu_6502::Byte val = FetchByte(nCycles, mem);
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
            } break;
            case INS_ADC_ZP:
                ADC(AddressingZeroPage(nCycles, mem));
            break;
            case INS_ADC_ZPX:
                ADC(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_ADC_AB:
                ADC(AddressingAbsolute(nCycles, mem));
            break;
            case INS_ADC_ABX:
                ADC(AddressingAbsoluteX(nCycles, mem));
            break;
            case INS_ADC_ABY:
                ADC(AddressingAbsoluteY(nCycles, mem));
            break;
            case INS_ADC_IDX:
                ADC(AddressingIndexedIndirect(nCycles, mem));
            break;
            case INS_ADC_IDY:
                ADC(AddressingIndirectIndexed(nCycles, mem));
            break;
            case INS_SBC_IM: {
                cpu_6502::Byte val = FetchByte(nCycles, mem);
                cpu_6502::Byte origA = A;

                cpu_6502::Word sum = A - val - SF.C;
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
            } break;
            case INS_SBC_ZP:
                SBC(AddressingZeroPage(nCycles, mem));
            break;
            case INS_SBC_ZPX:
                SBC(AddressingZeroPageX(nCycles, mem));
            break;
            case INS_SBC_AB:
                SBC(AddressingAbsolute(nCycles, mem));
            break;
            case INS_SBC_ABX:
                SBC(AddressingAbsoluteX(nCycles, mem));
            break;
            case INS_SBC_ABY:
                SBC(AddressingAbsoluteY(nCycles, mem));
            break;
            case INS_SBC_IDX:
                SBC(AddressingIndexedIndirect(nCycles, mem));
            break;
            case INS_SBC_IDY:
                SBC(AddressingIndirectIndexed(nCycles, mem));
            break;
            //Compare instructions
            case INS_CPX_IM: {
                cpu_6502::Byte val = FetchByte(nCycles, mem);
                SF.C = X >= val;
                SF.Z = (X == val);
                SF.N = ((X - val) & 0b10000000) > 0;
            } break;
            case INS_CPX_ZP:
                Compare(X, AddressingZeroPage(nCycles, mem));
            break;
            case INS_CPX_AB:
                Compare(X, AddressingAbsolute(nCycles, mem));
            break;
            case INS_CPY_IM: {
                cpu_6502::Byte val = FetchByte(nCycles, mem);
                SF.C = Y >= val;
                SF.Z = (Y == val);
                SF.N = ((Y - val) & 0b10000000) > 0;
            } break;
            case INS_CPY_ZP:
                Compare(Y, AddressingZeroPage(nCycles, mem));
            break;
            case INS_CPY_AB:
                Compare(Y, AddressingAbsolute(nCycles, mem));
            break;
            case INS_CMP_IM: {
                cpu_6502::Byte val = FetchByte(nCycles, mem);
                SF.C = A >= val;
                SF.Z = (A == val);
                SF.N = ((A - val) & 0b10000000) > 0;
            } break;
            case INS_CMP_ZP:
                Compare(A, AddressingZeroPage(nCycles, mem));
            break;
            case INS_CMP_ZPX:
                Compare(A, AddressingZeroPageX(nCycles, mem));
            break;
            case INS_CMP_AB:
                Compare(A, AddressingAbsolute(nCycles, mem));
            break;
            case INS_CMP_ABX:
                Compare(A, AddressingAbsoluteX(nCycles, mem));
            break;
            case INS_CMP_ABY:
                Compare(A, AddressingAbsoluteY(nCycles, mem));
            break;
            case INS_CMP_IDX:
                Compare(A, AddressingIndexedIndirect(nCycles, mem));
            break;
            case INS_CMP_IDY:
                Compare(A, AddressingIndirectIndexed(nCycles, mem));
            break;
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
                cpu_6502::Byte old = A;
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
                cpu_6502::Byte old = A;
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
                cpu_6502::Word target = FetchWord(nCycles, mem);
                cpu_6502::Word pcMinusOne = PC - 1;
                PushWord(nCycles, pcMinusOne, mem);
                PC = target;
            } break;
            case INS_RTS: {
                // It pulls the program counter (minus one) from the stack.
                cpu_6502::Word PCPlusOne = PopWord(nCycles, mem) + 1;
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

void cpu_6502::CPU::PushStatusFlagsToStack(unsigned int &nCycles, cpu_6502::Mem &mem) {
    PushByte(nCycles, PSF, mem);
}

void cpu_6502::CPU::PopStatusFlagsFromStack(unsigned int &nCycles, cpu_6502::Mem &mem) {
    PSF = PopByte(nCycles, mem);
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

cpu_6502::Byte cpu_6502::CPU::FetchByte(unsigned int &nCycles, cpu_6502::Mem &mem) {
    cpu_6502::Byte ins = mem[PC];
    PC++;
    nCycles--;
    return ins;
}

cpu_6502::Byte cpu_6502::CPU::ReadByte(unsigned int &nCycles, cpu_6502::Word addr, cpu_6502::Mem &mem) {
    cpu_6502::Byte ins = mem[addr];
    nCycles--;
    return ins;
}

cpu_6502::Word cpu_6502::CPU::FetchWord(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
    // BIT COMES FIRST.
    cpu_6502::Word Data = mem[PC];
    PC++;

    Data |= (mem[PC] << 8);
    PC++;
    nCycles-=2;

    return Data;
}

cpu_6502::Word cpu_6502::CPU::ReadWord(unsigned int &nCycles, cpu_6502::Word addr, cpu_6502::Mem &mem) {
    // Remember the 6502 is LITTLE ENDIAN, MEANING THE LEAST SIGNIFICANT
    // BIT COMES FIRST.
    cpu_6502::Word Data = mem[addr];
    addr++;
    Data |= (mem[addr] << 8);
    nCycles-=2;

    return Data;
}

void cpu_6502::CPU::WriteWord(cpu_6502::Word dta, unsigned int addr, unsigned int &cycles, cpu_6502::Mem &mem) {
    mem[addr] = dta & 0xFF;
    mem[addr+1] = (dta >> 8);
    cycles -= 2;
}

void cpu_6502::CPU::WriteByte(cpu_6502::Byte data, unsigned int addr, unsigned int &nCycles, cpu_6502::Mem &mem) {
    mem[addr] = data;
    nCycles--;
}

void cpu_6502::CPU::WriteToMemFromRegister(cpu_6502::Byte &reg, cpu_6502::Word addr, unsigned int& nCycles, cpu_6502::Mem &mem) {
    mem[addr] = reg;
    nCycles--;
}

void cpu_6502::CPU::WriteRegister(cpu_6502::Byte &reg, cpu_6502::Byte value) {
    reg = value;
    UpdateZeroAndNegativeFlags(reg);
}

cpu_6502::Word cpu_6502::CPU::SPToAddr() { return SP + 0x100; }

cpu_6502::Byte cpu_6502::CPU::PopByte(unsigned int &nCycles, cpu_6502::Mem &mem) {
    SP++;
    cpu_6502::Byte value = ReadByte(nCycles, SPToAddr(), mem);
    nCycles--;
    return value;
}

cpu_6502::Word cpu_6502::CPU::PopWord(unsigned int &nCycles, cpu_6502::Mem &mem) {
    cpu_6502::Word value = mem[SPToAddr()+1];
    SP++;
    value |= (mem[SPToAddr()+1] << 8);
    nCycles-=2;

    SP++;
    nCycles -= 2;
    return value;
}

void cpu_6502::CPU::PushByte(unsigned int &nCycles, cpu_6502::Byte value, cpu_6502::Mem &mem) {
    mem[SPToAddr()] = value;
    SP--;
    nCycles--;
}

void cpu_6502::CPU::PushWord(unsigned int &nCycles, cpu_6502::Word value, cpu_6502::Mem &mem) {
    mem[SPToAddr()] = value >> 8;
    SP--;
    mem[SPToAddr()] = value & 0xFF;
    SP--;
    nCycles -= 2;
}

// Addressing Mode Functions
// See (https://github.com/ejnAjaK3VgnnHBLk/6502-em/pull/20#issue-988360270) for why I don't implement
//      some addressing modes here!
cpu_6502::Byte cpu_6502::CPU::AddressingZeroPage(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // zero page addressing mode has only an 8 bit address operand
    return FetchByte(nCycles, mem);
}

cpu_6502::Byte cpu_6502::CPU::AddressingZeroPageX(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // taking the 8 bit zero page address from the instruction and adding the current value of 
    // the X register to it
    cpu_6502::Byte zpAddress = FetchByte(nCycles, mem);
    zpAddress += X;
    nCycles--; 
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; nCycles--; } 
    return zpAddress;
}

cpu_6502::Byte cpu_6502::CPU::AddressingZeroPageY(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // taking the 8 bit zero page address from the instruction and adding the current value of 
    // the Y register to it
    cpu_6502::Byte zpAddress = FetchByte(nCycles, mem);
    zpAddress += Y; 
    nCycles--; 
    if(zpAddress >= 0xFF) { zpAddress -= 0x100; nCycles--; } 
    return zpAddress;
}

cpu_6502::Word cpu_6502::CPU::AddressingAbsolute(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // contain a full 16 bit address to identify the target location
    return FetchWord(nCycles, mem);
}

cpu_6502::Word cpu_6502::CPU::AddressingAbsoluteX(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // taking the 16 bit address from the instruction and added the contents of the X register
    cpu_6502::Word addr = FetchWord(nCycles, mem);
    addr += X; // Add X register to the fetched address
    nCycles--; // Addition takes one cycle
    return addr;
}

cpu_6502::Word cpu_6502::CPU::AddressingAbsoluteY(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // same as the previous mode only with the contents of the Y register
    cpu_6502::Word addr = FetchWord(nCycles, mem);
    addr += Y; // Add X register to the fetched address
    nCycles--; // Addition takes one cycle
    return addr;
}

cpu_6502::Word cpu_6502::CPU::AddressingIndirect(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // The instruction contains a 16 bit address which identifies the location of the least 
    //significant byte of another 16 bit memory address which is the real target of the instruction
    cpu_6502::Word addr = FetchWord(nCycles, mem);
    return ReadWord(nCycles, addr, mem);

}

cpu_6502::Word cpu_6502::CPU::AddressingIndexedIndirect(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // The address of the table is taken from the instruction and the X register added to it (with 
    // zero page wrap around) to give the location of the least significant byte of the target address.
    cpu_6502::Byte addr = FetchByte(nCycles, mem);
    addr += X;
    nCycles--;
    return ReadWord(nCycles, addr, mem);
}

cpu_6502::Word cpu_6502::CPU::AddressingIndirectIndexed(unsigned int &nCycles, cpu_6502::Mem &mem) {
    // In instruction contains the zero page location of the least significant byte of 16 bit address. 
    // The Y register is dynamically added to this value to generated the actual target address for operation.
    cpu_6502::Byte addr = FetchByte(nCycles, mem);
    addr += Y;
    nCycles--;
    return ReadWord(nCycles, addr, mem);
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

#include "gtest/gtest.h"
#include "6502src/cpu.hpp"

class InstructionTest1 : public ::testing::Test {
    public:
        CPU cpu;
        Mem mem;

    void SetUp() override {
        // Called immediately after the constructor
        cpu.Reset( mem );
        cpu.PC = 0x0000;
        EXPECT_EQ(cpu.PC, 0x0);
    }

    void TearDown() override {
        // Called immediately after the test
    }
};
/*
    INS_JSR
 */
TEST_F(InstructionTest1, TestLDAImmediate) {
 /* Start - Inline test */
    mem[0x0000] = cpu.INS_LDA_IM;
    mem[0x0001] = 0x42;

    cpu.Execute(4, mem);

    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDAZeroPage) {
    mem[0x0000] = cpu.INS_LDA_ZP;
    mem[0x0001] = 0x69;

    mem[0x0069] = 0x49;

    cpu.Execute(3, mem);

    EXPECT_EQ( cpu.A, 0x49);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDAZeroPageX) {
    cpu.X = 0x1;
    mem[0x0000] = cpu.INS_LDA_ZPX;
    mem[0x0001] = 0x43;

    mem[0x0044] = 0x50;

    cpu.Execute(5, mem);

    EXPECT_EQ( cpu.A, 0x50);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDAZeroPageXWhenItWraps) {
    cpu.X = 0xFF;
    mem[0x0000] = cpu.INS_LDA_ZPX;
    mem[0x0001] = 0x80;

    mem[0x007F] = 0x50;

    cpu.Execute(5, mem);

    EXPECT_EQ( cpu.A, 0x50);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDAAbsolute) {
    mem[0x0000] = cpu.INS_LDA_AB;
    mem[0x0001] = 0x00;
    mem[0x0002] = 0x01;

    mem[0x0100] = 0x1;

    cpu.Execute(6, mem);

    EXPECT_EQ( cpu.A, 0x1);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);

}

TEST_F(InstructionTest1, TestLDAAbsoluteX) {
    cpu.X = 5;
    mem[0x0000] = cpu.INS_LDA_ABX;
    mem[0x0001] = 0x10;
    mem[0x0002] = 0x00;

    mem[0x0015] = 0x42;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);

}

TEST_F(InstructionTest1, TestLDAAbsoluteY) {
    cpu.Y = 5;
    mem[0x0000] = cpu.INS_LDA_ABY;
    mem[0x0001] = 0x10;
    mem[0x0002] = 0x00;

    mem[0x0015] = 0x42;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);

}

TEST_F(InstructionTest1, TestLDAIndirectX) {
    cpu.X = 0x5;
    mem[0x0000] = cpu.INS_LDA_IDX;
    mem[0x0001] = 0x41;

    mem[0x46] = 0x42;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDAIndirectY) {
    cpu.Y = 0x5;
    mem[0x0000] = cpu.INS_LDA_IDY;
    mem[0x0001] = 0x41;

    mem[0x46] = 0x42;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDXImmediate) {
    mem[0x0000] = cpu.INS_LDX_IM;
    mem[0x0001] = 0x69;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x69);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDXZeroPage) {
    mem[0x0000] = cpu.INS_LDX_ZP;
    mem[0x0001] = 0x80;

    mem[0x0080] = 0x69;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x69);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDXZeroPageY) {
    cpu.Y = 0x21;
    EXPECT_EQ(cpu.Y, 0x21);

    mem[0x0000] = cpu.INS_LDX_ZPY;
    mem[0x0001] = 0x21;

    mem[0x0042] = 0x69;

    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.X, 0x69);

    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDXAbsolute) {
    mem[0x0000] = cpu.INS_LDX_AB;
    mem[0x0001] = 0x34;
    mem[0x0002] = 0x12;

    mem[0x1234] = 0x1;

    cpu.Execute(6, mem);

    EXPECT_EQ( cpu.X, 0x1);

    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestLDXAbsoluteY) {
    cpu.Y = 5;
    mem[0x0000] = cpu.INS_LDX_ABY;
    mem[0x0001] = 0x10;
    mem[0x0002] = 0x00;

    mem[0x0015] = 0x42;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.X, 0x42);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.B);
    EXPECT_FALSE(cpu.V);
    EXPECT_FALSE(cpu.N);
}

TEST_F(InstructionTest1, TestJSR) {
    mem[0x0000] = cpu.INS_JSR;
    mem[0x0001] = 0x69;

    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.PC, 0x69);
}
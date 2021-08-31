#include "gtest/gtest.h"
#include "6502src/cpu.hpp"

class CPUTest1 : public ::testing::Test {
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
    INS_LDA_AB
    INS_LDA_ABX
    INS_LDA_ABY
    INS_LDA_IDX
    INS_LDA_IDY

    INS_LDX_IM
    INS_LDX_ZP
    INS_LDX_ZPY
    INS_LDX_ABS
    INS_LDX_ABY

    INS_JSR
 */
TEST_F(CPUTest1, TestLDAImmediate) {
 /* Start - Inline test */
    mem[0x0000] = cpu.INS_LDA_IM;
    mem[0x0001] = 0x42;

    cpu.Execute(4, mem);

    EXPECT_EQ( cpu.A, 0x42 );
}

TEST_F(CPUTest1, TestLDAZeroPage) {
    mem[0x0000] = cpu.INS_LDA_ZP;
    mem[0x0001] = 0x69;

    mem[0x0069] = 0x49;

    cpu.Execute(3, mem);

    EXPECT_EQ( cpu.A, 0x49);
}

TEST_F(CPUTest1, TestLDAZeroPageX) {
    cpu.X = 0x1;
    mem[0x0000] = cpu.INS_LDA_ZPX;
    mem[0x0001] = 0x43;

    mem[0x0044] = 0x50;

    cpu.Execute(5, mem);

    EXPECT_EQ( cpu.A, 0x50);
}

TEST_F(CPUTest1, TestLDAZeroPageXWhenItWraps) {
    cpu.X = 0xFF;
    mem[0x0000] = cpu.INS_LDA_ZPX;
    mem[0x0001] = 0x80;

    mem[0x007F] = 0x50;

    cpu.Execute(5, mem);

    EXPECT_EQ( cpu.A, 0x50);
}

TEST_F(CPUTest1, TestLDAAbsolute) {
    mem[0x0000] = cpu.INS_LDA_AB;
    mem[0x0001] = 0x10;
    mem[0x0002] = 0x00;

    mem[0x0010] = 0x1;

    cpu.Execute(6, mem);

    EXPECT_EQ( cpu.A, 0x1);

}

TEST_F(CPUTest1, TestLDAAbsoluteX) {
    cpu.X = 5;
    mem[0x0000] = cpu.INS_LDA_ABX;
    mem[0x0001] = 0x10;
    mem[0x0002] = 0x00;

    mem[0x0015] = 0x42;

    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x42);

}
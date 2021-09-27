#include "gtest/gtest.h"
#include "cpu_6502.hpp"

class ArithmeticTests : public ::testing::Test {
    public:
        cpu_6502::CPU cpu;
        cpu_6502::Mem mem;
        unsigned int nCycles = 50;

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

TEST_F(ArithmeticTests, TestCMPImmediate) {
    cpu.A = 0x42;
    mem[0x0] = cpu.INS_CMP_IM;
    mem[0x1] = 0x42;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 1);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.A = 0x43;
    mem[0x2] = cpu.INS_CMP_IM;
    mem[0x3] = 0x42;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.A = 0b11111111;
    mem[0x4] = cpu.INS_CMP_IM;
    mem[0x5] = 0b00000000;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 1);
}

TEST_F(ArithmeticTests, TestCMPZeroPage) {
    cpu.A = 0x42;
    mem[0x0] = cpu.INS_CMP_ZP;
    mem[0x1] = 0x24;
    mem[0x24] = 0x42;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 1);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.A = 0x43;
    mem[0x2] = cpu.INS_CMP_ZP;
    mem[0x3] = 0x24;
    mem[0x24] = 0x42;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.A = 0b10000000;
    mem[0x4] = cpu.INS_CMP_ZP;
    mem[0x5] = 0x24;
    mem[0x24] = 0b00000000;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 1);
}


TEST_F(ArithmeticTests, TestCPYImmediate) {
    cpu.Y = 0x42;
    mem[0x0] = cpu.INS_CPY_IM;
    mem[0x1] = 0x42;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 1);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.Y = 0x43;
    mem[0x2] = cpu.INS_CPY_IM;
    mem[0x3] = 0x42;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.Y = 0b11111111;
    mem[0x4] = cpu.INS_CPY_IM;
    mem[0x5] = 0b00000000;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 1);
}

TEST_F(ArithmeticTests, TestCPYZeroPage) {
    cpu.Y = 0x42;
    mem[0x0] = cpu.INS_CPY_ZP;
    mem[0x1] = 0x24;
    mem[0x24] = 0x42;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 1);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.Y = 0x43;
    mem[0x2] = cpu.INS_CPY_ZP;
    mem[0x3] = 0x24;
    mem[0x24] = 0x42;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.Y = 0b10000000;
    mem[0x4] = cpu.INS_CPY_ZP;
    mem[0x5] = 0x24;
    mem[0x24] = 0b00000000;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 1);
}


TEST_F(ArithmeticTests, TestCPXImmediate) {
    cpu.X = 0x42;
    mem[0x0] = cpu.INS_CPX_IM;
    mem[0x1] = 0x42;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 1);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.X = 0x43;
    mem[0x2] = cpu.INS_CPX_IM;
    mem[0x3] = 0x42;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.X = 0b11111111;
    mem[0x4] = cpu.INS_CPX_IM;
    mem[0x5] = 0b00000000;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 1);
}

TEST_F(ArithmeticTests, TestCPXZeroPage) {
    cpu.X = 0x42;
    mem[0x0] = cpu.INS_CPX_ZP;
    mem[0x1] = 0x24;
    mem[0x24] = 0x42;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 1);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.X = 0x43;
    mem[0x2] = cpu.INS_CPX_ZP;
    mem[0x3] = 0x24;
    mem[0x24] = 0x42;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 0);

    cpu.X = 0b10000000;
    mem[0x4] = cpu.INS_CPX_ZP;
    mem[0x5] = 0x24;
    mem[0x24] = 0b00000000;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 1);
    EXPECT_EQ(cpu.SF.N, 1);
}

TEST_F(ArithmeticTests, TestADCImmediate) {
    cpu.A = 0x42;
    mem[0x0] = cpu.INS_ADC_IM;
    mem[0x1] = 0x7;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.A, 0x49);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    EXPECT_FALSE(cpu.SF.V);
}

TEST_F(ArithmeticTests, TestADCImmediateWithCarry) {
    cpu.SF.C = 1;
    cpu.A = 0x42;
    mem[0x0] = cpu.INS_ADC_IM;
    mem[0x1] = 0x7;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.A, 0x4A);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    EXPECT_FALSE(cpu.SF.V);
}

TEST_F(ArithmeticTests, TestSBCImmediate) {
    cpu.A = 0x49;
    mem[0x0] = cpu.INS_SBC_IM;
    mem[0x1] = 0x7;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.A, 0x42);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
    EXPECT_FALSE(cpu.SF.V);
}

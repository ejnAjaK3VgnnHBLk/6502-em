#include "gtest/gtest.h"
#include "cpu_6502.hpp"

class IncrementDecrementTests : public ::testing::Test {
    public:
        cpu_6502::CPU cpu;
        cpu_6502::Mem mem;

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

TEST_F(IncrementDecrementTests, TestIncrementZeroPage) {
    mem[0x0000] = cpu.INS_INC_ZP;
    mem[0x0001] = 0x42;

    mem[0x42] = 0x68;

    cpu.Execute(5, mem);


    EXPECT_EQ(mem[0x42], 0x69);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.I);
    EXPECT_FALSE(cpu.SF.D);
    EXPECT_FALSE(cpu.SF.B);
    EXPECT_FALSE(cpu.SF.V);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(IncrementDecrementTests, TestDecrementZeroPage) {
    mem[0x0000] = cpu.INS_DEC_ZP;
    mem[0x0001] = 0x42;

    mem[0x42] = 0x6A;

    cpu.Execute(5, mem);


    EXPECT_EQ(mem[0x42], 0x69);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.I);
    EXPECT_FALSE(cpu.SF.D);
    EXPECT_FALSE(cpu.SF.B);
    EXPECT_FALSE(cpu.SF.V);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(IncrementDecrementTests, TestIncrementX) {
    cpu.X = 0x1;

    mem[0x0000] = cpu.INS_INX;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.X, 0x2);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.I);
    EXPECT_FALSE(cpu.SF.D);
    EXPECT_FALSE(cpu.SF.B);
    EXPECT_FALSE(cpu.SF.V);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(IncrementDecrementTests, TestDecrementX) {
    cpu.X = 0x2;

    mem[0x0000] = cpu.INS_DEX;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.X, 0x1);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.I);
    EXPECT_FALSE(cpu.SF.D);
    EXPECT_FALSE(cpu.SF.B);
    EXPECT_FALSE(cpu.SF.V);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(IncrementDecrementTests, TestIncrementY) {
    cpu.Y = 0x41;
    mem[0x0000] = cpu.INS_INY;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.Y, 0x42);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.I);
    EXPECT_FALSE(cpu.SF.D);
    EXPECT_FALSE(cpu.SF.B);
    EXPECT_FALSE(cpu.SF.V);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(IncrementDecrementTests, TestDecrementY) {
    cpu.Y = 0x1;
    mem[0x0000] = cpu.INS_DEY;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.Y, 0x0);
    EXPECT_FALSE(cpu.SF.C);
    EXPECT_TRUE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.I);
    EXPECT_FALSE(cpu.SF.D);
    EXPECT_FALSE(cpu.SF.B);
    EXPECT_FALSE(cpu.SF.V);
    EXPECT_FALSE(cpu.SF.N);
}

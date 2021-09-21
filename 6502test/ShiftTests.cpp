#include "gtest/gtest.h"
#include "cpu.hpp"

class ShiftTests : public ::testing::Test {
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

TEST_F(ShiftTests, TestASLAccumulator) {
    cpu.A = 0b10101010;

    mem[0x0000] = cpu.INS_ASL_ACC;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0b01010100);
    EXPECT_TRUE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.N);
    EXPECT_FALSE(cpu.SF.Z);
}

TEST_F(ShiftTests, TestASLZeroPage) {
    mem[0x0000] = cpu.INS_ASL_ZP;
    mem[0x0001] = 0x42;

    mem[0x42] = 0b10101010;

    cpu.Execute(5, mem);

    EXPECT_EQ(mem[0x42], 0b01010100);
    EXPECT_TRUE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.N);
    EXPECT_FALSE(cpu.SF.Z);
}

TEST_F(ShiftTests, TestLSRAccumulator) {
    cpu.A = 0b10101011;
    mem[0x0000] = cpu.INS_LSR_ACC;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0b01010101);
    EXPECT_TRUE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(ShiftTests, TestLSRZeroPage) {
    mem[0x0000] = cpu.INS_LSR_ZP;
    mem[0x0001] = 0x42;

    mem[0x42] = 0b10101011;

    cpu.Execute(5, mem);
    EXPECT_EQ(mem[0x42], 0b01010101);
    EXPECT_TRUE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(ShiftTests, TestROLAccumulator) {
    cpu.A = 0b10101010;
    cpu.SF.C = 1;

    mem[0x0000] = cpu.INS_ROL_ACC;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0b01010101);
    EXPECT_TRUE(cpu.SF.C);
}

TEST_F(ShiftTests, TestROLZeroPage) {
    cpu.SF.C = 1;

    mem[0x0000] = cpu.INS_ROL_ZP;
    mem[0x0001] = 0x42;

    mem[0x42] = 0b10101010; // 0xaa

    cpu.Execute(5, mem);

    EXPECT_EQ(mem[0x42], 0b01010101);
    EXPECT_TRUE(cpu.SF.C);
    EXPECT_FALSE(cpu.SF.Z);
    EXPECT_FALSE(cpu.SF.N);
}

TEST_F(ShiftTests, TestRORAccumulator) {
    cpu.SF.C = 1;
    cpu.A = 0b10101010;

    mem[0x0000] = cpu.INS_ROR_ACC;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0b11010101);
    EXPECT_FALSE(cpu.SF.C);

}

TEST_F(ShiftTests, TestRORZeroPage) {
    cpu.SF.C = 1;

    mem[0x0000] = cpu.INS_ROR_ZP;
    mem[0x0001] = 0x42;

    mem[0x42] = 0b10101010;
    cpu.Execute(5, mem);

    EXPECT_EQ(mem[0x42], 0b11010101);
    EXPECT_FALSE(cpu.SF.C);
}

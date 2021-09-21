#include "gtest/gtest.h"
#include "cpu.hpp"

class LogicalTests : public ::testing::Test {
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

TEST_F(LogicalTests, TestANDImmediate) {
    cpu.A = 0xFF;

    mem[0x0000] = cpu.INS_AND_IM;
    mem[0x0001] = 0xAA;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0xaa);
}

TEST_F(LogicalTests, TestANDZP) {
    cpu.A = 0xFF;
    mem[0x0000] = cpu.INS_AND_ZP;
    mem[0x0001] = 0x1B;
    mem[0x001B] = 0xAA;

    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

TEST_F(LogicalTests, TestANDZPX) {
    cpu.A = 0xFF;
    cpu.X = 0x10;
    mem[0x0000] = cpu.INS_AND_ZPX;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

TEST_F(LogicalTests, TestANDAbsolute) {
    cpu.A = 0xFF;
    mem[0x0000] = cpu.INS_AND_AB;
    mem[0x0001] = 0x08;
    mem[0x0002] = 0x80;
    mem[0x8008] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

TEST_F(LogicalTests, TestANDAbsoluteX) {
    cpu.A = 0xFF;
    cpu.X = 0x10;
    mem[0x0000] = cpu.INS_AND_ABX;
    mem[0x0001] = 0x40;
    mem[0x0002] = 0x80;
    mem[0x8050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

TEST_F(LogicalTests, TestANDAbsoluteY) {
    cpu.A = 0xFF;
    cpu.Y = 0x10;
    mem[0x0000] = cpu.INS_AND_ABY;
    mem[0x0001] = 0x40;
    mem[0x0002] = 0x80;
    mem[0x8050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

TEST_F(LogicalTests, TestANDIndexedIndirect) {
    cpu.A = 0xFF;
    cpu.X = 0x10;

    mem[0x0000] = cpu.INS_AND_IDX;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0x08;
    mem[0x0051] = 0x80;

    mem[0x8008] = 0xAA;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

TEST_F(LogicalTests, TestANDIndirectIndexed) {
    cpu.A = 0xFF;
    cpu.Y = 0x10;

    mem[0x0000] = cpu.INS_AND_IDY;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0x08;
    mem[0x0051] = 0x80;

    mem[0x8008] = 0xAA;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0xAA);
}

// EOR

TEST_F(LogicalTests, TestEORImmediate) {
    cpu.A = 0x1b;

    mem[0x0000] = cpu.INS_EOR_IM;
    mem[0x0001] = 0xAA;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORZP) {
    cpu.A = 0x1b;
    mem[0x0000] = cpu.INS_EOR_ZP;
    mem[0x0001] = 0x1B;
    mem[0x001B] = 0xAA;

    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORZPX) {
    cpu.A = 0x1b;
    cpu.X = 0x10;
    mem[0x0000] = cpu.INS_EOR_ZPX;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORAbsolute) {
    cpu.A = 0x1b;
    mem[0x0000] = cpu.INS_EOR_AB;
    mem[0x0001] = 0x08;
    mem[0x0002] = 0x80;
    mem[0x8008] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORAbsoluteX) {
    cpu.A = 0x1b;
    cpu.X = 0x10;
    mem[0x0000] = cpu.INS_EOR_ABX;
    mem[0x0001] = 0x40;
    mem[0x0002] = 0x80;
    mem[0x8050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORAbsoluteY) {
    cpu.A = 0x1b;
    cpu.Y = 0x10;
    mem[0x0000] = cpu.INS_EOR_ABY;
    mem[0x0001] = 0x40;
    mem[0x0002] = 0x80;
    mem[0x8050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORIndexedIndirect) {
    cpu.A = 0x1b;
    cpu.X = 0x10;

    mem[0x0000] = cpu.INS_EOR_IDX;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0x08;
    mem[0x0051] = 0x80;

    mem[0x8008] = 0xAA;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

TEST_F(LogicalTests, TestEORIndirectIndexed) {
    cpu.A = 0x1b;
    cpu.Y = 0x10;

    mem[0x0000] = cpu.INS_EOR_IDY;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0x08;
    mem[0x0051] = 0x80;

    mem[0x8008] = 0xAA;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0xb1);
}

// ORA tests

TEST_F(LogicalTests, TestORAImmediate) {
    cpu.A = 0x1b;

    mem[0x0000] = cpu.INS_ORA_IM;
    mem[0x0001] = 0xAA;

    cpu.Execute(2, mem);

    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAZP) {
    cpu.A = 0x1b;
    mem[0x0000] = cpu.INS_ORA_ZP;
    mem[0x0001] = 0x1B;
    mem[0x001B] = 0xAA;

    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAZPX) {
    cpu.A = 0x1b;
    cpu.X = 0x10;
    mem[0x0000] = cpu.INS_ORA_ZPX;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAAbsolute) {
    cpu.A = 0x1b;
    mem[0x0000] = cpu.INS_ORA_AB;
    mem[0x0001] = 0x08;
    mem[0x0002] = 0x80;
    mem[0x8008] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAAbsoluteX) {
    cpu.A = 0x1b;
    cpu.X = 0x10;
    mem[0x0000] = cpu.INS_ORA_ABX;
    mem[0x0001] = 0x40;
    mem[0x0002] = 0x80;
    mem[0x8050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAAbsoluteY) {
    cpu.A = 0x1b;
    cpu.Y = 0x10;
    mem[0x0000] = cpu.INS_ORA_ABY;
    mem[0x0001] = 0x40;
    mem[0x0002] = 0x80;
    mem[0x8050] = 0xAA;

    cpu.Execute(4, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAIndexedIndirect) {
    cpu.A = 0x1b;
    cpu.X = 0x10;

    mem[0x0000] = cpu.INS_ORA_IDX;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0x08;
    mem[0x0051] = 0x80;

    mem[0x8008] = 0xAA;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestORAIndirectIndexed) {
    cpu.A = 0x1b;
    cpu.Y = 0x10;

    mem[0x0000] = cpu.INS_ORA_IDY;
    mem[0x0001] = 0x40;
    mem[0x0050] = 0x08;
    mem[0x0051] = 0x80;

    mem[0x8008] = 0xAA;

    cpu.Execute(5, mem);
    EXPECT_EQ(cpu.A, 0xbb);
}

TEST_F(LogicalTests, TestBITZeroPage) {
    cpu.A = 0x42;
    mem[0x0000] = cpu.INS_BIT_ZP;
    mem[0x0001] = 0x42;
    mem[0x42] = 0b11000000;

    cpu.Execute(3, mem);

    EXPECT_EQ(cpu.SF.V, 1);
    EXPECT_EQ(cpu.SF.N, 1);
    EXPECT_EQ(cpu.SF.B, 0);
    EXPECT_EQ(cpu.SF.D, 0);
    EXPECT_EQ(cpu.SF.I, 0);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 0);
}

TEST_F(LogicalTests, TestBITAbsolute) {
    cpu.A = 0x42;
    mem[0x0000] = cpu.INS_BIT_AB;
    mem[0x0001] = 0x08;
    mem[0x0002] = 0x80;
    mem[0x8008] = 0b11000000;

    cpu.Execute(3, mem);

    EXPECT_EQ(cpu.SF.V, 1);
    EXPECT_EQ(cpu.SF.N, 1);
    EXPECT_EQ(cpu.SF.B, 0);
    EXPECT_EQ(cpu.SF.D, 0);
    EXPECT_EQ(cpu.SF.I, 0);
    EXPECT_EQ(cpu.SF.Z, 0);
    EXPECT_EQ(cpu.SF.C, 0);
}

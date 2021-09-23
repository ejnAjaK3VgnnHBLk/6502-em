#include "gtest/gtest.h"
#include "cpu_6502.hpp"

class BranchTests : public ::testing::Test {
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

TEST_F(BranchTests, TestBCC) {
    cpu.SF.C = false;
    EXPECT_FALSE(cpu.SF.C);
    mem[0x0] = cpu.INS_BCC;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.C = true;
    mem[0x12] = cpu.INS_BCC;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBCS) {
    cpu.SF.C = true;
    mem[0x0] = cpu.INS_BCS;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.C = false;
    mem[0x12] = cpu.INS_BCS;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBEQ) {
    cpu.SF.Z = true;
    mem[0x0] = cpu.INS_BEQ;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.Z = false;
    mem[0x12] = cpu.INS_BEQ;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBMI) {
    cpu.SF.N = true;
    mem[0x0] = cpu.INS_BMI;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.N = false;
    mem[0x12] = cpu.INS_BMI;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBNE) {
    cpu.SF.Z = false;
    mem[0x0] = cpu.INS_BNE;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.Z = true;
    mem[0x12] = cpu.INS_BNE;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBPL) {
    cpu.SF.N = false;
    mem[0x0] = cpu.INS_BPL;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.N = true;
    mem[0x12] = cpu.INS_BPL;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBVC) {
    cpu.SF.V = false;
    mem[0x0] = cpu.INS_BVC;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.V = true;
    mem[0x12] = cpu.INS_BVC;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

TEST_F(BranchTests, TestBVS) {
    cpu.SF.V = true;
    mem[0x0] = cpu.INS_BVS;
    mem[0x1] = 0x10;
    cpu.Execute(3, mem);
    EXPECT_EQ(cpu.PC, 0x12);

    cpu.SF.V = false;
    mem[0x12] = cpu.INS_BVS;
    mem[0x13] = 0x5;
    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.PC, 0x14);
}

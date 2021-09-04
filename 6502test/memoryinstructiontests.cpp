#include "gtest/gtest.h"
#include "cpu.hpp"

class MemoryInstructionTests : public ::testing::Test {
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

TEST_F(MemoryInstructionTests, TestJSR) {

    mem[0x0000] = cpu.INS_JSR;
    mem[0x0001] = 0x69;

    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.PC, 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAZeroPage) {
    cpu.A = 0x69;

    mem[0x0000] = cpu.INS_STA_ZP;
    mem[0x0001] = 0x42;

    cpu.Execute(3, mem);

    EXPECT_EQ(mem[0x0042], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAZeroPageX) {
    cpu.A = 0x69;
    cpu.X = 0x01;

    mem[0x0000] = cpu.INS_STA_ZPX;
    mem[0x0001] = 0x41;

    cpu.Execute(4, mem);

    EXPECT_EQ(mem[0x0042], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAAbsolute) {
    cpu.A = 0x69;

    mem[0x0000] = cpu.INS_STA_AB;
    mem[0x0001] = 0x34;
    mem[0x0002] = 0x12;

    cpu.Execute(4, mem);

    EXPECT_EQ(mem[0x1234], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAAbsoluteX) {
    cpu.A = 0x69;
    cpu.X = 0x01;

    mem[0x0000] = cpu.INS_STA_ABX;
    mem[0x0001] = 0x33;
    mem[0x0002] = 0x12;

    cpu.Execute(5, mem);

    EXPECT_EQ(mem[0x1234], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAAbsoluteY){
    cpu.A = 0x69;
    cpu.Y = 0x01;

    mem[0x0000] = cpu.INS_STA_ABY;
    mem[0x0001] = 0x33;
    mem[0x0002] = 0x12;

    cpu.Execute(5, mem);

    EXPECT_EQ(mem[0x1234], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAIndirectX) {
    cpu.A = 0x69;
    cpu.X = 0x01;

    mem[0x0000] = cpu.INS_STA_IDX;
    mem[0x0001] = 0x41;

    cpu.Execute(6, mem);

    EXPECT_EQ(mem[0x42], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTAIndirectY) {
    cpu.A = 0x69;
    cpu.Y = 0x01;

    mem[0x0000] = cpu.INS_STA_IDY;
    mem[0x0001] = 0x41;

    cpu.Execute(6, mem);

    EXPECT_EQ(mem[0x42], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTXZeroPage) {
    cpu.X = 0x69;

    mem[0x0000] = cpu.INS_STX_ZP;
    mem[0x0001] = 0x42;

    cpu.Execute(3, mem);

    EXPECT_EQ(mem[0x0042], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTXZeroPageY) {
    cpu.X = 0x69;
    cpu.Y = 0x01;

    mem[0x0000] = cpu.INS_STX_ZPY;
    mem[0x0001] = 0x41;

    cpu.Execute(4, mem);

    EXPECT_EQ(mem[0x0042], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTXAbsolute) {
    cpu.X = 0x69;

    mem[0x0000] = cpu.INS_STX_AB;
    mem[0x0001] = 0x34;
    mem[0x0002] = 0x12;

    cpu.Execute(4, mem);

    EXPECT_EQ(mem[0x1234], 0x69);
}



TEST_F(MemoryInstructionTests, TestSTYZeroPage) {
    cpu.Y = 0x69;

    mem[0x0000] = cpu.INS_STY_ZP;
    mem[0x0001] = 0x42;

    cpu.Execute(3, mem);

    EXPECT_EQ(mem[0x0042], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTYZeroPageX) {
    cpu.Y = 0x69;
    cpu.X = 0x01;

    mem[0x0000] = cpu.INS_STY_ZPX;
    mem[0x0001] = 0x41;

    cpu.Execute(4, mem);

    EXPECT_EQ(mem[0x0042], 0x69);
}

TEST_F(MemoryInstructionTests, TestSTYAbsolute) {
    cpu.Y = 0x69;

    mem[0x0000] = cpu.INS_STY_AB;
    mem[0x0001] = 0x34;
    mem[0x0002] = 0x12;

    cpu.Execute(4, mem);

    EXPECT_EQ(mem[0x1234], 0x69);
}
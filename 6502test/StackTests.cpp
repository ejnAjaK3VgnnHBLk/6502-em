#include "gtest/gtest.h"
#include "cpu.hpp"

class StackTests : public ::testing::Test {
    public:
        CPU cpu;
        Mem mem;
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

TEST_F(StackTests, PopByteTest) {
    mem[0x100] = 0x14;
    mem[0x101] = 0x21;

    Byte test1 = cpu.PopByte(nCycles, mem);
    Byte test2 = cpu.PopByte(nCycles, mem);

    EXPECT_EQ(test1, 0x14);
    EXPECT_EQ(test2, 0x21);
}

TEST_F(StackTests, PushByte) {
    cpu.PushByte(nCycles, 0x12, mem);
    EXPECT_EQ(mem[0x1FF], 0x12);
    EXPECT_EQ(cpu.SP, 0xFE);
}

TEST_F(StackTests, TestPHAAndPLA) {
    cpu.A = 0x42;
    mem[0x0] = cpu.INS_PHA;
    cpu.Execute(3, mem);

    EXPECT_EQ(mem[0x1FF], 0x42);
    cpu.A = 0x66;
    mem[0x1] = cpu.INS_PLA;
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.A, 0x42);
}

TEST_F(StackTests, TestPHPAndPLP) {
    cpu.PSF = 0b10101010;
    mem[0x0] = cpu.INS_PHP;

    cpu.Execute(3, mem);
    EXPECT_EQ(mem[0x1FF], 0b10101010);

    mem[0x1] = cpu.INS_PLP;
    cpu.Execute(4, mem);

    EXPECT_EQ(cpu.PSF, 0b10101010);
}

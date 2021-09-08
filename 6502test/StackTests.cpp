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
    mem[0x1FF] = 0x14;
    mem[0x100] = 0x21;

    Byte test1 = cpu.PopByte(nCycles, mem);
    Byte test2 = cpu.PopByte(nCycles, mem);

    EXPECT_EQ(test1, 0x14);
    EXPECT_EQ(test2, 0x21);
}

TEST_F(StackTests, PopWordTest) {
    cpu.SP = 0x21;
    mem[0x121] = 0x34;
    mem[0x122] = 0x12;
    Word cool = cpu.PopWord(nCycles, mem);

    EXPECT_EQ(cool, 0x1234);
}

TEST_F(StackTests, PushByte) {
    cpu.PushByte(nCycles, 0x12, mem);
    EXPECT_EQ(mem[0x1FF], 0x12);
    EXPECT_EQ(cpu.SP, 0xFE);
}
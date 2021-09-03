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
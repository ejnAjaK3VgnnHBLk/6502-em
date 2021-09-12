#include "gtest/gtest.h"
#include "cpu.hpp"

class TransferRegisterTests : public ::testing::Test {
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

TEST_F (TransferRegisterTests, TestTAX) {
    cpu.A = 0x42;
    mem[0x0000] = cpu.INS_TAX;

    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.X, 0x42);
}

TEST_F(TransferRegisterTests, TestTAY) {
    cpu.A = 0x42;
    mem[0x0000] = cpu.INS_TAY;

    cpu.Execute(2, mem);
    EXPECT_EQ(cpu.Y, 0x42);
}

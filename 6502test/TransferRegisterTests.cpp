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

TEST_F(TransferRegisterTests, TestStatusFlags) {
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.V);
    mem[0x0] = cpu.INS_SEC;
    mem[0x1] = cpu.INS_SED;
    mem[0x2] = cpu.INS_SEI;
    for(int i = 0; i<3; i++)
        cpu.Execute(1, mem);
    EXPECT_TRUE(cpu.C);
    EXPECT_TRUE(cpu.D);
    EXPECT_TRUE(cpu.I);
    mem[0x3] = cpu.INS_CLC;
    mem[0x4] = cpu.INS_CLD;
    mem[0x5] = cpu.INS_CLI;
    mem[0x6] = cpu.INS_CLV;
    for (int i = 0; i<4; i++)
        cpu.Execute(1, mem);
    EXPECT_FALSE(cpu.C);
    EXPECT_FALSE(cpu.D);
    EXPECT_FALSE(cpu.I);
    EXPECT_FALSE(cpu.V);
}

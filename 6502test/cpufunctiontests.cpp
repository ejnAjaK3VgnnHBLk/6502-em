#include "gtest/gtest.h"
#include "cpu.hpp"

class CPUFunctionTests : public ::testing::Test {
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

TEST_F(CPUFunctionTests, FetchByteTest) {
    mem[0x0000] = 0x69;
    Byte cool = cpu.FetchByte(nCycles, mem);
    EXPECT_EQ(cool, 0x69);
}

TEST_F(CPUFunctionTests, ReadByteTest) {
    mem[0x1111] = 0x69;
    Byte cool = cpu.ReadByte(nCycles, 0x1111, mem);

    EXPECT_EQ(cool, 0x69);
}

TEST_F(CPUFunctionTests, FetchWordTest) {
    mem[0x0000] = 0x34;
    mem[0x0001] = 0x12;
    Word cool = cpu.FetchWord(nCycles, mem);

    EXPECT_EQ(cool, 0x1234);
}

TEST_F(CPUFunctionTests, ReadWordTest) {
    mem[0x4321] = 0x34;
    mem[0x4322] = 0x12;
    Word cool = cpu.ReadWord(nCycles, 0x4321, mem);

    EXPECT_EQ(cool, 0x1234);
}

TEST_F(CPUFunctionTests, WriteWordTest) {
    cpu.WriteWord(0x1234, 0x4321, nCycles, mem);

    EXPECT_EQ(mem[0x4321], 0x34);
    EXPECT_EQ(mem[0x4322], 0x12);
}

TEST_F(CPUFunctionTests, WriteByteTest) {
    cpu.WriteByte(0x42, 0x4321, nCycles, mem);

    EXPECT_EQ(mem[0x4321], 0x42);
}

TEST_F(CPUFunctionTests, WriteToMemFromRegisterTest) {
    cpu.A = 0x69;

    cpu.WriteToMemFromRegister(cpu.A, 0x1234, nCycles, mem);
    EXPECT_EQ(mem[0x1234], 0x69);
}

TEST_F(CPUFunctionTests, WriteRegisterTest) {
    cpu.WriteRegister(cpu.A, 0x42);

    EXPECT_EQ(cpu.A, 0x42);
}

// Test for function Execute() is found in all the other tests

TEST_F(CPUFunctionTests, ResetTest) {
    cpu.Reset(mem);
    EXPECT_EQ(cpu.PC, 0xFFFC);
    EXPECT_EQ(cpu.SP, 0xFF);
    EXPECT_EQ(cpu.C, 0);
    EXPECT_EQ(cpu.Z, 0);
    EXPECT_EQ(cpu.I, 0);
    EXPECT_EQ(cpu.D, 0);
    EXPECT_EQ(cpu.B, 0);
    EXPECT_EQ(cpu.V, 0);
    EXPECT_EQ(cpu.N, 0);
    EXPECT_EQ(cpu.A, 0);
    EXPECT_EQ(cpu.X, 0);
    EXPECT_EQ(cpu.Y, 0);
    EXPECT_EQ(mem[0x1234], 0);
}

TEST_F(CPUFunctionTests, UpdateZeroAndNegativeFlagsTest) {
    cpu.A = 0xFF;
    cpu.UpdateZeroAndNegativeFlags(cpu.A);
    EXPECT_EQ(cpu.N, 1);
    EXPECT_EQ(cpu.Z, 0);

    cpu.A = 0x00;
    cpu.UpdateZeroAndNegativeFlags(cpu.A);
    EXPECT_EQ(cpu.N, 0);
    EXPECT_EQ(cpu.Z, 1);
}

TEST_F(CPUFunctionTests, AddressingZeroPageTest) {
    mem[0x0000] = 0x42;
    Byte zpAddr = cpu.AddressingZeroPage(nCycles, mem);
    EXPECT_EQ(zpAddr, 0x42);
}

TEST_F(CPUFunctionTests, AddressingZeroPageXTest) {
    cpu.X = 0x0F;
    mem[0x0000] = 0x80;
    Byte zpAddr = cpu.AddressingZeroPageX(nCycles, mem);
    EXPECT_EQ(zpAddr, 0x8F);
}

TEST_F(CPUFunctionTests, AddressingZeroPageXWhenItWrapsTest) {
    cpu.X = 0xFF;
    mem[0x0000] = 0x80;
    Byte zpAddr = cpu.AddressingZeroPageX(nCycles, mem);
    EXPECT_EQ(zpAddr, 0x7F);
}

TEST_F(CPUFunctionTests, AddressingZeroPageYTest) {
    cpu.Y = 0x0F;
    mem[0x0000] = 0x80;
    Byte zpAddr = cpu.AddressingZeroPageY(nCycles, mem);
    EXPECT_EQ(zpAddr, 0x8F);
}

TEST_F(CPUFunctionTests, AddressingZeroPageYWhenItWrapsTest) {
    cpu.Y = 0xFF;
    mem[0x0000] = 0x80;
    Byte zpAddr = cpu.AddressingZeroPageY(nCycles, mem);
    EXPECT_EQ(zpAddr, 0x7F);
}

TEST_F(CPUFunctionTests, AddressingAbsoluteTest) {
    mem[0x0000] = 0x34;
    mem[0x0001] = 0x12;
    Word absAddr = cpu.AddressingAbsolute(nCycles, mem);
    EXPECT_EQ(absAddr, 0x1234);
}

TEST_F(CPUFunctionTests, AddressingAbsoluteXTest) {
    cpu.X = 0x92;
    mem[0x0000] = 0x00;
    mem[0x0001] = 0x20;
    Word absAddr = cpu.AddressingAbsoluteX(nCycles, mem);
    EXPECT_EQ(absAddr, 0x2092);
}

TEST_F(CPUFunctionTests, AddressingAbsoluteYTest) {
    cpu.Y = 0x92;
    mem[0x0000] = 0x00;
    mem[0x0001] = 0x20;
    Word absAddr = cpu.AddressingAbsoluteY(nCycles, mem);
    EXPECT_EQ(absAddr, 0x2092);
}

TEST_F(CPUFunctionTests, AddressingIndirectTest) {
    mem[0x0000] = 0x20;
    mem[0x0001] = 0x01;

    mem[0x0120] = 0xFC;
    mem[0x0121] = 0xBA;

    Word indAddr = cpu.AddressingIndirect(nCycles, mem);
    EXPECT_EQ(indAddr, 0xBAFC);
}

TEST_F(CPUFunctionTests, AddressingIndexedIndirectTest) {
    cpu.X = 0x04;
	mem[0x0000] = 0x02;
	mem[0x0001] = 0x02;

	mem[0x0006] = 0x08;	//0x2 + 0x4
	mem[0x0007] = 0x80;	

    Word addr = cpu.AddressingIndexedIndirect(nCycles, mem);
    EXPECT_EQ(addr, 0x8008);
}

TEST_F(CPUFunctionTests, AddressingIndirectIndexedTest) {
    cpu.Y = 0x04;
	mem[0x0000] = 0x02;
	mem[0x0001] = 0x02;

	mem[0x0006] = 0x08;	//0x2 + 0x4
	mem[0x0007] = 0x80;	

    Word addr = cpu.AddressingIndirectIndexed(nCycles, mem);
    EXPECT_EQ(addr, 0x8008);
}

TEST_F(CPUFunctionTests, PopByteTest) {
    EXPECT_EQ(cpu.SP, 0xFF);
    mem[0x1FF] = 0x69;

    Byte poped = cpu.PopByte(nCycles, mem);
    EXPECT_EQ(poped, 0x69);
}

TEST_F(CPUFunctionTests, PushByteTest) {
    cpu.PushByte(nCycles, 0x69, mem);

    EXPECT_EQ(mem[0x1FF], 0x69);
}

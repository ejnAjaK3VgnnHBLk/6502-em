#include "gtest/gtest.h"
#include "cpu_6502.hpp"

class MemoryInstructionTests : public ::testing::Test {
    public:
        cpu_6502::CPU cpu;
        mem_28c256::Mem mem;

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

TEST_F(MemoryInstructionTests, TestBRK) {
    mem[0xFFFE] = 0x00;
    mem[0xFFFF] = 0x10;


    cpu.PSF = 0b10101010;
    mem[0x0] = cpu.INS_BRK;

    cpu.Execute(7, mem);

    EXPECT_EQ(mem[0x1FF], 0x0);
    EXPECT_EQ(mem[0x1FE], 0x2);
    EXPECT_EQ(mem[0x1FD], 0b10101010);
    EXPECT_EQ(cpu.PC, 0x1000);


   mem[0x1000] = cpu.INS_NOP;
   cpu.Execute(2, mem);

    cpu.PSF = 0b11101111;

    mem[0x1001] = cpu.INS_RTI;
    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.PC, 0x2);
    EXPECT_EQ(cpu.PSF, 0b10001010); // Remember that RTI flips B to a 0

}

TEST_F(MemoryInstructionTests, TestJSR) {
    mem[0x0000] = cpu.INS_JSR;
    mem[0x0001] = 0x69;

    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.PC, 0x69);
}

TEST_F(MemoryInstructionTests, CanJumpToSubroutineAndBack) {
    cpu.SP = 0x10;
    cpu.PC = 0x1000;
    mem[0x1000] = cpu.INS_JSR;
	mem[0x1001] = 0x08;
	mem[0x1002] = 0x80;
	mem[0x8008] = cpu.INS_RTS;

    cpu.Execute(6, mem);
    cpu.Execute(6, mem);

    EXPECT_EQ(cpu.PC, 0x1003);
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
    cpu.X = 0x04;
    cpu.A = 0x42;
	mem[0x0000] = cpu.INS_STA_IDX;
	mem[0x0001] = 0x02;

	mem[0x0006] = 0x00;	//0x2 + 0x4
	mem[0x0007] = 0x80;	

    cpu.Execute(6, mem);
    
    EXPECT_EQ(mem[0x8000], 0x42);
}

TEST_F(MemoryInstructionTests, TestSTAIndirectY) {
    cpu.Y = 0x04;
    cpu.A = 0x42;
	mem[0x0000] = cpu.INS_STA_IDY;
	mem[0x0001] = 0x02;

	mem[0x0006] = 0x00;	//0x2 + 0x4
	mem[0x0007] = 0x80;	

    cpu.Execute(6, mem);
    
    EXPECT_EQ(mem[0x8000], 0x42);
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

TEST_F(MemoryInstructionTests, TestJMPAbsolute) {
    mem[0x0000] = cpu.INS_JMP_AB;
    mem[0x0001] = 0x08;
    mem[0x0002] = 0x80;

    cpu.Execute(3, mem);

    EXPECT_EQ(cpu.PC, 0x8008); 
}

TEST_F(MemoryInstructionTests, TestJMPIndirect) {
    mem[0x0000] = cpu.INS_JMP_ID;
    mem[0x0001] = 0x08;
    mem[0x0002] = 0x80;

    mem[0x8008] = 0x42;
    mem[0x8009] = 0x80;

    cpu.Execute(5, mem);

    EXPECT_EQ(cpu.PC, 0x8042);
}

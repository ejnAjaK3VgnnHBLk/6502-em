class CPUTest1 : public ::testing::Test {
    void SetUp() override {
        // Called immediately after the constructor
    }

    void TearDown() override {
        // Called immediately after the test
    }
}


/*
#include "cpu.hpp"

int main() {
    CPU cpu;
    Mem mem;

    cpu.Reset( mem );

    /* Start - Inline test */
    mem[0xFFFC] = cpu.INS_JSR; // Jump to first address in stack
    mem[0xFFFD] = 0x01;
    mem[0xFFFE] = 0x00;

    mem[0x0001] = cpu.INS_LDX_ABS;  // We want the value in 0x42, and 2 is in the
                                    // X register so 0x42-0x2 = 0x0040;
    mem[0x0002] = 0x41;
    mem[0x0003] = 0x0F;

    mem[0x0F41] = 0x69;

    /* End - Inline test */

    cpu.Execute( 6, mem );
    cpu.Execute( 4, mem);

}
*/
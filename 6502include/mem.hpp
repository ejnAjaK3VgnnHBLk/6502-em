#ifndef __MEM_HPP__
#define __MEM_HPP__

#include <cassert>
#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;

const static unsigned int MAX_MEM = 1024 * 64;

struct Mem {
    Byte Data[MAX_MEM];

    void Init() {
        // Reset all of memory to 0's
        for ( unsigned int i = 0; i < MAX_MEM; i++ )
            Data[i] = 0;
    }

    // Read one byte
    Byte operator[] (unsigned int address)  const {
        assert (address <= MAX_MEM);
        return Data[address];
    }

    // Write one byte
    Byte& operator[] (unsigned int address) {
        assert (address <= MAX_MEM);
        return Data[address];
    }

    void WriteWord(Word dta, unsigned int addr, unsigned int &cycles) {
        Data[addr] = dta & 0xFF;
        Data[addr+1] = (dta >> 8);
        cycles -= 2;
    }
};

#endif
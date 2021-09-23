#ifndef __MEM_HPP__
#define __MEM_HPP__

#include <cassert>
#include <cstdint>
#include <iostream>

using Byte = uint8_t;
using Word = uint16_t;

const static unsigned int MAX_MEM = 1024 * 64;

struct Mem {
    Byte Data[MAX_MEM];

    void Init();
    void LoadMem(std::string filename);

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
};

#endif

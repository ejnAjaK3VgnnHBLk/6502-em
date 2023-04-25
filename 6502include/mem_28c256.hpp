#ifndef __MEM_28C256__
#define __MEM_28C256__

#include <iostream>
#include <cassert>
#include <cstdint>
#include <iostream>


namespace mem_28c256 {
    using Byte = uint8_t;
    using Word = uint16_t; 

    struct Mem;
}

const static unsigned int MAX_MEM = 1024 * 64;

struct mem_28c256::Mem {
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
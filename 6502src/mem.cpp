#include "mem.hpp"

void Mem::Init() {
    // Reset all of memory to 0's
    for ( unsigned int i = 0; i < MAX_MEM; i++ )
        Data[i] = 0;
}
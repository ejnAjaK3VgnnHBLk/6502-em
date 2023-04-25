#include "mem_28c256.hpp"

void mem_28c256::Mem::Init() {
    // Reset all of memory to 0's
    for ( unsigned int i = 0; i < MAX_MEM; i++ )
        Data[i] = 0;
}

void mem_28c256::Mem::LoadMem(std::string filename) {
    using namespace std;
    FILE *file = NULL;
    if ((file = fopen(filename.c_str(), "rb")) == NULL)
        cout << "Could not open specified file." << endl;
    fread(Data, 1, MAX_MEM, file);

    fclose(file);
}

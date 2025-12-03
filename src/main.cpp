#include <iostream>
#include <fstream>
#include <vector>
#include "cpu.h"
#include "memory.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: cpu <program.bin>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read((char*)buffer.data(), size)) {
        std::cerr << "Error reading file." << std::endl;
        return 1;
    }

    Memory mem;
    mem.loadProgram(buffer);

    CPU cpu(mem);

    std::cout << "Starting CPU..." << std::endl;
    
    while (cpu.step()) {
        // cpu.dumpState(); // Uncomment for debug trace
    }

    std::cout << "\nCPU Halted." << std::endl;
    cpu.dumpState();

    return 0;
}

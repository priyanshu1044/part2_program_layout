#include "memory.h"
#include <iostream>
#include <cstring>

Memory::Memory() {
    data.resize(SIZE, 0);
}

uint8_t Memory::read(uint16_t address) {
    uint8_t value = data[address];
    handleMMIORead(address, value);
    return value;
}

void Memory::write(uint16_t address, uint8_t value) {
    handleMMIOWrite(address, value);
    data[address] = value;
}

void Memory::loadProgram(const std::vector<uint8_t>& program, uint16_t startAddress) {
    if (startAddress + program.size() > SIZE) {
        std::cerr << "Error: Program too large for memory!" << std::endl;
        return;
    }
    std::memcpy(&data[startAddress], program.data(), program.size());
}

void Memory::handleMMIORead(uint16_t address, uint8_t& value) {
    // Example: Read from 0xFF00 could be input
    if (address == 0xFF00) {
        // For now, just return 0
        value = 0;
    }
}

void Memory::handleMMIOWrite(uint16_t address, uint8_t value) {
    // MMIO Output at 0xFF00
    if (address == 0xFF00) {
        std::cout << (char)value;
        std::cout.flush();
    }
    // MMIO Numeric Output at 0xFF01
    if (address == 0xFF01) {
        std::cout << (int)value;
        std::cout.flush();
    }
}

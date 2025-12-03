#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>

class Memory {
public:
    static const size_t SIZE = 65536; // 64KB

    Memory();
    
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t value);
    
    // Helper to load program into memory
    void loadProgram(const std::vector<uint8_t>& program, uint16_t startAddress = 0);
    
    // Direct access for dumping
    const std::vector<uint8_t>& getRawData() const { return data; }

private:
    std::vector<uint8_t> data;
    
    // MMIO handling
    void handleMMIORead(uint16_t address, uint8_t& value);
    void handleMMIOWrite(uint16_t address, uint8_t value);
};

#endif // MEMORY_H

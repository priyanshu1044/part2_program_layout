#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>
#include "memory.h"

// ISA Definitions
namespace ISA {
    // Opcodes
    enum Opcode : uint8_t {
        NOP = 0x00,
        LOAD = 0x01,    // LOAD R, Addr
        STORE = 0x02,   // STORE R, Addr
        MOV = 0x03,     // MOV R1, R2
        MOVI = 0x04,    // MOVI R, Imm
        ADD = 0x05,     // ADD R1, R2
        SUB = 0x06,     // SUB R1, R2
        CMP = 0x07,     // CMP R1, R2
        JMP = 0x08,     // JMP Addr
        JZ = 0x09,      // JZ Addr
        JNZ = 0x0A,     // JNZ Addr
        IN = 0x0B,      // IN R, Port
        OUT = 0x0C,     // OUT Port, R
        PUSH = 0x0D,    // PUSH R
        POP = 0x0E,     // POP R
        CALL = 0x0F,    // CALL Addr
        RET = 0x10,     // RET
        MUL = 0x11,     // MUL R1, R2
        HALT = 0xFF
    };

    // Registers
    enum Register : uint8_t {
        R0 = 0,
        R1 = 1,
        R2 = 2,
        R3 = 3,
        PC = 4, // Program Counter
        SP = 5, // Stack Pointer
        FL = 6, // Flags
        COUNT = 7
    };
}

class CPU {
public:
    CPU(Memory& mem);
    void reset();
    bool step(); // Execute one instruction, returns false on HALT
    void dumpState() const;

private:
    uint16_t registers[ISA::COUNT];
    Memory& memory;
    bool halted;

    // Helper methods
    uint8_t fetchByte();
    uint16_t fetchWord();
    void updateFlags(uint16_t result);
};

#endif // CPU_H

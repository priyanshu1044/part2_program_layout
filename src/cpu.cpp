#include "cpu.h"
#include <iostream>
#include <iomanip>

CPU::CPU(Memory& mem) : memory(mem) {
    reset();
}

void CPU::reset() {
    for (int i = 0; i < ISA::COUNT; ++i) {
        registers[i] = 0;
    }
    halted = false;
}

uint8_t CPU::fetchByte() {
    return memory.read(registers[ISA::PC]++);
}

uint16_t CPU::fetchWord() {
    uint8_t low = fetchByte();
    uint8_t high = fetchByte();
    return (high << 8) | low;
}

void CPU::updateFlags(uint16_t result) {
    // Simple flag update: Zero flag only for now
    if (result == 0) {
        registers[ISA::FL] |= 0x01; // Set Zero flag
    } else {
        registers[ISA::FL] &= ~0x01; // Clear Zero flag
    }
}

bool CPU::step() {
    if (halted) return false;

    uint8_t opcode = fetchByte();

    switch (opcode) {
        case ISA::NOP:
            break;
        case ISA::HALT:
            halted = true;
            return false;
        case ISA::MOVI: {
            uint8_t reg = fetchByte();
            uint16_t val = fetchWord();
            if (reg < ISA::COUNT) registers[reg] = val;
            break;
        }
        case ISA::MOV: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) registers[r1] = registers[r2];
            break;
        }
        case ISA::ADD: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                registers[r1] += registers[r2];
                updateFlags(registers[r1]);
            }
            break;
        }
        case ISA::SUB: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                registers[r1] -= registers[r2];
                updateFlags(registers[r1]);
            }
            break;
        }
        case ISA::CMP: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                uint16_t res = registers[r1] - registers[r2];
                updateFlags(res);
            }
            break;
        }
        case ISA::JMP: {
            uint16_t addr = fetchWord();
            registers[ISA::PC] = addr;
            break;
        }
        case ISA::JZ: {
            uint16_t addr = fetchWord();
            if (registers[ISA::FL] & 0x01) {
                registers[ISA::PC] = addr;
            }
            break;
        }
        case ISA::JNZ: {
            uint16_t addr = fetchWord();
            if (!(registers[ISA::FL] & 0x01)) {
                registers[ISA::PC] = addr;
            }
            break;
        }
        case ISA::OUT: {
            uint16_t port = fetchWord();
            uint8_t reg = fetchByte();
            if (reg < ISA::COUNT) {
                memory.write(port, (uint8_t)registers[reg]);
            }
            break;
        }
        case ISA::LOAD: {
             uint8_t reg = fetchByte();
             uint16_t addr = fetchWord();
             if (reg < ISA::COUNT) {
                 registers[reg] = memory.read(addr);
             }
             break;
        }
        case ISA::STORE: {
            uint8_t reg = fetchByte();
            uint16_t addr = fetchWord();
            if (reg < ISA::COUNT) {
                memory.write(addr, (uint8_t)registers[reg]);
            }
            break;
        }
        case ISA::PUSH: {
            uint8_t reg = fetchByte();
            if (reg < ISA::COUNT) {
                registers[ISA::SP]--;
                memory.write(registers[ISA::SP], (uint8_t)registers[reg]);
            }
            break;
        }
        case ISA::POP: {
            uint8_t reg = fetchByte();
            if (reg < ISA::COUNT) {
                registers[reg] = memory.read(registers[ISA::SP]);
                registers[ISA::SP]++;
            }
            break;
        }
        case ISA::CALL: {
            uint16_t addr = fetchWord();
            // Push PC (Return Address)
            // We need to push 16-bit PC, so push High then Low (or Low then High, depending on endianness convention)
            // Let's push High byte first, then Low byte, so SP points to Low byte.
            // Stack grows down.
            registers[ISA::SP]--;
            memory.write(registers[ISA::SP], (registers[ISA::PC] >> 8) & 0xFF);
            registers[ISA::SP]--;
            memory.write(registers[ISA::SP], registers[ISA::PC] & 0xFF);
            
            registers[ISA::PC] = addr;
            break;
        }
        case ISA::RET: {
            // Pop PC
            uint8_t low = memory.read(registers[ISA::SP]);
            registers[ISA::SP]++;
            uint8_t high = memory.read(registers[ISA::SP]);
            registers[ISA::SP]++;
            registers[ISA::PC] = (high << 8) | low;
            break;
        }
        case ISA::MUL: {
            uint8_t r1 = fetchByte();
            uint8_t r2 = fetchByte();
            if (r1 < ISA::COUNT && r2 < ISA::COUNT) {
                registers[r1] *= registers[r2];
                updateFlags(registers[r1]);
            }
            break;
        }
        default:
            std::cerr << "Unknown Opcode: " << std::hex << (int)opcode << std::endl;
            halted = true;
            return false;
    }
    return true;
}

void CPU::dumpState() const {
    std::cout << "Registers:" << std::endl;
    for (int i = 0; i < ISA::COUNT; ++i) {
        std::cout << "R" << i << ": " << std::hex << std::setw(4) << std::setfill('0') << registers[i] << " ";
    }
    std::cout << std::endl;
}

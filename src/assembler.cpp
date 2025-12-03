#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "cpu.h"

using namespace std;

// Map opcodes to their enum values
map<string, uint8_t> opcodeMap = {
    {"NOP", ISA::NOP}, {"LOAD", ISA::LOAD}, {"STORE", ISA::STORE},
    {"MOV", ISA::MOV}, {"MOVI", ISA::MOVI}, {"ADD", ISA::ADD},
    {"SUB", ISA::SUB}, {"CMP", ISA::CMP}, {"JMP", ISA::JMP},
    {"JZ", ISA::JZ}, {"JNZ", ISA::JNZ}, {"IN", ISA::IN},
    {"OUT", ISA::OUT}, {"PUSH", ISA::PUSH}, {"POP", ISA::POP},
    {"CALL", ISA::CALL}, {"RET", ISA::RET}, {"MUL", ISA::MUL},
    {"HALT", ISA::HALT}
};

// Map registers to their enum values
map<string, uint8_t> regMap = {
    {"R0", ISA::R0}, {"R1", ISA::R1}, {"R2", ISA::R2}, {"R3", ISA::R3},
    {"PC", ISA::PC}, {"SP", ISA::SP}, {"FL", ISA::FL}
};

struct Instruction {
    string opcode;
    vector<string> operands;
    int lineNum;
};

// Helper to trim whitespace
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

int parseLiteral(string s) {
    if (s.find("0x") == 0) return stoi(s, nullptr, 16);
    return stoi(s);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: assembler <input.asm> <output.bin>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    vector<Instruction> instructions;
    map<string, int> labels;
    int currentAddress = 0;
    string line;
    int lineNum = 0;

    // Pass 1: Parse and collect labels
    while (getline(infile, line)) {
        lineNum++;
        // Remove comments
        size_t commentPos = line.find(";");
        if (commentPos != string::npos) line = line.substr(0, commentPos);
        line = trim(line);
        if (line.empty()) continue;

        // Check for label
        if (line.back() == ':') {
            string label = line.substr(0, line.size() - 1);
            labels[label] = currentAddress;
            continue;
        }

        stringstream ss(line);
        string opcode, operand;
        ss >> opcode;
        
        // Uppercase opcode
        transform(opcode.begin(), opcode.end(), opcode.begin(), ::toupper);

        Instruction instr;
        instr.opcode = opcode;
        instr.lineNum = lineNum;

        // Parse operands (comma separated)
        string remainder;
        getline(ss, remainder);
        stringstream ss2(remainder);
        while (getline(ss2, operand, ',')) {
            instr.operands.push_back(trim(operand));
        }
        instructions.push_back(instr);

        // Estimate size (simplified: 1 byte opcode + operands)
        // This is a simplification. Real ISAs are more complex.
        // Our ISA:
        // NOP, HALT: 1 byte
        // MOVI: 1 byte opcode + 1 byte reg + 2 byte imm = 4 bytes
        // JMP, JZ, JNZ: 1 byte opcode + 2 byte addr = 3 bytes
        // OUT: 1 byte opcode + 2 byte port + 1 byte reg = 4 bytes
        // Others: 1 byte opcode + 1 byte reg + 1 byte reg/imm (simplified to 3 bytes for uniformity in this simple assembler if needed, but let's be precise)
        
        if (opcode == "NOP" || opcode == "HALT" || opcode == "RET") currentAddress += 1;
        else if (opcode == "MOVI") currentAddress += 4;
        else if (opcode == "JMP" || opcode == "JZ" || opcode == "JNZ" || opcode == "CALL") currentAddress += 3;
        else if (opcode == "OUT") currentAddress += 4;
        else if (opcode == "LOAD" || opcode == "STORE") currentAddress += 4; // Op + Reg + Addr
        else if (opcode == "PUSH" || opcode == "POP") currentAddress += 2; // Op + Reg
        else currentAddress += 3; // Op + R1 + R2 (Includes MUL)
    }

    ofstream outfile(argv[2], ios::binary);
    
    // Pass 2: Generate Code
    for (const auto& instr : instructions) {
        if (opcodeMap.find(instr.opcode) == opcodeMap.end()) {
            cerr << "Error: Unknown opcode " << instr.opcode << " at line " << instr.lineNum << endl;
            return 1;
        }
        uint8_t op = opcodeMap[instr.opcode];
        outfile.write((char*)&op, 1);

        if (instr.opcode == "NOP" || instr.opcode == "HALT" || instr.opcode == "RET") continue;

        if (instr.opcode == "MOVI") {
            uint8_t reg = regMap[instr.operands[0]];
            outfile.write((char*)&reg, 1);
            int imm = parseLiteral(instr.operands[1]);
            outfile.write((char*)&imm, 2);
        } else if (instr.opcode == "JMP" || instr.opcode == "JZ" || instr.opcode == "JNZ" || instr.opcode == "CALL") {
            string target = instr.operands[0];
            int addr;
            if (labels.count(target)) addr = labels[target];
            else addr = parseLiteral(target);
            outfile.write((char*)&addr, 2);
        } else if (instr.opcode == "PUSH" || instr.opcode == "POP") {
            uint8_t reg = regMap[instr.operands[0]];
            outfile.write((char*)&reg, 1);
        } else if (instr.opcode == "OUT") {
            int port = parseLiteral(instr.operands[0]);
            outfile.write((char*)&port, 2);
            uint8_t reg = regMap[instr.operands[1]];
            outfile.write((char*)&reg, 1);
        } else if (instr.opcode == "LOAD" || instr.opcode == "STORE") {
             uint8_t reg = regMap[instr.operands[0]];
             outfile.write((char*)&reg, 1);
             string target = instr.operands[1];
             int addr;
             if (labels.count(target)) addr = labels[target];
             else addr = parseLiteral(target);
             outfile.write((char*)&addr, 2);
        } else {
            // ALU ops: ADD R1, R2
            uint8_t r1 = regMap[instr.operands[0]];
            uint8_t r2 = regMap[instr.operands[1]];
            outfile.write((char*)&r1, 1);
            outfile.write((char*)&r2, 1);
        }
    }

    outfile.close();
    cout << "Assembly complete. Output written to " << argv[2] << endl;
    return 0;
}

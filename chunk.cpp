//
// Created by Andrew Huang on 1/9/2022.
//

#include "chunk.h"

#include <cstdio>

void Chunk::write(uint8_t byte, size_t line) {
    _code.push_back(byte);
    _lines.push_back(line);
}

size_t Chunk::addConstant(Value value) {
    size_t index = _constants.size();
    _constants.push_back(value);
    return index;
}

Value Chunk::getConstant(uint8_t index) {
    return _constants[index];
}

void Chunk::disassemble(const std::string &name) const {
    printf("== %s ==\n", name.c_str());
    size_t offset = 0;
    while (offset < _code.size()) {
        offset = disassembleInstruction(offset);
    }
}

size_t Chunk::disassembleInstruction(size_t offset) const {
    printf("%04zu ", offset);
    if (offset > 0 && _lines[offset] == _lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4zu ", _lines[offset]);
    }
    auto instruction = static_cast<OpCode>(_code[offset]);
    switch (instruction) {
        case OpCode::Constant:
            return constantInstruction("OP_CONSTANT", offset);
        case OpCode::Add:
            return simpleInstruction("OP_ADD", offset);
        case OpCode::Subtract:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OpCode::Multiply:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OpCode::Divide:
            return simpleInstruction("OP_DIVIDE", offset);
        case OpCode::Negate:
            return simpleInstruction("OP_NEGATE", offset);
        case OpCode::Return:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown op code %hhu\n", instruction);
            return offset + 1;
    }
}

size_t Chunk::simpleInstruction(const std::string &name, size_t offset) {
    printf("%s\n", name.c_str());
    return offset + 1;
}

size_t Chunk::constantInstruction(const std::string &name, size_t offset) const {
    uint8_t constant = _code[offset + 1];
    printf("%-16s %4d '", name.c_str(), constant);
    _constants[constant].print();
    printf("'\n");
    return offset + 2;
}

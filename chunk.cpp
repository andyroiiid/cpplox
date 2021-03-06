//
// Created by Andrew Huang on 1/9/2022.
//

#include "chunk.h"

#include "op_code.h"
#include "value.h"

void Chunk::write(uint8_t byte, int line) {
    _code.push_back(byte);
    _lines.push_back(line);
}

void Chunk::patch(int offset, uint8_t byte) {
    _code[offset] = byte;
}

int Chunk::addConstant(Value value) {
    int index = static_cast<int>(_constants.size());
    _constants.push_back(value);
    return index;
}

Value Chunk::getConstant(uint8_t index) {
    return _constants[index];
}

void Chunk::disassemble(const char *name) const {
    printf("== %s ==\n", name);
    int offset = 0;
    while (offset < _code.size()) {
        offset = disassembleInstruction(offset);
    }
}

int Chunk::disassembleInstruction(int offset) const {
    printf("%04d ", offset);
    if (offset > 0 && _lines[offset] == _lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", _lines[offset]);
    }
    auto instruction = static_cast<OpCode>(_code[offset]);
    switch (instruction) {
        case OpCode::Constant:
            return constantInstruction("OP_CONSTANT", offset);
        case OpCode::Nil:
            return simpleInstruction("OP_NIL", offset);
        case OpCode::True:
            return simpleInstruction("OP_TRUE", offset);
        case OpCode::False:
            return simpleInstruction("OP_FALSE", offset);
        case OpCode::Pop:
            return simpleInstruction("OP_POP", offset);
        case OpCode::GetLocal:
            return byteInstruction("OP_GET_LOCAL", offset);
        case OpCode::SetLocal:
            return byteInstruction("OP_SET_LOCAL", offset);
        case OpCode::GetGlobal:
            return constantInstruction("OP_GET_GLOBAL", offset);
        case OpCode::DefineGlobal:
            return constantInstruction("OP_DEFINE_GLOBAL", offset);
        case OpCode::SetGlobal:
            return constantInstruction("OP_SET_GLOBAL", offset);
        case OpCode::Equal:
            return simpleInstruction("OP_EQUAL", offset);
        case OpCode::NotEqual:
            return simpleInstruction("OP_NOT_EQUAL", offset);
        case OpCode::Greater:
            return simpleInstruction("OP_GREATER", offset);
        case OpCode::GreaterEqual:
            return simpleInstruction("OP_GREATER_EQUAL", offset);
        case OpCode::Less:
            return simpleInstruction("OP_LESS", offset);
        case OpCode::LessEqual:
            return simpleInstruction("OP_LESS_EQUAL", offset);
        case OpCode::Add:
            return simpleInstruction("OP_ADD", offset);
        case OpCode::Subtract:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OpCode::Multiply:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OpCode::Divide:
            return simpleInstruction("OP_DIVIDE", offset);
        case OpCode::Modulo:
            return simpleInstruction("OP_MODULO", offset);
        case OpCode::Not:
            return simpleInstruction("OP_NOT", offset);
        case OpCode::Negate:
            return simpleInstruction("OP_NEGATE", offset);
        case OpCode::Print:
            return simpleInstruction("OP_PRINT", offset);
        case OpCode::Jump:
            return jumpInstruction("OP_JUMP", 1, offset);
        case OpCode::JumpIfTrue:
            return jumpInstruction("OP_JUMP_IF_TRUE", 1, offset);
        case OpCode::JumpIfFalse:
            return jumpInstruction("OP_JUMP_IF_FALSE", 1, offset);
        case OpCode::Loop:
            return jumpInstruction("OP_LOOP", -1, offset);
        case OpCode::Return:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown op code %d\n", instruction);
            return offset + 1;
    }
}

int Chunk::simpleInstruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int Chunk::byteInstruction(const char *name, int offset) const {
    uint8_t slot = _code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

int Chunk::jumpInstruction(const char *name, int sign, int offset) const {
    auto jump = (uint16_t) (_code[offset + 1] << 8);
    jump |= _code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

int Chunk::constantInstruction(const char *name, int offset) const {
    uint8_t constant = _code[offset + 1];
    printf("%-16s %4d '", name, constant);
    _constants[constant].print();
    printf("'\n");
    return offset + 2;
}

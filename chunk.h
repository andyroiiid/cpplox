//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_CHUNK_H
#define CPPLOX_CHUNK_H

#include <vector>
#include <string>

#include "value.h"

enum class OpCode : uint8_t {
    Constant,
    Nil,
    True,
    False,
    Pop,
    GetLocal,
    SetLocal,
    GetGlobal,
    DefineGlobal,
    SetGlobal,
    Equal,
    Greater,
    Less,
    Add,
    Subtract,
    Multiply,
    Divide,
    Not,
    Negate,
    Print,
    Return,
};

class Chunk {
public:
    void write(uint8_t byte, size_t line);

    size_t addConstant(Value value);

    Value getConstant(uint8_t index);

    [[nodiscard]] const uint8_t *code() const { return _code.data(); }

    [[nodiscard]] size_t getInstructionLine(size_t instruction) const { return _lines[instruction]; }

    void disassemble(const std::string &name) const;

    size_t disassembleInstruction(size_t offset) const; // NOLINT(modernize-use-nodiscard)

private:
    static size_t simpleInstruction(const std::string &name, size_t offset);

    [[nodiscard]] size_t byteInstruction(const std::string &name, size_t offset) const;

    [[nodiscard]] size_t constantInstruction(const std::string &name, size_t offset) const;

    std::vector<uint8_t> _code;
    std::vector<size_t> _lines;
    std::vector<Value> _constants;
};

#endif //CPPLOX_CHUNK_H

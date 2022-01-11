//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_CHUNK_H
#define CPPLOX_CHUNK_H

#include <vector>

#include "value.h"

class Chunk {
public:
    void write(uint8_t byte, int line);

    void patch(int offset, uint8_t byte);

    int addConstant(Value value);

    Value getConstant(uint8_t index);

    [[nodiscard]] const uint8_t *code() const { return _code.data(); }

    [[nodiscard]] int count() const { return static_cast<int>(_code.size()); }

    [[nodiscard]] int getInstructionLine(int instruction) const { return _lines[instruction]; }

    void disassemble(const char *name) const;

    int disassembleInstruction(int offset) const; // NOLINT(modernize-use-nodiscard)

private:
    static int simpleInstruction(const char *name, int offset);

    [[nodiscard]] int byteInstruction(const char *name, int offset) const;

    [[nodiscard]] int jumpInstruction(const char *name, int sign, int offset) const;

    [[nodiscard]] int constantInstruction(const char *name, int offset) const;

    std::vector<uint8_t> _code;
    std::vector<int> _lines;
    std::vector<Value> _constants;
};

#endif //CPPLOX_CHUNK_H

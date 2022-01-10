//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_VM_H
#define CPPLOX_VM_H

#include "chunk.h"

class VM {
public:
    enum class InterpretResult {
        Ok,
        CompileError,
        RuntimeError,
    };

    InterpretResult interpret(const std::string &source);

private:
    void runtimeError(const char *format, ...) const;

    InterpretResult run();

    inline uint8_t readByte() { return *_ip++; }

    inline Value readConstant() { return _chunk.getConstant(readByte()); }

    inline void push(Value value) { _stack.push_back(value); }

    inline Value pop() {
        Value value = _stack.back();
        _stack.pop_back();
        return value;
    }

    [[nodiscard]] inline Value peek(int distance) const { return _stack[_stack.size() - 1 - distance]; }

    Chunk _chunk;
    const uint8_t *_ip = nullptr;
    std::vector<Value> _stack;
};

#endif //CPPLOX_VM_H

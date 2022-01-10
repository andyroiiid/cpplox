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
    InterpretResult run();

    inline uint8_t readByte() { return *_ip++; }

    inline Value readConstant() { return _chunk.getConstant(readByte()); }

    template<class BinaryOp>
    inline void binaryOp() {
        static BinaryOp op;
        Value b = pop();
        Value a = pop();
        push(op(a, b));
    }

    inline void push(Value value) { _stack.push_back(value); }

    inline Value pop() {
        Value value = _stack.back();
        _stack.pop_back();
        return value;
    }

    Chunk _chunk;
    const uint8_t *_ip = nullptr;
    std::vector<Value> _stack;
};

#endif //CPPLOX_VM_H

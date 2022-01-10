//
// Created by Andrew Huang on 1/9/2022.
//

#include "vm.h"

#include <functional>

VM::InterpretResult VM::interpret(Chunk *chunk) {
    _chunk = chunk;
    _ip = chunk->code();
    return run();
}

VM::InterpretResult VM::run() {
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (const Value &slot: _stack) {
            printf("[ ");
            slot.print();
            printf(" ]");
        }
        printf("\n");
        _chunk->disassembleInstruction(_ip - _chunk->code());
#endif

        auto instruction = static_cast<OpCode>(readByte());
        switch (instruction) {
            case OpCode::Constant: {
                Value constant = readConstant();
                push(constant);
                break;
            }
            case OpCode::Add: {
                binaryOp<std::plus<Value>>();
                break;
            }
            case OpCode::Subtract: {
                binaryOp<std::minus<Value>>();
                break;
            }
            case OpCode::Multiply: {
                binaryOp<std::multiplies<Value>>();
                break;
            }
            case OpCode::Divide: {
                binaryOp<std::divides<Value>>();
                break;
            }
            case OpCode::Negate: {
                push(-pop());
                break;
            }
            case OpCode::Return: {
                pop().print();
                printf("\n");
                return InterpretResult::Ok;
            }
            default: {
                return InterpretResult::RuntimeError;
            }
        }
    }
}

//
// Created by Andrew Huang on 1/9/2022.
//

#include "vm.h"

#include <cstdarg>

#include "compiler.h"

VM::InterpretResult VM::interpret(const std::string &source) {
    Chunk chunk;

    Compiler compiler;
    if (!compiler.compile(source, &chunk)) {
        return InterpretResult::CompileError;
    }

    _chunk = std::move(chunk);
    _ip = _chunk.code();

    return run();
}

void VM::runtimeError(const char *format, ...) const {
    va_list args;
    (va_start(args, format));
    vfprintf(stderr, format, args);
    (va_end(args));
    fprintf(stderr, "\n");

    size_t instruction = _ip - _chunk.code() - 1;
    size_t line = _chunk.getInstructionLine(instruction);
    fprintf(stderr, "[line %llu] in script\n", line);
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
        _chunk.disassembleInstruction(_ip - _chunk.code());
#endif

        auto instruction = static_cast<OpCode>(readByte());
        switch (instruction) {
            case OpCode::Constant: {
                Value constant = readConstant();
                push(constant);
                break;
            }
            case OpCode::Nil: {
                push(Value());
                break;
            }
            case OpCode::True: {
                push(Value(true));
                break;
            }
            case OpCode::False: {
                push(Value(false));
                break;
            }
            case OpCode::Equal: {
                Value b = pop();
                Value a = pop();
                push(Value(a == b));
                break;
            }
            case OpCode::Greater: {
                Value b = pop();
                Value a = pop();
                Value result = a > b;
                if (result.isNil()) {
                    runtimeError("Operand must be numbers.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
                break;
            }
            case OpCode::Less: {
                Value b = pop();
                Value a = pop();
                Value result = a < b;
                if (result.isNil()) {
                    runtimeError("Operand must be numbers.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
                break;
            }
            case OpCode::Add: {
                Value b = pop();
                Value a = pop();
                Value result = a + b;
                if (result.isNil()) {
                    runtimeError("Operand must be numbers.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
                break;
            }
            case OpCode::Subtract: {
                Value b = pop();
                Value a = pop();
                Value result = a - b;
                if (result.isNil()) {
                    runtimeError("Operand must be numbers.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
                break;
            }
            case OpCode::Multiply: {
                Value b = pop();
                Value a = pop();
                Value result = a * b;
                if (result.isNil()) {
                    runtimeError("Operand must be numbers.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
                break;
            }
            case OpCode::Divide: {
                Value b = pop();
                Value a = pop();
                Value result = a / b;
                if (result.isNil()) {
                    runtimeError("Operand must be numbers.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
                break;
            }
            case OpCode::Not: {
                push(Value(pop().isFalsey()));
                break;
            }
            case OpCode::Negate: {
                Value result = -pop();
                if (result.isNil()) {
                    runtimeError("Operand must be a number.");
                    return InterpretResult::RuntimeError;
                }
                push(result);
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

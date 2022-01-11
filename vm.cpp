//
// Created by Andrew Huang on 1/9/2022.
//

#include "vm.h"

#include <cstdarg>

#include "compiler.h"

VM::~VM() {
    Obj *object = _objects;
    while (object != nullptr) {
        Obj *next = object->next;
        Obj::free(object);
        object = next;
    }
}

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

    int instruction = static_cast<int>(_ip - _chunk.code() - 1);
    int line = _chunk.getInstructionLine(instruction);
    fprintf(stderr, "[line %d] in script\n", line);
}

VM::InterpretResult VM::run() {
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (const Value &slot: _stack) {
            printf("[");
            slot.print();
            printf("]");
        }
        printf("\n");
        _chunk.disassembleInstruction(static_cast<int>(_ip - _chunk.code()));
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
            case OpCode::Pop: {
                pop();
                break;
            }
            case OpCode::GetLocal: {
                uint8_t slot = readByte();
                push(_stack[slot]);
                break;
            }
            case OpCode::SetLocal: {
                uint8_t slot = readByte();
                _stack[slot] = peek(0);
                break;
            }
            case OpCode::GetGlobal: {
                ObjString *name = readString();
                Value value;
                if (!_globals.get(name, &value)) {
                    runtimeError("Undefined variable '%s'\n", name->chars());
                    return InterpretResult::RuntimeError;
                }
                push(value);
                break;
            }
            case OpCode::DefineGlobal: {
                ObjString *name = readString();
                _globals.set(name, peek(0));
                pop();
                break;
            }
            case OpCode::SetGlobal: {
                ObjString *name = readString();
                if (_globals.set(name, peek(0))) {
                    _globals.remove(name);
                    runtimeError("Undefined variable, '%s'.", name->chars());
                    return InterpretResult::RuntimeError;
                }
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
            case OpCode::Print: {
                pop().print();
                printf("\n");
                break;
            }
            case OpCode::Jump: {
                uint16_t offset = readShort();
                _ip += offset;
                break;
            }
            case OpCode::JumpIfFalse: {
                uint16_t offset = readShort();
                if (peek(0).isFalsey()) _ip += offset;
                break;
            }
            case OpCode::Return: {
                return InterpretResult::Ok;
            }
            default: {
                return InterpretResult::RuntimeError;
            }
        }
    }
}

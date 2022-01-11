//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_VM_H
#define CPPLOX_VM_H

#include <stack>

#include "chunk.h"
#include "object.h"
#include "singleton.h"
#include "table.h"

struct CallFrame {
    ObjFunction *function = nullptr;
    const uint8_t *ip = nullptr;

    inline Value &operator[](int slot) { return _stack[_stackOffset + slot]; }

    CallFrame(ObjFunction *function, std::vector<Value> &stack, int stackOffset)
            : function(function), ip(function->chunk.code()), _stack(stack), _stackOffset(stackOffset) {}

private:
    std::vector<Value> &_stack;
    int _stackOffset;
};

class VM final : public Singleton<VM> {
public:
    enum class InterpretResult {
        Ok,
        CompileError,
        RuntimeError,
    };

    InterpretResult interpret(const char *source);

    inline Table &strings() { return _strings; };

    inline Obj *&objects() { return _objects; }

private:
    friend class Singleton<VM>;

    VM() = default;

    ~VM();

    void runtimeError(const char *format, ...) const;

    InterpretResult run();

    inline uint8_t readByte() { return *_frames.top().ip++; }

    inline uint16_t readShort() {
        CallFrame &currentFrame = _frames.top();
        currentFrame.ip += 2;
        return static_cast<uint16_t>((currentFrame.ip[-2] << 8) | currentFrame.ip[-1]);
    }

    inline Value readConstant() { return _frames.top().function->chunk.getConstant(readByte()); }

    inline ObjString *readString() { return readConstant().asString(); }

    inline void push(Value value) { _stack.push_back(value); }

    inline Value pop() {
        Value value = _stack.back();
        _stack.pop_back();
        return value;
    }

    [[nodiscard]] inline Value peek(int distance) const { return _stack[_stack.size() - 1 - distance]; }

    std::stack<CallFrame> _frames;
    std::vector<Value> _stack;
    Table _globals;
    Table _strings;
    Obj *_objects = nullptr;
};

#endif //CPPLOX_VM_H

#include "vm.h"

int main() {
    VM vm;
    Chunk chunk;

    uint8_t constant = chunk.addConstant(Value(1.2));
    chunk.write(OpCode::Constant, 123);
    chunk.write(constant, 123);

    constant = chunk.addConstant(Value(3.4));
    chunk.write(OpCode::Constant, 123);
    chunk.write(constant, 123);

    chunk.write(OpCode::Add, 123);

    constant = chunk.addConstant(Value(5.6));
    chunk.write(OpCode::Constant, 123);
    chunk.write(constant, 123);

    chunk.write(OpCode::Divide, 123);
    chunk.write(OpCode::Negate, 123);

    chunk.write(OpCode::Return, 123);

    chunk.disassemble("test chunk");

    vm.interpret(&chunk);
    return 0;
}

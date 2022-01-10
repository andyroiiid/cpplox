//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_OP_CODE_H
#define CPPLOX_OP_CODE_H

#include <cstdint>

enum class OpCode : uint8_t {
    Constant,
    Add,
    Subtract,
    Multiply,
    Divide,
    Negate,
    Return,
};

#endif //CPPLOX_OP_CODE_H

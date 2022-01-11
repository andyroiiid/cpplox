//
// Created by Andrew Huang on 1/11/2022.
//

#ifndef CPPLOX_OP_CODE_H
#define CPPLOX_OP_CODE_H

enum class OpCode {
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
    NotEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Not,
    Negate,
    Print,
    Jump,
    JumpIfTrue,
    JumpIfFalse,
    Loop,
    Return,
};

#endif //CPPLOX_OP_CODE_H

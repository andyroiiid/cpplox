//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_COMPILER_H
#define CPPLOX_COMPILER_H

#include <string>

#include "chunk.h"
#include "scanner.h"

struct Parser {
    Token current;
    Token previous;
    bool hadError = false;
    bool panicMode = false;
};

class Compiler {
public:
    bool compile(const std::string &source, Chunk *chunk);

private:
    Chunk *currentChunk();

    void advance();

    void consume(TokenType type, const char *message);

    [[nodiscard]] inline bool check(TokenType type) const { return _parser.current.type == type; }

    bool match(TokenType type);

    void emitByte(uint8_t byte);

    inline void emitByte(OpCode opCode) { emitByte(static_cast<uint8_t>(opCode)); }

    void emitBytes(OpCode opCode, uint8_t byte);

    void emitReturn();

    uint8_t makeConstant(Value value);

    void emitConstant(Value value);

    void endCompile();

    void binary();

    void literal();

    void grouping();

    void number();

    void string();

    void namedVariable(Token name);

    void variable();

    void unary();

    enum class Precedence {
        None, Assignment, Or, And, Equality, Comparison, Term, Factor, Unary, Call, Primary
    };

    typedef void (Compiler::*ParseFn)();

    struct ParseRule {
        ParseFn prefix = nullptr;
        ParseFn infix = nullptr;
        Precedence precedence = Precedence::None;
    };

    static const ParseRule *getRule(TokenType type);

    void parsePrecedence(Precedence precedence);

    uint8_t identifierConstant(Token *name);

    uint8_t parseVariable(const char *errorMessage);

    void defineVariable(uint8_t global);

    void expression();

    void varDeclaration();

    void expressionStatement();

    void printStatement();

    void synchronize();

    void declaration();

    void statement();

    inline void errorAtCurrent(const char *message) { errorAt(_parser.current, message); }

    inline void error(const char *message) { errorAt(_parser.previous, message); }

    void errorAt(const Token &token, const char *message);

    Scanner _scanner;
    Parser _parser;
    Chunk *_compilingChunk = nullptr;
};

#endif //CPPLOX_COMPILER_H

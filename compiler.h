//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_COMPILER_H
#define CPPLOX_COMPILER_H

#include <string>

#include "chunk.h"
#include "scanner.h"
#include "scope.h"

class Compiler {
public:
    bool compile(const std::string &source, Chunk *chunk);

private:
    struct Parser {
        Token current;
        Token previous;
        bool hadError = false;
        bool panicMode = false;
    };

    Chunk *currentChunk();

    void advance();

    void consume(TokenType type, const char *message);

    [[nodiscard]] inline bool check(TokenType type) const { return _parser.current.type == type; }

    bool match(TokenType type);

    void emitByte(uint8_t byte);

    inline void emitByte(OpCode opCode) { emitByte(static_cast<uint8_t>(opCode)); }

    void emitBytes(OpCode opCode, uint8_t byte);

    int emitJump(OpCode instruction);

    void emitReturn();

    uint8_t makeConstant(Value value);

    void emitConstant(Value value);

    void patchJump(int offset);

    void beginCompile(Scope *scope);

    void endCompile();

    void beginScope();

    void endScope();

    void binary(bool canAssign);

    void literal(bool canAssign);

    void grouping(bool canAssign);

    void number(bool canAssign);

    void string(bool canAssign);

    void namedVariable(Token name, bool canAssign);

    void variable(bool canAssign);

    void unary(bool canAssign);

    enum class Precedence {
        None, Assignment, Or, And, Equality, Comparison, Term, Factor, Unary, Call, Primary
    };

    typedef void (Compiler::*ParseFn)(bool canAssign);

    struct ParseRule {
        ParseFn prefix = nullptr;
        ParseFn infix = nullptr;
        Precedence precedence = Precedence::None;
    };

    static const ParseRule *getRule(TokenType type);

    void parsePrecedence(Precedence precedence);

    uint8_t identifierConstant(Token *name);

    void addLocal(const Token &name);

    void declareVariable();

    uint8_t parseVariable(const char *errorMessage);

    void defineVariable(uint8_t global);

    void logicalAnd(bool canAssign);

    void logicalOr(bool canAssign);

    void expression();

    void block();

    void varDeclaration();

    void expressionStatement();

    void ifStatement();

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
    Scope *_current = nullptr;
};

#endif //CPPLOX_COMPILER_H

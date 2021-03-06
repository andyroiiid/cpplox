//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_COMPILER_H
#define CPPLOX_COMPILER_H

#include <vector>

#include "compiler_context.h"
#include "object.h"
#include "parser.h"

class Compiler {
public:
    ObjFunction *compile(const char *source);

private:
    inline void advance() { _parser.advance(); }

    inline void consume(TokenType type, const char *message) { _parser.consume(type, message); }

    [[nodiscard]] inline bool check(TokenType type) const { return _parser.check(type); }

    inline bool match(TokenType type) { return _parser.match(type); }

    [[nodiscard]] inline bool hadError() const { return _parser.hadError(); }

    inline void error(const char *message) { _parser.error(message); }

    inline Chunk &currentChunk() { return _current->function()->chunk; }

    void emitByte(uint8_t byte);

    inline void emitByte(OpCode opCode) { emitByte(static_cast<uint8_t>(opCode)); }

    void emitBytes(OpCode opCode, uint8_t byte);

    void emitLoop(int loopStart);

    int emitJump(OpCode instruction);

    void emitReturn();

    uint8_t makeConstant(Value value);

    void emitConstant(Value value);

    void patchJump(int offset);

    void beginCompile(CompilerContext *context);

    ObjFunction *endCompile();

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

    uint8_t identifierConstant(const Token &name);

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

    void breakStatement();

    void continueStatement();

    void forStatement();

    void ifStatement();

    void printStatement();

    void whileStatement();

    void statement();

    void declaration();

    Parser _parser;
    CompilerContext *_current = nullptr;

    int _loopStart = -1;
    int _loopScopeDepth = 0;
    std::vector<int> _loopBreakJumps;
};

#endif //CPPLOX_COMPILER_H

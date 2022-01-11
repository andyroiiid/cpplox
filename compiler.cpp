//
// Created by Andrew Huang on 1/9/2022.
//

#include "compiler.h"

#include "chunk.h"
#include "op_code.h"
#include "value.h"

ObjFunction *Compiler::compile(const char *source) {
    _parser = Parser(source);
    CompilerContext context(FunctionType::Script);
    beginCompile(&context);

    advance();

    while (!_parser.match(TokenType::Eof)) {
        declaration();
    }

    ObjFunction *function = endCompile();
    return _parser.hadError() ? nullptr : function;
}

void Compiler::emitByte(uint8_t byte) {
    currentChunk().write(byte, _parser.previousLine());
}

void Compiler::emitBytes(OpCode opCode, uint8_t byte) {
    emitByte(opCode);
    emitByte(byte);
}

void Compiler::emitLoop(int loopStart) {
    emitByte(OpCode::Loop);

    int offset = currentChunk().count() - loopStart + 2;
    if (offset > UINT16_MAX) error("Loop body too large.");

    emitByte((offset >> 8) & 0xFF);
    emitByte(offset & 0xFF);
}

int Compiler::emitJump(OpCode instruction) {
    emitByte(instruction);
    emitByte(0xFF);
    emitByte(0xFF);
    return currentChunk().count() - 2;
}

void Compiler::emitReturn() {
    emitByte(OpCode::Return);
}

uint8_t Compiler::makeConstant(Value value) {
    int constant = currentChunk().addConstant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return constant;
}

void Compiler::emitConstant(Value value) {
    emitBytes(OpCode::Constant, makeConstant(value));
}

void Compiler::patchJump(int offset) {
    int jump = currentChunk().count() - offset - 2;

    if (jump > UINT16_MAX) {
        error("Too much code to jump over.");
    }

    currentChunk().patch(offset, (jump >> 8) & 0xFF);
    currentChunk().patch(offset + 1, jump & 0xFF);
}

void Compiler::beginCompile(CompilerContext *context) {
    _current = context;
}

ObjFunction *Compiler::endCompile() {
    emitReturn();
    ObjFunction *function = _current->function();

#ifdef DEBUG_PRINT_CODE
    if (!_parser.hadError()) {
        ObjString *name = function->name;
        currentChunk().disassemble(name != nullptr ? name->chars() : "<script>");
    }
#endif

    return function;
}

void Compiler::beginScope() {
    _current->beginScope();
}

void Compiler::endScope() {
    int count = _current->endScope();
    for (int i = 0; i < count; i++) {
        emitByte(OpCode::Pop);
    }
}

void Compiler::binary(bool) {
    TokenType operatorType = _parser.previousType();
    const ParseRule *rule = getRule(operatorType);
    parsePrecedence(static_cast<Precedence>(static_cast<int>(rule->precedence) + 1));

    switch (operatorType) {
        case TokenType::BangEqual:
            emitByte(OpCode::NotEqual);
            break;
        case TokenType::EqualEqual:
            emitByte(OpCode::Equal);
            break;
        case TokenType::Greater:
            emitByte(OpCode::Greater);
            break;
        case TokenType::GreaterEqual:
            emitByte(OpCode::GreaterEqual);
            break;
        case TokenType::Less:
            emitByte(OpCode::Less);
            break;
        case TokenType::LessEqual:
            emitByte(OpCode::LessEqual);
            break;
        case TokenType::Plus:
            emitByte(OpCode::Add);
            break;
        case TokenType::Minus:
            emitByte(OpCode::Subtract);
            break;
        case TokenType::Star:
            emitByte(OpCode::Multiply);
            break;
        case TokenType::Slash:
            emitByte(OpCode::Divide);
            break;
        case TokenType::Percent:
            emitByte(OpCode::Modulo);
            break;
        default:
            break;
    }
}

void Compiler::literal(bool) {
    switch (_parser.previousType()) {
        case TokenType::False:
            emitByte(OpCode::False);
            break;
        case TokenType::Nil:
            emitByte(OpCode::Nil);
            break;
        case TokenType::True:
            emitByte(OpCode::True);
            break;
        default:
            break;
    }
}

void Compiler::grouping(bool) {
    expression();
    consume(TokenType::RightParen, "Expect ')' after expression.");
}

void Compiler::number(bool) {
    emitConstant(_parser.number());
}

void Compiler::string(bool) {
    emitConstant(_parser.string());
}

void Compiler::namedVariable(Token name, bool canAssign) {
    OpCode getOp, setOp;
    int arg = -1;
    CompilerContext::ResolveResult result = _current->resolveLocal(name, arg);
    switch (result) {
        case CompilerContext::ResolveResult::Local:
            getOp = OpCode::GetLocal;
            setOp = OpCode::SetLocal;
            break;
        case CompilerContext::ResolveResult::Global:
            arg = identifierConstant(name);
            getOp = OpCode::GetGlobal;
            setOp = OpCode::SetGlobal;
            break;
        case CompilerContext::ResolveResult::Uninitialized:
            error("Can't read local variable in its own initializer.");
            break;
    }

    if (canAssign && match(TokenType::Equal)) {
        expression();
        emitBytes(setOp, arg);
    } else {
        emitBytes(getOp, arg);
    }
}

void Compiler::variable(bool canAssign) {
    namedVariable(_parser.previous(), canAssign);
}

void Compiler::unary(bool) {
    TokenType operatorType = _parser.previousType();

    expression();

    switch (operatorType) {
        case TokenType::Bang:
            emitByte(OpCode::Not);
            break;
        case TokenType::Minus:
            emitByte(OpCode::Negate);
            break;
        default:
            break;
    }
}

const Compiler::ParseRule *Compiler::getRule(TokenType type) {
    static const ParseRule RULES[]{
            {&Compiler::grouping, nullptr,               Precedence::None}, //left paren
            {nullptr,             nullptr,               Precedence::None}, // right paren
            {nullptr,             nullptr,               Precedence::None}, // left brace
            {nullptr,             nullptr,               Precedence::None}, // right brace
            {nullptr,             nullptr,               Precedence::None}, // comma
            {nullptr,             nullptr,               Precedence::None}, // dot
            {&Compiler::unary,    &Compiler::binary,     Precedence::Term}, // minus
            {nullptr,             &Compiler::binary,     Precedence::Term}, // plus
            {nullptr,             nullptr,               Precedence::None}, // semicolon
            {nullptr,             &Compiler::binary,     Precedence::Factor}, // slash
            {nullptr,             &Compiler::binary,     Precedence::Factor}, // star
            {nullptr,             &Compiler::binary,     Precedence::Factor}, // percent
            {&Compiler::unary,    nullptr,               Precedence::None}, // bang
            {nullptr,             &Compiler::binary,     Precedence::Equality}, // bang equal
            {nullptr,             nullptr,               Precedence::None}, // equal
            {nullptr,             &Compiler::binary,     Precedence::Equality}, // equal equal
            {nullptr,             &Compiler::binary,     Precedence::Comparison}, // greater
            {nullptr,             &Compiler::binary,     Precedence::Comparison}, // greater equal
            {nullptr,             &Compiler::binary,     Precedence::Comparison}, // less
            {nullptr,             &Compiler::binary,     Precedence::Comparison}, // less equal
            {&Compiler::variable, nullptr,               Precedence::None}, // identifier
            {&Compiler::string,   nullptr,               Precedence::None}, // string
            {&Compiler::number,   nullptr,               Precedence::None}, // number
            {nullptr,             &Compiler::logicalAnd, Precedence::And}, // and
            {nullptr,             nullptr,               Precedence::None}, // break
            {nullptr,             nullptr,               Precedence::None}, // class
            {nullptr,             nullptr,               Precedence::None}, // continue
            {nullptr,             nullptr,               Precedence::None}, // else
            {&Compiler::literal,  nullptr,               Precedence::None}, // false
            {nullptr,             nullptr,               Precedence::None}, // for
            {nullptr,             nullptr,               Precedence::None}, // fun
            {nullptr,             nullptr,               Precedence::None}, // if
            {&Compiler::literal,  nullptr,               Precedence::None}, // nil
            {nullptr,             &Compiler::logicalOr,  Precedence::Or}, // or
            {nullptr,             nullptr,               Precedence::None}, // print
            {nullptr,             nullptr,               Precedence::None}, // return
            {nullptr,             nullptr,               Precedence::None}, // super
            {nullptr,             nullptr,               Precedence::None}, // this
            {&Compiler::literal,  nullptr,               Precedence::None}, // true
            {nullptr,             nullptr,               Precedence::None}, // var
            {nullptr,             nullptr,               Precedence::None}, // while
            {nullptr,             nullptr,               Precedence::None}, // error
            {nullptr,             nullptr,               Precedence::None}, // eof
    };
    return RULES + static_cast<int>(type);
}

void Compiler::parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(_parser.previousType())->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    bool canAssign = precedence <= Precedence::Assignment;
    (this->*prefixRule)(canAssign);

    while (precedence <= getRule(_parser.currentType())->precedence) {
        advance();
        ParseFn infixRule = getRule(_parser.previousType())->infix;
        (this->*infixRule)(canAssign);
    }

    if (canAssign && match(TokenType::Equal)) {
        error("Invalid assignment target.");
    }
}

uint8_t Compiler::identifierConstant(const Token &name) {
    return makeConstant(Value(name.start, name.length));
}

void Compiler::addLocal(const Token &name) {
    if (!_current->addLocal(name)) {
        error("Too many local variables in function.");
    }
}

void Compiler::declareVariable() {
    if (_current->inGlobalScope()) return;

    const Token &name = _parser.previous();
    if (_current->hasLocal(name)) {
        error("Already a variable with this name in this scope.");
    }

    addLocal(name);
}

uint8_t Compiler::parseVariable(const char *errorMessage) {
    consume(TokenType::Identifier, errorMessage);

    declareVariable();
    if (!_current->inGlobalScope()) return 0;

    return identifierConstant(_parser.previous());
}

void Compiler::defineVariable(uint8_t global) {
    if (!_current->inGlobalScope()) {
        _current->markLastLocalInitialized();
        return;
    }

    emitBytes(OpCode::DefineGlobal, global);
}

void Compiler::logicalAnd(bool) {
    int endJump = emitJump(OpCode::JumpIfFalse);
    emitByte(OpCode::Pop);
    parsePrecedence(Precedence::And);
    patchJump(endJump);
}

void Compiler::logicalOr(bool) {
    int endJump = emitJump(OpCode::JumpIfTrue);
    emitByte(OpCode::Pop);
    parsePrecedence(Precedence::Or);
    patchJump(endJump);
}

void Compiler::expression() {
    parsePrecedence(Precedence::Assignment);
}

void Compiler::block() {
    while (!check(TokenType::RightBrace) && !check(TokenType::Eof)) {
        declaration();
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
}

void Compiler::varDeclaration() {
    uint8_t global = parseVariable("Expect variable name.");

    if (match(TokenType::Equal)) {
        expression();
    } else {
        emitByte(OpCode::Nil);
    }
    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");

    defineVariable(global);
}

void Compiler::expressionStatement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    emitByte(OpCode::Pop);
}

void Compiler::breakStatement() {
    if (_loopStart == -1) {
        error("Can't use 'break' outside of a loop.");
    }

    consume(TokenType::Semicolon, "Expect';' after 'break'.");

    // loop statements always introduce a new scope, thus _loopScopeDepth - 1
    int numLocals = _current->numLocalsTill(_loopScopeDepth - 1);
    for (int i = 0; i < numLocals; i++) {
        emitByte(OpCode::Pop);
    }

    _loopBreakJumps.push_back(emitJump(OpCode::Jump));
}

void Compiler::continueStatement() {
    if (_loopStart == -1) {
        error("Can't use 'continue' outside of a loop.");
    }

    consume(TokenType::Semicolon, "Expect';' after 'continue'.");

    int numLocals = _current->numLocalsTill(_loopScopeDepth);
    for (int i = 0; i < numLocals; i++) {
        emitByte(OpCode::Pop);
    }

    emitLoop(_loopStart);
}

void Compiler::forStatement() {
    beginScope();

    consume(TokenType::LeftParen, "Expect '(' after 'for'.");
    if (match(TokenType::Semicolon)) {
        // no initializer
    } else if (match(TokenType::Var)) {
        varDeclaration();
    } else {
        expressionStatement();
    }

    int loopStart = _loopStart;
    int loopScopeDepth = _loopScopeDepth;
    std::vector<int> loopBreakJumps = std::move(_loopBreakJumps);
    _loopStart = currentChunk().count();
    _loopScopeDepth = _current->scopeDepth();

    int exitJump = -1;
    if (!match(TokenType::Semicolon)) {
        expression();
        consume(TokenType::Semicolon, "Expect ';' after loop condition.");
        exitJump = emitJump(OpCode::JumpIfFalse);
        emitByte(OpCode::Pop);
    }

    if (!match(TokenType::RightParen)) {
        int bodyJump = emitJump(OpCode::Jump);
        int incrementStart = currentChunk().count();
        expression();
        emitByte(OpCode::Pop);
        consume(TokenType::RightParen, "Expect ')' after for clauses.");

        emitLoop(_loopStart);
        _loopStart = incrementStart;
        patchJump(bodyJump);
    }

    statement();
    emitLoop(_loopStart);

    if (exitJump != -1) {
        patchJump(exitJump);
        emitByte(OpCode::Pop);
    }

    _loopStart = loopStart;
    _loopScopeDepth = loopScopeDepth;
    std::swap(_loopBreakJumps, loopBreakJumps);

    endScope();

    for (int breakJump: loopBreakJumps) {
        patchJump(breakJump);
    }
}

void Compiler::ifStatement() {
    consume(TokenType::LeftParen, "Expect '(' after 'if'.");
    expression();
    consume(TokenType::RightParen, "Expect ')' after condition.");

    int thenJump = emitJump(OpCode::JumpIfFalse);
    emitByte(OpCode::Pop);
    statement();
    int elseJump = emitJump(OpCode::Jump);
    patchJump(thenJump);
    emitByte(OpCode::Pop);
    if (match(TokenType::Else)) {
        statement();
    }
    patchJump(elseJump);
}

void Compiler::printStatement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    emitByte(OpCode::Print);
}

void Compiler::whileStatement() {
    beginScope();

    int loopStart = _loopStart;
    int loopScopeDepth = _loopScopeDepth;
    std::vector<int> loopBreakJumps = std::move(_loopBreakJumps);
    _loopStart = currentChunk().count();
    _loopScopeDepth = _current->scopeDepth();

    consume(TokenType::LeftParen, "Expect '(' after 'while'.");
    expression();
    consume(TokenType::RightParen, "Expect ')' after condition.");

    int exitJump = emitJump(OpCode::JumpIfFalse);
    emitByte(OpCode::Pop);
    statement();
    emitLoop(_loopStart);

    patchJump(exitJump);
    emitByte(OpCode::Pop);

    _loopStart = loopStart;
    _loopScopeDepth = loopScopeDepth;
    std::swap(_loopBreakJumps, loopBreakJumps);

    endScope();

    for (int breakJump: loopBreakJumps) {
        patchJump(breakJump);
    }
}

void Compiler::declaration() {
    if (match(TokenType::Var)) {
        varDeclaration();
    } else {
        statement();
    }

    if (_parser.panicMode()) _parser.synchronize();
}

void Compiler::statement() {
    if (match(TokenType::Print)) {
        printStatement();
    } else if (match(TokenType::Break)) {
        breakStatement();
    } else if (match(TokenType::Continue)) {
        continueStatement();
    } else if (match(TokenType::For)) {
        forStatement();
    } else if (match(TokenType::If)) {
        ifStatement();
    } else if (match(TokenType::While)) {
        whileStatement();
    } else if (match(TokenType::LeftBrace)) {
        beginScope();
        block();
        endScope();
    } else {
        expressionStatement();
    }
}

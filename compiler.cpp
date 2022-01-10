//
// Created by Andrew Huang on 1/9/2022.
//

#include "compiler.h"

bool Compiler::compile(const std::string &source, Chunk *chunk) {
    _scanner = Scanner(source);
    _parser = Parser();
    _compilingChunk = chunk;

    advance();

    while (!match(TokenType::Eof)) {
        declaration();
    }

    endCompile();
    return !_parser.hadError;
}

Chunk *Compiler::currentChunk() {
    return _compilingChunk;
}

void Compiler::advance() {
    _parser.previous = _parser.current;
    while (true) {
        _parser.current = _scanner.scan();
        if (_parser.current.type != TokenType::Error) break;
        errorAtCurrent(_parser.current.start);
    }
}

void Compiler::consume(TokenType type, const char *message) {
    if (_parser.current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

bool Compiler::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

void Compiler::emitByte(uint8_t byte) {
    currentChunk()->write(byte, _parser.previous.line);
}

void Compiler::emitBytes(OpCode opCode, uint8_t byte) {
    emitByte(opCode);
    emitByte(byte);
}

void Compiler::emitReturn() {
    emitByte(OpCode::Return);
}

uint8_t Compiler::makeConstant(Value value) {
    size_t constant = currentChunk()->addConstant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return constant;
}

void Compiler::emitConstant(Value value) {
    emitBytes(OpCode::Constant, makeConstant(value));
}

void Compiler::endCompile() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!_parser.hadError) {
        currentChunk()->disassemble("code");
    }
#endif
}

void Compiler::binary() {
    TokenType operatorType = _parser.previous.type;
    const ParseRule *rule = getRule(operatorType);
    parsePrecedence(static_cast<Precedence>(static_cast<int>(rule->precedence) + 1));

    switch (operatorType) {
        case TokenType::BangEqual:
            emitByte(OpCode::Equal);
            emitByte(OpCode::Not);
            break;
        case TokenType::EqualEqual:
            emitByte(OpCode::Equal);
            break;
        case TokenType::Greater:
            emitByte(OpCode::Greater);
            break;
        case TokenType::GreaterEqual:
            emitByte(OpCode::Less);
            emitByte(OpCode::Not);
            break;
        case TokenType::Less:
            emitByte(OpCode::Less);
            break;
        case TokenType::LessEqual:
            emitByte(OpCode::Greater);
            emitByte(OpCode::Not);
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
        default:
            break;
    }
}

void Compiler::literal() {
    switch (_parser.previous.type) {
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

void Compiler::grouping() {
    expression();
    consume(TokenType::RightParen, "Expect ')' after expression.");
}

void Compiler::number() {
    double value = strtod(_parser.previous.start, nullptr);
    emitConstant(Value(value));
}

void Compiler::string() {
    emitConstant(Value(_parser.previous.start + 1, _parser.previous.length - 2));
}

void Compiler::namedVariable(Token name) {
    uint8_t arg = identifierConstant(&name);
    if (match(TokenType::Equal)) {
        expression();
        emitBytes(OpCode::SetGlobal, arg);
    } else {
        emitBytes(OpCode::GetGlobal, arg);
    }
}

void Compiler::variable() {
    namedVariable(_parser.previous);
}

void Compiler::unary() {
    TokenType operatorType = _parser.previous.type;

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
            {&Compiler::grouping, nullptr,           Precedence::None}, //left paren
            {nullptr,             nullptr,           Precedence::None}, // right paren
            {nullptr,             nullptr,           Precedence::None}, // left brace
            {nullptr,             nullptr,           Precedence::None}, // right brace
            {nullptr,             nullptr,           Precedence::None}, // comma
            {nullptr,             nullptr,           Precedence::None}, // dot
            {&Compiler::unary,    &Compiler::binary, Precedence::Term}, // minus
            {nullptr,             &Compiler::binary, Precedence::Term}, // plus
            {nullptr,             nullptr,           Precedence::None}, // semicolon
            {nullptr,             &Compiler::binary, Precedence::Factor}, // slash
            {nullptr,             &Compiler::binary, Precedence::Factor}, // star
            {&Compiler::unary,    nullptr,           Precedence::None}, // bang
            {nullptr,             &Compiler::binary, Precedence::Equality}, // bang equal
            {nullptr,             nullptr,           Precedence::None}, // equal
            {nullptr,             &Compiler::binary, Precedence::Equality}, // equal equal
            {nullptr,             &Compiler::binary, Precedence::Comparison}, // greater
            {nullptr,             &Compiler::binary, Precedence::Comparison}, // greater equal
            {nullptr,             &Compiler::binary, Precedence::Comparison}, // less
            {nullptr,             &Compiler::binary, Precedence::Comparison}, // less equal
            {&Compiler::variable, nullptr,           Precedence::None}, // identifier
            {&Compiler::string,   nullptr,           Precedence::None}, // string
            {&Compiler::number,   nullptr,           Precedence::None}, // number
            {nullptr,             nullptr,           Precedence::None}, // and
            {nullptr,             nullptr,           Precedence::None}, // class
            {nullptr,             nullptr,           Precedence::None}, // else
            {&Compiler::literal,  nullptr,           Precedence::None}, // false
            {nullptr,             nullptr,           Precedence::None}, // for
            {nullptr,             nullptr,           Precedence::None}, // fun
            {nullptr,             nullptr,           Precedence::None}, // if
            {&Compiler::literal,  nullptr,           Precedence::None}, // nil
            {nullptr,             nullptr,           Precedence::None}, // or
            {nullptr,             nullptr,           Precedence::None}, // print
            {nullptr,             nullptr,           Precedence::None}, // return
            {nullptr,             nullptr,           Precedence::None}, // super
            {nullptr,             nullptr,           Precedence::None}, // this
            {&Compiler::literal,  nullptr,           Precedence::None}, // true
            {nullptr,             nullptr,           Precedence::None}, // var
            {nullptr,             nullptr,           Precedence::None}, // while
            {nullptr,             nullptr,           Precedence::None}, // error
            {nullptr,             nullptr,           Precedence::None}, // eof
    };
    return RULES + static_cast<int>(type);
}

void Compiler::parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(_parser.previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    (this->*prefixRule)();

    while (precedence <= getRule(_parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(_parser.previous.type)->infix;
        (this->*infixRule)();
    }
}

uint8_t Compiler::identifierConstant(Token *name) {
    return makeConstant(Value(name->start, name->length));
}

uint8_t Compiler::parseVariable(const char *errorMessage) {
    consume(TokenType::Identifier, errorMessage);
    return identifierConstant(&_parser.previous);
}

void Compiler::defineVariable(uint8_t global) {
    emitBytes(OpCode::DefineGlobal, global);
}

void Compiler::expression() {
    parsePrecedence(Precedence::Assignment);
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

void Compiler::printStatement() {
    expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    emitByte(OpCode::Print);
}

void Compiler::synchronize() {
    _parser.panicMode = false;
    while (_parser.current.type != TokenType::Eof) {
        if (_parser.previous.type == TokenType::Semicolon) return;
        switch (_parser.current.type) {
            case TokenType::Class:
            case TokenType::Fun:
            case TokenType::Var:
            case TokenType::For:
            case TokenType::If:
            case TokenType::While:
            case TokenType::Print:
            case TokenType::Return:
                return;
            default:
                break;
        }
        advance();
    }
}

void Compiler::declaration() {
    if (match(TokenType::Var)) {
        varDeclaration();
    } else {
        statement();
    }

    if (_parser.panicMode) synchronize();
}

void Compiler::statement() {
    if (match(TokenType::Print)) {
        printStatement();
    } else {
        expressionStatement();
    }
}

void Compiler::errorAt(const Token &token, const char *message) {
    if (_parser.panicMode) return;
    _parser.panicMode = true;

    fprintf(stderr, "[line %zu] Error", token.line);

    if (token.type == TokenType::Eof) {
        fprintf(stderr, " at end");
    } else if (token.type == TokenType::Error) {
        // nothing
    } else {
        fprintf(stderr, " at '%.*s'", token.length, token.start);
    }

    fprintf(stderr, ": %s\n", message);
    _parser.hadError = true;
}

//
// Created by Andrew Huang on 1/11/2022.
//

#include "parser.h"

#include <cstdio>

void Parser::advance() {
    _previous = _current;
    while (true) {
        _current = _scanner.scan();
        if (_current.type != TokenType::Error) break;
        errorAtCurrent(_current.start);
    }
}

void Parser::consume(TokenType type, const char *message) {
    if (_current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

void Parser::synchronize() {
    _panicMode = false;
    while (_current.type != TokenType::Eof) {
        if (_previous.type == TokenType::Semicolon) return;
        switch (_current.type) {
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

void Parser::errorAt(const Token &token, const char *message) {
    if (_panicMode) return;
    _panicMode = true;

    fprintf(stderr, "[line %d] Error", token.line);

    if (token.type == TokenType::Eof) {
        fprintf(stderr, " at endScope");
    } else if (token.type == TokenType::Error) {
        // nothing
    } else {
        fprintf(stderr, " at '%.*s'", token.length, token.start);
    }

    fprintf(stderr, ": %s\n", message);
    _hadError = true;
}

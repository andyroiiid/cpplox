//
// Created by Andrew Huang on 1/11/2022.
//

#include "token.h"

#include <cstring>

const char *toString(TokenType type) {
    static const char *STRINGS[]{
            // single-character tokens
            "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
            "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR", "PERCENT",
            // one or two character tokens
            "BANG", "BANG_EQUAL", "EQUAL", "EQUAL_EQUAL",
            "GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL",
            // literals
            "IDENTIFIER", "STRING", "NUMBER",
            // keywords
            "AND", "BREAK", "CLASS", "CONTINUE", "ELSE", "FALSE", "FOR", "FUN", "IF",
            "NIL", "OR", "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
            // other
            "ERROR", "EOF"
    };
    return STRINGS[static_cast<int>(type)];
}

bool Token::lexemeEqual(const Token &rhs) const {
    if (length != rhs.length) return false;
    return memcmp(start, rhs.start, length) == 0;
}

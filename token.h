//
// Created by Andrew Huang on 1/11/2022.
//

#ifndef CPPLOX_TOKEN_H
#define CPPLOX_TOKEN_H

enum class TokenType {
    // single-character tokens
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Dot, Minus, Plus, Semicolon, Slash, Star, Percent,
    // one or two character tokens
    Bang, BangEqual, Equal, EqualEqual,
    Greater, GreaterEqual, Less, LessEqual,
    // literals
    Identifier, String, Number,
    // keywords
    And, Break, Class, Continue, Else, False, For, Fun, If,
    Nil, Or, Print, Return, Super, This, True, Var, While,
    // other
    Error, Eof
};

const char *toString(TokenType type);

struct Token {
    TokenType type = TokenType::Error;
    const char *start = nullptr;
    int length = 0;
    int line = 0;

    [[nodiscard]] bool lexemeEqual(const Token &rhs) const;
};

#endif //CPPLOX_TOKEN_H

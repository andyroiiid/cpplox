//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_SCANNER_H
#define CPPLOX_SCANNER_H

#include <string>

enum class TokenType {
    // single-character tokens
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Dot, Minus, Plus, Semicolon, Slash, Star,
    // one or two character tokens
    Bang, BangEqual, Equal, EqualEqual,
    Greater, GreaterEqual, Less, LessEqual,
    // literals
    Identifier, String, Number,
    // keywords
    And, Class, Else, False, For, Fun, If, Nil, Or,
    Print, Return, Super, This, True, Var, While,
    // other
    Error, Eof
};

const char *toString(TokenType type);

struct Token {
    TokenType type = TokenType::Error;
    const char *start = nullptr;
    int length = 0;
    size_t line = 0;
};

class Scanner {
public:
    Scanner() = default;

    explicit Scanner(const std::string &source);

    Token scan();

private:
    [[nodiscard]] static inline bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    [[nodiscard]] static inline bool isDigit(char c) { return c >= '0' && c <= '9'; }

    [[nodiscard]] inline bool isAtEnd() const { return *_current == '\0'; }

    inline char advance() { return *_current++; }

    [[nodiscard]] inline char peek() const { return *_current; }

    [[nodiscard]] char peekNext() const;

    bool match(char expected);

    [[nodiscard]] Token makeToken(TokenType type) const;

    [[nodiscard]] Token errorToken(const char *message) const;

    void skipWhitespace();

    TokenType checkKeyword(int start, int length, const char *rest, TokenType type);

    TokenType identifierType();

    Token identifier();

    Token number();

    Token string();

    const char *_start = nullptr;
    const char *_current = nullptr;
    size_t _line = 0;
};

#endif //CPPLOX_SCANNER_H

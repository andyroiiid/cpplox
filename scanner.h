//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_SCANNER_H
#define CPPLOX_SCANNER_H

#include "token.h"

class Scanner {
public:
    inline void init(const char *source) {
        _start = source;
        _current = source;
        _line = 1;
    }

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
    int _line = 0;
};

#endif //CPPLOX_SCANNER_H

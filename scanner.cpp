//
// Created by Andrew Huang on 1/9/2022.
//

#include "scanner.h"

#include <cstring>

#include "token.h"

Scanner::Scanner(const char *source)
        : _start(source), _current(source), _line(1) {
}

Token Scanner::scan() {
    skipWhitespace();

    _start = _current;
    if (isAtEnd()) return makeToken(TokenType::Eof);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(':
            return makeToken(TokenType::LeftParen);
        case ')':
            return makeToken(TokenType::RightParen);
        case '{':
            return makeToken(TokenType::LeftBrace);
        case '}':
            return makeToken(TokenType::RightBrace);
        case ';':
            return makeToken(TokenType::Semicolon);
        case ',':
            return makeToken(TokenType::Comma);
        case '.':
            return makeToken(TokenType::Dot);
        case '-':
            return makeToken(TokenType::Minus);
        case '+':
            return makeToken(TokenType::Plus);
        case '/':
            return makeToken(TokenType::Slash);
        case '*':
            return makeToken(TokenType::Star);
        case '%':
            return makeToken(TokenType::Percent);
        case '!':
            return makeToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
        case '=':
            return makeToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
        case '<':
            return makeToken(match('=') ? TokenType::LessEqual : TokenType::Less);
        case '>':
            return makeToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
        case '"':
            return string();
        default:
            return errorToken("Unexpected character.");
    }
}

char Scanner::peekNext() const {
    if (isAtEnd()) return '\0';
    return _current[1];
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (*_current != expected) return false;
    _current++;
    return true;
}

Token Scanner::makeToken(TokenType type) const {
    return {type, _start, static_cast<int>(_current - _start), _line};
}

Token Scanner::errorToken(const char *message) const {
    return {TokenType::Error, message, static_cast<int>(strlen(message)), _line};
}

void Scanner::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                _line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

TokenType Scanner::checkKeyword(int start, int length, const char *rest, TokenType type) {
    if (_current - _start == start + length && memcmp(_start + start, rest, length) == 0) {
        return type;
    }
    return TokenType::Identifier;
}

TokenType Scanner::identifierType() {
    switch (*_start) {
        case 'a':
            return checkKeyword(1, 2, "nd", TokenType::And);
        case 'b':
            return checkKeyword(1, 4, "reak", TokenType::Break);
        case 'c':
            if (_current - _start > 1) {
                switch (_start[1]) {
                    case 'l':
                        return checkKeyword(2, 3, "ass", TokenType::Class);
                    case 'o':
                        return checkKeyword(2, 6, "ntinue", TokenType::Continue);
                }
            }
            break;
        case 'e':
            return checkKeyword(1, 3, "lse", TokenType::Else);
        case 'f':
            if (_current - _start > 1) {
                switch (_start[1]) {
                    case 'a':
                        return checkKeyword(2, 3, "lse", TokenType::False);
                    case 'o':
                        return checkKeyword(2, 1, "r", TokenType::For);
                    case 'u':
                        return checkKeyword(2, 1, "n", TokenType::Fun);
                }
            }
            break;
        case 'i':
            return checkKeyword(1, 1, "f", TokenType::If);
        case 'n':
            return checkKeyword(1, 2, "il", TokenType::Nil);
        case 'o':
            return checkKeyword(1, 1, "r", TokenType::Or);
        case 'p':
            return checkKeyword(1, 4, "rint", TokenType::Print);
        case 'r':
            return checkKeyword(1, 5, "eturn", TokenType::Return);
        case 's':
            return checkKeyword(1, 4, "uper", TokenType::Super);
        case 't':
            if (_current - _start > 1) {
                switch (_start[1]) {
                    case 'h':
                        return checkKeyword(2, 2, "is", TokenType::This);
                    case 'r':
                        return checkKeyword(2, 2, "ue", TokenType::True);
                }
            }
            break;
        case 'v':
            return checkKeyword(1, 2, "ar", TokenType::Var);
        case 'w':
            return checkKeyword(1, 4, "hile", TokenType::While);
        default:
            break;
    }
    return TokenType::Identifier;
}

Token Scanner::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();
    return makeToken(identifierType());
}

Token Scanner::number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }

    return makeToken(TokenType::Number);
}

Token Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') _line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TokenType::String);
}

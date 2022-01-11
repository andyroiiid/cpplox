//
// Created by Andrew Huang on 1/11/2022.
//

#ifndef CPPLOX_PARSER_H
#define CPPLOX_PARSER_H

#include <cstdlib>

#include "scanner.h"
#include "value.h"

// actually just a token iterator
class Parser {
public:
    inline void init(const char *source) { _scanner.init(source); }

    [[nodiscard]] inline bool hadError() const { return _hadError; }

    inline void error(const char *message) { errorAt(_previous, message); }

    void advance();

    void consume(TokenType type, const char *message);

    [[nodiscard]] inline bool check(TokenType type) const { return _current.type == type; }

    bool match(TokenType type);

    void synchronize();

    [[nodiscard]] inline Value number() const { return Value(strtod(_previous.start, nullptr)); }

    [[nodiscard]] inline Value string() const { return {_previous.start + 1, _previous.length - 2}; }

    [[nodiscard]] inline const Token &previous() const { return _previous; }

    [[nodiscard]] inline const Token &current() const { return _current; }

private:
    inline void errorAtCurrent(const char *message) { errorAt(_current, message); }

    void errorAt(const Token &token, const char *message);

    Scanner _scanner;
    Token _current;
    Token _previous;
    bool _hadError = false;
    bool _panicMode = false;
};

#endif //CPPLOX_PARSER_H

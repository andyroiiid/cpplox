//
// Created by Andrew Huang on 1/9/2022.
//

#include "compiler.h"

#include "scanner.h"

void Compiler::compile(const std::string &source) {
    Scanner scanner(source);
    int prevLine = 0;
    while (true) {
        Token token = scanner.scan();
        if (token.line != prevLine) {
            printf("%4d", token.line);
            prevLine = token.line;
        } else {
            printf("   |");
        }
        printf(" %-32s '%.*s'\n", toString(token.type), token.length, token.start);
        if (token.type == TokenType::Eof) {
            break;
        }
    }
}

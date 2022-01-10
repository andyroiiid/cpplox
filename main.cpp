#include "vm.h"

#include <iostream>
#include <fstream>
#include <sstream>

static void repl(VM &vm) {
    while (std::cin) {
        printf("> ");
        std::string line;
        std::getline(std::cin, line);
        vm.interpret(line);
    }
}

static std::string readFile(const std::string &path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void runFile(VM &vm, const std::string &path) {
    std::string source = readFile(path);
    VM::InterpretResult result = vm.interpret(source);
    if (result == VM::InterpretResult::CompileError) exit(65);
    if (result == VM::InterpretResult::RuntimeError) exit(70);
}

int main(int argc, const char *argv[]) {
    VM vm;
    if (argc == 1) {
        repl(vm);
    } else if (argc == 2) {
        runFile(vm, argv[1]);
    } else {
        fprintf(stderr, "Usage cpplox [path]\n");
        exit(64);
    }
    return 0;
}

//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_COMPILER_CONTEXT_H
#define CPPLOX_COMPILER_CONTEXT_H

#include <array>

#include "forward.h"
#include "token.h"

enum class FunctionType {
    Function,
    Script,
};

class CompilerContext {
public:
    explicit CompilerContext(FunctionType type);

    ObjFunction *function() { return _function; }

    void beginScope();

    int endScope();

    [[nodiscard]] inline int scopeDepth() const { return _scopeDepth; }

    [[nodiscard]] inline bool inGlobalScope() const { return _scopeDepth == 0; }

    bool addLocal(const Token &name);

    [[nodiscard]] bool hasLocal(const Token &name) const;

    enum class ResolveResult {
        Local,
        Global,
        Uninitialized
    };

    ResolveResult resolveLocal(const Token &name, int &slot) const;

    inline void markLastLocalInitialized() { _locals[_localsCount - 1].depth = _scopeDepth; }

    int numLocalsTill(int targetDepth);

private:
    static constexpr int UINT8_COUNT = UINT8_MAX + 1;

    struct Local {
        Token name;
        int depth = 0;
    };

    ObjFunction *_function = nullptr;
    FunctionType _type;

    std::array<Local, UINT8_COUNT> _locals;
    int _localsCount = 0;
    int _scopeDepth = 0;
};

#endif //CPPLOX_COMPILER_CONTEXT_H

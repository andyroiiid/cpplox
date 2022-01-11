//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_SCOPE_H
#define CPPLOX_SCOPE_H

#include <array>

#include "scanner.h"

class Scope {
public:
    void begin();

    int end();

    [[nodiscard]] inline bool isGlobal() const { return _depth == 0; }

    bool addLocal(const Token &name);

    [[nodiscard]] bool hasLocal(const Token &name) const;

    enum class ResolveResult {
        Local,
        Global,
        Uninitialized
    };

    ResolveResult resolveLocal(const Token &name, int &slot) const;

    inline void markLastLocalInitialized() { lastLocal().depth = _depth; }

private:
    static constexpr size_t UINT8_COUNT = UINT8_MAX + 1;

    struct Local {
        Token name;
        int depth = 0;
    };

    inline Local &lastLocal() { return _locals[_count - 1]; }

    std::array<Local, UINT8_COUNT> _locals;
    int _count = 0;
    int _depth = 0;
};

#endif //CPPLOX_SCOPE_H

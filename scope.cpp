//
// Created by Andrew Huang on 1/10/2022.
//

#include "scope.h"

void Scope::begin() {
    _depth++;
}

int Scope::end() {
    _depth--;

    int count = 0;
    while (_count > 0 && lastLocal().depth > _depth) {
        count++;
        _count--;
    }

    return count;
}

bool Scope::addLocal(const Token &name) {
    if (_count == UINT8_COUNT) return false;

    Local &local = _locals[_count++];
    local.name = name;
    local.depth = -1;

    return true;
}

bool Scope::hasLocal(const Token &name) const {
    for (int i = _count - 1; i >= 0; i--) {
        const Local &local = _locals[i];
        if (local.depth != -1 && local.depth < _depth) {
            break;
        }
        if (name.lexemeEqual(local.name)) {
            return true;
        }
    }
    return false;
}

Scope::ResolveResult Scope::resolveLocal(const Token &name, int &slot) const {
    for (int i = _count - 1; i >= 0; i--) {
        const Local &local = _locals[i];
        if (name.lexemeEqual(local.name)) {
            if (local.depth == -1) {
                return ResolveResult::Uninitialized;
            } else {
                slot = i;
                return ResolveResult::Local;
            }
        }
    }
    return ResolveResult::Global;
}

//
// Created by Andrew Huang on 1/10/2022.
//

#include "compiler_context.h"

#include "object.h"

CompilerContext::CompilerContext(FunctionType type)
        : _type(type) {
    _function = new ObjFunction;

    // reserve stack slot 0
    Local &local = _locals[_localsCount++];
    local.name.start = "";
}

void CompilerContext::beginScope() {
    _scopeDepth++;
}

int CompilerContext::endScope() {
    int numLocalsToDiscard = numLocalsTill(_scopeDepth - 1);
    _localsCount -= numLocalsToDiscard;
    _scopeDepth--;
    return numLocalsToDiscard;
}

bool CompilerContext::addLocal(const Token &name) {
    if (_localsCount == UINT8_COUNT) return false;

    Local &local = _locals[_localsCount++];
    local.name = name;
    local.depth = -1;

    return true;
}

bool CompilerContext::hasLocal(const Token &name) const {
    for (int i = _localsCount - 1; i >= 0; i--) {
        const Local &local = _locals[i];
        if (local.depth != -1 && local.depth < _scopeDepth) {
            break;
        }
        if (name.lexemeEqual(local.name)) {
            return true;
        }
    }
    return false;
}

CompilerContext::ResolveResult CompilerContext::resolveLocal(const Token &name, int &slot) const {
    for (int i = _localsCount - 1; i >= 0; i--) {
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

int CompilerContext::numLocalsTill(int targetDepth) {
    int count = 0;
    for (int i = _localsCount - 1; i >= 0 && _locals[i].depth > targetDepth; i--) {
        count++;
    }
    return count;
}

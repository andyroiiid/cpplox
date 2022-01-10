//
// Created by Andrew Huang on 1/9/2022.
//

#include "value.h"

#include <cstdio>

void Value::print() const {
    switch (_type) {
        case ValueType::Bool:
            printf(asBool() ? "true" : "false");
            break;
        case ValueType::Nil:
            printf("nil");
            break;
        case ValueType::Number:
            printf("%g", asNumber());
            break;
        case ValueType::Obj:
            asObj()->print();
            break;
    }
}

Value Value::operator-() const {
    if (!isNumber()) return {};
    return Value(-asNumber());
}

Value Value::operator+(const Value &rhs) const {
    if (_type != rhs._type) return {};
    if (isNumber()) return Value(asNumber() + rhs.asNumber());
    if (isString()) return Value(ObjString::concatenate(asString(), rhs.asString()));
    return {};
}

Value Value::operator-(const Value &rhs) const {
    if (_type != rhs._type || !isNumber()) return {};
    return Value(asNumber() - rhs.asNumber());
}

Value Value::operator*(const Value &rhs) const {
    if (_type != rhs._type || !isNumber()) return {};
    return Value(asNumber() * rhs.asNumber());
}

Value Value::operator/(const Value &rhs) const {
    if (_type != rhs._type || !isNumber()) return {};
    return Value(asNumber() / rhs.asNumber());
}

bool Value::operator==(const Value &rhs) const {
    if (_type != rhs._type) return false;
    switch (_type) {
        case ValueType::Bool:
            return asBool() == rhs.asBool();
        case ValueType::Nil:
            return true;
        case ValueType::Number:
            return asNumber() == rhs.asNumber();
        case ValueType::Obj:
            return asObj() == rhs.asObj();
        default:
            return false;
    }
}

Value Value::operator>(const Value &rhs) const {
    if (_type != rhs._type || !isNumber()) return {};
    return Value(asNumber() > rhs.asNumber());
}

Value Value::operator<(const Value &rhs) const {
    if (_type != rhs._type || !isNumber()) return {};
    return Value(asNumber() < rhs.asNumber());
}

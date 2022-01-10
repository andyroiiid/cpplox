//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_VALUE_H
#define CPPLOX_VALUE_H

#include "object.h"

enum class ValueType {
    Nil,
    Bool,
    Number,
    Obj,
};

class Value {
public:
    inline Value() : _type(ValueType::Nil) {}

    explicit inline Value(bool value) : _type(ValueType::Bool) { _as.boolean = value; }

    explicit inline Value(double value) : _type(ValueType::Number) { _as.number = value; }

    inline Value(const char *chars, int length) : Value(ObjString::create(chars, length)) {}

    [[nodiscard]] inline bool isBool() const { return _type == ValueType::Bool; }

    [[nodiscard]] inline bool isNil() const { return _type == ValueType::Nil; }

    [[nodiscard]] inline bool isNumber() const { return _type == ValueType::Number; }

    [[nodiscard]] inline bool isObj() const { return _type == ValueType::Obj; }

    [[nodiscard]] inline bool asBool() const { return _as.boolean; }

    [[nodiscard]] inline double asNumber() const { return _as.number; }

    [[nodiscard]] inline Obj *asObj() const { return _as.obj; }

    [[nodiscard]] inline ObjType objType() const { return asObj()->type; }

    [[nodiscard]] inline bool isString() const { return isObj() && objType() == ObjType::String; }

    [[nodiscard]] inline ObjString *asString() const { return reinterpret_cast<ObjString *>(asObj()); }

    [[nodiscard]] inline bool isFalsey() const { return isNil() || (isBool() && !asBool()); }

    Value operator-() const;

    Value operator+(const Value &rhs) const;

    Value operator-(const Value &rhs) const;

    Value operator*(const Value &rhs) const;

    Value operator/(const Value &rhs) const;

    bool operator==(const Value &rhs) const;

    Value operator>(const Value &rhs) const;

    Value operator<(const Value &rhs) const;

    void print() const;

private:
    explicit inline Value(Obj *value) : _type(ValueType::Obj) { _as.obj = value; }

    ValueType _type;
    union {
        bool boolean;
        double number;
        Obj *obj;
    } _as{};
};

#endif //CPPLOX_VALUE_H

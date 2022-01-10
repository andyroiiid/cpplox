//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_VALUE_H
#define CPPLOX_VALUE_H

enum class ValueType {
    Bool,
    Nil,
    Number,
};

class Value {
public:
    inline Value() : _type(ValueType::Nil) {}

    explicit inline Value(bool value) : _type(ValueType::Bool) {
        _as.boolean = value;
    }

    explicit inline Value(double value) : _type(ValueType::Number) {
        _as.number = value;
    }

    [[nodiscard]] inline bool isBool() const { return _type == ValueType::Bool; }

    [[nodiscard]] inline bool isNil() const { return _type == ValueType::Nil; }

    [[nodiscard]] inline bool isNumber() const { return _type == ValueType::Number; }

    [[nodiscard]] inline bool asBool() const { return _as.boolean; }

    [[nodiscard]] inline double asNumber() const { return _as.number; }

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
    ValueType _type;
    union {
        bool boolean;
        double number;
    } _as{};
};

#endif //CPPLOX_VALUE_H

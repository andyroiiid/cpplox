//
// Created by Andrew Huang on 1/9/2022.
//

#ifndef CPPLOX_VALUE_H
#define CPPLOX_VALUE_H

class Value {
public:
    explicit Value(double value) : _value(value) {}

    inline Value operator-() const {
        return Value(-_value);
    }

    inline Value operator+(const Value &rhs) const {
        return Value(_value + rhs._value);
    }

    inline Value operator-(const Value &rhs) const {
        return Value(_value - rhs._value);
    }

    inline Value operator*(const Value &rhs) const {
        return Value(_value * rhs._value);
    }

    inline Value operator/(const Value &rhs) const {
        return Value(_value / rhs._value);
    }

    void print() const;

private:
    double _value;
};

#endif //CPPLOX_VALUE_H

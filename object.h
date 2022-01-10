//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_OBJECT_H
#define CPPLOX_OBJECT_H

#include <string>

enum class ObjType {
    String,
};

struct Obj {
    explicit Obj(ObjType type);

    void print() const;

    static void free(Obj *obj);

    ObjType type;
    Obj *next = nullptr;
};

struct ObjString : Obj {
    const std::string string;

    ObjString(const char *chars, int length) : Obj(ObjType::String), string(chars, length) {}

    explicit ObjString(std::string &&string) : Obj(ObjType::String), string(std::move(string)) {}
};

#endif //CPPLOX_OBJECT_H

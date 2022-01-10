//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_OBJECT_H
#define CPPLOX_OBJECT_H

#include <cstdint>

enum class ObjType {
    String,
};

struct Obj {
    inline explicit Obj(ObjType type) : type(type) {}

    void addToVM();

    void print() const;

    static void free(Obj *obj);

    ObjType type;
    Obj *next = nullptr;
};

class ObjString : public Obj {
public:
    static ObjString *create(const char *chars, int length);

    static ObjString *concatenate(const ObjString *a, const ObjString *b);

    static void free(ObjString *string);

    [[nodiscard]] inline const char *chars() const {
        return reinterpret_cast<const char *>(this) + sizeof(ObjString);
    }

    [[nodiscard]] int length() const { return _length; }

    [[nodiscard]] uint32_t hash() const { return _hash; }

private:
    static uint32_t hash(const char *key, int length);

    ObjString(const char *chars, int length, uint32_t hash);

    ObjString(const ObjString *a, const ObjString *b);

    int _length = 0;
    uint32_t _hash = 0;
};

#endif //CPPLOX_OBJECT_H

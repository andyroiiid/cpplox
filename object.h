//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_OBJECT_H
#define CPPLOX_OBJECT_H

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

class ObjString : public Obj {
public:
    static ObjString *create(const char *chars, int length);

    static ObjString *concatenate(const ObjString *a, const ObjString *b);

    static void free(ObjString *string);

    [[nodiscard]] inline const char *chars() const {
        return reinterpret_cast<const char *>(this) + sizeof(ObjString);
    }

    bool equals(const ObjString *rhs) const;

private:
    ObjString(const char *chars, int length);

    ObjString(const ObjString *a, const ObjString *b);

private:
    int _length = 0;
};

#endif //CPPLOX_OBJECT_H

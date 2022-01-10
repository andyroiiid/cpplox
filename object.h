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
    ObjString(const char *chars, int length);

    ObjString(const ObjString &a, const ObjString &b);

    ~ObjString();

    [[nodiscard]] inline const char *chars() const { return _chars; }

    bool operator==(const ObjString &rhs) const;

    ObjString *concatenate(const ObjString *string);

private:
    char *_chars = nullptr;
    int _length = 0;
};

#endif //CPPLOX_OBJECT_H

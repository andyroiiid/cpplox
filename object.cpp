//
// Created by Andrew Huang on 1/10/2022.
//

#include "object.h"

#include "vm.h"

Obj::Obj(ObjType type) : type(type) {
    Obj *&objects = VM::instance().objects();
    next = objects;
    objects = this;
}

void Obj::print() const {
    switch (type) {
        case ObjType::String:
            printf("%s", reinterpret_cast<const ObjString *>(this)->chars());
            break;
    }
}

void Obj::free(Obj *obj) {
    switch (obj->type) {
        case ObjType::String:
            delete reinterpret_cast<ObjString *>(obj);
            break;
    }
}

ObjString::ObjString(const char *chars, int length) : Obj(ObjType::String) {
    _chars = new char[length + 1];
    _length = length;
    memcpy(_chars, chars, length);
    _chars[_length] = '\0';
}

ObjString::ObjString(const ObjString &a, const ObjString &b) : Obj(ObjType::String) {
    _chars = new char[a._length + b._length + 1];
    _length = a._length + b._length;
    memcpy(_chars, a._chars, a._length);
    memcpy(_chars + a._length, b._chars, b._length);
    _chars[_length] = '\0';
}

ObjString::~ObjString() {
    delete[] _chars;
}

bool ObjString::operator==(const ObjString &rhs) const {
    return _length == rhs._length && memcmp(_chars, rhs._chars, _length) == 0;
}

ObjString *ObjString::concatenate(const ObjString *string) {
    return new ObjString(*this, *string);
}

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
            ObjString::free(reinterpret_cast<ObjString *>(obj));
            break;
    }
}

ObjString *ObjString::create(const char *chars, int length) {
    char *buf = new char[sizeof(ObjString) + length + 1];
    new(buf) ObjString(chars, length);
    return reinterpret_cast<ObjString *>(buf);
}

ObjString *ObjString::concatenate(const ObjString *a, const ObjString *b) {
    char *buf = new char[sizeof(ObjString) + a->_length + b->_length + 1];
    new(buf) ObjString(a, b);
    return reinterpret_cast<ObjString *>(buf);
}

void ObjString::free(ObjString *string) {
    delete[] reinterpret_cast<char *>(string);
}

ObjString::ObjString(const char *chars, int length) : Obj(ObjType::String) {
    char *buf = reinterpret_cast<char *>(this) + sizeof(ObjString);
    _length = length;
    memcpy(buf, chars, length);
    buf[_length] = '\0';
}

ObjString::ObjString(const ObjString *a, const ObjString *b) : Obj(ObjType::String) {
    char *buf = reinterpret_cast<char *>(this) + sizeof(ObjString);
    _length = a->_length + b->_length;
    memcpy(buf, a->chars(), a->_length);
    memcpy(buf + a->_length, b->chars(), b->_length);
    buf[_length] = '\0';
}

bool ObjString::equals(const ObjString *rhs) const {
    return _length == rhs->_length && memcmp(chars(), rhs->chars(), _length) == 0;
}

//
// Created by Andrew Huang on 1/10/2022.
//

#include "object.h"

#include "vm.h"

Obj::Obj(ObjType type, bool manualAddToVM) : type(type) {
    if (!manualAddToVM) {
        addToVM();
    }
}

void Obj::addToVM() {
    auto &objects = VM::instance().objects();
    next = objects;
    objects = this;
}

void Obj::print() const {
    switch (type) {
        case ObjType::String:
            reinterpret_cast<const ObjString *>(this)->doPrint();
            break;
        case ObjType::Function:
            reinterpret_cast<const ObjFunction *>(this)->doPrint();
            break;
    }
}

void Obj::free(Obj *obj) {
    switch (obj->type) {
        case ObjType::String:
            ObjString::free(reinterpret_cast<ObjString *>(obj));
            break;
        case ObjType::Function:
            delete reinterpret_cast<ObjFunction *>(obj);
            break;
    }
}

ObjString *ObjString::create(const char *chars, int length) {
    Table &strings = VM::instance().strings();

    uint32_t h = hash(chars, length);
    ObjString *interned = strings.find(chars, length, h);

    if (interned != nullptr) {
        return interned;
    }

    char *buf = new char[sizeof(ObjString) + length + 1];
    new(buf) ObjString(chars, length, h);
    auto string = reinterpret_cast<ObjString *>(buf);

    strings.set(string, Value());
    return string;
}

ObjString *ObjString::concatenate(const ObjString *a, const ObjString *b) {
    Table &strings = VM::instance().strings();

    char *buf = new char[sizeof(ObjString) + a->_length + b->_length + 1];
    new(buf) ObjString(a, b);
    auto string = reinterpret_cast<ObjString *>(buf);

    ObjString *interned = strings.find(string->chars(), string->_length, string->_hash);
    if (interned != nullptr) {
        delete[] buf;
        return interned;
    }

    string->addToVM();
    strings.set(string, Value());
    return string;
}

void ObjString::free(ObjString *string) {
    delete[] reinterpret_cast<char *>(string);
}

void ObjString::doPrint() const {
    printf("%s", chars());
}

uint32_t ObjString::hash(const char *key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t) key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString::ObjString(const char *chars, int length, uint32_t hash) : Obj(ObjType::String) {
    char *buf = reinterpret_cast<char *>(this) + sizeof(ObjString);
    _length = length;
    memcpy(buf, chars, length);
    buf[_length] = '\0';
    _hash = hash;
}

ObjString::ObjString(const ObjString *a, const ObjString *b) : Obj(ObjType::String, true) {
    char *buf = reinterpret_cast<char *>(this) + sizeof(ObjString);
    _length = a->_length + b->_length;
    memcpy(buf, a->chars(), a->_length);
    memcpy(buf + a->_length, b->chars(), b->_length);
    buf[_length] = '\0';
    _hash = hash(buf, _length);
}

void ObjFunction::doPrint() const {
    if (name == nullptr) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", name->chars());
}

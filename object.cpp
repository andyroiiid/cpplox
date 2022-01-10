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
            printf("%s", reinterpret_cast<const ObjString *>(this)->string.c_str());
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

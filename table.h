//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_TABLE_H
#define CPPLOX_TABLE_H

#include <cstdint>

#include "value.h"

class Table {
public:
    ~Table();

    bool get(const ObjString *key, Value *value);

    bool set(ObjString *key, Value value);

    bool remove(const ObjString *key);

    void merge(Table &from);

    ObjString *find(const char *chars, int length, uint32_t hash);

private:
    struct Entry {
        ObjString *key = nullptr;
        Value value;
    };

    static Entry *find(Entry *entries, int capacity, const ObjString *key);

    inline Entry *find(const ObjString *key) { return find(_entries, _capacity, key); }

    void resize(int capacity);

    int _count = 0;
    int _capacity = 0;
    Entry *_entries = nullptr;
};

#endif //CPPLOX_TABLE_H

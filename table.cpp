//
// Created by Andrew Huang on 1/10/2022.
//

#include "table.h"

#include <utility>
#include <cstring>

#include "object.h"

Table::~Table() {
    delete[] _entries;
}

Table::Entry *Table::find(Entry *entries, int capacity, const ObjString *key) {
    uint32_t index = key->hash() % capacity;
    Entry *tombstone = nullptr;
    while (true) {
        Entry *entry = entries + index;
        if (entry->key == nullptr) {
            if (entry->value.isNil()) {
                return tombstone != nullptr ? tombstone : entry;
            } else {
                if (tombstone == nullptr) tombstone = entry;
            }
        } else if (entry->key == key) {
            return entry;
        }
        index = (index + 1) % capacity;
    }
}

void Table::resize(int capacity) {
    auto entries = new Entry[capacity];

    _count = 0;
    for (int i = 0; i < _capacity; i++) {
        Entry *entry = _entries + i;
        if (entry->key == nullptr) continue;

        Entry *dest = find(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        _count++;
    }

    std::swap(_entries, entries);
    delete[] entries;
    _capacity = capacity;
}

bool Table::get(const ObjString *key, Value *value) {
    if (_count == 0) return false;

    Entry *entry = find(key);
    if (entry->key == nullptr) return false;

    *value = entry->value;
    return true;
}

bool Table::set(ObjString *key, Value value) {
    if (_count + 1 > static_cast<int>(static_cast<double>(_capacity) * 0.75)) {
        int capacity = _capacity < 8 ? 8 : 2 * _capacity;
        resize(capacity);
    }

    Entry *entry = find(key);
    bool isNewKey = entry->key == nullptr;
    if (isNewKey && entry->value.isNil()) _count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool Table::remove(const ObjString *key) {
    if (_count == 0) return false;

    Entry *entry = find(key);
    if (entry->key == nullptr) return false;

    entry->key = nullptr;
    entry->value = Value(true);
    return true;
}

void Table::merge(Table &from) {
    for (int i = 0; i < from._capacity; i++) {
        Entry *entry = from._entries + i;
        if (entry->key != nullptr) {
            set(entry->key, entry->value);
        }
    }
}

ObjString *Table::find(const char *chars, int length, uint32_t hash) {
    if (_count == 0) return nullptr;
    uint32_t index = hash % _capacity;
    while (true) {
        Entry *entry = _entries + index;
        ObjString *key = entry->key;
        if (key == nullptr) {
            if (entry->value.isNil()) return nullptr;
        } else if (key->length() == length &&
                   key->hash() == hash &&
                   memcmp(key->chars(), chars, length) == 0) {
            return key;
        }
        index = (index + 1) % _capacity;
    }
}

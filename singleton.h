//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_SINGLETON_H
#define CPPLOX_SINGLETON_H

#include "non_copyable.h"

template<class T>
class Singleton : NonCopyable {
public:
    static T &instance() {
        static T instance;
        return instance;
    }
};

#endif //CPPLOX_SINGLETON_H

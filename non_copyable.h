//
// Created by Andrew Huang on 1/10/2022.
//

#ifndef CPPLOX_NON_COPYABLE_H
#define CPPLOX_NON_COPYABLE_H

struct NonCopyable {
    NonCopyable() = default;

    NonCopyable(const NonCopyable &) = delete;

    NonCopyable &operator=(const NonCopyable &) = delete;
};

#endif //CPPLOX_NON_COPYABLE_H

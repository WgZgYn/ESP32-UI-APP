//
// Created by 20495 on 24-10-7.
//

#ifndef UTILS_H
#define UTILS_H

namespace utils {
    template<typename T>
    T abs_diff(T& a, T& b) {
        return a > b ? a - b : b - a;
    }
} // utils

#endif //UTILS_H

//
// Created by 20495 on 24-10-7.
//

#ifndef UTILS_H
#define UTILS_H

#define DEBUG_SRT() do {
#define DEBUG_END() } while(0)


namespace utils {
    template<typename T>
    T abs_diff(const T& a, const T& b) {
        return a > b ? a - b : b - a;
    }
} // utils

#endif //UTILS_H

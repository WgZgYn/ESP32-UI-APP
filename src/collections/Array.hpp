//
// Created by 20495 on 24-10-8.
//

#ifndef ARRAY_H
#define ARRAY_H

namespace collections {
    template<size_t N>
    class Array {
    private:
        size_t _size;
        char _data[N];

    public:
        Array() = default;

        ~Array() = default;

        template<typename T>
        T *operator[](size_t offset) {
            return reinterpret_cast<T *>(_data + offset);
        }

        template<typename T>
        const T *operator[](size_t offset) const {
            return reinterpret_cast<const T *>(_data + offset);
        }
    };

    template<>
    class Array<0> {
    public:
        Array() = default;

        ~Array() = default;

        template<typename T>
        T *operator[](size_t offset) {
            return nullptr;
        }

        template<typename T>
        const T *operator[](size_t offset) const {
            return nullptr;
        }
    };
}

#endif //ARRAY_H

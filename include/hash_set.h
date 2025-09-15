#ifndef STRUCTZ_HASH_SET_H
#define STRUCTZ_HASH_SET_H

#include <cstddef>
#include <utility>
#include <variant>
#include "hash_map.h"

template<typename T>
class HashSet {
    HashMap<T, std::monostate> map;

public:
    explicit HashSet(const std::size_t initial_capacity = 8)
        : map(initial_capacity) {}

    bool insert(T key) {
        return map.set(std::move(key), std::monostate{});
    }

    bool remove(const T& key) {
        return map.remove(key);
    }

    [[nodiscard]] bool contains(const T& key) const {
        return map.contains(key);
    }
};

#endif

#ifndef STRUCTZ_HASH_SET_H
#define STRUCTZ_HASH_SET_H

#include <cstddef>
#include <utility>
#include <variant>
#include "hash_map.h"

template<typename T>
class HashSet {
    HashMap<T, std::monostate> map;

    void swap(HashSet<T>& other) noexcept {
        std::swap(map, other.map);
    }

public:
    explicit HashSet(const std::size_t initial_capacity = 8)
        : map(initial_capacity) {}

    [[nodiscard]] std::size_t size() const {
        return map.size();
    }

    [[nodiscard]] bool is_empty() const {
        return map.is_empty();
    }

    [[nodiscard]] bool contains(const T& key) const {
        return map.contains(key);
    }

    bool insert(T key) {
        return map.set(std::move(key), std::monostate{});
    }

    bool remove(const T& key) {
        return map.remove(key);
    }

    void clear() {
        HashSet<T>().swap(*this);
    }
};

#endif

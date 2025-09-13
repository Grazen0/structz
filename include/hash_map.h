#ifndef STRUCTZ_HASH_MAP_H
#define STRUCTZ_HASH_MAP_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include "linked_list.h"
#include "vec.h"

constexpr std::size_t MAX_COLLISIONS = 3;
constexpr double MAX_FILL_FACTOR = 0.8;

template<typename K, typename T>
class HashMap {
    struct Entry {
        K key;
        std::size_t hash;
        T value;

        Entry(K key, const std::size_t hash, T value)
            : key(std::move(key)),
              hash(hash),
              value(std::move(value)) {};
    };

    using Bucket = LinkedList<Entry>;

    std::size_t size = 0;
    std::size_t used_buckets = 0;
    Vec<Bucket> buckets;

    static std::size_t get_hash_code(const K& key) {
        return std::hash<K>()(key);
    }

    [[nodiscard]] double fill_factor() const {
        return (double)used_buckets / (double)buckets.size();
    }

    void rehash() {
        const std::size_t new_capacity = buckets.size() * 2;
        Vec<Bucket> new_buckets(new_capacity);

        for (std::size_t i = 0; i < buckets.size(); ++i) {
            while (!buckets[i].empty()) {
                const Entry entry = buckets[i].pop_front();
                const size_t new_index = entry.hash % new_capacity;

                new_buckets[new_index].push_front(std::move(entry));

                if (new_buckets.size() == 1)
                    ++used_buckets;
            }
        }

        buckets = new_buckets;
    }

public:
    explicit HashMap(const std::size_t initial_capacity = 8)
        : buckets(initial_capacity) {}

    void set(K key, T value) {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % buckets.size();

        for (auto& entry : buckets[index]) {
            if (entry.key == key) {
                entry.value = std::move(value);
                return;
            }
        }

        buckets[index].push_front(
            Entry(std::move(key), hash, std::move(value)));
        ++size;

        if (buckets[index].size() == 1)
            ++used_buckets;

        if (buckets[index].size() > MAX_COLLISIONS ||
            fill_factor() > MAX_FILL_FACTOR)
            rehash();
    }

    T& get(const K& key) {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % buckets.size();

        for (auto& entry : buckets[index]) {
            if (entry.key == key)
                return entry.value;
        }

        throw std::out_of_range("Key not found");
    }

    const T& get(const K& key) const {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % buckets.size();

        for (const auto& entry : buckets[index]) {
            if (entry.key == key)
                return entry.value;
        }

        throw std::out_of_range("Key not found");
    }

    bool remove(const K& key) {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % buckets.size();
        auto& bucket = buckets[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if ((*it).key == key) {
                buckets[index].remove(it);
                return true;
            }
        }

        return false;
    }

    [[nodiscard]] bool contains(const K& key) const {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % buckets.size();

        for (auto& el : buckets[index]) {
            if (el.key == key)
                return true;
        }

        return false;
    }
};

#endif

#ifndef STRUCTZ_HASH_MAP_H
#define STRUCTZ_HASH_MAP_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include "linked_list.h"

constexpr size_t MAX_COLLISIONS = 3;
constexpr double MAX_FILL_FACTOR = 0.8;

template<typename K, typename T>
class HashMap {
    struct Entry {
        K key;
        size_t hash;
        T value;

        Entry(K key, const size_t hash, T value)
            : key(std::move(key)),
              hash(hash),
              value(std::move(value)) {};
    };

    size_t size = 0;
    size_t used_buckets = 0;
    size_t capacity;
    LinkedList<Entry>* buckets;

    static size_t get_hash_code(const K& key) {
        return std::hash<K>()(key);
    }

    [[nodiscard]] double fill_factor() const {
        return (double)used_buckets / (double)capacity;
    }

    void rehash() {
        const size_t new_capacity = capacity * 2;
        auto* const new_buckets = new LinkedList<Entry>[new_capacity]();

        for (size_t i = 0; i < capacity; ++i) {
            for (const Entry& entry : buckets[i]) {
                const size_t new_index = entry.hash % new_capacity;
                new_buckets[new_index].push_front(std::move(entry));
            }
        }

        delete[] std::exchange(buckets, new_buckets);
        capacity = new_capacity;
    }

public:
    explicit HashMap(const size_t initial_capacity = 8)
        : capacity(initial_capacity),
          buckets(new LinkedList<Entry>[capacity]()) {}

    HashMap(const HashMap<K, T>& other) = delete;
    HashMap(HashMap<K, T>&& other) = delete;
    HashMap& operator=(const HashMap<K, T>& other) = delete;
    HashMap& operator=(HashMap<K, T>&& other) = delete;

    ~HashMap() {
        delete[] buckets;
    }

    void set(K key, T value) {
        const size_t hash = get_hash_code(key);
        const size_t index = hash % capacity;

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
        const size_t hash = get_hash_code(key);
        const size_t index = hash % capacity;

        for (auto& entry : buckets[index]) {
            if (entry.key == key)
                return entry.value;
        }

        throw std::out_of_range("Key not found");
    }

    const T& get(const K& key) const {
        const size_t hash = get_hash_code(key);
        const size_t index = hash % capacity;

        for (const auto& entry : buckets[index]) {
            if (entry.key == key)
                return entry.value;
        }

        throw std::out_of_range("Key not found");
    }

    bool remove(const K& key) {
        const size_t hash = get_hash_code(key);
        const size_t index = hash % capacity;
        auto& bucket = buckets[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if ((*it).key == key) {
                buckets[index].remove(it);
                return true;
            }
        }

        return false;
    }

    bool contains(const K& key) const {
        const size_t hash = get_hash_code(key);
        const size_t index = hash % capacity;

        for (auto& el : buckets[index]) {
            if (el.key == key)
                return true;
        }

        return false;
    }

    [[nodiscard]] size_t bucket_count() const {
        return capacity;
    }

    [[nodiscard]] size_t bucket_size(size_t index) const {
        return buckets[index].size();
    }

    auto begin(const size_t index) {
        if (index >= capacity)
            throw std::out_of_range("Bucket index out of bounds");

        return buckets[index].begin();
    }

    auto end(const size_t index) {
        if (index >= capacity)
            throw std::out_of_range("Bucket index out of bounds");

        return buckets[index].end();
    }
};

#endif

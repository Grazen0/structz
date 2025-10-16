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

    std::size_t m_size = 0;
    std::size_t m_used_buckets = 0;
    Vec<Bucket> m_buckets;

    static std::size_t get_hash_code(const K& key) {
        return std::hash<K>()(key);
    }

    [[nodiscard]] double fill_factor() const {
        return static_cast<double>(m_used_buckets) / static_cast<double>(m_buckets.size());
    }

    void swap(HashMap<K, T>& other) noexcept {
        std::swap(m_size, other.m_size);
        std::swap(m_used_buckets, other.m_used_buckets);
        std::swap(m_buckets, other.m_buckets);
    }

    void rehash() {
        const std::size_t new_capacity = m_buckets.size() * 2;
        Vec<Bucket> new_buckets(new_capacity);

        for (std::size_t i = 0; i < m_buckets.size(); ++i) {
            while (!m_buckets[i].is_empty()) {
                const Entry entry = m_buckets[i].pop_front();
                const size_t new_index = entry.hash % new_capacity;

                new_buckets[new_index].push_front(std::move(entry));

                if (new_buckets.size() == 1)
                    ++m_used_buckets;
            }
        }

        m_buckets = std::move(new_buckets);
    }

public:
    explicit HashMap(const std::size_t initial_capacity = 8)
        : m_buckets(initial_capacity) {}

    [[nodiscard]] std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] bool is_empty() const {
        return m_size == 0;
    }

    [[nodiscard]] bool contains(const K& key) const {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % m_buckets.size();

        for (auto& el : m_buckets[index]) {
            if (el.key == key)
                return true;
        }

        return false;
    }

    bool set(K key, T value) {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % m_buckets.size();

        for (auto& entry : m_buckets[index]) {
            if (entry.key == key) {
                entry.value = std::move(value);
                return false;
            }
        }

        m_buckets[index].push_front(Entry(std::move(key), hash, std::move(value)));
        ++m_size;

        if (m_buckets[index].size() == 1)
            ++m_used_buckets;

        if (m_buckets[index].size() > MAX_COLLISIONS || fill_factor() > MAX_FILL_FACTOR)
            rehash();

        return true;
    }

    T& get(const K& key) {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % m_buckets.size();

        for (auto& entry : m_buckets[index]) {
            if (entry.key == key)
                return entry.value;
        }

        throw std::out_of_range("Key not found");
    }

    const T& get(const K& key) const {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % m_buckets.size();

        for (const auto& entry : m_buckets[index]) {
            if (entry.key == key)
                return entry.value;
        }

        throw std::out_of_range("Key not found");
    }

    bool remove(const K& key) {
        const std::size_t hash = get_hash_code(key);
        const std::size_t index = hash % m_buckets.size();
        auto& bucket = m_buckets[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                m_buckets[index].remove(it);
                --m_size;
                return true;
            }
        }

        return false;
    }

    void clear() {
        HashMap<K, T>().swap(*this);
    }
};

#endif

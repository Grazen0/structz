#ifndef STRUCTZ_BTREE
#define STRUCTZ_BTREE

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include "stack.h"
#include "vec.h"

template<typename K, typename T, std::size_t M = 5>
class BTree {
    struct Entry {
        K key;
        T value;

        Entry(K key, T value)
            : key(std::move(key)),
              value(std::move(value)) {}
    };

    struct Node {
        std::array<Entry, M - 1> entries{};
        std::array<Node*, M> children{nullptr};
        std::size_t entry_count = 0;
        bool is_leaf = true;
    };

    std::size_t m_size = 0;
    Node* m_root = nullptr;

    template<typename Self>
    static auto& get(Self& self, const K& key) {
        auto* cur = self.m_root;

        while (cur != nullptr) {
            std::size_t i = 0;

            while (i < cur->entry_count && cur->entries[i].key < key)
                ++i;

            if (i < cur->entry_count && cur->entries[i].key == key)
                return cur->entries[i].value;

            if (cur->is_leaf)
                break;

            cur = cur->children[i];
        }

        throw std::runtime_error("Key not found");
    }

    static std::string to_string(const Node* const node, const std::string& sep = ", ") {
        if (node == nullptr)
            return "";

        std::string result;

        for (std::size_t i = 0; i < node->entry_count; ++i) {
            if (!node->is_leaf)
                result += to_string(node->children[i], sep);

            result += std::to_string(node->entries[i].key) + sep;
        }

        if (!node->is_leaf)
            result += to_string(node->children[node->entry_count]);

        return result;
    }

    void range_search(Node* const node, Vec<std::pair<K&, T&>> out, const K& begin, const K& end) {
        if (node == nullptr)
            return;

        // TODO: implement
    }

    void range_search(const Node* const node,
                      Vec<std::pair<K&, T&>> out,
                      const K& begin,
                      const K& end) {
        if (node == nullptr)
            return;

        // TODO: implement
    }

    void swap(BTree& other) noexcept {
        std::swap(m_root, other.m_root);
        std::swap(m_size, other.m_size);
    }

public:
    BTree() = default;

    BTree(const BTree& other)
        : m_size(other.m_size) {
        Stack<std::pair<Node**, Node*>> stack;
        stack.push({&m_root, other.m_root});

        while (!stack.is_empty()) {
            const auto [dest, src] = stack.pop();
            if (src == nullptr)
                continue;

            *dest = new Node();

            (*dest)->entries = src->entries;
            (*dest)->key_count = src->key_count;
            (*dest)->is_leaf = src->is_leaf;

            for (std::size_t i = 0; i < M; ++i)
                stack.push({&(*dest)->children[i], src->children[i]});
        }
    }

    BTree(BTree&& other) noexcept {
        swap(other);
    }

    ~BTree() {
        Stack<Node*> stack;
        stack.push(std::exchange(m_root, nullptr));

        while (!stack.is_empty()) {
            Node* const node = stack.pop();
            if (node == nullptr)
                continue;

            for (Node* const child : node->children)
                stack.push(child);

            delete node;
        }

        m_size = 0;
    }

    BTree& operator=(const BTree& other) {
        BTree(other).swap(*this);
        return *this;
    }

    BTree& operator=(BTree&& other) noexcept {
        swap(other);
        return *this;
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return m_size == 0;
    }

    [[nodiscard]] bool contains(const K& key) const {
        Node* cur = m_root;

        while (cur != nullptr) {
            std::size_t i = 0;

            while (i < cur->entry_count && cur->entries[i].key < key)
                ++i;

            if (i < cur->entry_count && cur->entries[i].key == key)
                return true;

            if (cur->is_leaf)
                break;

            cur = cur->children[i];
        }

        return false;
    }

    [[nodiscard]] const T& get(const K& key) const {
        return get(*this, key);
    }

    [[nodiscard]] T& get(const K& key) {
        return get(*this, key);
    }

    [[nodiscard]] std::string to_string() const {
        return to_string(m_root);
    }

    [[nodiscard]] Vec<std::pair<K&, T&>> range_search(const K& begin, const K& end) {
        Vec<std::pair<K&, T&>> out;
        range_search(m_root, out, begin, end);
        return out;
    }

    [[nodiscard]] Vec<std::pair<const K&, const T&>> range_search(const K& begin,
                                                                  const K& end) const {
        Vec<std::pair<const K&, const T&>> out;
        range_search(m_root, out, begin, end);
        return out;
    }

    bool insert(K key, T value) {
        Node* cur = m_root;
    }

    void clear() {
        BTree().swap(*this);
    }
};

#endif

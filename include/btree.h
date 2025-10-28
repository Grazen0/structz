#ifndef STRUCTZ_BTREE
#define STRUCTZ_BTREE

#include <array>
#include <cmath>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include "stack.h"
#include "vec.h"

template<typename K, typename T, typename Compare = std::less<K>, std::size_t M = 5>
class BTree {
public:
    struct Entry {
        K key;
        T value;

        Entry() = default;

        Entry(K key, T value)
            : key(std::move(key)),
              value(std::move(value)) {}
    };

    struct Node {
        std::array<Entry, M - 1> entries{};
        std::array<Node*, M> children{nullptr};
        std::size_t entry_count = 0;
        bool is_leaf = true;

        Node(std::initializer_list<std::pair<K, T>> elements)
            : entry_count(elements.size()) {
            std::size_t i = 0;

            for (auto& [key, value] : elements) {
                entries[i] = Entry(key, value);
                ++i;
            }
        }

        void link_child(const std::size_t idx, std::initializer_list<std::pair<K, T>> elements) {
            is_leaf = false;
            children[idx] = new Node(elements);
        }
    };

    std::size_t m_size = 0;
    Node* m_root = nullptr;
    Compare cmp{};

    template<typename Self>
    static auto& get(Self& self, const K& key) {
        auto* cur = self.m_root;

        while (cur != nullptr) {
            std::size_t i = 0;

            while (i < cur->entry_count && self.cmp(cur->entries[i].key, key))
                ++i;

            if (i < cur->entry_count && cur->entries[i].key == key)
                return cur->entries[i].value;

            if (cur->is_leaf)
                break;

            cur = cur->children[i];
        }

        throw std::runtime_error("Key not found");
    }

    [[nodiscard]] static const K& min_key(const Node* const node) {
        const Node* cur = node;

        while (!cur->is_leaf)
            cur = cur->children[0];

        return cur->entries[0].key;
    }

    [[nodiscard]] static const K& max_key(const Node* const node) {
        const Node* cur = node;

        while (!cur->is_leaf)
            cur = cur->children[cur->entry_count];

        return cur->entries[cur->entry_count - 1].key;
    }

    static std::string to_string(const Node* const node, const std::string& sep = ",") {
        if (node == nullptr)
            return "";

        std::string result;

        for (std::size_t i = 0; i < node->entry_count; ++i) {
            if (!node->is_leaf)
                result += to_string(node->children[i], sep) + sep;

            result += std::to_string(node->entries[i].key);

            if (i < node->entry_count - 1)
                result += sep;
        }

        if (!node->is_leaf)
            result += sep + to_string(node->children[node->entry_count]);

        return result;
    }

    template<typename Self>
    void range_search(Self& self,
                      Node* const node,
                      Vec<std::pair<const K*, T*>>& out,
                      const K& begin,
                      const K& end) {
        if (node == nullptr)
            return;

        for (std::size_t i = 0; i < node->entry_count; ++i) {
            auto& entry = node->entries[i];

            if (self.cmp(begin, entry.key))
                range_search(self, node->children[i], out, begin, end);

            if (!self.cmp(entry.key, begin) && !self.cmp(end, entry.key))
                out.push({&entry.key, &entry.value});
        }

        range_search(self, node->children[node->entry_count], out, begin, end);
    }

    void swap(BTree& other) noexcept {
        std::swap(m_root, other.m_root);
        std::swap(m_size, other.m_size);
    }

    // template<typename Iter>
    // static Node* build_node_from_ordered_iterator(const Iter& begin, const Iter& end) {
    //     const std::size_t n = std::distance(begin, end);
    //
    //     if (n == 0)
    //         return nullptr;
    //
    //     const std::size_t partition_size = n / M;
    //
    //     Node* const node = new Node();
    //
    //     for (std::size_t i = 0; i < M; ++i) {
    //         const std::size_t base = i * partition_size;
    //     }
    //
    //     return node;
    // }

    std::pair<bool, std::optional<std::pair<Entry, Node*>>> insert(Node* const node,
                                                                   K key,
                                                                   T value) {
        std::size_t i = 0;
        while (i < node->entry_count && cmp(node->entries[i].key, key))
            ++i;

        if (i < node->entry_count && node->entries[i].key == key) {
            // Key already exists, replace value
            node->entries[i].value = std::move(value);
            return {false, std::nullopt};
        }

        if (!node->is_leaf) {
            // Didn't find key here, go down.
            auto& [result, split] = insert(node->children[i], std::move(key), std::move(value));

            if (split) {
                // TODO: handle split
            }

            return result;
        }

        // Didn't find but we're already at a leaf!! Insert here.
        if (node->entry_count < M - 1) {
            // There still is space to insert.
            for (std::size_t j = i + 1; j < node->entry_count + 1; ++j)
                node->entries[j] = std::move(node->entries[j - 1]);

            node->entries[i] = Entry(std::move(key), std::move(value));
            ++node->entry_count;
            return {true, std::nullopt};
        }

        // No space left to insert, do split.
        // FIX: mid might be the newly inserted element
        Entry mid = std::move(node->entries[(M - 1) / 2]);

        Node* const split_node = new Node();

        node->entry_count = node->entry_count / 2;

        return {
            true, {mid, split_node}
        };
    }

    [[nodiscard]] std::size_t height(const Node* const node) const {
        std::size_t height = 0;
        const Node* cur = node;

        while (cur != nullptr) {
            cur = cur->children[0];
            ++height;
        }

        return height;
    }

    [[nodiscard]] bool check_properties(const Node* const node) const {
        const std::size_t min_entries = node == m_root ? 1 : std::ceil(M / 2.0) - 1;
        const std::size_t max_entries = M - 1;

        // Check entry count
        if (node->entry_count < min_entries || node->entry_count > max_entries)
            return false;

        // Keys must be ordered
        for (std::size_t i = 0; i < node->entry_count - 1; ++i) {
            if (!cmp(node->entries[i].key, node->entries[i + 1].key))
                return false;
        }

        if (!node->is_leaf) {
            // Non-leaf nodes must have entry_count + 1 children
            for (std::size_t i = 0; i < node->entry_count + 1; ++i) {
                if (node->children[i] == nullptr)
                    return false;
            }

            // Check properties recursively
            for (std::size_t i = 0; i < node->entry_count + 1; ++i) {
                if (!check_properties(node->children[i]))
                    return false;
            }

            const std::size_t expected_height = height(node->children[0]);

            // Subtree heights should be equal
            for (std::size_t i = 1; i < node->entry_count + 1; ++i) {
                // NOTE: esta llamada es costosa. Podría aplicarse un caché o algo parecido, pero no
                // estamos considerando mucho el costo de check_properties.
                if (height(node->children[i]) != expected_height)
                    return false;
            }

            // Leftmost subtree should be less than first key
            if (!cmp(max_key(node->children[0]), node->entries[0].key))
                return false;

            // Rightmost subtree should be greater than last key
            if (!cmp(min_key(node->children[node->entry_count]),
                     node->entries[node->entry_count - 1].key))
                return false;

            // Check that subtrees go inside the correct entries
            for (std::size_t i = 1; i < node->entry_count; ++i) {
                const K& min = min_key(node->children[i]);
                const K& max = max_key(node->children[i]);

                if (!cmp(node->entries[i - 1].key, min) || !cmp(max, node->entries[i].key))
                    return false;
            }

        } else {
            // Leaf nodes should have no children
            for (std::size_t i = 0; i < node->entry_count + 1; ++i) {
                if (node->children[i] != nullptr)
                    return false;
            }
        }

        return true;
    }

    static BTree build_from_ordered_vector(const Vec<std::pair<K, T>>& elements) {
        BTree result;

        for (const auto& el : elements)
            result.insert(el.first, el.second);

        return result;
    }

public:
    BTree() = default;

    explicit BTree(Node* const root)
        : m_root(root) {}

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

    [[nodiscard]] std::size_t height() const {
        return height(m_root);
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return m_size == 0;
    }

    [[nodiscard]] bool check_properties() const {
        if (m_root == nullptr)
            return true;

        return check_properties(m_root);
    }

    [[nodiscard]] bool contains_key(const K& key) const {
        const Node* cur = m_root;

        while (cur != nullptr) {
            std::size_t i = 0;

            while (i < cur->entry_count && cmp(cur->entries[i].key, key))
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

    [[nodiscard]] Vec<std::pair<const K*, T*>> range_search(const K& begin, const K& end) {
        Vec<std::pair<const K*, T*>> out;
        range_search(*this, m_root, out, begin, end);
        return out;
    }

    [[nodiscard]] Vec<std::pair<const K*, const T*>> range_search(const K& begin,
                                                                  const K& end) const {
        Vec<std::pair<const K*, const T*>> out;
        range_search(*this, m_root, out, begin, end);
        return out;
    }

    [[nodiscard]] const K& min_key() const {
        if (m_root == nullptr)
            throw std::runtime_error("BTree is empty");

        return min_key(m_root);
    }

    [[nodiscard]] const K& max_key() const {
        if (m_root == nullptr)
            throw std::runtime_error("BTree is empty");

        return max_key(m_root);
    }

    bool insert(K key, T value) {
        if (m_root == nullptr) {
            m_root = new Node({
                {key, value}
            });
            ++m_size;
            return true;
        }

        const bool result = insert(m_root, key, value);

        if (result)
            ++m_size;

        return result;
    }

    void clear() {
        BTree().swap(*this);
    }
};

#endif

#ifndef STRUCTZ_BTREE
#define STRUCTZ_BTREE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include "stack.h"
#include "vec.h"

template<typename K, typename T, typename Compare = std::less<K>, std::size_t M = 5>
class BTree {
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

        Node() = default;

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
            result += sep + to_string(node->children[node->entry_count], sep);

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

    std::variant<bool, std::pair<Entry, Node*>> insert(Node* const node, K key, T value) {
        if (node == nullptr)
            return std::pair{Entry(std::move(key), std::move(value)), nullptr};

        std::size_t i = 0;
        while (i < node->entry_count && cmp(node->entries[i].key, key))
            ++i;

        if (i < node->entry_count && node->entries[i].key == key) {
            node->entries[i].value = std::move(value);
            return false;
        }

        auto result = insert(node->children[i], std::move(key), std::move(value));

        if (std::holds_alternative<bool>(result))
            return result;

        auto& [new_entry, new_child] = std::get<std::pair<Entry, Node*>>(result);

        if (node->entry_count < M - 1) {
            node->children[node->entry_count + 1] = node->children[node->entry_count];

            for (std::size_t j = node->entry_count; j > i; --j) {
                node->entries[j] = std::move(node->entries[j - 1]);
                node->children[j] = node->children[j - 1];
            }

            node->entries[i] = std::move(new_entry);
            node->children[i] = new_child;
            ++node->entry_count;

            return true;
        }

        std::array<Entry, M> entries_tmp;
        std::array<Node*, M + 1> children_tmp{};
        std::size_t e = 0;
        bool new_entry_pending = true;

        for (std::size_t j = 0; j < M; ++j) {
            if (new_entry_pending &&
                (e >= node->entry_count || cmp(new_entry.key, node->entries[e].key))) {
                entries_tmp[j] = std::move(new_entry);
                children_tmp[j] = new_child;
                new_entry_pending = false;
            } else {
                entries_tmp[j] = std::move(node->entries[e]);
                children_tmp[j] = std::exchange(node->children[e], nullptr);
                ++e;
            }
        }

        children_tmp[M] = std::exchange(node->children[M - 1], nullptr);

        const std::size_t mid = (M - 1) / 2;
        const Entry lifted = std::move(entries_tmp[mid]);

        Node* const left_split = new Node();
        left_split->is_leaf = node->is_leaf;

        left_split->entry_count = mid;
        node->entry_count = M - 1 - mid;

        for (std::size_t i = 0; i < left_split->entry_count; ++i) {
            left_split->entries[i] = std::move(entries_tmp[i]);
            left_split->children[i] = children_tmp[i];
        }

        left_split->children[left_split->entry_count] = children_tmp[mid];

        for (std::size_t i = 0; i < node->entry_count; ++i) {
            node->entries[i] = std::move(entries_tmp[mid + 1 + i]);
            node->children[i] = children_tmp[mid + 1 + i];
        }

        node->children[node->entry_count] = children_tmp[M];

        return std::pair{lifted, left_split};
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
        if (node == nullptr)
            return true;

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

            // Last key should be less than rightmost subtree
            if (!cmp(node->entries[node->entry_count - 1].key,
                     min_key(node->children[node->entry_count])))
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

public:
    static BTree build_from_ordered_vector(const Vec<std::pair<K, T>>& elements) {
        BTree result;

        for (const auto& el : elements)
            result.insert(el.first, el.second);

        return result;
    }

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
        const auto result = insert(m_root, key, value);

        if (const bool* const inserted = std::get_if<bool>(&result)) {
            if (*inserted)
                ++m_size;

            return *inserted;
        }

        auto [new_entry, new_child] = std::get<std::pair<Entry, Node*>>(result);

        Node* const new_root = new Node();
        new_root->is_leaf = new_child == nullptr;

        new_root->entries[0] = std::move(new_entry);
        new_root->entry_count = 1;

        new_root->children[0] = new_child;
        new_root->children[1] = m_root;

        m_root = new_root;

        ++m_size;
        return true;
    }

    // bool remove(const K& key) {}

    void clear() {
        BTree().swap(*this);
    }
};

#endif

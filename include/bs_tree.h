#ifndef STRUCTZ_BS_TREE_H
#define STRUCTZ_BS_TREE_H

#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>
#include "stack.h"

template<typename K, typename T, typename Compare = std::less<K>>
class BSTree {
    struct Node {
        K key;
        T value;
        Node* left = nullptr;
        Node* right = nullptr;

        Node(K key, T value)
            : key(std::move(key)),
              value(std::move(value)) {}
    };

    Node* m_root = nullptr;
    std::size_t m_size = 0;
    Compare cmp{};

    static std::ptrdiff_t height(Node* const node) {
        if (node == nullptr)
            return -1;

        return 1 + std::max(height(node->left), height(node->right));
    }

    void swap(BSTree& other) noexcept {
        std::swap(m_root, other.m_root);
        std::swap(m_size, other.m_size);
    }

public:
    BSTree() = default;

    BSTree(const BSTree& other) {
        Stack<std::pair<Node**, Node*>> stack;
        stack.push({&m_root, other.m_root});

        while (!stack.is_empty()) {
            const auto [dest, src] = stack.pop();
            if (src == nullptr)
                continue;

            *dest = new Node(src->key, src->value);
            stack.push({&(*dest)->left, src->left});
            stack.push({&(*dest)->right, src->right});
        }

        m_size = other.m_size;
    }

    BSTree(BSTree&& other) noexcept {
        swap(other);
    }

    ~BSTree() {
        Stack<Node*> stack;
        stack.push(std::exchange(m_root, nullptr));

        while (!stack.is_empty()) {
            Node* const node = stack.pop();
            if (node == nullptr)
                continue;

            stack.push(node->left);
            stack.push(node->right);
            delete node;
        }

        m_size = 0;
    }

    BSTree& operator=(const BSTree& other) {
        BSTree(other).swap(*this);
        return *this;
    }

    BSTree& operator=(BSTree&& other) noexcept {
        swap(other);
        return *this;
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return m_size == 0;
    }

    [[nodiscard]] std::ptrdiff_t height() const {
        return height(m_root);
    }

    [[nodiscard]] bool contains(const K& key) const {
        Node* cur = m_root;

        while (cur != nullptr) {
            if (cur->key == key)
                return true;

            if (cmp(key, cur->key))
                cur = cur->left;
            else
                cur = cur->right;
        }

        return false;
    }

    [[nodiscard]] const T& get(const K& key) const {
        Node* cur = m_root;

        while (cur != nullptr) {
            if (cur->key == key)
                return cur->value;

            if (cmp(key, cur->key))
                cur = cur->left;
            else
                cur = cur->right;
        }

        throw std::out_of_range("Key not found");
    }

    [[nodiscard]] T& get(const K& key) {
        Node* cur = m_root;

        while (cur != nullptr) {
            if (cur->key == key)
                return cur->value;

            if (cmp(key, cur->key))
                cur = cur->left;
            else
                cur = cur->right;
        }

        throw std::out_of_range("Key not found");
    }

    bool insert(K key, T value) {
        Node* parent = nullptr;
        Node** cur = &m_root;

        while (*cur != nullptr) {
            if ((*cur)->key == key) {
                (*cur)->value = std::move(value);
                return false;
            }

            parent = *cur;

            if (cmp(key, (*cur)->key))
                cur = &(*cur)->left;
            else
                cur = &(*cur)->right;
        }

        *cur = new Node(std::move(key), std::move(value));
        ++m_size;

        return true;
    }

    bool remove(const K& key) {
        Node** cur = &m_root;

        while (*cur != nullptr && (*cur)->key != key) {
            if (cmp(key, (*cur)->key))
                cur = &(*cur)->left;
            else
                cur = &(*cur)->right;
        }

        if (*cur == nullptr)
            return false;

        if ((*cur)->left == nullptr) {
            delete std::exchange(*cur, (*cur)->right);
        } else if ((*cur)->right == nullptr) {
            delete std::exchange(*cur, (*cur)->left);
        } else {
            Node** succ = &(*cur)->right;

            while ((*succ)->left != nullptr)
                succ = &(*succ)->left;

            (*cur)->key = std::move((*succ)->key);
            (*cur)->value = std::move((*succ)->value);

            delete std::exchange(*succ, (*succ)->right);
        }

        --m_size;
        return true;
    }

    void clear() {
        BSTree().swap(*this);
    }
};

#endif

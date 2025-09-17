#ifndef STRUCTZ_AVL_TREE_H
#define STRUCTZ_AVL_TREE_H

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <utility>
#include "stack.h"
#include "vec.h"

template<typename K, typename T, typename Compare = std::less<K>>
class AvlTree {
    struct Node {
        K key;
        T value;
        std::size_t height;
        Node* left = nullptr;
        Node* right = nullptr;

        explicit Node(K key, T value, const std::size_t height)
            : key(std::move(key)),
              value(std::move(value)),
              height(height) {}
    };

    std::size_t m_size = 0;
    Node* m_root = nullptr;
    Compare cmp{};

    [[nodiscard]] static std::ptrdiff_t bf(Node* const node) {
        return height(node->right) - height(node->left);
    }

    [[nodiscard]] static std::ptrdiff_t height(Node* const node) {
        if (node == nullptr)
            return -1;

        return node->height;
    }

    constexpr static void update_height(Node* const node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }

    static void rotate_left(Node** const x_ptr, Node* const z) {
        Node* const x = *x_ptr;

        *x_ptr = z;
        x->right = z->left;
        z->left = x;

        update_height(x);
        update_height(z);
    }

    static void rotate_right(Node** const x_ptr, Node* const z) {
        Node* const x = *x_ptr;

        *x_ptr = z;
        x->left = z->right;
        z->right = x;

        update_height(x);
        update_height(z);
    }

    static void rebalance(Node** const x) {
        Node* const z =
            height((*x)->left) > height((*x)->right) ? (*x)->left : (*x)->right;

        if (z == (*x)->right) {
            if (bf(z) >= 0) {
                rotate_left(x, z);
            } else {
                rotate_right(&(*x)->right, z->left);
                rotate_left(x, (*x)->right);
            }
        } else if (z == (*x)->left) {
            if (bf(z) <= 0) {
                rotate_right(x, z);
            } else {
                rotate_left(&(*x)->left, z->right);
                rotate_right(x, (*x)->left);
            }
        }
    }

    void swap(AvlTree& other) noexcept {
        std::swap(m_root, other.m_root);
        std::swap(m_size, other.m_size);
    }

public:
    AvlTree() = default;

    AvlTree(const AvlTree& other)
        : m_size(other.m_size) {
        Vec<std::pair<Node**, Node*>> stack;
        stack.push({&m_root, other.m_root});

        while (!stack.is_empty()) {
            const auto [dest, src] = stack.pop();
            if (src == nullptr)
                continue;

            *dest = new Node(src->key, src->value, src->height);
            stack.push({&(*dest)->left, src->left});
            stack.push({&(*dest)->right, src->right});
        }
    }

    AvlTree(AvlTree&& other) noexcept {
        swap(other);
    }

    ~AvlTree() {
        Vec<Node*> stack;
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

    AvlTree& operator=(const AvlTree& other) noexcept {
        AvlTree(other).swap(*this);
        return *this;
    }

    AvlTree& operator=(AvlTree&& other) noexcept {
        swap(other);
        return *this;
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return m_size == 0;
    }

    [[nodiscard]] constexpr std::ptrdiff_t height() const {
        return height(m_root);
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

        throw std::out_of_range("key not found");
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

        throw std::out_of_range("key not found");
    }

    bool insert(K key, T value) {
        Stack<Node**> path;

        Node** cur = &m_root;

        while (*cur != nullptr) {
            path.push(cur);

            if ((*cur)->key == key) {
                (*cur)->value = std::move(value);
                return false;
            }

            if (cmp(key, (*cur)->key))
                cur = &(*cur)->left;
            else
                cur = &(*cur)->right;
        }

        *cur = new Node(std::move(key), std::move(value), 0);
        ++m_size;

        while (!path.is_empty()) {
            Node** const node = path.pop();
            update_height(*node);

            if (std::abs(bf(*node)) > 1) {
                rebalance(node);
                break;
            }
        }

        return true;
    }

    bool remove(const K& key) {
        Stack<Node**> path;
        Node** cur = &m_root;

        while (*cur != nullptr && (*cur)->key != key) {
            path.push(cur);

            if (cmp(key, (*cur)->key))
                cur = &(*cur)->left;
            else
                cur = &(*cur)->right;
        }

        if (*cur == nullptr)
            return false;

        if ((*cur)->left == nullptr && (*cur)->right == nullptr) {
            delete std::exchange(*cur, nullptr);
        } else if ((*cur)->left == nullptr) {
            delete std::exchange(*cur, (*cur)->right);
        } else if ((*cur)->right == nullptr) {
            delete std::exchange(*cur, (*cur)->left);
        } else {
            path.push(cur);
            Node** succ = &(*cur)->right;

            while ((*succ)->left != nullptr) {
                path.push(succ);
                succ = &(*succ)->left;
            }

            (*cur)->key = std::move((*succ)->key);
            (*cur)->value = std::move((*succ)->value);

            delete std::exchange(*succ, (*succ)->right);
        }

        while (!path.is_empty()) {
            Node** const node = path.pop();
            update_height(*node);

            if (std::abs(bf(*node)) > 1)
                rebalance(node);
        }

        --m_size;
        return true;
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

    void clear() {
        AvlTree().swap(*this);
    }
};

#endif

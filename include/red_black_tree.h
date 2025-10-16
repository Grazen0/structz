#ifndef STRUCTZ_RED_BLACK_TREE_H
#define STRUCTZ_RED_BLACK_TREE_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>
#include "vec.h"

template<typename K, typename T, typename Compare = std::less<K>>
class RedBlackTree {
    enum class Color : std::uint8_t {
        Red,
        Black
    };

    struct Node {
        K key;
        T value;
        Color color;
        std::size_t height;
        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;

        explicit Node(K key,
                      T value,
                      const Color color,
                      const std::size_t height,
                      Node* const parent)
            : key(std::move(key)),
              value(std::move(value)),
              color(color),
              height(height),
              parent(parent) {}
    };

    std::size_t m_size = 0;
    Node* m_root = nullptr;
    Compare cmp{};

    [[nodiscard]] constexpr static std::ptrdiff_t height(Node* const node) {
        if (node == nullptr)
            return -1;

        return node->height;
    }

    [[nodiscard]] constexpr static Color color(Node* const node) {
        if (node == nullptr)
            return Color::Black;

        return node->color;
    }

    [[nodiscard]] constexpr static Node* sibling(Node* const node) {
        if (node->parent == nullptr)
            throw std::runtime_error("Node does not have parent");

        Node* const parent = node->parent;
        return node == parent->left ? parent->right : parent->left;
    }

    constexpr static void update_height(Node* const node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }

    constexpr static void update_heights_upward(Node* const leaf) {
        Node* cur = leaf;

        while (cur != nullptr) {
            update_height(cur);
            cur = cur->parent;
        }
    }

    static void rotate_left(Node** const x_ptr, Node* const z) {
        Node* const x = *x_ptr;

        *x_ptr = z;
        (*x_ptr)->parent = x->parent;

        x->right = z->left;
        if (x->right != nullptr)
            x->right->parent = x;

        z->left = x;
        z->left->parent = z;

        update_heights_upward(x);
    }

    static void rotate_right(Node** const x_ptr, Node* const z) {
        Node* const x = *x_ptr;

        *x_ptr = z;
        (*x_ptr)->parent = x->parent;

        x->left = z->right;
        if (x->left != nullptr)
            x->left->parent = x;

        z->right = x;
        z->right->parent = z;

        update_heights_upward(x);
    }

    void swap(RedBlackTree& other) noexcept {
        std::swap(m_root, other.m_root);
        std::swap(m_size, other.m_size);
    }

    [[nodiscard]] constexpr Node** holder(Node* const node) {
        if (node->parent == nullptr)
            return &m_root;

        if (node == node->parent->left)
            return &node->parent->left;

        return &node->parent->right;
    }

    void rebalance_from(Node* const leaf) {
        Node* node = leaf;

        while (node->parent != nullptr) {
            Node* const parent = node->parent;

            if (parent == nullptr) {
                // Case 3
                break;
            }

            if (color(parent) == Color::Black) {
                // Case 1
                break;
            }

            Node* const grandpa = parent->parent;
            if (grandpa == nullptr) {
                // Case 4
                parent->color = Color::Black;
                break;
            }

            Node* const uncle = sibling(parent);
            if (color(uncle) == Color::Red) {
                // Case 2
                parent->color = Color::Black;
                uncle->color = Color::Black;
                grandpa->color = Color::Red;

                node = grandpa;
                continue;
            }

            if (node == parent->right && parent == grandpa->left) {
                // Case 5 (left)
                rotate_left(&grandpa->left, node);
                node = parent;
                continue;
            }

            if (node == parent->left && parent == grandpa->right) {
                // Case 5 (right)
                rotate_right(&grandpa->right, node);
                node = parent;
                continue;
            }

            if (node == parent->left) {
                // Case 6 (left)
                rotate_right(holder(grandpa), parent);
                parent->color = Color::Black;
                grandpa->color = Color::Red;
                break;
            }

            if (node == parent->right) {
                // Case 6 (right)
                rotate_left(holder(grandpa), parent);
                parent->color = Color::Black;
                grandpa->color = Color::Red;
                break;
            }

            throw std::runtime_error("case not found");
        }

        if (m_root)
            m_root->color = Color::Black;
    }

public:
    RedBlackTree() = default;

    RedBlackTree(const RedBlackTree& other)
        : m_size(other.m_size) {
        Vec<std::tuple<Node**, Node*, Node*>> stack;
        stack.push({&m_root, other.m_root, nullptr});

        while (!stack.is_empty()) {
            const auto [dest, src, parent] = stack.pop();
            if (src == nullptr)
                continue;

            *dest = new Node(src->key, src->value, src->color, src->height, parent);
            stack.push({&(*dest)->left, src->left, *dest});
            stack.push({&(*dest)->right, src->right, *dest});
        }
    }

    RedBlackTree(RedBlackTree&& other) noexcept {
        swap(other);
    }

    ~RedBlackTree() {
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

    RedBlackTree& operator=(const RedBlackTree& other) {
        RedBlackTree(other).swap(*this);
        return *this;
    }

    RedBlackTree& operator=(RedBlackTree&& other) noexcept {
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

        *cur = new Node(std::move(key), std::move(value), Color::Red, 0, parent);

        ++m_size;
        update_heights_upward(parent);
        rebalance_from(*cur);

        return true;
    }

    // bool remove(const T& key) {}

    void clear() {
        RedBlackTree().swap(*this);
    }
};

#endif

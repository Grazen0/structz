#include "trie.h"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string_view>

std::size_t Trie::idx(const char ch) {
    return ch - 'a';
}

void Trie::insert(const std::string_view word) {
    Node** cur = &root;

    for (const char ch : word) {
        if (ch < 'a' || ch > 'z')
            throw std::runtime_error("Trie only accepts lowercase ASCII characters.");

        if (*cur == nullptr)
            *cur = new Node();

        cur = &(*cur)->children.at(idx(ch));
    }

    if (*cur == nullptr)
        *cur = new Node();

    (*cur)->is_end = true;
}

bool Trie::contains(const std::string_view word) const {
    if (root == nullptr)
        return false;

    Node* cur = root;

    for (const char ch : word) {
        cur = cur->children.at(idx(ch));

        if (cur == nullptr)
            return false;
    }

    return cur->is_end;
}

bool Trie::has_prefix(const std::string_view prefix) const {
    if (root == nullptr)
        return false;

    Node* cur = root;

    for (const char ch : prefix) {
        cur = cur->children.at(idx(ch));

        if (cur == nullptr)
            return false;
    }

    return true;
}

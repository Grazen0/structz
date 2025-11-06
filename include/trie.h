#ifndef STRUCTZ_TRIE_H
#define STRUCTZ_TRIE_H

#include <array>
#include <cstddef>
#include <string_view>

class Trie {
    struct Node {
        static constexpr std::size_t SIZE = 'z' - 'a' + 1;

        std::array<Node*, SIZE> children{nullptr};
        bool is_end = false;
    };

    Node* root = nullptr;

    static std::size_t idx(char ch);

public:
    void insert(std::string_view word);

    [[nodiscard]] bool contains(std::string_view word) const;

    [[nodiscard]] bool has_prefix(std::string_view prefix) const;
};

#endif

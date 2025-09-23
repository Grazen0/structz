#include <catch2/catch_test_macros.hpp>
#include "red_black_tree.h"

TEST_CASE("Empty tree properties") {
    RedBlackTree<int, std::string> tree;

    REQUIRE(tree.is_empty());
    REQUIRE(tree.size() == 0);
    REQUIRE(tree.height() == -1);

    SECTION("Contains and get throw on empty") {
        REQUIRE_FALSE(tree.contains(10));
        REQUIRE_THROWS_AS(tree.get(10), std::out_of_range);
    }
}

TEST_CASE("Insert single element") {
    RedBlackTree<int, std::string> tree;
    REQUIRE(tree.insert(1, "one"));
    REQUIRE(tree.size() == 1);
    REQUIRE_FALSE(tree.is_empty());
    REQUIRE(tree.contains(1));
    REQUIRE(tree.get(1) == "one");
}

TEST_CASE("Insert duplicate keys replaces value but does not grow") {
    RedBlackTree<int, int> tree;
    REQUIRE(tree.insert(42, 1));
    REQUIRE_FALSE(tree.insert(42, 2));  // duplicate
    REQUIRE(tree.size() == 1);
    REQUIRE(tree.get(42) == 2);
}

TEST_CASE("Multiple inserts produce valid structure") {
    RedBlackTree<int, int> tree;
    std::vector<int> keys = {10, 5, 15, 1, 7, 12, 20};
    for (int k : keys) {
        REQUIRE(tree.insert(k, k * 10));
    }

    REQUIRE(tree.size() == keys.size());
    for (int k : keys) {
        REQUIRE(tree.contains(k));
        REQUIRE(tree.get(k) == k * 10);
    }

    SECTION("Unknown key") {
        REQUIRE_FALSE(tree.contains(99));
        REQUIRE_THROWS_AS(tree.get(99), std::out_of_range);
    }
}

TEST_CASE("Height grows with insertions") {
    RedBlackTree<int, int> tree;
    REQUIRE(tree.height() == -1);
    tree.insert(1, 10);
    REQUIRE(tree.height() >= 0);
    tree.insert(2, 20);
    tree.insert(3, 30);
    REQUIRE(tree.height() >= 1);
}

TEST_CASE("Clear empties the tree") {
    RedBlackTree<int, int> tree;
    tree.insert(1, 10);
    tree.insert(2, 20);
    REQUIRE(tree.size() == 2);

    tree.clear();
    REQUIRE(tree.is_empty());
    REQUIRE(tree.size() == 0);
    REQUIRE(tree.height() == -1);
    REQUIRE_FALSE(tree.contains(1));
}

TEST_CASE("Copy constructor produces deep copy") {
    RedBlackTree<int, int> tree;
    tree.insert(1, 10);
    tree.insert(2, 20);

    RedBlackTree<int, int> copy(tree);
    REQUIRE(copy.size() == 2);
    REQUIRE(copy.contains(1));
    REQUIRE(copy.contains(2));

    // Mutating original should not affect copy
    tree.insert(3, 30);
    REQUIRE(tree.contains(3));
    REQUIRE_FALSE(copy.contains(3));
}

TEST_CASE("Move constructor transfers ownership") {
    RedBlackTree<int, int> tree;
    tree.insert(1, 10);
    tree.insert(2, 20);

    RedBlackTree<int, int> moved(std::move(tree));
    REQUIRE(moved.size() == 2);
    REQUIRE(moved.contains(1));
    REQUIRE(moved.contains(2));

    REQUIRE(tree.size() == 0);
    REQUIRE(tree.is_empty());
}

TEST_CASE("Assignment operators work correctly") {
    RedBlackTree<int, int> a;
    a.insert(1, 100);

    RedBlackTree<int, int> b;
    b.insert(2, 200);

    SECTION("Copy assignment") {
        b = a;
        REQUIRE(b.size() == 1);
        REQUIRE(b.contains(1));
        REQUIRE_FALSE(b.contains(2));
    }

    SECTION("Move assignment") {
        b = std::move(a);
        REQUIRE(b.size() == 1);
        REQUIRE(b.contains(1));
    }
}

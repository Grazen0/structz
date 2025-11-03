#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <utility>
#include "btree.h"
#include "vec.h"

TEST_CASE("Teacher's test #3", "[btree]") {
    using BTree = BTree<int, int>;

    const Vec<std::pair<int, int>> elements = {
        { 1,  1},
        { 2,  2},
        { 3,  3},
        { 4,  4},
        { 5,  5},
        { 6,  6},
        { 7,  7},
        { 8,  8},
        { 9,  9},
        {10, 10},
        {11, 11},
        {12, 12},
        {13, 13},
        {14, 14},
        {15, 15},
        {16, 16},
        {17, 17},
        {18, 18},
        {19, 19},
        {20, 20}
    };

    const BTree btree = BTree::build_from_ordered_vector(elements);

    REQUIRE(btree.check_properties());
    REQUIRE(btree.to_string() == "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20");
}

TEST_CASE("insertion", "[btree]") {
    using BTree = BTree<int, int>;

    BTree btree;

    btree.insert(1, 1);
    btree.insert(1, 1);
}

TEST_CASE("main test", "[btree]") {
    BTree<int, int, std::less<>, 3> btree;

    int numbers[10] = {45, 75, 100, 36, 120, 70, 11, 111, 47, 114};

    for (int i = 0; i < 10; i++)
        btree.insert(numbers[i], numbers[i]);

    REQUIRE(btree.check_properties());

    REQUIRE(btree.to_string(" ") == "11 36 45 47 70 75 100 111 114 120");
    REQUIRE(btree.height() == 2);

    for (int i = 0; i < 10; i = i + 2)
        REQUIRE(btree.contains_key(numbers[i]));

    REQUIRE(btree.min_key() == 11);
    REQUIRE(btree.max_key() == 120);

    btree.remove(100);
    btree.remove(111);
    btree.remove(45);

    REQUIRE(btree.check_properties());

    REQUIRE(btree.to_string(" ") == "11 36 47 70 75 114 120");
    REQUIRE(btree.height() == 1);

    btree.remove(114);
    btree.insert(40, 40);

    REQUIRE(btree.to_string(" - ") == "11 - 36 - 40 - 47 - 70 - 75 - 120");
    REQUIRE(btree.height() == 2);

    btree.insert(125, 125);
    btree.insert(115, 115);

    REQUIRE(btree.max_key() == 125);
    REQUIRE(btree.contains_key(47));

    btree.remove(11);

    REQUIRE(!btree.contains_key(11));
    REQUIRE(btree.min_key() == 36);
    REQUIRE(btree.size() == 8);

    btree.clear();
    REQUIRE(btree.size() == 0);
    REQUIRE(btree.is_empty());
    REQUIRE(btree.height() == -1);

    Vec<std::pair<int, int>> elements = {
        { 1,  1},
        { 2,  2},
        { 3,  3},
        { 4,  4},
        { 5,  5},
        { 6,  6},
        { 7,  7},
        { 8,  8},
        { 9,  9},
        {10, 10},
        {11, 11},
        {12, 12},
        {13, 13},
        {14, 14},
        {15, 15},
        {16, 16},
        {17, 17},
        {18, 18},
        {19, 19},
        {20, 20}
    };

    auto btree2 = BTree<int, int, std::less<>, 4>::build_from_ordered_vector(elements);

    REQUIRE(btree2.to_string(",") == "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20");
    REQUIRE(btree2.check_properties());
}

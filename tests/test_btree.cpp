#include <catch2/catch_test_macros.hpp>
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

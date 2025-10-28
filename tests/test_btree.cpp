#include <catch2/catch_test_macros.hpp>
#include <string>
#include <utility>
#include "btree.h"
#include "vec.h"

TEST_CASE("Teacher's test #1", "[btree]") {
    using BTree = BTree<int, int>;
    using Node = BTree::Node;

    Node* root = new Node({
        {11, 11},
        {16, 16},
        {20, 20},
        {31, 31}
    });
    root->link_child(0, {
                            {3, 3},
                            {5, 5}
    });
    root->link_child(1, {
                            {12, 12},
                            {15, 15}
    });
    root->link_child(2, {
                            {17, 17},
                            {19, 19}
    });
    root->link_child(3, {
                            {22, 22},
                            {25, 25},
                            {27, 27},
                            {28, 28}
    });
    root->link_child(4, {
                            {32, 32},
                            {36, 36},
                            {37, 37},
                            {38, 38}
    });
    BTree btree(root);

    REQUIRE(btree.contains_key(25));
    REQUIRE(btree.contains_key(16));
    REQUIRE(!btree.contains_key(35));

    REQUIRE(btree.to_string() == "3,5,11,12,15,16,17,19,20,22,25,27,28,31,32,36,37,38");

    std::string range_result;
    const auto result = btree.range_search(14, 20);

    for (auto elem : result)
        range_result += std::to_string(*elem.first) + ",";

    REQUIRE(range_result == "15,16,17,19,20,");
}

TEST_CASE("Teacher's test #2", "[btree]") {
    using BTree = BTree<int, int>;
    using Node = BTree::Node;

    Node* root = new Node({
        {20, 20}
    });
    root->link_child(0, {
                            {11, 11},
                            {16, 16}
    });
    root->link_child(1, {
                            {31, 31},
                            {35, 35}
    });
    root->children[0]->link_child(0, {
                                         {3, 3},
                                         {5, 5}
    });
    root->children[0]->link_child(1, {
                                         {12, 12},
                                         {15, 15}
    });
    root->children[0]->link_child(2, {
                                         {17, 17},
                                         {19, 19}
    });
    root->children[1]->link_child(0, {
                                         {22, 22},
                                         {25, 25},
                                         {27, 27},
                                         {28, 28}
    });
    root->children[1]->link_child(1, {
                                         {32, 32},
                                         {33, 33}
    });
    root->children[1]->link_child(2, {
                                         {36, 36},
                                         {38, 38}
    });

    BTree btree(root);

    REQUIRE(btree.contains_key(25));
    REQUIRE(btree.contains_key(16));
    REQUIRE(!btree.contains_key(37));

    REQUIRE(btree.to_string() == "3,5,11,12,15,16,17,19,20,22,25,27,28,31,32,33,35,36,38");

    std::string range_result;

    const auto result = btree.range_search(14, 21);

    for (auto elem : result)
        range_result += std::to_string(*elem.first) + ",";

    REQUIRE(range_result == "15,16,17,19,20,");
}

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

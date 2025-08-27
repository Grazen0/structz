#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include "linked_list.h"

TEST_CASE("linked lists can be constructed", "[linked_list]") {
    const LinkedList<int> list;
    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());

    REQUIRE_THROWS_AS(list[0], std::out_of_range);
    REQUIRE_THROWS_AS(list[1], std::out_of_range);
    REQUIRE_THROWS_AS(list[2], std::out_of_range);
}

TEST_CASE("linked lists can be copied", "[linked_list]") {
    LinkedList<int> list_a;
    list_a.push_front(1);
    list_a.push_front(2);
    list_a.push_front(3);
    list_a.push_front(4);

    LinkedList<int> list_b(list_a);
    list_b.push_front(5);

    REQUIRE(list_a.size() == 4);
    REQUIRE(list_b.size() == 5);

    REQUIRE(list_a[0] == 4);
    REQUIRE(list_a[1] == 3);
    REQUIRE(list_a[2] == 2);
    REQUIRE(list_a[3] == 1);

    REQUIRE(list_b[0] == 5);
    REQUIRE(list_b[1] == 4);
    REQUIRE(list_b[2] == 3);
    REQUIRE(list_b[3] == 2);
    REQUIRE(list_b[4] == 1);
}

TEST_CASE("can perform multiple operations on a linked list", "[linked_list]") {
    LinkedList<int> list;

    REQUIRE(list.size() == 0);
    REQUIRE(list.empty());

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    REQUIRE(list.size() == 3);
    REQUIRE(list[0] == 1);
    REQUIRE(list[1] == 2);
    REQUIRE(list[2] == 3);
    REQUIRE_THROWS_AS(list[3], std::out_of_range);
    REQUIRE_THROWS_AS(list[42], std::out_of_range);
    REQUIRE_THROWS_AS(list[-1], std::out_of_range);
    REQUIRE(list.front() == 1);
    REQUIRE(list.back() == 3);

    list.push_front(0);

    REQUIRE(list.size() == 4);
    REQUIRE(list[0] == 0);
    REQUIRE(list[1] == 1);
    REQUIRE(list[2] == 2);
    REQUIRE(list[3] == 3);
    REQUIRE(list.front() == 0);
    REQUIRE(list.back() == 3);

    list.reverse();

    REQUIRE(list.size() == 4);
    REQUIRE(list[0] == 3);
    REQUIRE(list[1] == 2);
    REQUIRE(list[2] == 1);
    REQUIRE(list[3] == 0);
    REQUIRE(list.front() == 3);
    REQUIRE(list.back() == 0);

    const int first = list.pop_front();
    const int last = list.pop_back();
    const int prev_last = list.pop_back();

    REQUIRE(list.size() == 1);
    REQUIRE(list[0] == 2);
    REQUIRE(first == 3);
    REQUIRE(prev_last == 1);
    REQUIRE(last == 0);

    list.clear();

    REQUIRE(list.empty());

    list.push_back(42);

    REQUIRE(list[0] == 42);
}

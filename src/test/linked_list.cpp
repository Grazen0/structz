#include "../linked_list.hpp"
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

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

TEST_CASE("linked lists can be pushed from to the front", "[linked_list]") {
    LinkedList<int> list;
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);

    REQUIRE(list.size() == 3);
    REQUIRE(list[0] == 3);
    REQUIRE(list[1] == 2);
    REQUIRE(list[2] == 1);
}

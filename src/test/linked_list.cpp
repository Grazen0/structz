#include "../linked_list.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("linked lists can be constructed", "[linked_list]") {
    const LinkedList<int> list;
    REQUIRE(list.size() == 0);
    REQUIRE(list.is_empty());
}

TEST_CASE("linked lists can be copied", "[linked_list]") {
    LinkedList<int> list_a;
    list_a.prepend(1);
    list_a.prepend(2);
    list_a.prepend(3);
    list_a.prepend(4);

    LinkedList<int> list_b{list_a};
    list_b.prepend(5);

    REQUIRE(list_a.size() == 4);
    REQUIRE(list_b.size() == 5);
}

TEST_CASE("linked lists can be prepended to", "[linked_list]") {
    LinkedList<int> list;
    list.prepend(1);
    list.prepend(2);
    list.prepend(3);

    REQUIRE(list.size() == 3);
    REQUIRE(list.get_at(0) == 3);
    REQUIRE(list.get_at(1) == 2);
    REQUIRE(list.get_at(2) == 1);
}

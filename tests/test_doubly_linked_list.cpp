#include <catch2/catch_test_macros.hpp>
#include "doubly_linked_list.h"

TEST_CASE("Empty list basics", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;

    REQUIRE(list.is_empty());
    REQUIRE(list.size() == 0);

    SECTION("front/back/pop throw on empty") {
        REQUIRE_THROWS_AS(list.front(), std::runtime_error);
        REQUIRE_THROWS_AS(list.back(), std::runtime_error);
        REQUIRE_THROWS_AS(list.pop_front(), std::runtime_error);
        REQUIRE_THROWS_AS(list.pop_back(), std::runtime_error);
    }
}

TEST_CASE("Push and access front/back", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;

    list.push_front(10);
    REQUIRE_FALSE(list.is_empty());
    REQUIRE(list.size() == 1);
    REQUIRE(list.front() == 10);
    REQUIRE(list.back() == 10);

    list.push_front(5);
    REQUIRE(list.front() == 5);
    REQUIRE(list.back() == 10);
    REQUIRE(list.size() == 2);

    list.push_back(20);
    REQUIRE(list.back() == 20);
    REQUIRE(list.front() == 5);
    REQUIRE(list.size() == 3);
}

TEST_CASE("Pop front/back", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    REQUIRE(list.pop_front() == 1);
    REQUIRE(list.front() == 2);
    REQUIRE(list.size() == 2);

    REQUIRE(list.pop_back() == 3);
    REQUIRE(list.back() == 2);
    REQUIRE(list.size() == 1);

    REQUIRE(list.pop_back() == 2);
    REQUIRE(list.is_empty());
}

TEST_CASE("Indexing", "[doublylinkedlist]") {
    DoublyLinkedList<std::string> list;
    list.push_back("a");
    list.push_back("b");
    list.push_back("c");

    REQUIRE(list[0] == "a");
    REQUIRE(list[1] == "b");
    REQUIRE(list[2] == "c");

    list[1] = "beta";
    REQUIRE(list[1] == "beta");

    REQUIRE_THROWS_AS(list[3], std::out_of_range);
}

TEST_CASE("Iterators", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 3; ++i)
        list.push_back(i);

    std::vector<int> forward;
    for (int n : list)
        forward.push_back(n);

    REQUIRE(forward == std::vector<int>{1, 2, 3});
}

TEST_CASE("Const iterators", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;
    list.push_back(10);
    list.push_back(20);

    const auto& const_list = list;

    std::vector<int> values;
    for (int n : const_list)
        values.push_back(n);

    REQUIRE(values == std::vector<int>{10, 20});
}

TEST_CASE("Reverse list", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 5; ++i)
        list.push_back(i);

    list.reverse();

    std::vector<int> values;
    for (int n : list)
        values.push_back(n);

    REQUIRE(values == std::vector<int>{5, 4, 3, 2, 1});
}

TEST_CASE("Clear", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;
    list.push_back(100);
    list.push_back(200);

    list.clear();
    REQUIRE(list.is_empty());
    REQUIRE(list.size() == 0);

    REQUIRE_THROWS_AS(list.front(), std::runtime_error);
}

TEST_CASE("Remove elements", "[doublylinkedlist]") {
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 5; ++i)
        list.push_back(i);

    SECTION("Remove middle element") {
        list.remove(2);
        REQUIRE(list.size() == 4);
        REQUIRE(list[0] == 1);
        REQUIRE(list[1] == 2);
        REQUIRE(list[2] == 4);
        REQUIRE(list[3] == 5);
    }

    SECTION("Remove head") {
        list.remove(0);
        REQUIRE(list.size() == 4);
        REQUIRE(list.front() == 2);
        REQUIRE(list[0] == 2);
    }

    SECTION("Remove tail") {
        list.remove(4);
        REQUIRE(list.size() == 4);
        REQUIRE(list.back() == 4);
        REQUIRE(list[3] == 4);
    }

    SECTION("Out of range") {
        REQUIRE_THROWS_AS(list.remove(10), std::out_of_range);
    }
}

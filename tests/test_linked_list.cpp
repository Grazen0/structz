#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
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

TEST_CASE("linked lists can be iterated", "[linked_list]") {
    LinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    auto it = list.begin();

    REQUIRE(*(it++) == 1);
    REQUIRE(*(it++) == 2);
    REQUIRE(*(it++) == 3);
    REQUIRE(*(it++) == 4);
    REQUIRE(*(it++) == 5);
    REQUIRE(it == list.end());
}

TEST_CASE("linked lists support remove() with iterator", "[linked_list]") {
    LinkedList<int> list;
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    list.push_back(40);

    SECTION("remove from the middle") {
        auto it = list.begin();
        ++it;  // points to 20
        list.remove(it);

        REQUIRE(list.size() == 3);
        REQUIRE(list[0] == 10);
        REQUIRE(list[1] == 30);
        REQUIRE(list[2] == 40);
    }

    SECTION("remove first element") {
        auto it = list.begin();  // points to 10
        list.remove(it);

        REQUIRE(list.size() == 3);
        REQUIRE(list[0] == 20);
        REQUIRE(list.front() == 20);
    }

    SECTION("remove last element") {
        auto it = list.begin();
        ++it;
        ++it;
        ++it;  // points to 40
        list.remove(it);

        REQUIRE(list.size() == 3);
        REQUIRE(list[2] == 30);
        REQUIRE(list.back() == 30);
    }

    SECTION("remove only element") {
        LinkedList<int> single;
        single.push_back(99);
        auto it = single.begin();
        single.remove(it);

        REQUIRE(single.empty());
        REQUIRE(single.size() == 0);
    }

    SECTION("remove multiple elements in sequence") {
        auto it = list.begin();
        ++it;             // points to 20
        list.remove(it);  // remove 20

        it = list.begin();
        ++it;             // now points to 30
        list.remove(it);  // remove 30

        REQUIRE(list.size() == 2);
        REQUIRE(list[0] == 10);
        REQUIRE(list[1] == 40);
    }
}

TEST_CASE("empty linked list does not iterate", "[linked_list]") {
    std::vector<int> result;
    LinkedList<int> list;

    for (auto& el : list)
        result.push_back(el);

    REQUIRE(result == std::vector<int>{});
}

TEST_CASE("iterator validity and behavior", "[linked_list]") {
    LinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);

    SECTION("post-increment returns old value, advances iterator") {
        auto it = list.begin();
        auto old = it++;
        REQUIRE(*old == 1);
        REQUIRE(*it == 2);
    }

    SECTION("pre-increment advances iterator, returns new value") {
        auto it = list.begin();
        auto& ref = ++it;
        REQUIRE(&ref == &it);  // pre-increment returns same object
        REQUIRE(*it == 2);
    }

    SECTION("iterators can traverse from begin to end") {
        std::vector<int> values;
        for (int& el : list) {
            values.push_back(el);
        }
        REQUIRE(values == std::vector<int>{1, 2, 3, 4});
    }

    SECTION("iterator equality and inequality works") {
        auto it1 = list.begin();
        auto it2 = list.begin();
        REQUIRE(it1 == it2);

        ++it2;
        REQUIRE(it1 != it2);
    }
}

#include <catch2/catch_test_macros.hpp>
#include "vec.h"

TEST_CASE("Vec basic properties", "[Vec]") {
    Vec<int> v;

    SECTION("Initially empty") {
        REQUIRE(v.size() == 0);
        REQUIRE(v.is_empty());
        REQUIRE_THROWS_AS(v.first(), std::out_of_range);
        REQUIRE_THROWS_AS(v.last(), std::out_of_range);
        REQUIRE_THROWS_AS(v.pop(), std::out_of_range);
    }
}

TEST_CASE("Vec push and pop", "[Vec]") {
    Vec<int> v;

    SECTION("Push single element") {
        v.push(42);
        REQUIRE(v.size() == 1);
        REQUIRE_FALSE(v.is_empty());
        REQUIRE(v.first() == 42);
        REQUIRE(v.last() == 42);
        REQUIRE(v[0] == 42);
    }

    SECTION("Push multiple elements and pop") {
        v.push(1);
        v.push(2);
        v.push(3);

        REQUIRE(v.size() == 3);
        REQUIRE(v.first() == 1);
        REQUIRE(v.last() == 3);

        int popped = v.pop();
        REQUIRE(popped == 3);
        REQUIRE(v.size() == 2);
        REQUIRE(v.last() == 2);
    }
}

TEST_CASE("Vec indexing", "[Vec]") {
    Vec<int> v;
    v.push(10);
    v.push(20);

    SECTION("Valid indices") {
        REQUIRE(v[0] == 10);
        REQUIRE(v[1] == 20);
    }

    SECTION("Out-of-bounds access throws") {
        REQUIRE_THROWS_AS(v[2], std::out_of_range);
        REQUIRE_THROWS_AS(v[999], std::out_of_range);
    }
}

TEST_CASE("Vec copy semantics", "[Vec]") {
    Vec<int> v;
    v.push(5);
    v.push(10);

    SECTION("Copy constructor") {
        Vec<int> copy(v);
        REQUIRE(copy.size() == v.size());
        REQUIRE(copy[0] == 5);
        REQUIRE(copy[1] == 10);
    }

    SECTION("Copy assignment") {
        Vec<int> copy;
        copy = v;
        REQUIRE(copy.size() == v.size());
        REQUIRE(copy[0] == 5);
        REQUIRE(copy[1] == 10);
    }
}

TEST_CASE("Vec move semantics", "[Vec]") {
    Vec<int> v;
    v.push(100);
    v.push(200);

    SECTION("Move constructor") {
        Vec<int> moved(std::move(v));
        REQUIRE(moved.size() == 2);
        REQUIRE(moved[0] == 100);
        REQUIRE(moved[1] == 200);
    }

    SECTION("Move assignment") {
        Vec<int> moved;
        moved = std::move(v);
        REQUIRE(moved.size() == 2);
        REQUIRE(moved[0] == 100);
        REQUIRE(moved[1] == 200);
    }
}

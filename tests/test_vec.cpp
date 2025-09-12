#include <catch2/catch_test_macros.hpp>
#include <string>
#include "vec.h"

TEST_CASE("Vec: Default construction", "[Vec]") {
    Vec<int> v;
    REQUIRE(v.size() == 0);
    REQUIRE(v.is_empty());
}

TEST_CASE("Vec: Construction with size", "[Vec]") {
    Vec<int> v(5);
    REQUIRE(v.size() == 5);
    for (std::size_t i = 0; i < v.size(); ++i) {
        REQUIRE(v[i] == 0);  // value-initialized
    }
}

TEST_CASE("Vec: with_capacity static factory", "[Vec]") {
    auto v = Vec<int>::with_capacity(10);
    REQUIRE(v.size() == 0);
    REQUIRE(v.is_empty());
    v.push(42);
    REQUIRE(v.size() == 1);
    REQUIRE(v[0] == 42);
}

TEST_CASE("Vec: Copy construction", "[Vec]") {
    Vec<std::string> v;
    v.push("hello");
    v.push("world");
    Vec<std::string> copy(v);
    REQUIRE(copy.size() == 2);
    REQUIRE(copy[0] == "hello");
    REQUIRE(copy[1] == "world");
}

TEST_CASE("Vec: Move construction", "[Vec]") {
    Vec<int> v;
    v.push(10);
    v.push(20);
    Vec<int> moved(std::move(v));
    REQUIRE(moved.size() == 2);
    REQUIRE(moved[0] == 10);
    REQUIRE(moved[1] == 20);
    REQUIRE(v.size() == 0);  // moved-from state
}

TEST_CASE("Vec: Copy assignment", "[Vec]") {
    Vec<int> v1;
    v1.push(1);
    v1.push(2);
    Vec<int> v2;
    v2 = v1;
    REQUIRE(v2.size() == 2);
    REQUIRE(v2[0] == 1);
    REQUIRE(v2[1] == 2);
}

TEST_CASE("Vec: Move assignment", "[Vec]") {
    Vec<int> v1;
    v1.push(5);
    v1.push(6);
    Vec<int> v2;
    v2 = std::move(v1);
    REQUIRE(v2.size() == 2);
    REQUIRE(v2[0] == 5);
    REQUIRE(v2[1] == 6);
    REQUIRE(v1.size() == 0);
}

TEST_CASE("Vec: Push and automatic resize", "[Vec]") {
    Vec<int> v;
    for (int i = 0; i < 10; ++i) {
        v.push(i);
    }
    REQUIRE(v.size() == 10);
    for (int i = 0; i < 10; ++i) {
        REQUIRE(v[i] == i);
    }
}

TEST_CASE("Vec: Pop elements", "[Vec]") {
    Vec<int> v;
    v.push(1);
    v.push(2);
    v.push(3);
    REQUIRE(v.pop() == 3);
    REQUIRE(v.size() == 2);
    REQUIRE(v.pop() == 2);
    REQUIRE(v.pop() == 1);
    REQUIRE(v.size() == 0);
    REQUIRE_THROWS_AS(v.pop(), std::out_of_range);
}

TEST_CASE("Vec: Access first and last", "[Vec]") {
    Vec<int> v;
    v.push(7);
    v.push(8);
    v.push(9);
    REQUIRE(v.first() == 7);
    REQUIRE(v.last() == 9);
    const auto& cv = v;
    REQUIRE(cv.first() == 7);
    REQUIRE(cv.last() == 9);

    Vec<int> empty;
    REQUIRE_THROWS_AS(empty.first(), std::out_of_range);
    REQUIRE_THROWS_AS(empty.last(), std::out_of_range);
}

TEST_CASE("Vec: operator[] bounds checking", "[Vec]") {
    Vec<int> v(3);
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    REQUIRE(v[1] == 2);
    const auto& cv = v;
    REQUIRE(cv[2] == 3);
    REQUIRE_THROWS_AS(v[3], std::out_of_range);
    REQUIRE_THROWS_AS(cv[3], std::out_of_range);
}

TEST_CASE("Vec: Iterators basic traversal", "[Vec]") {
    Vec<int> v;
    for (int i = 0; i < 5; ++i)
        v.push(i);
    int expected = 0;
    for (int& el : v) {
        REQUIRE(el == expected++);
    }
}

TEST_CASE("Vec: Const iterators", "[Vec]") {
    Vec<int> v;
    for (int i = 0; i < 5; ++i)
        v.push(i * 10);
    const auto& cv = v;
    int expected = 0;
    for (int el : cv) {
        REQUIRE(el == expected * 10);
        ++expected;
    }
}

TEST_CASE("Vec: Reverse iterators", "[Vec]") {
    Vec<int> v;
    for (int i = 1; i <= 5; ++i)
        v.push(i);
    int expected = 1;
    for (auto it = v.rend(); it != v.rbegin(); --it) {
        if (it == v.rend())
            continue;  // skip the end sentinel
        REQUIRE(*it == expected++);
    }
}

TEST_CASE("Vec: Const reverse iterators", "[Vec]") {
    Vec<int> v;
    for (int i = 1; i <= 3; ++i)
        v.push(i);
    const auto& cv = v;
    auto it = cv.rbegin();
    REQUIRE(*it == 3);
    ++it;
    REQUIRE(*it == 2);
    ++it;
    REQUIRE(*it == 1);
    ++it;
    REQUIRE(it == cv.rend());
}

TEST_CASE("Vec: Iterator relational ops", "[Vec]") {
    Vec<int> v(3);
    auto it1 = v.begin();
    auto it2 = v.begin() + 2;
    REQUIRE(it1 < it2);
    REQUIRE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it1 != it2);
}

TEST_CASE("Vec: Reverse iterator relational ops", "[Vec]") {
    Vec<int> v(3);
    auto it1 = v.rbegin();
    auto it2 = v.rend();
    REQUIRE(it1 != it2);
}

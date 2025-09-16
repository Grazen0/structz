#include <catch2/catch_test_macros.hpp>
#include "vec.h"

TEST_CASE("Vec basic construction", "[vec]") {
    Vec<int> v;

    REQUIRE(v.size() == 0);
    REQUIRE(v.is_empty());
}

TEST_CASE("Vec with initial size", "[vec]") {
    Vec<int> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE_FALSE(v.is_empty());

    for (std::size_t i = 0; i < v.size(); i++) {
        REQUIRE(v[i] == 0);  // default initialized
    }
}

TEST_CASE("Vec with_capacity", "[vec]") {
    auto v = Vec<int>::with_capacity(10);

    REQUIRE(v.size() == 0);
    REQUIRE(v.is_empty());

    // capacity is internal, but we can still push up to it without realloc
    v.push(42);
    REQUIRE(v.size() == 1);
    REQUIRE(v[0] == 42);
}

TEST_CASE("Vec copy constructor and assignment", "[vec]") {
    Vec<int> a;
    a.push(1);
    a.push(2);

    SECTION("Copy constructor") {
        Vec<int> b(a);
        REQUIRE(b.size() == 2);
        REQUIRE(b[0] == 1);
        REQUIRE(b[1] == 2);

        // modify original, copy shouldn't change
        a[0] = 99;
        REQUIRE(b[0] == 1);
    }

    SECTION("Copy assignment") {
        Vec<int> b;
        b = a;
        REQUIRE(b.size() == 2);
        REQUIRE(b[0] == 1);
        REQUIRE(b[1] == 2);
    }
}

TEST_CASE("Vec move constructor and assignment", "[vec]") {
    Vec<int> a;
    a.push(10);
    a.push(20);

    SECTION("Move constructor") {
        Vec<int> b(std::move(a));
        REQUIRE(b.size() == 2);
        REQUIRE(b[0] == 10);
        REQUIRE(b[1] == 20);
    }

    SECTION("Move assignment") {
        Vec<int> b;
        b = std::move(a);
        REQUIRE(b.size() == 2);
        REQUIRE(b[0] == 10);
        REQUIRE(b[1] == 20);
    }
}

TEST_CASE("Vec indexing", "[vec]") {
    Vec<int> v;
    v.push(1);
    v.push(2);

    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);

    v[0] = 42;
    REQUIRE(v[0] == 42);

    REQUIRE_THROWS_AS(v[2], std::out_of_range);
    REQUIRE_THROWS_AS(v[999], std::out_of_range);
}

TEST_CASE("Vec first and last", "[vec]") {
    Vec<int> v;

    SECTION("Empty vec throws") {
        REQUIRE_THROWS_AS(v.first(), std::out_of_range);
        REQUIRE_THROWS_AS(v.last(), std::out_of_range);
    }

    SECTION("Non-empty vec") {
        v.push(5);
        v.push(10);

        REQUIRE(v.first() == 5);
        REQUIRE(v.last() == 10);
    }
}

TEST_CASE("Vec push and pop", "[vec]") {
    Vec<int> v;

    SECTION("Push grows vector") {
        v.push(1);
        v.push(2);
        v.push(3);

        REQUIRE(v.size() == 3);
        REQUIRE(v.last() == 3);
    }

    SECTION("Pop returns last element") {
        v.push(42);
        v.push(99);

        int val = v.pop();
        REQUIRE(val == 99);
        REQUIRE(v.size() == 1);
        REQUIRE(v.last() == 42);
    }

    SECTION("Pop empty throws") {
        REQUIRE_THROWS_AS(v.pop(), std::out_of_range);
    }

    SECTION("Push beyond initial capacity resizes") {
        for (int i = 0; i < 10; i++) {
            v.push(i);
        }
        REQUIRE(v.size() == 10);
        REQUIRE(v.last() == 9);
    }
}

TEST_CASE("Vec clear", "[vec]") {
    Vec<int> v;
    v.push(1);
    v.push(2);

    v.clear();
    REQUIRE(v.size() == 0);
    REQUIRE(v.is_empty());

    REQUIRE_THROWS_AS(v.first(), std::out_of_range);
    REQUIRE_THROWS_AS(v.last(), std::out_of_range);
}

TEST_CASE("Vec iterators", "[vec]") {
    Vec<int> v;
    for (int i = 0; i < 5; i++)
        v.push(i);

    SECTION("Forward iteration") {
        int expected = 0;
        for (auto it = v.begin(); it != v.end(); ++it) {
            REQUIRE(*it == expected++);
        }
    }

    SECTION("Const forward iteration") {
        const auto& cv = v;
        int expected = 0;
        for (auto it = cv.begin(); it != cv.end(); ++it) {
            REQUIRE(*it == expected++);
        }
    }

    SECTION("Reverse iteration") {
        int expected = 4;
        for (auto it = v.rbegin(); it != v.rend(); ++it) {
            REQUIRE(*it == expected--);
        }
    }

    SECTION("Const reverse iteration") {
        const auto& cv = v;
        int expected = 4;
        for (auto it = cv.rbegin(); it != cv.rend(); ++it) {
            REQUIRE(*it == expected--);
        }
    }
}

TEST_CASE("Vec with non-trivial types", "[vec]") {
    struct Obj {
        int value;
        Obj(int v = 0)
            : value(v) {}
        bool operator==(const Obj& other) const {
            return value == other.value;
        }
    };

    Vec<Obj> v;
    v.push(Obj(10));
    v.push(Obj(20));

    REQUIRE(v.size() == 2);
    REQUIRE(v[0].value == 10);
    REQUIRE(v[1].value == 20);

    Obj popped = v.pop();
    REQUIRE(popped.value == 20);
    REQUIRE(v.size() == 1);
}

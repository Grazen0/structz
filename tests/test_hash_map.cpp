#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <string>
#include "hash_map.h"

// Simple smoke test
TEST_CASE("HashMap basic properties", "[hashmap]") {
    HashMap<int, std::string> map;

    REQUIRE(map.is_empty());
    REQUIRE(map.size() == 0);
    REQUIRE_FALSE(map.contains(42));
}

TEST_CASE("HashMap insert and retrieve", "[hashmap]") {
    HashMap<int, std::string> map;

    REQUIRE(map.set(1, "one"));
    REQUIRE(map.set(2, "two"));
    REQUIRE(map.set(3, "three"));

    REQUIRE(map.size() == 3);
    REQUIRE_FALSE(map.is_empty());

    REQUIRE(map.contains(1));
    REQUIRE(map.contains(2));
    REQUIRE(map.contains(3));
    REQUIRE_FALSE(map.contains(99));

    REQUIRE(map.get(1) == "one");
    REQUIRE(map.get(2) == "two");
    REQUIRE(map.get(3) == "three");
}

TEST_CASE("HashMap overwrite existing key", "[hashmap]") {
    HashMap<int, std::string> map;

    REQUIRE(map.set(1, "one"));        // inserted
    REQUIRE_FALSE(map.set(1, "ONE"));  // updated, not inserted

    REQUIRE(map.size() == 1);
    REQUIRE(map.contains(1));
    REQUIRE(map.get(1) == "ONE");
}

TEST_CASE("HashMap remove elements", "[hashmap]") {
    HashMap<int, std::string> map;

    map.set(1, "one");
    map.set(2, "two");

    REQUIRE(map.remove(1));
    REQUIRE_FALSE(map.contains(1));
    REQUIRE(map.size() == 1);

    REQUIRE_FALSE(map.remove(42));  // removing non-existent key
    REQUIRE(map.size() == 1);

    REQUIRE(map.remove(2));
    REQUIRE(map.is_empty());
}

TEST_CASE("HashMap get throws on missing key", "[hashmap]") {
    HashMap<int, int> map;

    map.set(1, 10);

    REQUIRE_THROWS_AS(map.get(99), std::out_of_range);
    REQUIRE_NOTHROW(map.get(1));
}

TEST_CASE("HashMap clear", "[hashmap]") {
    HashMap<std::string, int> map;

    map.set("a", 1);
    map.set("b", 2);
    REQUIRE(map.size() == 2);

    map.clear();
    REQUIRE(map.is_empty());
    REQUIRE(map.size() == 0);

    REQUIRE_FALSE(map.contains("a"));
    REQUIRE_THROWS_AS(map.get("b"), std::out_of_range);
}

TEST_CASE("HashMap handles many inserts and rehashing", "[hashmap][rehash]") {
    HashMap<int, int> map(
        2);  // small initial capacity to trigger rehash quickly

    const int N = 200;
    for (int i = 0; i < N; i++) {
        REQUIRE(map.set(i, i * i));
    }

    REQUIRE(map.size() == N);

    for (int i = 0; i < N; i++) {
        REQUIRE(map.contains(i));
        REQUIRE(map.get(i) == i * i);
    }
}

TEST_CASE("HashMap works with non-primitive keys", "[hashmap]") {
    HashMap<std::string, int> map;

    map.set("apple", 5);
    map.set("banana", 7);

    REQUIRE(map.contains("apple"));
    REQUIRE(map.get("apple") == 5);

    REQUIRE(map.contains("banana"));
    REQUIRE(map.get("banana") == 7);

    map.remove("apple");
    REQUIRE_FALSE(map.contains("apple"));
    REQUIRE_THROWS_AS(map.get("apple"), std::out_of_range);
}

TEST_CASE("HashMap supports overwrite with different types", "[hashmap]") {
    HashMap<int, std::vector<int>> map;

    REQUIRE(map.set(1, {1, 2, 3}));
    REQUIRE_FALSE(map.set(1, {4, 5, 6}));  // update

    auto v = map.get(1);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 4);
    REQUIRE(v[2] == 6);
}

// Force collisions with custom key type
struct BadKey {
    int x;
    bool operator==(const BadKey& other) const {
        return x == other.x;
    }
};

namespace std {
    template<>
    struct hash<BadKey> {
        std::size_t operator()(const BadKey& /* bad_hash */) const {
            return 42;
        }
    };
}

TEST_CASE("HashMap edge cases", "[hashmap]") {
    SECTION("Empty map remove/get") {
        HashMap<int, int> map;
        REQUIRE_FALSE(map.remove(10));
        REQUIRE_THROWS_AS(map.get(10), std::out_of_range);
    }

    SECTION("Insert nullptr values when T is pointer") {
        HashMap<int, const char*> map;
        REQUIRE(map.set(1, nullptr));
        REQUIRE(map.get(1) == nullptr);
    }

    SECTION("Large number of collisions") {
        HashMap<BadKey, int> map(2);

        for (int i = 0; i < 10; i++) {
            REQUIRE(map.set({i}, i));
        }

        for (int i = 0; i < 10; i++) {
            REQUIRE(map.contains({i}));
            REQUIRE(map.get({i}) == i);
        }
    }
}

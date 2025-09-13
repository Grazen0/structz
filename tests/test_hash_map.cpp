#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <string>
#include "hash_map.h"

TEST_CASE("HashMap basic operations", "[HashMap]") {
    HashMap<std::string, int> map;

    SECTION("Empty map lookups fail") {
        REQUIRE_THROWS_AS(map.get("missing"), std::out_of_range);
        REQUIRE_FALSE(map.contains("missing"));
        REQUIRE_FALSE(map.remove("missing"));
    }

    SECTION("Insert and retrieve single element") {
        REQUIRE(map.set("one", 1));

        REQUIRE(map.contains("one"));
        REQUIRE(map.get("one") == 1);
    }

    SECTION("Insert multiple elements") {
        REQUIRE(map.set("one", 1));
        REQUIRE(map.set("two", 2));
        REQUIRE(map.set("three", 3));

        REQUIRE(map.contains("one"));
        REQUIRE(map.contains("two"));
        REQUIRE(map.contains("three"));

        REQUIRE(map.get("two") == 2);
    }

    SECTION("Overwriting existing key updates value") {
        REQUIRE(map.set("one", 1));
        REQUIRE(map.get("one") == 1);

        REQUIRE(!map.set("one", 42));
        REQUIRE(map.get("one") == 42);
    }

    SECTION("Remove existing key") {
        REQUIRE(map.set("one", 1));
        REQUIRE(map.contains("one"));

        REQUIRE(map.remove("one"));
        REQUIRE_FALSE(map.contains("one"));
        REQUIRE_THROWS_AS(map.get("one"), std::out_of_range);
    }

    SECTION("Remove non-existing key returns false") {
        REQUIRE_FALSE(map.remove("nonexistent"));
    }
}

TEST_CASE("HashMap collisions and rehashing", "[HashMap]") {
    // Use small capacity to force collisions/rehashing
    HashMap<int, std::string> map(2);

    SECTION("Handles collisions by chaining") {
        REQUIRE(map.set(1, "one"));
        REQUIRE(map.set(3, "three"));  // May collide with 1 depending on hash

        REQUIRE(map.contains(1));
        REQUIRE(map.contains(3));
    }

    SECTION("Rehash expands capacity transparently") {
        for (int i = 0; i < 20; ++i) {
            REQUIRE(map.set(i, "val" + std::to_string(i)));
        }

        for (int i = 0; i < 20; ++i) {
            REQUIRE(map.contains(i));
            REQUIRE(map.get(i) == "val" + std::to_string(i));
        }
    }
}

TEST_CASE("HashMap with non-trivial types", "[HashMap]") {
    struct Obj {
        int id;
        std::string name;
    };

    HashMap<int, Obj> map;

    REQUIRE(map.set(1, {1, "Alice"}));
    REQUIRE(map.set(2, {2, "Bob"}));

    REQUIRE(map.contains(1));
    REQUIRE(map.contains(2));

    auto& alice = map.get(1);
    REQUIRE(alice.id == 1);
    REQUIRE(alice.name == "Alice");

    auto& bob = map.get(2);
    REQUIRE(bob.id == 2);
    REQUIRE(bob.name == "Bob");
}

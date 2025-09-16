#include <catch2/catch_test_macros.hpp>
#include <string>
#include "hash_set.h"

TEST_CASE("HashSet basic properties", "[hashset]") {
    HashSet<int> set;

    REQUIRE(set.is_empty());
    REQUIRE(set.size() == 0);
    REQUIRE_FALSE(set.contains(42));
}

TEST_CASE("HashSet insert and contains", "[hashset]") {
    HashSet<int> set;

    REQUIRE(set.insert(1));
    REQUIRE(set.insert(2));
    REQUIRE(set.insert(3));

    REQUIRE(set.size() == 3);
    REQUIRE(set.contains(1));
    REQUIRE(set.contains(2));
    REQUIRE(set.contains(3));
    REQUIRE_FALSE(set.contains(99));
}

TEST_CASE("HashSet duplicate insertions", "[hashset]") {
    HashSet<int> set;

    REQUIRE(set.insert(5));
    REQUIRE_FALSE(set.insert(5));  // inserting same element again

    REQUIRE(set.size() == 1);
    REQUIRE(set.contains(5));
}

TEST_CASE("HashSet remove elements", "[hashset]") {
    HashSet<int> set;

    set.insert(10);
    set.insert(20);

    REQUIRE(set.remove(10));
    REQUIRE_FALSE(set.contains(10));
    REQUIRE(set.size() == 1);

    REQUIRE_FALSE(set.remove(42));  // removing non-existent key
    REQUIRE(set.size() == 1);

    REQUIRE(set.remove(20));
    REQUIRE(set.is_empty());
}

TEST_CASE("HashSet clear", "[hashset]") {
    HashSet<std::string> set;

    set.insert("apple");
    set.insert("banana");
    REQUIRE(set.size() == 2);

    set.clear();
    REQUIRE(set.is_empty());
    REQUIRE(set.size() == 0);

    REQUIRE_FALSE(set.contains("apple"));
    REQUIRE_FALSE(set.contains("banana"));
}

TEST_CASE("HashSet many insertions (rehash stress)", "[hashset][rehash]") {
    HashSet<int> set(2);  // small initial capacity

    const int N = 200;
    for (int i = 0; i < N; i++) {
        REQUIRE(set.insert(i));
    }

    REQUIRE(set.size() == N);

    for (int i = 0; i < N; i++) {
        REQUIRE(set.contains(i));
    }
}

TEST_CASE("HashSet with non-primitive keys", "[hashset]") {
    HashSet<std::string> set;

    set.insert("hello");
    set.insert("world");

    REQUIRE(set.contains("hello"));
    REQUIRE(set.contains("world"));
    REQUIRE_FALSE(set.contains("test"));

    set.remove("hello");
    REQUIRE_FALSE(set.contains("hello"));
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

TEST_CASE("HashSet edge cases", "[hashset]") {
    SECTION("Removing from empty set") {
        HashSet<int> set;
        REQUIRE_FALSE(set.remove(10));
    }

    SECTION("Inserting nullptr values when T is pointer") {
        HashSet<const char*> set;
        REQUIRE(set.insert(nullptr));
        REQUIRE(set.contains(nullptr));
        REQUIRE_FALSE(set.insert(nullptr));  // duplicate nullptr
    }

    SECTION("Large number of collisions with custom key") {
        HashMap<BadKey, std::monostate> custom_map(2);
        HashSet<BadKey> set;

        // Insert several colliding keys
        for (int i = 0; i < 10; i++) {
            REQUIRE(set.insert({i}));
        }

        for (int i = 0; i < 10; i++) {
            REQUIRE(set.contains({i}));
        }
    }
}

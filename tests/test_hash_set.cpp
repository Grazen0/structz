#include <catch2/catch_test_macros.hpp>
#include <string>
#include "hash_set.h"

TEST_CASE("HashSet basic insert and contains") {
    HashSet<int> set;

    SECTION("Insert and check presence") {
        REQUIRE(set.insert(10));
        REQUIRE(set.contains(10));
        REQUIRE_FALSE(set.contains(20));
    }

    SECTION("Duplicate insert does not add") {
        REQUIRE(set.insert(10));
        REQUIRE_FALSE(set.insert(10));
        REQUIRE(set.contains(10));
    }
}

TEST_CASE("HashSet remove") {
    HashSet<int> set;
    set.insert(1);
    set.insert(2);

    SECTION("Remove existing element") {
        REQUIRE(set.contains(1));
        REQUIRE(set.remove(1));
        REQUIRE_FALSE(set.contains(1));
    }

    SECTION("Remove twice fails") {
        set.remove(1);
        REQUIRE_FALSE(set.remove(1));
    }

    SECTION("Other elements remain") {
        set.remove(1);
        REQUIRE(set.contains(2));
    }
}

TEST_CASE("HashSet empty set behavior") {
    HashSet<std::string> set;

    SECTION("Contains returns false for empty") {
        REQUIRE_FALSE(set.contains("hello"));
    }

    SECTION("Remove returns false for empty") {
        REQUIRE_FALSE(set.remove("hello"));
    }
}

struct BadHash {
    int value;
    bool operator==(const BadHash& other) const {
        return value == other.value;
    }
};

// Custom hash specialization that forces collisions
namespace std {
    template<>
    struct hash<BadHash> {
        size_t operator()(const BadHash& /* bad_hash */) const {
            return 42;
        }
    };
}

TEST_CASE("HashSet collision handling") {
    HashSet<BadHash> set(2);

    SECTION("Insert multiple colliding elements") {
        for (int i = 0; i < 5; i++) {
            REQUIRE(set.insert({i}));
        }
        for (int i = 0; i < 5; i++) {
            REQUIRE(set.contains({i}));
        }
    }

    SECTION("Duplicate colliding insert fails") {
        REQUIRE(set.insert({3}));
        REQUIRE_FALSE(set.insert({3}));
    }
}

TEST_CASE("HashSet rehashing") {
    SECTION("Rehash triggered by fill factor") {
        HashSet<int> set(2);  // small capacity
        for (int i = 0; i < 20; i++) {
            REQUIRE(set.insert(i));
        }
        for (int i = 0; i < 20; i++) {
            REQUIRE(set.contains(i));
        }
    }
}

TEST_CASE("HashSet with std::string keys") {
    HashSet<std::string> set;

    REQUIRE(set.insert("alpha"));
    REQUIRE(set.insert("beta"));
    REQUIRE(set.insert("gamma"));

    REQUIRE(set.contains("alpha"));
    REQUIRE(set.contains("beta"));
    REQUIRE(set.contains("gamma"));

    REQUIRE(set.remove("beta"));
    REQUIRE_FALSE(set.contains("beta"));
}

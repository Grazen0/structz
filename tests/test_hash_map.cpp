#include <catch2/catch_test_macros.hpp>
#include <string>
#include "hash_map.h"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <string>
#include "hash_map.h"

TEST_CASE("hash map basic set/get", "[hashmap]") {
    HashMap<std::string, int> map;

    map.set("one", 1);
    map.set("two", 2);

    REQUIRE(map.get("one") == 1);
    REQUIRE(map.get("two") == 2);

    // overwrite existing key
    map.set("one", 42);
    REQUIRE(map.get("one") == 42);
}

TEST_CASE("hash map get throws on missing key", "[hashmap]") {
    HashMap<int, int> map;
    map.set(1, 100);

    REQUIRE_THROWS_AS(map.get(2), std::out_of_range);
    REQUIRE_THROWS_AS(map.get(999), std::out_of_range);
}

TEST_CASE("hash map contains works", "[hashmap]") {
    HashMap<int, std::string> map;
    map.set(10, "ten");

    REQUIRE(map.contains(10));
    REQUIRE_FALSE(map.contains(11));
}

TEST_CASE("hash map remove works", "[hashmap]") {
    HashMap<int, int> map;
    map.set(1, 111);
    map.set(2, 222);

    REQUIRE(map.contains(1));
    REQUIRE(map.remove(1));
    REQUIRE_FALSE(map.contains(1));

    // removing again should fail
    REQUIRE_FALSE(map.remove(1));

    // removing non-existent key should fail
    REQUIRE_FALSE(map.remove(999));
}

TEST_CASE("hash map bucket queries", "[hashmap]") {
    HashMap<int, int> map;

    REQUIRE(map.bucket_count() == 8);

    // all empty at first
    for (size_t i = 0; i < map.bucket_count(); ++i) {
        REQUIRE(map.bucket_size(i) == 0);
    }

    map.set(1, 100);
    size_t index = std::hash<int>()(1) % map.bucket_count();
    REQUIRE(map.bucket_size(index) == 1);
}

TEST_CASE("hash map iterators per bucket", "[hashmap]") {
    HashMap<int, int> map;

    // Put colliding keys in the same bucket
    size_t index;
    int k1 = 1, k2 = 1 + 8;  // same mod 8
    map.set(k1, 111);
    map.set(k2, 222);

    index = std::hash<int>()(k1) % map.bucket_count();
    REQUIRE(map.bucket_size(index) == 2);

    std::vector<int> values;
    for (auto it = map.begin(index); it != map.end(index); ++it) {
        values.push_back((*it).value);
    }

    REQUIRE(values.size() == 2);
    REQUIRE((values[0] == 111 || values[0] == 222));
    REQUIRE((values[1] == 111 || values[1] == 222));
}

TEST_CASE("hash map iterator bucket bounds check", "[hashmap]") {
    HashMap<int, int> map;
    REQUIRE_THROWS_AS(map.begin(999), std::out_of_range);
    REQUIRE_THROWS_AS(map.end(999), std::out_of_range);
}

#include <catch2/catch_test_macros.hpp>
#include <string>
#include "hash_map.h"

TEST_CASE("rehash triggers on too many collisions", "[hashmap][rehash]") {
    HashMap<int, int> map;

    // Force collisions by using keys that differ by multiples of capacity
    size_t start_capacity = map.bucket_count();
    int base_key = 0;
    for (size_t i = 0; i <= MAX_COLLISIONS; ++i) {
        map.set(base_key + i * start_capacity, static_cast<int>(i));
    }

    // After this, rehash should have been triggered
    REQUIRE(map.bucket_count() > start_capacity);

    // All inserted values must still be present
    for (size_t i = 0; i <= MAX_COLLISIONS; ++i) {
        REQUIRE(map.get(base_key + i * start_capacity) == static_cast<int>(i));
    }
}

TEST_CASE("rehash triggers on load factor", "[hashmap][rehash]") {
    HashMap<int, int> map;

    size_t start_capacity = map.bucket_count();
    size_t target_size =
        static_cast<size_t>(MAX_FILL_FACTOR * start_capacity) + 1;

    for (size_t i = 0; i < target_size; ++i) {
        map.set(static_cast<int>(i), static_cast<int>(i * 10));
    }

    REQUIRE(map.bucket_count() > start_capacity);

    // Check that all inserted keys are still retrievable
    for (size_t i = 0; i < target_size; ++i) {
        REQUIRE(map.get(static_cast<int>(i)) == static_cast<int>(i * 10));
    }
}

TEST_CASE("rehash redistributes elements across buckets", "[hashmap][rehash]") {
    HashMap<int, int> map;

    size_t start_capacity = map.bucket_count();
    int k1 = 1, k2 = 1 + static_cast<int>(start_capacity);

    // Put two colliding keys into the same bucket
    map.set(k1, 100);
    map.set(k2, 200);

    size_t bucket_index_before = std::hash<int>()(k1) % start_capacity;
    REQUIRE(map.bucket_size(bucket_index_before) == 2);

    // Trigger rehash by forcing too many collisions
    for (size_t i = 2; i <= MAX_COLLISIONS; ++i) {
        map.set(k1 + static_cast<int>(i * start_capacity),
                static_cast<int>(i * 100));
    }

    size_t new_capacity = map.bucket_count();
    REQUIRE(new_capacity > start_capacity);

    // Now at least one of the original colliding keys should be in a different
    // bucket
    size_t bucket_index_after_k1 = std::hash<int>()(k1) % new_capacity;
    size_t bucket_index_after_k2 = std::hash<int>()(k2) % new_capacity;
    REQUIRE((bucket_index_after_k1 != bucket_index_after_k2));

    // Values are still accessible
    REQUIRE(map.get(k1) == 100);
    REQUIRE(map.get(k2) == 200);
}

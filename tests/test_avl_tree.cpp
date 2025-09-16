#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "avl_tree.h"

static std::size_t avl_height_upper_bound(std::size_t n) {
    if (n == 0)
        return static_cast<std::size_t>(
            -1);  // matches AvlTree::height() returning -1 for nullptr
    double h = std::floor(1.44 * std::log2(static_cast<double>(n) + 2.0)) - 1.0;
    if (h < 0)
        return 0;
    return static_cast<std::size_t>(h);
}

template<typename K, typename V>
static void verify_present(const AvlTree<K, V>& tree,
                           const std::vector<std::pair<K, V>>& pairs) {
    for (const auto& kv : pairs) {
        REQUIRE(tree.contains(kv.first));
        REQUIRE(tree.get(kv.first) == kv.second);
    }
}

template<typename K, typename V>
static void verify_absent(const AvlTree<K, V>& tree,
                          const std::vector<K>& keys) {
    for (const auto& k : keys) {
        REQUIRE_FALSE(tree.contains(k));
        REQUIRE_THROWS_AS(tree.get(k), std::out_of_range);
    }
}

TEST_CASE("Empty tree properties", "[avl][empty]") {
    AvlTree<int, int> t;
    REQUIRE(t.is_empty());
    REQUIRE(t.size() == 0);
    REQUIRE_FALSE(t.contains(42));
    REQUIRE_THROWS_AS(t.get(42), std::out_of_range);
    REQUIRE_FALSE(t.remove(42));
}

TEST_CASE("Basic insert and get", "[avl][insert][get]") {
    AvlTree<int, int> t;
    REQUIRE(t.insert(10, 100));
    REQUIRE(t.size() == 1);
    REQUIRE(t.contains(10));
    REQUIRE(t.get(10) == 100);

    REQUIRE(t.insert(5, 50));
    REQUIRE(t.insert(20, 200));
    REQUIRE(t.size() == 3);

    std::vector<std::pair<int, int>> expected = {
        {10, 100},
        { 5,  50},
        {20, 200}
    };
    verify_present(t, expected);

    REQUIRE_FALSE(t.insert(10, 1234));  // should update existing value
    REQUIRE(t.size() == 3);
    REQUIRE(t.get(10) == 1234);
}

TEST_CASE("Insert where key != value to detect ordering-by-value bug",
          "[avl][bug-detection][ordering]") {
    AvlTree<int, int> t;

    REQUIRE(t.insert(1, 100));
    REQUIRE(t.insert(2, 50));  // value 50 < 100 but key 2 > 1
    REQUIRE(t.insert(3, 75));  // more mixing
    REQUIRE(t.size() == 3);

    std::vector<std::pair<int, int>> expected = {
        {1, 100},
        {2,  50},
        {3,  75}
    };
    verify_present(t, expected);

    REQUIRE(t.remove(2));
    REQUIRE(t.size() == 2);
    REQUIRE_FALSE(t.contains(2));
    REQUIRE_THROWS_AS(t.get(2), std::out_of_range);
    verify_present(t, {
                          {1, 100},
                          {3,  75}
    });
}

TEST_CASE("Removal: leaf, one-child, two-children", "[avl][remove]") {
    AvlTree<int, std::string> t;
    std::vector<std::pair<int, std::string>> pairs = {
        {50, "a"},
        {30, "b"},
        {70, "c"},
        {20, "d"},
        {40, "e"},
        {60, "f"},
        {80, "g"},
        {35, "h"}
    };

    for (auto& p : pairs)
        REQUIRE(t.insert(p.first, p.second));
    REQUIRE(t.size() == pairs.size());

    verify_present(t, pairs);

    REQUIRE(t.remove(35));
    REQUIRE_FALSE(t.contains(35));
    REQUIRE(t.size() == pairs.size() - 1);

    REQUIRE(t.remove(20));
    REQUIRE_FALSE(t.contains(20));

    REQUIRE(t.remove(30));
    REQUIRE_FALSE(t.contains(30));

    REQUIRE_FALSE(t.remove(9999));

    std::vector<std::pair<int, std::string>> expected_remaining;
    for (auto& p : pairs) {
        if (p.first == 35 || p.first == 20 || p.first == 30)
            continue;
        expected_remaining.push_back(p);
    }
    verify_present(t, expected_remaining);
}

TEST_CASE("Large insertions keep tree balanced (height upper bound)",
          "[avl][balance][height]") {
    AvlTree<int, int> t;
    const int N = 1000;
    for (int i = 1; i <= N; ++i) {
        REQUIRE(t.insert(i, i * 10));
    }
    REQUIRE(t.size() == static_cast<std::size_t>(N));

    for (int i = 1; i <= N; ++i)
        REQUIRE(t.contains(i));

    std::size_t h = t.height();
    std::size_t ub = avl_height_upper_bound(t.size());
    REQUIRE(h <=
            ub + 2);  // allow a small slack of +2 for implementation variance
}

TEST_CASE("Clear, copy and move semantics", "[avl][copy][move][clear]") {
    AvlTree<int, int> t;
    std::vector<std::pair<int, int>> pairs;
    for (int i = 0; i < 50; ++i) {
        pairs.emplace_back(i, i * 1000);
        REQUIRE(t.insert(pairs.back().first, pairs.back().second));
    }

    AvlTree<int, int> tcopy(t);  // If copy is buggy (swapped key/value or wrong
    REQUIRE(tcopy.size() == t.size());
    verify_present(tcopy, pairs);

    REQUIRE(t.remove(0));
    REQUIRE_FALSE(t.contains(0));
    REQUIRE(tcopy.contains(0));  // copy should still contain 0

    AvlTree<int, int> tmoved(std::move(tcopy));
    REQUIRE(tmoved.size() == 50);
    REQUIRE(!tcopy.contains(0));
    REQUIRE(tcopy.size() == 0);

    AvlTree<int, int> t2;
    t2 = std::move(tmoved);
    REQUIRE(t2.size() == 50);

    t2.clear();
    REQUIRE(t2.is_empty());
    REQUIRE(t2.size() == 0);
}

TEST_CASE("Const and non-const get overloads and exception semantics",
          "[avl][get][exceptions]") {
    AvlTree<int, std::string> t;
    REQUIRE(t.insert(1, "one"));
    REQUIRE(t.insert(2, "two"));

    std::string& val = t.get(2);
    REQUIRE(val == "two");
    val = "TWO";  // modify value via non-const get
    REQUIRE(t.get(2) == "TWO");

    const AvlTree<int, std::string>& tc = t;
    REQUIRE(tc.get(1) == "one");

    REQUIRE_THROWS_AS(tc.get(999), std::out_of_range);
    REQUIRE_THROWS_AS(t.get(999), std::out_of_range);
}

TEST_CASE("Randomized stress: insert/remove and membership consistency",
          "[avl][stress][random]") {
    AvlTree<int, int> t;
    const int N = 200;
    std::vector<int> keys;
    keys.reserve(N);
    for (int i = 0; i < N; ++i)
        keys.push_back(i);

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(keys.begin(), keys.end(), g);
    for (int k : keys) {
        REQUIRE(t.insert(k, k + 1000));
        REQUIRE(t.contains(k));
        REQUIRE(t.get(k) == k + 1000);
    }
    REQUIRE(t.size() == static_cast<std::size_t>(N));

    std::shuffle(keys.begin(), keys.end(), g);
    for (int i = 0; i < N / 2; ++i) {
        int k = keys[i];
        REQUIRE(t.remove(k));
        REQUIRE_FALSE(t.contains(k));
    }

    for (int i = N / 2; i < N; ++i) {
        int k = keys[i];
        REQUIRE(t.contains(k));
        REQUIRE(t.get(k) == k + 1000);
    }
}

TEST_CASE("Edge cases: extreme small trees and repeated remove/insert",
          "[avl][edge]") {
    AvlTree<int, int> t;
    for (int i = 0; i < 100; ++i) {
        REQUIRE(t.insert(42, i));
        REQUIRE(t.get(42) == i);
        REQUIRE(t.remove(42));
        REQUIRE_FALSE(t.contains(42));
    }

    REQUIRE_FALSE(t.remove(42));
}

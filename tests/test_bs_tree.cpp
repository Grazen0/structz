#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <random>
#include <string>
#include <vector>
#include "bs_tree.h"

// helper: try-get which returns optional-like behavior via pair<bool, T>
template<typename K, typename T>
static std::pair<bool, T> try_get(BSTree<K, T> const& tree, const K& key) {
    try {
        return {true, tree.get(key)};
    } catch (...) {
        return {false, T{}};
    }
}

TEST_CASE("Empty tree properties") {
    BSTree<int, std::string> t;
    REQUIRE(t.is_empty());
    REQUIRE(t.size() == 0);
    REQUIRE(t.height() == -1);

    REQUIRE_FALSE(t.contains(42));
    REQUIRE_THROWS_AS(t.get(42), std::out_of_range);
}

TEST_CASE("Single insert and lookup") {
    BSTree<int, std::string> t;
    REQUIRE(t.insert(5, "five"));
    REQUIRE_FALSE(t.is_empty());
    REQUIRE(t.size() == 1);
    REQUIRE(t.contains(5));
    REQUIRE(t.get(5) == "five");

    SECTION("Replace value for existing key") {
        REQUIRE_FALSE(t.insert(5, "cinco"));  // replace should return false
        REQUIRE(t.size() == 1);
        REQUIRE(t.get(5) == "cinco");
    }
}

TEST_CASE("Multiple inserts and basic properties") {
    BSTree<int, int> t;
    std::vector<int> keys = {10, 5, 15, 3, 7, 12, 17};
    for (int k : keys)
        REQUIRE(t.insert(k, k * 10));
    REQUIRE(t.size() == keys.size());

    for (int k : keys) {
        REQUIRE(t.contains(k));
        REQUIRE(t.get(k) == k * 10);
    }

    REQUIRE_FALSE(t.contains(999));
    REQUIRE_THROWS_AS(t.get(999), std::out_of_range);
}

TEST_CASE("Height behaviour") {
    BSTree<int, int> t;
    REQUIRE(t.height() == -1);
    t.insert(10, 0);
    REQUIRE(t.height() >= 0);

    // build a skewed tree (increasing keys creates right-skewed)
    BSTree<int, int> skew;
    for (int i = 0; i < 10; ++i)
        skew.insert(i, i);
    REQUIRE(skew.height() >= 9);  // skewed height (edges) should be large
}

TEST_CASE("Remove leaf node") {
    BSTree<int, int> t;
    t.insert(10, 100);
    t.insert(5, 50);
    t.insert(15, 150);

    REQUIRE(t.remove(5) == true);
    REQUIRE_FALSE(t.contains(5));
    REQUIRE(t.size() == 2);

    // removing again returns false
    REQUIRE(t.remove(5) == false);
}

TEST_CASE("Remove node with single child") {
    BSTree<int, int> t;
    t.insert(10, 100);
    t.insert(5, 50);
    t.insert(2, 20);  // left child of 5

    REQUIRE(t.remove(5) == true);
    REQUIRE_FALSE(t.contains(5));
    REQUIRE(t.contains(2));
    REQUIRE(t.size() == 2);

    // Right-child single-child case
    BSTree<int, int> t2;
    t2.insert(10, 10);
    t2.insert(15, 150);
    t2.insert(17, 170);
    REQUIRE(t2.remove(15) == true);
    REQUIRE_FALSE(t2.contains(15));
    REQUIRE(t2.contains(17));
    REQUIRE(t2.size() == 2);
}

TEST_CASE("Remove node with two children (use successor)") {
    // Build tree:
    //        8
    //       / \
    //      3  10
    //     / \   \
    //    1  6   14
    //       / \  /
    //      4  7 13
    BSTree<int, int> t;
    std::vector<int> keys = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (int k : keys)
        t.insert(k, k);
    REQUIRE(t.size() == keys.size());

    // remove node with two children (3)
    REQUIRE(t.remove(3) == true);
    REQUIRE_FALSE(t.contains(3));
    REQUIRE(t.size() == keys.size() - 1);

    // ensure remaining keys still accessible
    for (int k : keys)
        if (k != 3) {
            REQUIRE(t.contains(k));
            REQUIRE(t.get(k) == k);
        }

    // remove root with two children
    REQUIRE(t.remove(8) == true);
    REQUIRE_FALSE(t.contains(8));
}

TEST_CASE("Remove non-existent key") {
    BSTree<int, int> t;
    t.insert(1, 1);
    REQUIRE(t.remove(42) == false);
    REQUIRE(t.size() == 1);
}

TEST_CASE("Clear empties everything") {
    BSTree<int, int> t;
    for (int i = 0; i < 20; ++i)
        t.insert(i, i * 2);
    REQUIRE(t.size() == 20);
    t.clear();
    REQUIRE(t.size() == 0);
    REQUIRE(t.is_empty());
    REQUIRE(t.height() == -1);
    for (int i = 0; i < 20; ++i)
        REQUIRE_FALSE(t.contains(i));
}

TEST_CASE("Copy constructor produces deep copy") {
    BSTree<int, int> a;
    for (int i : {5, 2, 8, 1, 3})
        a.insert(i, i * 10);
    BSTree<int, int> b(a);  // copy
    REQUIRE(b.size() == a.size());
    for (int k : {5, 2, 8, 1, 3}) {
        REQUIRE(b.contains(k));
        REQUIRE(b.get(k) == k * 10);
    }

    // mutate original - copy should remain unchanged
    a.insert(10, 100);
    REQUIRE(a.contains(10));
    REQUIRE_FALSE(b.contains(10));
}

TEST_CASE("Move constructor and move assignment transfer ownership") {
    BSTree<int, int> a;
    for (int i : {1, 2, 3})
        a.insert(i, i);
    BSTree<int, int> moved(std::move(a));
    REQUIRE(moved.size() == 3);
    // original may be empty (moved-from)
    REQUIRE(a.size() == 0);

    BSTree<int, int> x;
    x.insert(42, 4242);
    x = std::move(moved);
    REQUIRE(x.size() == 3);
}

TEST_CASE("Assignment operator (copy & move)") {
    BSTree<int, int> a;
    a.insert(1, 10);
    a.insert(2, 20);

    BSTree<int, int> b;
    b.insert(77, 7700);

    SECTION("copy assign") {
        b = a;
        REQUIRE(b.size() == 2);
        REQUIRE(b.contains(1));
        REQUIRE(b.contains(2));
        REQUIRE_FALSE(b.contains(77));
    }

    SECTION("move assign") {
        b = std::move(a);
        REQUIRE(b.size() == 2);
    }
}

TEST_CASE("Bulk insert/remove randomized stress test") {
    BSTree<int, int> t;
    constexpr int N = 200;
    std::vector<int> v(N);
    for (int i = 0; i < N; i++)
        v[i] = i;
    std::shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));

    for (int x : v)
        REQUIRE(t.insert(x, x));
    REQUIRE(t.size() == (size_t)N);

    // remove half of them (every even number)
    for (int i = 0; i < N; i += 2) {
        bool removed = t.remove(i);
        REQUIRE(removed == true);
    }

    // remaining contain odd numbers
    for (int i = 1; i < N; i += 2)
        REQUIRE(t.contains(i));

    // removed evens do not exist
    for (int i = 0; i < N; i += 2)
        REQUIRE_FALSE(t.contains(i));
}

// Helper to collect all elements via iterator into a vector
template<typename Iter>
std::vector<typename Iter::value_type> collect(Iter first, Iter last) {
    std::vector<typename Iter::value_type> result;
    for (; first != last; ++first) {
        result.push_back(*first);
    }
    return result;
}

TEST_CASE("Iterators on empty tree") {
    BSTree<int, std::string> tree;

    auto it = tree.begin();
    auto end = tree.end();

    SECTION("begin == end for empty tree") {
        REQUIRE(it == end);
        REQUIRE(it == tree.end());
    }

    SECTION("const_iterator works on empty tree") {
        const BSTree<int, std::string>& ctree = tree;
        REQUIRE(ctree.begin() == ctree.end());
    }
}

TEST_CASE("Single element tree iteration") {
    BSTree<int, std::string> tree;
    tree.insert(10, "ten");

    auto it = tree.begin();
    auto end = tree.end();

    SECTION("begin != end") {
        REQUIRE(it != end);
    }

    SECTION("Dereferencing works") {
        auto [k, v] = *it;
        REQUIRE(k == 10);
        REQUIRE(v == "ten");
    }

    SECTION("Incrementing reaches end") {
        ++it;
        REQUIRE(it == end);
    }
}

TEST_CASE("Multiple elements in-order traversal") {
    BSTree<int, std::string> tree;
    tree.insert(5, "five");
    tree.insert(3, "three");
    tree.insert(7, "seven");
    tree.insert(6, "six");
    tree.insert(8, "eight");

    auto values = collect(tree.begin(), tree.end());

    SECTION("Traversal yields sorted order by key") {
        std::vector<int> keys;
        for (auto& [k, v] : values)
            keys.push_back(k);

        REQUIRE(keys == std::vector<int>{3, 5, 6, 7, 8});
    }

    SECTION("Values correspond to keys") {
        std::vector<std::string> vals;
        for (auto& [k, v] : values)
            vals.push_back(v);

        REQUIRE(vals == std::vector<std::string>{"three", "five", "six",
                                                 "seven", "eight"});
    }
}

TEST_CASE("Iterator postfix increment works") {
    BSTree<int, int> tree;
    for (int i : {2, 1, 3})
        tree.insert(i, i * 10);

    auto it = tree.begin();
    auto it2 = it++;  // postfix

    SECTION("Postfix returns old value") {
        REQUIRE((*it2).first == 1);
    }

    SECTION("Iterator advances") {
        REQUIRE((*it).first == 2);
    }
}

TEST_CASE("Const iterator traversal works") {
    BSTree<int, int> tree;
    for (int i : {4, 2, 6, 1, 3, 5, 7})
        tree.insert(i, i);

    const BSTree<int, int>& ctree = tree;
    auto values = collect(ctree.begin(), ctree.end());

    SECTION("Const iteration yields sorted order") {
        std::vector<int> keys;
        for (auto& [k, v] : values)
            keys.push_back(k);
        REQUIRE(keys == std::vector<int>{1, 2, 3, 4, 5, 6, 7});
    }
}

TEST_CASE("Iterator equality/inequality checks") {
    BSTree<int, int> tree;
    tree.insert(1, 10);
    tree.insert(2, 20);

    auto it1 = tree.begin();
    auto it2 = tree.begin();
    auto end = tree.end();

    SECTION("Two begin iterators compare equal") {
        REQUIRE(it1 == it2);
    }

    SECTION("Iterator and end compare not equal") {
        REQUIRE(it1 != end);
    }

    SECTION("After increment, not equal to original begin") {
        ++it1;
        REQUIRE(it1 != it2);
    }

    SECTION("Increment until end equals end") {
        ++it1;  // go to 2
        ++it1;  // past end
        REQUIRE(it1 == end);
    }
}

TEST_CASE("Iterator on skewed tree (degenerate case)") {
    BSTree<int, int> tree;
    // Insert sorted keys to make a degenerate right-skewed tree
    for (int i = 1; i <= 5; ++i)
        tree.insert(i, i * 10);

    auto values = collect(tree.begin(), tree.end());

    SECTION("Traversal still works in sorted order") {
        std::vector<int> keys;
        for (auto& [k, v] : values)
            keys.push_back(k);
        REQUIRE(keys == std::vector<int>{1, 2, 3, 4, 5});
    }
}

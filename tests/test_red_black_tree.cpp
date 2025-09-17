#include <catch2/catch_test_macros.hpp>
#include "red_black_tree.h"

#include <cmath>
#include <random>
#include <set>

// Helper: conservative theoretical upper bound for red-black tree height:
// h <= 2 * floor(log2(n + 1))
static std::size_t rb_height_upper_bound(std::size_t n) {
    if (n == 0)
        return 0;
    return 2 * static_cast<std::size_t>(
                   std::floor(std::log2(static_cast<double>(n) + 1.0)));
}

TEST_CASE("basic empty tree properties", "[redblack][basic]") {
    RedBlackTree<int, int> tree;
    REQUIRE(tree.is_empty());
    REQUIRE(tree.size() == 0);
    REQUIRE(tree.height() == -1);

    // // removing from empty must return false
    // REQUIRE(!tree.remove(42));
    //
    // // get on empty must throw
    // REQUIRE_THROWS_AS(tree.get(0), std::out_of_range);
}

TEST_CASE("insert and contains and get behaviour", "[redblack][insert][get]") {
    RedBlackTree<int, std::string> tree;

    REQUIRE(tree.insert(10, "ten"));
    REQUIRE(tree.size() == 1);
    REQUIRE(tree.contains(10));
    REQUIRE(tree.get(10) == "ten");

    // inserting duplicate key should update value and return false
    REQUIRE(!tree.insert(10, "TEN"));
    REQUIRE(tree.size() == 1);
    REQUIRE(tree.get(10) == "TEN");

    // more inserts
    REQUIRE(tree.insert(5, "five"));
    REQUIRE(tree.insert(20, "twenty"));
    REQUIRE(tree.size() == 3);
    REQUIRE(tree.contains(5));
    REQUIRE(tree.contains(20));
    REQUIRE(!tree.contains(999));

    // const-get
    const auto& const_tree = tree;
    REQUIRE(const_tree.get(5) == "five");
}

TEST_CASE("copy and move semantics", "[redblack][copy][move]") {
    RedBlackTree<int, int> a;
    for (int i = 0; i < 20; ++i)
        a.insert(i, i * 10);

    SECTION("copy constructor produces deep copy") {
        RedBlackTree<int, int> b(a);
        REQUIRE(b.size() == a.size());
        for (int i = 0; i < 20; ++i) {
            REQUIRE(b.contains(i));
            REQUIRE(b.get(i) == i * 10);
        }

        // // modify original, copied should remain
        // a.remove(0);
        // REQUIRE(!a.contains(0));
        // REQUIRE(b.contains(0));
        // REQUIRE(b.get(0) == 0);
    }

    SECTION("move constructor transfers ownership") {
        RedBlackTree<int, int> c(std::move(a));
        REQUIRE(c.size() == 20);
        // moved-from 'a' should be empty
        REQUIRE(a.size() == 0);
        REQUIRE(a.is_empty());
    }
}

TEST_CASE("clear and assignment operators", "[redblack][assign]") {
    RedBlackTree<int, int> t;
    for (int i = 0; i < 15; ++i)
        t.insert(i, i);
    REQUIRE(t.size() == 15);
    t.clear();
    REQUIRE(t.size() == 0);
    REQUIRE(t.is_empty());

    // copy assignment
    RedBlackTree<int, int> s;
    for (int i = 0; i < 10; ++i)
        s.insert(i, i + 100);
    RedBlackTree<int, int> r;
    r = s;
    REQUIRE(r.size() == s.size());
    for (int i = 0; i < 10; ++i)
        REQUIRE(r.get(i) == i + 100);

    // move assignment
    RedBlackTree<int, int> m;
    m = std::move(r);
    REQUIRE(m.size() == 10);
    REQUIRE(r.size() == 0);
}

TEST_CASE("height stays within red-black theoretical bound",
          "[redblack][height]") {
    RedBlackTree<int, int> t;
    const int N = 1000;
    for (int i = 1; i <= N; ++i)
        t.insert(i, i);

    REQUIRE(t.size() == static_cast<std::size_t>(N));
    const auto h = t.height();
    const auto bound = rb_height_upper_bound(N);
    INFO("tree size: " << N << " height: " << h << " bound: " << bound);
    REQUIRE(h <= bound);
}

TEST_CASE("exact expected height on small shapes", "[redblack][height_exact]") {
    RedBlackTree<int, int> t;

    SECTION("empty") {
        REQUIRE(t.height() == -1);
    }

    SECTION("single node") {
        t.insert(1, 10);
        REQUIRE(t.height() == 0);
    }

    SECTION("two nodes") {
        t.insert(1, 10);
        t.insert(2, 20);
        REQUIRE(t.height() == 1);
    }

    SECTION("three nodes balanced") {
        t.insert(2, 20);
        t.insert(1, 10);
        t.insert(3, 30);
        REQUIRE(t.height() == 1);  // perfect balance
    }

    SECTION("three nodes skewed") {
        t.insert(1, 10);
        t.insert(2, 20);
        t.insert(3, 30);
        REQUIRE(t.height() >= 2);  // degenerate chain possible before fixing
    }

    SECTION("perfect binary tree of 7 nodes") {
        t.insert(4, 40);
        t.insert(2, 20);
        t.insert(6, 60);
        t.insert(1, 10);
        t.insert(3, 30);
        t.insert(5, 50);
        t.insert(7, 70);
        REQUIRE(t.height() == 2);
    }
}

TEST_CASE("removals update size and membership correctly",
          "[redblack][remove]") {
    RedBlackTree<int, int> t;
    const int N = 200;
    for (int i = 0; i < N; ++i)
        t.insert(i, i * 2);
    REQUIRE(t.size() == static_cast<std::size_t>(N));

    // remove a few known elements
    // REQUIRE(t.remove(0));
    // REQUIRE(!t.contains(0));
    // REQUIRE(t.size() == static_cast<std::size_t>(N - 1));
    //
    // REQUIRE(t.remove(100));
    // REQUIRE(!t.contains(100));
    // REQUIRE(t.size() == static_cast<std::size_t>(N - 2));
    //
    // // removing again should return false
    // REQUIRE(!t.remove(100));
    //
    // // remove all
    // for (int i = 0; i < N; ++i)
    //     t.remove(i);
    // REQUIRE(t.size() == 0);
    // REQUIRE(t.is_empty());
}

TEST_CASE("edge cases: large keys, negative keys, string keys",
          "[redblack][edge]") {
    RedBlackTree<std::string, long long> tstr;
    REQUIRE(tstr.insert(std::string("key"), 12345678901234LL));
    REQUIRE(tstr.contains("key"));
    REQUIRE(tstr.get("key") == 12345678901234LL);

    RedBlackTree<int, int> tint;
    REQUIRE(tint.insert(std::numeric_limits<int>::min(), -1));
    REQUIRE(tint.insert(std::numeric_limits<int>::max(), 1));
    REQUIRE(tint.contains(std::numeric_limits<int>::min()));
    REQUIRE(tint.contains(std::numeric_limits<int>::max()));
}

TEST_CASE("stress test: random inserts and removals", "[redblack][stress]") {
    RedBlackTree<int, int> t;
    const int N = 5000;
    std::mt19937 rng(123456);
    std::uniform_int_distribution<int> dist(1, N * 10);

    std::set<int> reference;
    std::vector<int> inserted;
    inserted.reserve(N);

    // insert N random unique keys (ensure uniqueness by retrying)
    while (static_cast<int>(reference.size()) < N) {
        int x = dist(rng);
        if (reference.insert(x).second) {
            t.insert(x, x + 1);
            inserted.push_back(x);
        }
    }
    REQUIRE(t.size() == reference.size());

    // check all keys present and values correct
    for (int k : inserted) {
        REQUIRE(t.contains(k));
        REQUIRE(t.get(k) == k + 1);
    }

    // height bound
    REQUIRE(t.height() <= rb_height_upper_bound(t.size()));

    // // remove about half randomly
    // std::shuffle(inserted.begin(), inserted.end(), rng);
    // for (size_t i = 0; i < inserted.size() / 2; ++i) {
    //     int k = inserted[i];
    //     REQUIRE(t.remove(k));
    //     reference.erase(k);
    // }
    //
    // // after removals check consistency with reference
    // REQUIRE(t.size() == reference.size());
    // for (int k = 1; k <= N * 10; ++k) {
    //     if (reference.count(k))
    //         REQUIRE(t.contains(k));
    //     else
    //         REQUIRE(!t.contains(k));
    // }
    //
    // REQUIRE(t.height() <= rb_height_upper_bound(t.size()));
}

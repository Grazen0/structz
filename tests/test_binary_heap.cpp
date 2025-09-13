#include <catch2/catch_test_macros.hpp>
#include "binary_heap.h"

TEST_CASE("BinaryHeap basic properties", "[binary_heap]") {
    BinaryHeap<int> heap;
    REQUIRE(heap.size() == 0);
    REQUIRE(heap.is_empty());
}

TEST_CASE("BinaryHeap push maintains heap property", "[binary_heap]") {
    BinaryHeap<int> heap;

    heap.push(5);
    REQUIRE(heap.size() == 1);
    REQUIRE_FALSE(heap.is_empty());
    REQUIRE(heap.peek() == 5);

    heap.push(3);
    heap.push(7);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.peek() == 3);  // min-heap: smallest element at root

    heap.push(1);
    REQUIRE(heap.peek() == 1);  // still smallest
}

TEST_CASE("BinaryHeap pop returns and removes min element", "[binary_heap]") {
    BinaryHeap<int> heap;

    heap.push(5);
    heap.push(3);
    heap.push(7);
    heap.push(1);

    REQUIRE(heap.size() == 4);
    REQUIRE(heap.peek() == 1);

    REQUIRE(heap.pop() == 1);
    REQUIRE(heap.size() == 3);
    REQUIRE(heap.peek() == 3);

    REQUIRE(heap.pop() == 3);
    REQUIRE(heap.size() == 2);
    REQUIRE(heap.peek() == 5);

    REQUIRE(heap.pop() == 5);
    REQUIRE(heap.pop() == 7);

    REQUIRE(heap.is_empty());
    REQUIRE_THROWS_AS(heap.pop(), std::out_of_range);
}

TEST_CASE("BinaryHeap peek throws on empty heap", "[binary_heap]") {
    BinaryHeap<int> heap;
    REQUIRE_THROWS_AS(heap.peek(), std::out_of_range);

    const BinaryHeap<int> constHeap;
    REQUIRE_THROWS_AS(constHeap.peek(), std::out_of_range);
}

TEST_CASE("BinaryHeap clear empties the heap", "[binary_heap]") {
    BinaryHeap<int> heap;
    heap.push(10);
    heap.push(20);

    REQUIRE(heap.size() == 2);
    heap.clear();
    REQUIRE(heap.is_empty());
    REQUIRE_THROWS_AS(heap.peek(), std::out_of_range);
}

TEST_CASE("BinaryHeap ordering with duplicates", "[binary_heap]") {
    BinaryHeap<int> heap;
    heap.push(4);
    heap.push(4);
    heap.push(2);
    heap.push(2);

    REQUIRE(heap.size() == 4);
    REQUIRE(heap.peek() == 2);

    REQUIRE(heap.pop() == 2);
    REQUIRE(heap.pop() == 2);
    REQUIRE(heap.pop() == 4);
    REQUIRE(heap.pop() == 4);
    REQUIRE(heap.is_empty());
}

TEST_CASE("BinaryHeap works with many elements", "[binary_heap]") {
    BinaryHeap<int> heap;
    const int N = 20;

    for (int i = N; i > 0; --i) {
        heap.push(i);
    }
    REQUIRE(heap.size() == N);
    REQUIRE(heap.peek() == 1);

    for (int i = 1; i <= N; ++i) {
        REQUIRE(heap.pop() == i);  // should always return next smallest
    }
    REQUIRE(heap.is_empty());
}

struct Item {
    int key;
    std::string name;

    bool operator<(const Item& other) const {
        return key < other.key;
    }
};

TEST_CASE("BinaryHeap works with custom types", "[binary_heap]") {
    BinaryHeap<Item> heap;
    heap.push({3, "three"});
    heap.push({1, "one"});
    heap.push({2, "two"});

    REQUIRE(heap.peek().key == 1);
    REQUIRE(heap.pop().name == "one");
    REQUIRE(heap.pop().name == "two");
    REQUIRE(heap.pop().name == "three");
}

TEST_CASE("BinaryHeap with std::less (min-heap)", "[binary_heap][min]") {
    BinaryHeap<int, std::less<>> heap;

    heap.push(5);
    heap.push(3);
    heap.push(7);
    heap.push(1);

    REQUIRE(heap.peek() == 1);

    REQUIRE(heap.pop() == 1);
    REQUIRE(heap.pop() == 3);
    REQUIRE(heap.pop() == 5);
    REQUIRE(heap.pop() == 7);
    REQUIRE(heap.is_empty());
}

TEST_CASE("BinaryHeap with std::greater (max-heap)", "[binary_heap][max]") {
    BinaryHeap<int, std::greater<>> heap;

    heap.push(5);
    heap.push(3);
    heap.push(7);
    heap.push(1);

    REQUIRE(heap.peek() == 7);

    REQUIRE(heap.pop() == 7);
    REQUIRE(heap.pop() == 5);
    REQUIRE(heap.pop() == 3);
    REQUIRE(heap.pop() == 1);
    REQUIRE(heap.is_empty());
}

struct AbsLess {
    bool operator()(int a, int b) const {
        return std::abs(a) < std::abs(b);
    }
};

TEST_CASE("BinaryHeap with custom comparator (absolute value min-heap)",
          "[binary_heap][custom]") {
    BinaryHeap<int, AbsLess> heap;

    heap.push(-10);
    heap.push(2);
    heap.push(-3);
    heap.push(1);

    REQUIRE(heap.peek() == 1);  // abs(1) = 1 is smallest

    REQUIRE(heap.pop() == 1);
    REQUIRE(heap.pop() == 2);
    REQUIRE(heap.pop() == -3);
    REQUIRE(heap.pop() == -10);
    REQUIRE(heap.is_empty());
}

struct ReverseStringLength {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.size() > b.size();  // treat "longer string" as "smaller"
    }
};

TEST_CASE("BinaryHeap with custom comparator (string length, reversed)",
          "[binary_heap][custom]") {
    BinaryHeap<std::string, ReverseStringLength> heap;

    heap.push("a");
    heap.push("abcd");
    heap.push("abc");
    heap.push("abcdef");

    REQUIRE(heap.peek() == "abcdef");  // longest first

    REQUIRE(heap.pop() == "abcdef");
    REQUIRE(heap.pop() == "abcd");
    REQUIRE(heap.pop() == "abc");
    REQUIRE(heap.pop() == "a");
    REQUIRE(heap.is_empty());
}

TEST_CASE("BinaryHeap handles duplicates with different comparators",
          "[binary_heap]") {
    SECTION("Min-heap") {
        BinaryHeap<int, std::less<>> heap;
        heap.push(4);
        heap.push(4);
        heap.push(2);
        heap.push(2);

        REQUIRE(heap.pop() == 2);
        REQUIRE(heap.pop() == 2);
        REQUIRE(heap.pop() == 4);
        REQUIRE(heap.pop() == 4);
        REQUIRE(heap.is_empty());
    }

    SECTION("Max-heap") {
        BinaryHeap<int, std::greater<>> heap;
        heap.push(4);
        heap.push(4);
        heap.push(2);
        heap.push(2);

        REQUIRE(heap.pop() == 4);
        REQUIRE(heap.pop() == 4);
        REQUIRE(heap.pop() == 2);
        REQUIRE(heap.pop() == 2);
        REQUIRE(heap.is_empty());
    }
}

TEST_CASE("BinaryHeap iterators basic functionality") {
    BinaryHeap<int> heap;

    SECTION("Empty heap iteration") {
        REQUIRE(heap.begin() == heap.end());
        REQUIRE(heap.rbegin() == heap.rend());
    }

    SECTION("Single element") {
        heap.push(42);

        // Forward iteration
        auto it = heap.begin();
        REQUIRE(it != heap.end());
        REQUIRE(*it == 42);
        ++it;
        REQUIRE(it == heap.end());

        // Reverse iteration
        auto rit = heap.rbegin();
        REQUIRE(rit != heap.rend());
        REQUIRE(*rit == 42);
        ++rit;
        REQUIRE(rit == heap.rend());
    }

    SECTION("Multiple elements") {
        std::vector<int> values = {10, 4, 7, 1, 3};
        for (int v : values)
            heap.push(v);

        // Forward iteration
        std::vector<int> iterated(heap.begin(), heap.end());
        REQUIRE(iterated.size() == values.size());

        // Reverse iteration
        std::vector<int> rev_iterated(heap.rbegin(), heap.rend());
        REQUIRE(rev_iterated.size() == values.size());

        // Iterated values match underlying heap storage order (not sorted)
        for (std::size_t i = 0; i < values.size(); ++i) {
            REQUIRE(iterated[i] == heap.begin()[i]);  // basic consistency
            REQUIRE(rev_iterated[i] == *(heap.rend() - heap.size() + i));
        }
    }

    SECTION("Const iterators") {
        for (int i = 1; i <= 3; ++i)
            heap.push(i);
        const BinaryHeap<int>& const_heap = heap;

        std::vector<int> iterated(const_heap.begin(), const_heap.end());
        REQUIRE(iterated.size() == 3);
        REQUIRE(iterated[0] == const_heap.begin()[0]);

        std::vector<int> rev_iterated(const_heap.rbegin(), const_heap.rend());
        REQUIRE(rev_iterated.size() == 3);
        REQUIRE(rev_iterated[0] == *(const_heap.rend() - heap.size()));
    }
}

TEST_CASE("BinaryHeap iterator edge cases") {
    BinaryHeap<int> heap;

    SECTION("Iterator after clear") {
        heap.push(5);
        heap.push(10);
        heap.clear();
        REQUIRE(heap.begin() == heap.end());
        REQUIRE(heap.rbegin() == heap.rend());
    }

    SECTION("Incrementing past end is invalid") {
        heap.push(1);
        auto it = heap.begin();
        ++it;
        REQUIRE(it == heap.end());
    }

    SECTION("Decrementing reverse iterator past rend is invalid") {
        heap.push(1);
        auto rit = heap.rbegin();
        ++rit;
        REQUIRE(rit == heap.rend());
    }

    SECTION("Iterator on large heap") {
        for (int i = 0; i < 100; ++i)
            heap.push(i);
        std::size_t count = 0;
        for (auto it = heap.begin(); it != heap.end(); ++it)
            count++;
        REQUIRE(count == 100);

        count = 0;
        for (auto rit = heap.rbegin(); rit != heap.rend(); ++rit)
            count++;
        REQUIRE(count == 100);
    }
}

TEST_CASE("BinaryHeap iterator stability during pops") {
    BinaryHeap<int> heap;
    heap.push(1);
    heap.push(2);
    heap.push(3);

    SECTION("Iterators reflect storage, not heap order") {
        auto it = heap.begin();
        std::vector<int> vals;
        while (it != heap.end()) {
            vals.push_back(*it);
            ++it;
        }

        int popped = heap.pop();
        REQUIRE(popped == 1);

        REQUIRE(vals.size() == 3);
    }
}

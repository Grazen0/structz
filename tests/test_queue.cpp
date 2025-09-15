#include <catch2/catch_test_macros.hpp>
#include "queue.h"

TEST_CASE("Queue basic operations", "[queue]") {
    Queue<int> q;

    SECTION("New queue is empty") {
        REQUIRE(q.is_empty());
        REQUIRE(q.size() == 0);
    }

    SECTION("Enqueue increases size") {
        q.enqueue(10);
        REQUIRE_FALSE(q.is_empty());
        REQUIRE(q.size() == 1);

        q.enqueue(20);
        REQUIRE(q.size() == 2);
    }

    SECTION("Peek returns front without removal") {
        q.enqueue(42);
        q.enqueue(43);

        REQUIRE(q.peek() == 42);
        REQUIRE(q.size() == 2);  // size unchanged
    }

    SECTION("Dequeue removes elements in FIFO order") {
        q.enqueue(1);
        q.enqueue(2);
        q.enqueue(3);

        REQUIRE(q.dequeue() == 1);
        REQUIRE(q.dequeue() == 2);
        REQUIRE(q.dequeue() == 3);

        REQUIRE(q.is_empty());
    }

    SECTION("Interleaving enqueue and dequeue works correctly") {
        q.enqueue(5);
        REQUIRE(q.dequeue() == 5);

        q.enqueue(10);
        q.enqueue(20);
        REQUIRE(q.dequeue() == 10);

        q.enqueue(30);
        REQUIRE(q.dequeue() == 20);
        REQUIRE(q.dequeue() == 30);
        REQUIRE(q.is_empty());
    }
}

TEST_CASE("Queue const correctness", "[queue]") {
    Queue<int> q;
    q.enqueue(100);
    q.enqueue(200);

    const Queue<int>& cq = q;

    REQUIRE(cq.peek() == 100);
    REQUIRE(cq.size() == 2);
    REQUIRE_FALSE(cq.is_empty());
}

TEST_CASE("Queue edge cases", "[queue]") {
    Queue<int> q;

    SECTION("Dequeue on empty queue should fail") {
        REQUIRE_THROWS(q.dequeue());
    }

    SECTION("Peek on empty queue should fail") {
        REQUIRE_THROWS(q.peek());
    }

    SECTION("Enqueue and dequeue a large number of elements") {
        const int N = 10000;
        for (int i = 0; i < N; i++) {
            q.enqueue(i);
        }
        REQUIRE(q.size() == N);

        for (int i = 0; i < N; i++) {
            REQUIRE(q.dequeue() == i);
        }
        REQUIRE(q.is_empty());
    }
}

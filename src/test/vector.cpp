#include "../vector.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("vectors can be constructed", "[vector]") {
    const Vector<int> vec;
    REQUIRE(vec.is_empty());
}

TEST_CASE("vectors can be constructed with elements", "[vector]") {
    const Vector<int> vec{1, 3, 5, 2, 7, 4};

    REQUIRE(vec.size() == 6);
    REQUIRE(vec.capacity() >= 6);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 3);
    REQUIRE(vec[2] == 5);
    REQUIRE(vec[3] == 2);
    REQUIRE(vec[4] == 7);
    REQUIRE(vec[5] == 4);
}

TEST_CASE("vectors can be constructed with capacity", "[vector]") {
    const auto vec = Vector<int>::with_capacity(10);

    REQUIRE(vec.is_empty());
    REQUIRE(vec.capacity() == 10);
}

TEST_CASE("vectors cannot be indexed out of bounds", "[vector]") {
    const Vector<int> vec{1, 2, 3, 4, 5};

    REQUIRE_THROWS(vec[5]);
    REQUIRE_THROWS(vec[6]);
    REQUIRE_THROWS(vec[42]);
}

TEST_CASE("vectors can be pushed to", "[vector]") {
    Vector<int> vec{1, 2, 3, 4};
    vec.push(5);
    vec.push(6);

    REQUIRE(vec.size() == 6);
    REQUIRE(vec.capacity() >= 6);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
    REQUIRE(vec[3] == 4);
    REQUIRE(vec[4] == 5);
    REQUIRE(vec[5] == 6);
}

TEST_CASE("vectors can be constructed by copy", "[vector]") {
    const Vector<int> vec_a{1, 2, 3, 4};
    Vector<int> vec_b{vec_a};
    vec_b.push(5);
    vec_b.push(6);

    REQUIRE(vec_a.size() == 4);
    REQUIRE(vec_a.capacity() >= 4);
    REQUIRE(vec_b.size() == 6);
    REQUIRE(vec_b.capacity() >= 6);
    REQUIRE(vec_b[0] == 1);
    REQUIRE(vec_b[1] == 2);
    REQUIRE(vec_b[2] == 3);
    REQUIRE(vec_b[3] == 4);
    REQUIRE(vec_b[4] == 5);
    REQUIRE(vec_b[5] == 6);
}

TEST_CASE("vectors can be constructed by move", "[vector]") {
    Vector<int> vec_a{1, 2, 3, 4};
    Vector<int> vec_b{std::move(vec_a)};
    vec_b.push(5);

    REQUIRE(vec_a.size() == 0);
    REQUIRE(vec_a.capacity() == 0);
    REQUIRE(vec_b.size() == 5);
    REQUIRE(vec_b.capacity() >= 5);
    REQUIRE(vec_b[0] == 1);
    REQUIRE(vec_b[1] == 2);
    REQUIRE(vec_b[2] == 3);
    REQUIRE(vec_b[3] == 4);
    REQUIRE(vec_b[4] == 5);
}

TEST_CASE("vectors can be copied", "[vector]") {
    const Vector<int> vec_a{1, 2, 3, 4};
    Vector<int> vec_b{1, 2};
    vec_b = vec_a;

    REQUIRE(vec_a.size() == vec_b.size());
    REQUIRE(vec_a[0] == vec_b[0]);
    REQUIRE(vec_a[1] == vec_b[1]);
    REQUIRE(vec_a[2] == vec_b[2]);
    REQUIRE(vec_a[3] == vec_b[3]);
}

TEST_CASE("vectors can be moved", "[vector]") {
    Vector<int> vec_a{1, 2, 3, 4};
    Vector<int> vec_b{1, 2};
    vec_b = std::move(vec_a);

    REQUIRE(vec_a.size() == 0);
    REQUIRE(vec_b.size() == 4);
    REQUIRE(vec_b[0] == 1);
    REQUIRE(vec_b[1] == 2);
    REQUIRE(vec_b[2] == 3);
    REQUIRE(vec_b[3] == 4);
}

TEST_CASE("vectors can popped from", "[vector]") {
    Vector<int> vec{1, 2, 3, 4};
    int last = vec.pop();
    REQUIRE(vec.size() == 3);
    REQUIRE(last == 4);
}

TEST_CASE("vectors can be modified", "[vector]") {
    Vector<int> vec{1, 2, 3, 4};
    vec[0] = 42;
    vec[2] = 36;

    REQUIRE(vec.size() == 4);
    REQUIRE(vec[0] == 42);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 36);
    REQUIRE(vec[3] == 4);
}

TEST_CASE("reserving bigger changes capacity but not size", "[vector]") {
    Vector<int> vec{1, 2, 3, 4, 5};
    vec.reserve(10);

    REQUIRE(vec.size() == 5);
    REQUIRE(vec.capacity() >= 10);

    SECTION("reserving down unused capacity does not change capacity") {
        vec.reserve(7);
        REQUIRE(vec.size() == 5);
        REQUIRE(vec.capacity() >= 10);
    }
}

TEST_CASE("vectors can be concatenated", "[vector]") {
    Vector<int> vec_a{1, 2, 3, 4};
    Vector<int> vec_b{5, 6};

    SECTION("concatenating by copy preserves the concatenated vector") {
        vec_a.concat(vec_b);

        REQUIRE(vec_a.size() == 6);
        REQUIRE(vec_b.size() == 2);
    }

    SECTION("concatenating by move empties the concatenated vector") {
        vec_a.concat(std::move(vec_b));

        REQUIRE(vec_a.size() == 6);
        REQUIRE(vec_b.is_empty());
    }
}

TEST_CASE("vectors can be compared", "[vector]") {
    const Vector<int> vec_a{1, 2, 3};
    const Vector<int> vec_b{1, 2, 3};
    const Vector<int> vec_c{1, 2};
    const Vector<int> vec_d{1, 2, 4};

    REQUIRE(vec_a == vec_a);
    REQUIRE(vec_a == vec_b);
    REQUIRE(vec_a != vec_c);
    REQUIRE(vec_a != vec_d);

    REQUIRE(vec_b == vec_a);
    REQUIRE(vec_b == vec_b);
    REQUIRE(vec_b != vec_c);
    REQUIRE(vec_b != vec_d);

    REQUIRE(vec_c != vec_a);
    REQUIRE(vec_c != vec_b);
    REQUIRE(vec_c == vec_c);
    REQUIRE(vec_c != vec_d);

    REQUIRE(vec_d != vec_a);
    REQUIRE(vec_d != vec_b);
    REQUIRE(vec_d != vec_c);
    REQUIRE(vec_d == vec_d);
}

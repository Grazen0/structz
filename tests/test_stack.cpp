#include <catch2/catch_test_macros.hpp>
#include <vector>
#include "stack.h"

TEST_CASE("can perform multiple operations on stack", "[stack]") {
    Stack<int> stack;
    REQUIRE(stack.empty());

    stack.push(8);
    stack.push(5);
    stack.push(4);
    stack.push(2);

    std::vector<int> result;

    while (!stack.empty()) {
        int val = stack.pop();
        result.push_back(val);
    }

    REQUIRE(result == std::vector<int>{2, 4, 5, 8});

    REQUIRE_THROWS(stack.top());
    REQUIRE_THROWS(stack.pop());
}

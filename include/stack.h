#ifndef STUCTZ_STACK_H
#define STUCTZ_STACK_H

#include <stdexcept>
#include <utility>
#include "linked_list.h"

template<typename T>
class Stack {
    LinkedList<T> list;

public:
    Stack() = default;

    void push(T data) {
        list.push_front(std::move(data));
    }

    T pop() {
        if (list.is_empty())
            throw std::runtime_error("stack underflow");

        return list.pop_front();
    }

    T& top() {
        if (list.is_empty())
            throw std::runtime_error("stack is empty");

        return list.front();
    }

    const T& top() const {
        if (list.is_empty())
            throw std::runtime_error("stack is empty");

        return list.front();
    }

    [[nodiscard]] bool is_empty() const {
        return list.is_empty();
    }
};

#endif

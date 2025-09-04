#ifndef STUCTZ_STACK_H
#define STUCTZ_STACK_H

#include <stdexcept>
#include <utility>

template<typename T>
class Stack {
    struct Node {
        T data;
        Node* next = nullptr;

        Node(T data)
            : data(std::move(data)) {}

        Node(T data, Node* const next)
            : data(std::move(data)),
              next(next) {}
    };

    Node* head = nullptr;

public:
    Stack() = default;

    Stack(const Stack<T>& other) {
        Node** cur_this = &head;
        Node* cur_other = other.head;

        while (cur_other != nullptr) {
            *cur_this = new Node(cur_other->data);

            cur_this = &(cur_this)->next;
            cur_other = cur_other->next;
        }
    }

    Stack(Stack<T>&& other) noexcept {
        std::swap(head, other.head);
    }

    ~Stack() {
        Node* cur = head;
        while (cur != nullptr)
            delete std::exchange(cur, cur->next);
    }

    Stack<T>& operator=(const Stack<T>& other) = delete;

    Stack<T>& operator=(Stack<T>&& other) = delete;

    void push(T data) {
        head = new Node(data, head);
    }

    T pop() {
        if (head == nullptr)
            throw std::runtime_error("stack underflow");

        const T data = std::move(head->data);
        delete std::exchange(head, head->next);
        return data;
    }

    T& top() {
        if (head == nullptr)
            throw std::runtime_error("stack is empty");

        return head->data;
    }

    const T& top() const {
        if (head == nullptr)
            throw std::runtime_error("stack is empty");

        return head->data;
    }

    [[nodiscard]] bool empty() const {
        return head == nullptr;
    }
};

#endif

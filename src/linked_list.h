#ifndef DATA_STRUCTURES_LINKED_LIST_H
#define DATA_STRUCTURES_LINKED_LIST_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T>
class LinkedList {
    struct Node {
        T data;
        Node* next = nullptr;

        Node(T data)
            : data(std::move(data)) {}

        Node(T data, Node* const next)
            : data(std::move(data)),
              next(next) {}
    };

    Node* m_head = nullptr;
    size_t m_size = 0;

public:
    LinkedList() = default;

    LinkedList(const LinkedList<T>& other)
        : m_size(other.m_size) {
        Node** this_cur = &m_head;
        Node* other_cur = other.m_head;

        while (other_cur != nullptr) {
            *this_cur = new Node(other_cur->data);
            this_cur = &(*this_cur)->next;
            other_cur = other_cur->next;
        }
    }

    LinkedList(LinkedList<T>&& other) noexcept {
        std::swap(m_head, other.m_head);
        std::swap(m_size, other.m_size);
    }

    ~LinkedList() {
        Node* cur = m_head;

        while (cur != nullptr) {
            Node* const next = cur->next;
            delete cur;
            cur = next;
        }
    }

    T& front() {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        return m_head->data;
    }

    const T& front() const {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        return m_head->data;
    }

    [[nodiscard]] size_t size() const {
        return m_size;
    }

    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    void push_front(T data) {
        m_head = new Node(std::move(data), m_head);
        ++m_size;
    }

    // TODO: implement
    LinkedList<T>& operator=(LinkedList<T> other) = delete;

    // TODO: implement
    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept = delete;

    T const& operator[](const size_t index) const {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = m_head;

        for (size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    T& operator[](const size_t index) {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = m_head;

        for (size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }
};

#endif

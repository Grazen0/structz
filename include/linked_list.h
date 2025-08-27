#ifndef STRUCTZ_LINKED_LIST_H
#define STRUCTZ_LINKED_LIST_H

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <utility>

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

    class iterator {
        Node* cur;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        iterator(Node* const head)
            : cur(head) {}

        iterator& operator++() {
            cur = cur->next;
            return *this;
        }

        iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(const iterator& other) const {
            return this->cur == other.cur;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        value_type& operator*() {
            return cur->data;
        }
    };

    class const_iterator {
        const Node* cur;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const Node* const head)
            : cur(head) {}

        const_iterator& operator++() {
            cur = cur->next;
            return *this;
        }

        const_iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        bool operator==(const const_iterator& other) const {
            return this->cur == other.cur;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        value_type& operator*() {
            return cur->data;
        }
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
        clear();
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) = delete;

    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept = delete;

    [[nodiscard]] T& front() {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        return m_head->data;
    }

    [[nodiscard]] const T& front() const {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        return m_head->data;
    }

    [[nodiscard]] T& back() {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        Node* cur = m_head;

        while (cur->next != nullptr)
            cur = cur->next;

        return cur->data;
    }

    [[nodiscard]] const T& back() const {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        Node* cur = m_head;

        while (cur->next != nullptr)
            cur = cur->next;

        return cur->data;
    }

    void push_front(T data) {
        m_head = new Node(std::move(data), m_head);
        ++m_size;
    }

    void push_back(T data) {
        Node** cur = &m_head;

        while (*cur != nullptr)
            cur = &(*cur)->next;

        *cur = new Node(std::move(data));
        ++m_size;
    }

    T pop_front() {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        const T data = std::move(m_head->data);

        Node* const prev_head = m_head;
        m_head = m_head->next;
        delete prev_head;

        --m_size;
        return data;
    }

    T pop_back() {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        Node** cur = &m_head;

        while ((*cur)->next != nullptr)
            cur = &(*cur)->next;

        const T data = std::move((*cur)->data);

        Node* prev_tail = *cur;
        delete prev_tail;
        *cur = nullptr;

        --m_size;
        return data;
    }

    [[nodiscard]] const T& operator[](const size_t index) const {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = m_head;

        for (size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    [[nodiscard]] T& operator[](const size_t index) {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = m_head;

        for (size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    [[nodiscard]] size_t size() const {
        return m_size;
    }

    void clear() {
        Node* cur = m_head;
        m_head = nullptr;

        while (cur != nullptr) {
            Node* const next = cur->next;
            delete cur;
            cur = next;
        }

        m_size = 0;
    }

    void sort() {
        // TODO: implement
    }

    void reverse() {
        if (m_size <= 1)
            return;

        Node* cur_first = m_head;
        Node* cur_second = m_head->next;

        while (cur_second != nullptr) {
            Node* const next = cur_second->next;
            cur_second->next = cur_first;

            cur_first = cur_second;
            cur_second = next;
        }

        m_head->next = nullptr;
        m_head = cur_first;
    }

    iterator begin() {
        return iterator(m_head);
    }

    iterator end() {
        return iterator(nullptr);
    }

    const_iterator begin() const {
        return const_iterator(m_head);
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }
};

#endif

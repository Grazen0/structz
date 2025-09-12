#ifndef STRUCTZ_DOUBLY_LINKED_LIST_H
#define STRUCTZ_DOUBLY_LINKED_LIST_H

#include <cstddef>
#include <iterator>
#include <stdexcept>

template<typename T>
class DoublyLinkedList {
    struct Node {
        T data;
        Node* prev = nullptr;
        Node* next = nullptr;

        Node(T data)
            : data(std::move(data)) {}
    };

    class iterator {
        Node* cur;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
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

        iterator& operator--() {
            cur = cur->prev;
            return *this;
        }

        iterator operator--(int) {
            iterator retval = *this;
            --(*this);
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
            const_iterator retval = *this;
            ++(*this);
            return retval;
        }

        const_iterator& operator--() {
            cur = cur->prev;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator retval = *this;
            --(*this);
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

    class reverse_iterator {
        Node* cur;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        reverse_iterator(Node* const head)
            : cur(head) {}

        reverse_iterator& operator++() {
            cur = cur->prev;
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator retval = *this;
            ++(*this);
            return retval;
        }

        reverse_iterator& operator--() {
            cur = cur->next;
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator retval = *this;
            --(*this);
            return retval;
        }

        bool operator==(const reverse_iterator& other) const {
            return this->cur == other.cur;
        }

        bool operator!=(const reverse_iterator& other) const {
            return !(*this == other);
        }

        value_type& operator*() {
            return cur->data;
        }
    };

    class const_reverse_iterator {
        const Node* cur;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;

        const_reverse_iterator(const Node* const head)
            : cur(head) {}

        const_reverse_iterator& operator++() {
            cur = cur->prev;
            return *this;
        }

        const_reverse_iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        const_reverse_iterator& operator--() {
            cur = cur->next;
            return *this;
        }

        const_reverse_iterator operator--(int) {
            const_reverse_iterator retval = *this;
            --(*this);
            return retval;
        }

        bool operator==(const const_reverse_iterator& other) const {
            return this->cur == other.cur;
        }

        bool operator!=(const const_reverse_iterator& other) const {
            return !(*this == other);
        }

        value_type& operator*() {
            return cur->data;
        }
    };

    Node* m_head = nullptr;
    Node* m_tail = nullptr;
    std::size_t m_size = 0;

public:
    DoublyLinkedList() = default;

    ~DoublyLinkedList() {
        clear();
    }

    DoublyLinkedList<T>& operator=(const DoublyLinkedList<T>& other) = delete;
    DoublyLinkedList<T>& operator=(DoublyLinkedList<T>&& other) = delete;

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
        if (m_tail == nullptr)
            throw std::runtime_error("list is empty");

        return m_tail->data;
    }

    [[nodiscard]] const T& back() const {
        if (m_tail == nullptr)
            throw std::runtime_error("list is empty");

        return m_tail->data;
    }

    void push_front(T data) {
        Node* const node = new Node(std::move(data));

        if (m_head == nullptr) {
            m_head = node;
            m_tail = node;
        } else {
            node->next = m_head;
            m_head->prev = node;
            m_head = node;
        }

        ++m_size;
    }

    void push_back(T data) {
        Node* const node = new Node(std::move(data));

        if (m_tail == nullptr) {
            m_head = node;
            m_tail = node;
        } else {
            node->prev = m_tail;
            m_tail->next = node;
            m_tail = node;
        }

        ++m_size;
    }

    T pop_front() {
        if (m_head == nullptr)
            throw std::runtime_error("list is empty");

        const T data = std::move(m_head->data);
        Node* const new_head = m_head->next;
        delete m_head;

        if (new_head != nullptr)
            new_head->prev = nullptr;
        else
            m_tail = nullptr;

        m_head = new_head;
        --m_size;
        return data;
    }

    T pop_back() {
        if (m_tail == nullptr)
            throw std::runtime_error("list is empty");

        const T data = std::move(m_tail->data);
        Node* const new_tail = m_tail->prev;
        delete m_tail;

        if (new_tail != nullptr)
            new_tail->next = nullptr;
        else
            m_head = nullptr;

        m_tail = new_tail;
        --m_size;
        return data;
    }

    void remove(const std::size_t index) {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node** cur = &m_head;

        for (std::size_t i = 0; i < index; ++i)
            cur = &(*cur)->next;

        Node* const next = (*cur)->next;

        if (next != nullptr)
            next->prev = *cur == nullptr ? nullptr : (*cur)->prev;

        delete *cur;
        *cur = next;

        --m_size;
    }

    [[nodiscard]] const T& operator[](const std::size_t index) const {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = m_head;

        for (std::size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    [[nodiscard]] T& operator[](const std::size_t index) {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = m_head;

        for (std::size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    [[nodiscard]] bool empty() const {
        return m_size == 0;
    }

    [[nodiscard]] std::size_t size() const {
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

        m_tail = nullptr;
        m_size = 0;
    }

    void reverse() {
        if (m_head == nullptr)
            return;

        Node* cur_first = m_head;
        Node* cur_second = m_head->next;

        while (cur_second != nullptr) {
            Node* const next = cur_second->next;
            cur_second->next = cur_first;
            cur_first->prev = cur_second;

            cur_first = cur_second;
            cur_second = next;
        }

        m_head->next = nullptr;
        m_tail->prev = nullptr;

        Node* const tmp = m_head;
        m_head = m_tail;
        m_tail = tmp;
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

    reverse_iterator rbegin() {
        return reverse_iterator(m_tail);
    }

    reverse_iterator rend() {
        return reverse_iterator(nullptr);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(m_tail);
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(nullptr);
    }
};

#endif

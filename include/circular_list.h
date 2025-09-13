#ifndef STRUCTZ_CIRCULAR_LIST_H
#define STRUCTZ_CIRCULAR_LIST_H

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

template<typename T>
class CircularList {
    struct Node {
        T data;
        Node* prev;
        Node* next;

        explicit Node(T data, Node* const prev, Node* const next)
            : data(std::move(data)),
              prev(prev),
              next(next) {}
    };

    Node* m_sentinel = new Node(T(), nullptr, nullptr);
    std::size_t m_size = 0;

    void swap(CircularList<T>& other) noexcept {
        std::swap(m_sentinel, other.m_sentinel);
        std::swap(m_size, other.m_size);
    }

    [[nodiscard]] constexpr Node*& head() {
        return m_sentinel->next;
    }

    [[nodiscard]] constexpr const Node* const& head() const {
        return m_sentinel->next;
    }

    [[nodiscard]] constexpr Node*& tail() {
        return m_sentinel->prev;
    }

    [[nodiscard]] constexpr const Node* const& tail() const {
        return m_sentinel->prev;
    }

public:
    class iterator {
        Node* cur;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;

        explicit iterator(Node* const cur)
            : cur(cur) {}

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
            return cur == other.cur;
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
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using difference_type = std::ptrdiff_t;

        explicit const_iterator(const Node* const cur)
            : cur(cur) {}

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
            return cur == other.cur;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        value_type& operator*() {
            return cur->data;
        }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    CircularList() {
        m_sentinel->next = m_sentinel;
        m_sentinel->prev = m_sentinel;
    }

    CircularList(const CircularList<T>& other)
        : CircularList() {
        for (const auto& el : other)
            push_back(el);
    }

    CircularList(CircularList<T>&& other) noexcept {
        swap(other);
    }

    ~CircularList() {
        clear();
    }

    CircularList<T>& operator=(const CircularList<T>& other) {
        CircularList<T>(other).swap(*this);
        return *this;
    }

    CircularList<T>& operator=(CircularList<T>&& other) noexcept {
        swap(other);
        return *this;
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return m_size == 0;
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] constexpr T& front() {
        if (m_size == 0)
            throw std::runtime_error("list is empty");

        return head()->data;
    }

    [[nodiscard]] constexpr const T& front() const {
        if (m_size == 0)
            throw std::runtime_error("list is empty");

        return head()->data;
    }

    [[nodiscard]] constexpr T& back() {
        if (m_size == 0)
            throw std::runtime_error("list is empty");

        return tail()->data;
    }

    [[nodiscard]] constexpr const T& back() const {
        if (m_size == 0)
            throw std::runtime_error("list is empty");

        return tail()->data;
    }

    void push_front(T data) {
        Node* const node = new Node(std::move(data), m_sentinel, head());
        head()->prev = node;
        head() = node;

        ++m_size;
    }

    void push_back(T data) {
        Node* const node = new Node(std::move(data), tail(), m_sentinel);
        tail()->next = node;
        tail() = node;

        ++m_size;
    }

    T pop_front() {
        if (m_size == 0)
            throw std::runtime_error("list is empty");

        const T value = std::move(head()->data);

        delete std::exchange(head(), head()->next);
        head()->prev = m_sentinel;

        --m_size;
        return value;
    }

    T pop_back() {
        if (m_size == 0)
            throw std::runtime_error("list is empty");

        const T value = std::move(tail()->data);

        delete std::exchange(tail(), tail()->prev);
        tail()->next = m_sentinel;

        --m_size;
        return value;
    }

    void remove(const std::size_t index) {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = head();

        for (std::size_t i = 0; i < index; ++i)
            cur = cur->next;

        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        delete std::exchange(cur, nullptr);

        --m_size;
    }

    [[nodiscard]] const T& operator[](const std::size_t index) const {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = head();

        for (std::size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    [[nodiscard]] T& operator[](const std::size_t index) {
        if (index >= m_size)
            throw std::out_of_range("list index out of bounds");

        Node* cur = head();

        for (std::size_t i = 0; i < index; ++i)
            cur = cur->next;

        return cur->data;
    }

    void clear() {
        Node* cur = head();

        while (cur != m_sentinel)
            delete std::exchange(cur, cur->next);

        m_sentinel->next = m_sentinel;
        m_sentinel->prev = m_sentinel;
        m_size = 0;
    }

    void reverse() {
        Node* cur_first = head();
        Node* cur_second = head()->next;

        while (cur_second != m_sentinel) {
            Node* const next = cur_second->next;
            cur_second->next = cur_first;
            cur_first->prev = cur_second;

            cur_first = cur_second;
            cur_second = next;
        }

        head()->next = m_sentinel;
        tail()->prev = m_sentinel;
        std::swap(head(), tail());
    }

    [[nodiscard]] constexpr iterator begin() {
        return iterator(head());
    }

    [[nodiscard]] constexpr iterator end() {
        return iterator(m_sentinel);
    }

    [[nodiscard]] constexpr const_iterator begin() const {
        return const_iterator(head());
    }

    [[nodiscard]] constexpr const_iterator end() const {
        return const_iterator(m_sentinel);
    }

    [[nodiscard]] constexpr reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    [[nodiscard]] constexpr reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] constexpr const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
};

#endif

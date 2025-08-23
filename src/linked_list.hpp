#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <cstddef>
#include <utility>

template<typename T>
struct Node {
    T data;
    Node<T>* next = nullptr;
};

template<typename T>
class LinkedList {
    Node<T>* m_head = nullptr;
    size_t m_size = 0;

public:
    LinkedList();
    LinkedList(const LinkedList<T>& other);
    LinkedList(LinkedList<T>&& other) noexcept;

    ~LinkedList();

    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool is_empty() const;

    template<typename Tt>
    void prepend(Tt data);

    template<typename Tt>
    void append(Tt data);

    T const& get_at(size_t index) const;
    T& get_at(size_t index);

    LinkedList<T>& operator=(LinkedList<T> other);
    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept;
};

template<typename T>
LinkedList<T>::LinkedList() = default;

template<typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& other)
    : m_size(other.m_size) {
    auto*& this_current = this->m_head;
    auto* other_current = other.m_head;

    while (other_current != nullptr) {
        this_current = new Node<T>(other_current->data);
        this_current = this_current->next;
        other_current = other_current->next;
    }
}

template<typename T>
LinkedList<T>::LinkedList(LinkedList<T>&& other) noexcept
    : LinkedList() {
    std::swap(this->m_head, other.m_head);
    std::swap(this->m_size, other.m_size);
}

template<typename T>
LinkedList<T>::~LinkedList() {
    auto* current = this->m_head;

    while (current != nullptr) {
        auto* next = current->next;
        delete current;
        current = next;
    }
}

template<typename T>
size_t LinkedList<T>::size() const {
    return this->m_size;
}

template<typename T>
bool LinkedList<T>::is_empty() const {
    return this->m_size == 0;
}

template<typename T>
template<typename Tt>
void LinkedList<T>::prepend(Tt data) {
    auto* node = new Node<T>();
    node->data = data;
    node->next = std::exchange(this->m_head, node);
    this->m_size++;
}

template<typename T>
T const& LinkedList<T>::get_at(size_t index) const {
    if (index >= this->m_size) {
        throw "linked list index out of bounds";
    }

    auto* current = this->m_head;

    for (; index != 0; index--) {
        current = current->next;
    }

    return current->data;
}

template<typename T>
T& LinkedList<T>::get_at(size_t index) {
    if (index >= this->m_size) {
        throw "linked list index out of bounds";
    }

    auto* current = this->m_head;

    for (; index != 0; index--) {
        current = current->next;
    }

    return current->data;
}

#endif

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include <initializer_list>
#include <utility>

template<typename T>
class Vector {
    T* m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

public:
    Vector();
    Vector(std::initializer_list<T> elements);
    Vector(const Vector& other);
    Vector(Vector&& other) noexcept;

    ~Vector();

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t capacity() const;
    [[nodiscard]] bool is_empty() const;

    void reserve(size_t capacity);

    template<typename Tt>
    void push(Tt value);

    T&& pop();

    Vector<T>& concat(Vector<T> other);

    Vector& operator=(Vector<T> other);
    T const& operator[](size_t index) const;
    T& operator[](size_t index);
    bool operator==(const Vector<T>& other) const;
    bool operator!=(const Vector<T>& other) const;

    static Vector<T> with_capacity(size_t initial_capacity);
};

template<typename T>
Vector<T>::Vector() = default;

template<typename T>
Vector<T>::Vector(const std::initializer_list<T> elements)
    : m_data(elements.size() == 0 ? nullptr : new T[elements.size()]),
      m_capacity(elements.size()) {
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        this->push(*it);
    }
}

template<typename T>
Vector<T>::Vector(const Vector<T>& other)
    : m_data(other.m_capacity == 0 ? nullptr : new T[other.m_capacity]),
      m_size(other.m_size),
      m_capacity(other.m_capacity) {
    for (size_t i = 0; i < other.m_size; i++) {
        this->m_data[i] = other.m_data[i];
    }
}

template<typename T>
Vector<T>::Vector(Vector<T>&& other) noexcept
    : Vector() {
    std::swap(this->m_size, other.m_size);
    std::swap(this->m_capacity, other.m_capacity);
    std::swap(this->m_data, other.m_data);
}

template<typename T>
Vector<T>::~Vector() {
    delete[] this->m_data;
}

template<typename T>
size_t Vector<T>::size() const {
    return this->m_size;
}

template<typename T>
size_t Vector<T>::capacity() const {
    return this->m_capacity;
}

template<typename T>
bool Vector<T>::is_empty() const {
    return this->m_size == 0;
}

template<typename T>
void Vector<T>::reserve(const size_t capacity) {
    if (this->m_capacity >= capacity) {
        return;
    }

    auto* new_data = new T[capacity];

    for (size_t i = 0; i < this->m_size; i++) {
        new_data[i] = this->m_data[i];
    }

    delete[] this->m_data;
    this->m_data = new_data;
    this->m_capacity = capacity;
}

template<typename T>
template<typename Tt>
void Vector<T>::push(Tt value) {
    if (this->m_size == this->m_capacity) {
        this->reserve(2 * this->m_capacity);
    }

    this->m_data[this->m_size] = std::move(value);
    this->m_size++;
}

template<typename T>
T&& Vector<T>::pop() {
    if (this->m_size == 0) {
        throw "vector underflow";
    }

    this->m_size--;
    return std::move(this->m_data[this->m_size]);
}

template<typename T>
Vector<T>& Vector<T>::concat(Vector<T> other) {
    if (other.is_empty()) {
        return *this;
    }

    const size_t new_size = this->m_size + other.m_size;
    this->reserve(new_size);

    for (size_t i = 0; i < other.m_size; i++) {
        std::swap(this->m_data[this->m_size + i], other.m_data[i]);
    }

    this->m_size = new_size;

    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T> other) {
    if (this == &other) {
        return *this;
    }

    std::swap(this->m_size, other.m_size);
    std::swap(this->m_capacity, other.m_capacity);
    std::swap(this->m_data, other.m_data);

    return *this;
}

template<typename T>
T const& Vector<T>::operator[](const size_t index) const {
    if (index >= this->m_size) {
        throw "vector index out of bounds";
    }

    return this->m_data[index];
}

template<typename T>
T& Vector<T>::operator[](const size_t index) {
    if (index >= this->m_size) {
        throw "vector index out of bounds";
    }

    return this->m_data[index];
}

template<typename T>
bool Vector<T>::operator==(const Vector<T>& other) const {
    if (this->m_size != other.m_size) {
        return false;
    }

    for (size_t i = 0; i < this->m_size; i++) {
        if (this->m_data[i] != other.m_data[i]) {
            return false;
        }
    }

    return true;
}

template<typename T>
bool Vector<T>::operator!=(const Vector<T>& other) const {
    return !(*this == other);
}

template<typename T>
Vector<T> Vector<T>::with_capacity(const size_t initial_capacity) {
    if (initial_capacity == 0) {
        return {};
    }

    Vector<T> vec;
    vec.m_capacity = initial_capacity;
    vec.m_data = new T[initial_capacity];
    return vec;
}

#endif

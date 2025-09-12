#ifndef STRUCTZ_VECTOR_H
#define STRUCTZ_VECTOR_H

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

template<typename T>
class Vec {
    std::size_t m_capacity = 0;
    std::size_t m_size = 0;
    T* m_data = nullptr;

    void swap(Vec<T>& other) noexcept {
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_size, other.m_size);
        std::swap(m_data, other.m_data);
    }

public:
    Vec() = default;

    Vec(const Vec<T>& other)
        : m_capacity(other.m_capacity),
          m_size(other.m_size),
          m_data(m_capacity != 0 ? new T[m_capacity]() : nullptr) {
        for (std::size_t i = 0; i < other.m_size; ++i)
            m_data[i] = other.m_data[i];
    }

    Vec(Vec<T>&& other) noexcept {
        swap(other);
    }

    Vec(const std::size_t initial_size)
        : m_capacity(initial_size),
          m_size(initial_size),
          m_data(m_capacity != 0 ? new T[m_capacity]() : nullptr) {}

    ~Vec() {
        delete[] std::exchange(m_data, nullptr);
    }

    static constexpr Vec<T> with_capacity(const std::size_t initial_capacity) {
        if (initial_capacity == 0)
            return Vec<T>();

        Vec<T> vec;
        vec.m_capacity = initial_capacity;
        vec.m_data = new T[vec.m_capacity]();
        return vec;
    }

    Vec<T>& operator=(const Vec<T>& other) {
        Vec<T>(other).swap(*this);
        return *this;
    }

    Vec<T>& operator=(Vec<T>&& other) noexcept {
        swap(other);
        return *this;
    }

    [[nodiscard]] constexpr std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return m_size == 0;
    }

    constexpr T& operator[](const std::size_t index) {
        if (index >= m_size)
            throw std::out_of_range("Index out of bounds");

        return m_data[index];
    }

    const T& operator[](const std::size_t index) const {
        if (index >= m_size)
            throw std::out_of_range("Index out of bounds");

        return m_data[index];
    }

    constexpr T& first() {
        if (m_size == 0)
            throw std::out_of_range("Vector is empty");

        return m_data[0];
    }

    constexpr const T& first() const {
        if (m_size == 0)
            throw std::out_of_range("Vector is empty");

        return m_data[0];
    }

    constexpr T& last() {
        if (m_size == 0)
            throw std::out_of_range("Vector is empty");

        return m_data[m_size - 1];
    }

    constexpr const T& last() const {
        if (m_size == 0)
            throw std::out_of_range("Vector is empty");

        return m_data[m_size - 1];
    }

    void push(T value) {
        if (m_size == m_capacity) {
            const std::size_t new_capacity =
                m_capacity == 0 ? 4 : 2 * m_capacity;

            T* const new_data = new T[new_capacity]();

            for (std::size_t i = 0; i < m_size; ++i)
                new_data[i] = m_data[i];

            delete[] std::exchange(m_data, new_data);

            m_capacity = new_capacity;
        }

        m_data[m_size] = std::move(value);
        ++m_size;
    }

    T pop() {
        if (m_size == 0)
            throw std::out_of_range("Vector is empty");

        const T value = std::move(m_data[m_size - 1]);
        --m_size;
        return value;
    }
};

#endif

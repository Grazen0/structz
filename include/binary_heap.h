#ifndef STUCTZ_BINARY_HEAP_H
#define STUCTZ_BINARY_HEAP_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include "vec.h"

template<typename T, typename Compare = std::less<>>
class BinaryHeap {
    Vec<T> data;
    Compare cmp;

    static constexpr std::size_t parent(const std::size_t idx) {
        return (idx - 1) / 2;
    }

    static constexpr std::size_t left(const std::size_t idx) {
        return (2 * idx) + 1;
    }

    static constexpr std::size_t right(const std::size_t idx) {
        return (2 * idx) + 2;
    }

    void bubble_up(const std::size_t idx) {
        if (idx == 0)
            return;

        const std::size_t paren = parent(idx);

        if (cmp(data[idx], data[paren])) {
            std::swap(data[idx], data[paren]);
            bubble_up(paren);
        }
    }

    void bubble_down(const std::size_t idx) {
        const std::size_t le = left(idx);
        const std::size_t ri = right(idx);

        if (le < data.size() && cmp(data[le], data[idx]) &&
            (ri >= data.size() || cmp(data[le], data[ri]))) {
            std::swap(data[le], data[idx]);
            bubble_down(le);
        } else if (ri < data.size() && cmp(data[ri], data[idx])) {
            std::swap(data[ri], data[idx]);
            bubble_down(ri);
        }
    }

public:
    using iterator = typename Vec<T>::iterator;
    using const_iterator = typename Vec<T>::const_iterator;
    using reverse_iterator = typename Vec<T>::reverse_iterator;
    using const_reverse_iterator = typename Vec<T>::const_reverse_iterator;

    BinaryHeap() = default;

    [[nodiscard]] constexpr std::size_t size() const {
        return data.size();
    }

    [[nodiscard]] constexpr bool is_empty() const {
        return data.is_empty();
    }

    void push(T value) {
        data.push(std::move(value));
        bubble_up(data.size() - 1);
    }

    T pop() {
        if (data.is_empty())
            throw std::out_of_range("Heap is empty");

        std::swap(data.first(), data.last());
        const T value = data.pop();
        bubble_down(0);

        return value;
    }

    T& peek() {
        if (data.is_empty())
            throw std::out_of_range("Heap is empty");

        return data.first();
    }

    const T& peek() const {
        if (data.is_empty())
            throw std::out_of_range("Heap is empty");

        return data.first();
    }

    void clear() {
        data.clear();
    }

    [[nodiscard]] constexpr iterator begin() {
        return data.begin();
    }

    [[nodiscard]] constexpr iterator end() {
        return data.end();
    }

    [[nodiscard]] constexpr const_iterator begin() const {
        return data.begin();
    }

    [[nodiscard]] constexpr const_iterator end() const {
        return data.end();
    }

    [[nodiscard]] constexpr reverse_iterator rbegin() {
        return data.rbegin();
    }

    [[nodiscard]] constexpr reverse_iterator rend() {
        return data.rend();
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const {
        return data.rbegin();
    }

    [[nodiscard]] constexpr const_reverse_iterator rend() const {
        return data.rend();
    }
};

#endif

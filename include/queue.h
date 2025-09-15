#ifndef STRUCTZ_QUEUE_H
#define STRUCTZ_QUEUE_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include "vec.h"

template<typename T>
class Queue {
    Vec<T> data;

    std::size_t head = 0;
    std::size_t tail = 0;

public:
    explicit Queue(const std::size_t initial_capacity = 8)
        : data(initial_capacity) {}

    void enqueue(T value) {
        std::size_t new_tail = (tail + 1) % data.size();

        if (new_tail == head) {
            // Grow queue
            const std::size_t prev_size = size();
            Vec<T> new_data(2 * prev_size);

            for (std::size_t i = 0; i < prev_size; ++i)
                new_data[i] = std::move(data[(head + i) % data.size()]);

            data = std::move(new_data);
            head = 0;
            tail = prev_size;
            new_tail = tail + 1;
        }

        data[tail] = std::move(value);
        tail = new_tail;
    }

    T dequeue() {
        if (is_empty())
            throw std::out_of_range("queue is empty");

        const T value = std::move(data[head]);
        head = (head + 1) % data.size();

        return value;
    }

    T& peek() {
        if (is_empty())
            throw std::out_of_range("queue is empty");

        return data[head];
    }

    const T& peek() const {
        if (is_empty())
            throw std::out_of_range("queue is empty");

        return data[head];
    }

    [[nodiscard]] std::size_t size() const {
        return (tail + data.size() - head) % data.size();
    }

    [[nodiscard]] bool is_empty() const {
        return head == tail;
    }
};

#endif

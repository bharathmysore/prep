#ifndef PREP_CIRCULAR_QUEUE_H
#define PREP_CIRCULAR_QUEUE_H

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

// Fixed-capacity, single-threaded FIFO. Full queues reject new values.
class CircularQueue {
public:
    explicit CircularQueue(std::size_t capacity) : buffer_(capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("capacity must be positive");
        }
    }

    // Keep queue instances in place; copying/moving is outside this exercise.
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;
    CircularQueue(CircularQueue&&) = delete;
    CircularQueue& operator=(CircularQueue&&) = delete;

    bool enqueue(int value) noexcept {
        if (full()) return false;

        buffer_[tail_] = value;
        tail_ = next(tail_);
        ++count_;
        return true;
    }

    std::optional<int> dequeue() noexcept {
        if (empty()) return std::nullopt;

        const int value = buffer_[head_];
        head_ = next(head_);
        --count_;
        return value;
    }

    std::optional<int> front() const noexcept {
        if (empty()) return std::nullopt;
        return buffer_[head_];
    }

    bool empty() const noexcept { return count_ == 0; }
    bool full() const noexcept { return count_ == capacity(); }
    std::size_t size() const noexcept { return count_; }
    std::size_t capacity() const noexcept { return buffer_.size(); }

private:
    std::size_t next(std::size_t index) const noexcept {
        return index == capacity() - 1 ? 0 : index + 1;
    }

    std::vector<int> buffer_;
    std::size_t head_ = 0;   // Oldest live element, when nonempty.
    std::size_t tail_ = 0;   // Next insertion slot, when nonfull.
    std::size_t count_ = 0;
};

#endif

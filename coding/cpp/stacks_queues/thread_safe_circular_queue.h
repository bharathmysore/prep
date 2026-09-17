#ifndef PREP_THREAD_SAFE_CIRCULAR_QUEUE_H
#define PREP_THREAD_SAFE_CIRCULAR_QUEUE_H

#include "circular_queue.h"

#include <mutex>

// Fixed-capacity MPMC FIFO. May wait for the mutex, never for space or data.
// Stop/join all users before destroying the queue. Observers are snapshots.
class ThreadSafeCircularQueue {
public:
    explicit ThreadSafeCircularQueue(std::size_t capacity) : queue_(capacity) {}

    ThreadSafeCircularQueue(const ThreadSafeCircularQueue&) = delete;
    ThreadSafeCircularQueue& operator=(const ThreadSafeCircularQueue&) = delete;
    ThreadSafeCircularQueue(ThreadSafeCircularQueue&&) = delete;
    ThreadSafeCircularQueue& operator=(ThreadSafeCircularQueue&&) = delete;

    bool enqueue(int value) {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.enqueue(value);
    }

    std::optional<int> dequeue() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.dequeue();
    }

    std::optional<int> front() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.front();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool full() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.full();
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    std::size_t capacity() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.capacity();
    }

private:
    CircularQueue queue_;
    mutable std::mutex mutex_;
};

#endif

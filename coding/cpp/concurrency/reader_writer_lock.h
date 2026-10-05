#pragma once

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <mutex>

// Nonrecursive, writer-preference, C++17 reader-writer lock.
// New readers cannot bypass an enrolled writer. Neither FIFO ordering nor
// starvation freedom is promised; continuous writers can starve readers.
// No try/timed locking, cancellation, upgrade, or downgrade operations.
// Unlock must match an acquisition by the same thread. Every caller must finish
// (including holders and waiters) before destruction; there is no shutdown API.
class ReaderWriterLock {
public:
    ReaderWriterLock() = default;
    ReaderWriterLock(const ReaderWriterLock&) = delete;
    ReaderWriterLock& operator=(const ReaderWriterLock&) = delete;
    ReaderWriterLock(ReaderWriterLock&&) = delete;
    ReaderWriterLock& operator=(ReaderWriterLock&&) = delete;

    void lock_shared() {
        std::unique_lock<std::mutex> guard(mutex_);
        readers_.wait(guard, [this]() noexcept {
            return !writer_active_ && waiting_writers_ == 0;
        });
        ++active_readers_;
    }

    void unlock_shared() noexcept {
        std::lock_guard<std::mutex> guard(mutex_);
        assert(active_readers_ > 0 && !writer_active_);
        --active_readers_;
        if (active_readers_ == 0 && waiting_writers_ > 0) {
            writers_.notify_one();
        }
    }

    void lock() {
        std::unique_lock<std::mutex> guard(mutex_);
        ++waiting_writers_; // Enrollment closes admission to new readers.
        // Untimed condition_variable::wait only propagates predicate exceptions;
        // our predicate is noexcept. Timed/cancellable variants need cleanup.
        writers_.wait(guard, [this]() noexcept {
            return !writer_active_ && active_readers_ == 0;
        });
        --waiting_writers_;
        writer_active_ = true;
    }

    void unlock() noexcept {
        std::lock_guard<std::mutex> guard(mutex_);
        assert(writer_active_ && active_readers_ == 0);
        writer_active_ = false;
        if (waiting_writers_ > 0) {
            writers_.notify_one();
        } else {
            readers_.notify_all(); // Every eligible reader may proceed.
        }
    }

private:
    friend struct ReaderWriterLockTestAccess;
    std::mutex mutex_;
    std::condition_variable readers_;
    std::condition_variable writers_;
    std::size_t active_readers_ = 0;
    std::size_t waiting_writers_ = 0;
    bool writer_active_ = false;
};

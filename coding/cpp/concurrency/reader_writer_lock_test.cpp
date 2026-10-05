#include "reader_writer_lock.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

// Fail the test process rather than unwinding into a blocked future destructor.
// Checks remain enabled in optimized NDEBUG builds.
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAIL: " #condition " at line " << __LINE__ << '\n'; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)

// White-box synchronization only: prove writer enrollment, not just thread start.
// No test-only observer methods are added to the lock's public API.
struct ReaderWriterLockTestAccess {
    static std::size_t waiting_writers(ReaderWriterLock& lock) {
        std::lock_guard<std::mutex> guard(lock.mutex_);
        return lock.waiting_writers_;
    }
};

static_assert(!std::is_copy_constructible_v<ReaderWriterLock>);
static_assert(!std::is_copy_assignable_v<ReaderWriterLock>);
static_assert(!std::is_move_constructible_v<ReaderWriterLock>);
static_assert(!std::is_move_assignable_v<ReaderWriterLock>);

namespace {
using namespace std::chrono_literals;

// Independent of the tested mutex: a broken lock must not hang the test runner.
class Watchdog {
public:
    Watchdog() : worker_([this] {
        std::unique_lock<std::mutex> guard(mutex_);
        if (!changed_.wait_for(guard, 30s, [this] { return done_; })) {
            std::cerr << "FAIL: test exceeded 30-second watchdog\n";
            std::_Exit(EXIT_FAILURE);
        }
    }) {}

    ~Watchdog() {
        {
            std::lock_guard<std::mutex> guard(mutex_);
            done_ = true;
        }
        changed_.notify_one();
        worker_.join();
    }

private:
    std::mutex mutex_;
    std::condition_variable changed_;
    bool done_ = false;
    std::thread worker_;
};

template <class Predicate>
void wait_until(Predicate ready) {
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    while (!ready()) {
        CHECK(std::chrono::steady_clock::now() < deadline);
        std::this_thread::yield();
    }
}

template <class T>
void ready(std::future<T>& result) {
    CHECK(result.wait_for(5s) == std::future_status::ready);
}

// Catches accidentally implementing shared access as a plain exclusive mutex.
void readers_overlap() {
    ReaderWriterLock lock;
    std::shared_lock<ReaderWriterLock> first(lock);
    auto second = std::async(std::launch::async, [&] {
        std::shared_lock<ReaderWriterLock> guard(lock);
        return 42;
    });
    ready(second); // First reader still owns its lock.
    CHECK(second.get() == 42);
}

// Catches readers ignoring writer_active or missing the writer-to-reader wakeup.
void writer_blocks_reader() {
    ReaderWriterLock lock;
    std::unique_lock<ReaderWriterLock> writer(lock);
    std::promise<void> started;
    auto started_future = started.get_future();
    auto reader = std::async(std::launch::async, [&] {
        started.set_value();
        std::shared_lock<ReaderWriterLock> guard(lock);
    });
    ready(started_future);
    CHECK(reader.wait_for(50ms) == std::future_status::timeout);
    writer.unlock();
    ready(reader);
    reader.get();
}

// Catches writers ignoring writer_active or a missing writer-to-writer wakeup.
void writer_blocks_writer() {
    ReaderWriterLock lock;
    std::unique_lock<ReaderWriterLock> first(lock);
    auto second = std::async(std::launch::async, [&] {
        std::unique_lock<ReaderWriterLock> guard(lock);
    });
    wait_until([&] { return ReaderWriterLockTestAccess::waiting_writers(lock) == 1; });
    CHECK(second.wait_for(50ms) == std::future_status::timeout);
    first.unlock();
    ready(second);
    second.get();
}

// Catches admitting a writer before the last reader leaves, or failing to wake it.
void writer_waits_for_last_reader() {
    ReaderWriterLock lock;
    std::shared_lock<ReaderWriterLock> first(lock);
    std::promise<void> second_entered;
    auto entered = second_entered.get_future();
    std::promise<void> release_second;
    auto release = release_second.get_future();
    auto second = std::async(std::launch::async, [&] {
        std::shared_lock<ReaderWriterLock> guard(lock);
        second_entered.set_value();
        release.wait();
    });
    ready(entered);
    auto writer = std::async(std::launch::async, [&] {
        std::unique_lock<ReaderWriterLock> guard(lock);
    });
    wait_until([&] { return ReaderWriterLockTestAccess::waiting_writers(lock) == 1; });
    first.unlock();
    CHECK(writer.wait_for(50ms) == std::future_status::timeout);
    release_second.set_value();
    ready(second);
    second.get();
    ready(writer);
    writer.get();
}

// Catches removal of the waiting-writer reader gate. No FIFO assertion for writers.
void queued_writers_precede_late_reader() {
    ReaderWriterLock lock;
    std::shared_lock<ReaderWriterLock> first_reader(lock);
    std::promise<void> release_writers;
    auto release = release_writers.get_future().share();
    std::atomic<int> order{0};
    const auto write = [&] {
        std::unique_lock<ReaderWriterLock> guard(lock);
        const int position = order.fetch_add(1, std::memory_order_relaxed) + 1;
        release.wait();
        return position;
    };
    auto writer_one = std::async(std::launch::async, write);
    auto writer_two = std::async(std::launch::async, write);
    wait_until([&] { return ReaderWriterLockTestAccess::waiting_writers(lock) == 2; });

    std::promise<void> reader_started;
    auto started = reader_started.get_future();
    auto late_reader = std::async(std::launch::async, [&] {
        reader_started.set_value();
        std::shared_lock<ReaderWriterLock> guard(lock);
        return order.fetch_add(1, std::memory_order_relaxed) + 1;
    });
    ready(started);
    // A bounded negative observation, not a proof the reader has been scheduled.
    CHECK(late_reader.wait_for(50ms) == std::future_status::timeout);
    first_reader.unlock();
    wait_until([&] { return order.load(std::memory_order_relaxed) > 0; });
    CHECK(late_reader.wait_for(50ms) == std::future_status::timeout);
    release_writers.set_value();
    ready(writer_one);
    ready(writer_two);
    ready(late_reader);
    const int first = writer_one.get();
    const int second = writer_two.get();
    CHECK((first == 1 && second == 2) || (first == 2 && second == 1));
    CHECK(late_reader.get() == 3);
}

// Catches notifying only one reader: every reader must enter before any exits.
void writer_releases_reader_batch() {
    ReaderWriterLock lock;
    std::unique_lock<ReaderWriterLock> writer(lock);
    std::promise<void> release_readers;
    auto release = release_readers.get_future().share();
    std::atomic<int> started{0};
    std::atomic<int> entered{0};
    std::vector<std::future<void>> readers;
    for (int i = 0; i < 4; ++i) {
        readers.push_back(std::async(std::launch::async, [&] {
            started.fetch_add(1, std::memory_order_relaxed);
            std::shared_lock<ReaderWriterLock> guard(lock);
            entered.fetch_add(1, std::memory_order_relaxed);
            release.wait();
        }));
    }
    wait_until([&] { return started.load(std::memory_order_relaxed) == 4; });
    for (auto& reader : readers) {
        CHECK(reader.wait_for(25ms) == std::future_status::timeout);
    }
    CHECK(entered.load(std::memory_order_relaxed) == 0);
    writer.unlock();
    wait_until([&] { return entered.load(std::memory_order_relaxed) == 4; });
    release_readers.set_value();
    for (auto& reader : readers) {
        ready(reader);
        reader.get();
    }
}

struct CriticalSectionError {};

// Catches a write ownership flag left set after an RAII guard is destroyed.
void exclusive_raii_releases_on_exception() {
    ReaderWriterLock lock;
    try {
        std::unique_lock<ReaderWriterLock> guard(lock);
        throw CriticalSectionError{};
    } catch (const CriticalSectionError&) {
    }
    auto reader = std::async(std::launch::async, [&] {
        std::shared_lock<ReaderWriterLock> guard(lock);
    });
    ready(reader);
    reader.get();
}

// Catches a reader count left nonzero after an RAII guard is destroyed.
void shared_raii_releases_on_exception() {
    ReaderWriterLock lock;
    try {
        std::shared_lock<ReaderWriterLock> guard(lock);
        throw CriticalSectionError{};
    } catch (const CriticalSectionError&) {
    }
    auto writer = std::async(std::launch::async, [&] {
        std::unique_lock<ReaderWriterLock> guard(lock);
    });
    ready(writer);
    writer.get();
}

// Catches exclusion/count/wakeup defects under contention. Relaxed diagnostic
// atomics must not establish extra happens-before edges for the ordinary payload.
void mixed_contention_and_visibility() {
    ReaderWriterLock lock;
    std::promise<void> start_workers;
    auto start = start_workers.get_future().share();
    std::atomic<int> occupants{0}; // -1 writer; >=0 reader count.
    int value = 0;
    int mirror = 0;
    std::vector<std::future<void>> workers;
    for (int w = 0; w < 3; ++w) {
        workers.push_back(std::async(std::launch::async, [&] {
            start.wait();
            for (int i = 0; i < 2000; ++i) {
                std::unique_lock<ReaderWriterLock> guard(lock);
                CHECK(occupants.exchange(-1, std::memory_order_relaxed) == 0);
                ++value;
                std::this_thread::yield();
                mirror = value;
                CHECK(occupants.exchange(0, std::memory_order_relaxed) == -1);
            }
        }));
    }
    for (int r = 0; r < 4; ++r) {
        workers.push_back(std::async(std::launch::async, [&] {
            start.wait();
            for (int i = 0; i < 2000; ++i) {
                std::shared_lock<ReaderWriterLock> guard(lock);
                CHECK(occupants.fetch_add(1, std::memory_order_relaxed) >= 0);
                CHECK(value == mirror);
                std::this_thread::yield();
                CHECK(occupants.fetch_sub(1, std::memory_order_relaxed) > 0);
            }
        }));
    }
    start_workers.set_value();
    for (auto& worker : workers) {
        ready(worker);
        worker.get();
    }
    CHECK(value == 6000);
    CHECK(mirror == 6000);
    CHECK(occupants.load(std::memory_order_relaxed) == 0);
}
} // namespace

int main(int argc, char** argv) {
    const struct { const char* name; void (*run)(); } tests[] = {
        {"readers_overlap", readers_overlap},
        {"writer_blocks_reader", writer_blocks_reader},
        {"writer_blocks_writer", writer_blocks_writer},
        {"writer_waits_for_last_reader", writer_waits_for_last_reader},
        {"queued_writers_precede_late_reader", queued_writers_precede_late_reader},
        {"writer_releases_reader_batch", writer_releases_reader_batch},
        {"exclusive_raii_releases_on_exception", exclusive_raii_releases_on_exception},
        {"shared_raii_releases_on_exception", shared_raii_releases_on_exception},
        {"mixed_contention_and_visibility", mixed_contention_and_visibility},
    };
    int passed = 0;
    int failed = 0;
    for (const auto& test : tests) {
        if (argc > 1 && std::string(argv[1]) != test.name) continue;
        Watchdog watchdog;
        try {
            test.run();
            ++passed;
            std::cout << "PASS " << test.name << '\n';
        } catch (const std::exception& error) {
            ++failed;
            std::cerr << "FAIL " << test.name << ": " << error.what() << '\n';
        }
    }
    CHECK(passed + failed > 0);
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

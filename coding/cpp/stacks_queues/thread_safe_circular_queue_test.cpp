#include "thread_safe_circular_queue.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

// Remains active in optimized NDEBUG builds.
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("check failed: " #condition); \
        } \
    } while (false)

static_assert(!std::is_copy_constructible_v<ThreadSafeCircularQueue>);
static_assert(!std::is_copy_assignable_v<ThreadSafeCircularQueue>);
static_assert(!std::is_move_constructible_v<ThreadSafeCircularQueue>);
static_assert(!std::is_move_assignable_v<ThreadSafeCircularQueue>);

namespace {

void zero_capacity_rejected() {
    bool rejected = false;
    try {
        ThreadSafeCircularQueue queue(0);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    CHECK(rejected);
}

// Detects incorrect initial state, empty underflow, and const observer wiring.
void empty_and_const_observers() {
    ThreadSafeCircularQueue queue(3);
    const auto& view = queue;
    CHECK(view.capacity() == 3);
    CHECK(view.size() == 0);
    CHECK(view.empty());
    CHECK(!view.full());
    CHECK(!view.front());
    CHECK(!queue.dequeue());
    CHECK(view.size() == 0);
}

// Detects lost capacity, destructive peek, overflow overwrite, and bad wrap.
void full_rejection_and_wrapped_fifo() {
    ThreadSafeCircularQueue queue(3);
    CHECK(queue.enqueue(10));
    CHECK(queue.enqueue(20));
    CHECK(queue.enqueue(30));
    CHECK(queue.full());
    CHECK(!queue.empty());
    CHECK(queue.size() == 3);
    CHECK(!queue.enqueue(99));
    const auto& view = queue;
    CHECK(view.front() == 10);
    CHECK(view.front() == 10);
    CHECK(queue.dequeue() == 10);
    CHECK(queue.enqueue(40));
    CHECK(queue.full());
    CHECK(queue.dequeue() == 20);
    CHECK(queue.dequeue() == 30);
    CHECK(queue.dequeue() == 40);
    CHECK(queue.empty());
    CHECK(!queue.dequeue());
}

void capacity_one_reuse() {
    ThreadSafeCircularQueue queue(1);
    for (int i = 0; i < 100; ++i) {
        CHECK(queue.enqueue(i));
        CHECK(queue.full());
        CHECK(!queue.enqueue(-1));
        CHECK(queue.front() == i);
        CHECK(queue.dequeue() == i);
        CHECK(queue.empty());
        CHECK(!queue.dequeue());
    }
}

// Detects sentinel collisions and borrowed results pointing into recycled slots.
void integer_values_and_result_ownership() {
    std::optional<int> saved;
    {
        ThreadSafeCircularQueue queue(6);
        const int values[] = {0, -1, -1, std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max(), 0};
        for (int value : values) CHECK(queue.enqueue(value));
        saved = queue.front();
        for (int value : values) CHECK(queue.dequeue() == value);
        CHECK(queue.enqueue(77));
        CHECK(saved == 0);
    }
    CHECK(saved == 0);
}

// Joins even if creating a later worker throws. No detached users of the queue.
class ThreadGroup {
public:
    ThreadGroup(std::atomic<bool>& stop, std::atomic<bool>& failed,
                std::size_t count) : stop_(stop), failed_(failed) {
        threads_.reserve(count);
    }

    template <class Function>
    void start(Function function) {
        threads_.emplace_back([this, function = std::move(function)]() {
            try {
                function();
            } catch (...) {
                failed_.store(true);
                stop_.store(true);
            }
        });
    }

    void join() {
        for (auto& thread : threads_) {
            if (thread.joinable()) thread.join();
        }
    }

    ~ThreadGroup() {
        stop_.store(true);
        join();
    }

private:
    std::atomic<bool>& stop_;
    std::atomic<bool>& failed_;
    std::vector<std::thread> threads_;
};

// No external lock serializes queue calls. Each consumer records in its own
// vector; validation happens after join, never by post-return log ordering.
void concurrent_transfer(std::size_t capacity, int producers, int consumers,
                         int per_producer) {
    ThreadSafeCircularQueue queue(capacity);
    const int total = producers * per_producer;
    std::atomic<bool> start{false};
    std::atomic<bool> stop{false};
    std::atomic<bool> failed{false};
    std::atomic<int> completed_producers{0};
    std::atomic<int> received{0};
    std::vector<std::vector<int>> results(static_cast<std::size_t>(consumers));
    const auto deadline = std::chrono::steady_clock::now()
                        + std::chrono::seconds(15);

    const auto keep_running = [&]() {
        if (stop.load()) return false;
        if (std::chrono::steady_clock::now() >= deadline) {
            failed.store(true);
            stop.store(true);
            return false;
        }
        return true;
    };
    const auto await_start = [&]() {
        while (!start.load()) {
            if (!keep_running()) return false;
            std::this_thread::yield();
        }
        return keep_running();
    };

    ThreadGroup workers(stop, failed,
                        static_cast<std::size_t>(producers + consumers + 1));
    for (int producer = 0; producer < producers; ++producer) {
        workers.start([&, producer]() {
            if (!await_start()) return;
            for (int sequence = 0; sequence < per_producer; ++sequence) {
                const int value = producer * per_producer + sequence;
                for (;;) {
                    if (!keep_running()) return;
                    if (queue.enqueue(value)) break;
                    std::this_thread::yield();
                }
            }
            completed_producers.fetch_add(1);
        });
    }
    for (int consumer = 0; consumer < consumers; ++consumer) {
        workers.start([&, consumer]() {
            if (!await_start()) return;
            auto& local = results[static_cast<std::size_t>(consumer)];
            while (received.load() < total && keep_running()) {
                if (const auto value = queue.dequeue()) {
                    CHECK(*value >= 0 && *value < total);
                    local.push_back(*value);
                    received.fetch_add(1);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }
    workers.start([&]() {
        if (!await_start()) return;
        const auto& view = queue;
        do {
            // Only check each snapshot independently: separate calls are not
            // an atomic compound observation of the queue.
            CHECK(view.capacity() == capacity);
            CHECK(view.size() <= capacity);
            if (const auto value = view.front()) {
                CHECK(*value >= 0 && *value < total);
            }
            (void)view.empty();
            (void)view.full();
            std::this_thread::yield();
        } while (keep_running() &&
                 (completed_producers.load() < producers || received.load() < total));
    });

    start.store(true);
    workers.join();
    CHECK(!failed.load());
    CHECK(completed_producers.load() == producers);
    CHECK(received.load() == total);
    CHECK(queue.empty());
    CHECK(queue.size() == 0);
    CHECK(!queue.dequeue());

    std::vector<int> seen(static_cast<std::size_t>(total), 0);
    std::vector<int> next_sequence(static_cast<std::size_t>(producers), 0);
    for (const auto& local : results) {
        for (int value : local) {
            CHECK(++seen[static_cast<std::size_t>(value)] == 1);
            if (consumers == 1) {
                const auto producer = static_cast<std::size_t>(value / per_producer);
                CHECK(value % per_producer == next_sequence[producer]++);
            }
        }
    }
    for (int count : seen) CHECK(count == 1);
}

// Catches reorder/loss across wraps, including the single-slot boundary.
void single_producer_fifo() {
    concurrent_transfer(1, 1, 1, 10000);
    concurrent_transfer(7, 1, 1, 10000);
}

// One consumer can verify each producer's sequential enqueue order.
void multiple_producer_order() {
    concurrent_transfer(3, 4, 1, 3000);
}

// Catches duplication/loss under competing enqueue and dequeue operations.
void multiple_producers_and_consumers() {
    for (std::size_t capacity : {1, 3, 64}) {
        concurrent_transfer(capacity, 4, 4, 2000);
    }
}

}  // namespace

int main() {
    struct Test { const char* name; void (*run)(); };
    const Test sequential[] = {
        {"zero capacity", zero_capacity_rejected},
        {"empty and const observers", empty_and_const_observers},
        {"full rejection and wrapped FIFO", full_rejection_and_wrapped_fifo},
        {"capacity one reuse", capacity_one_reuse},
        {"integer values and result ownership", integer_values_and_result_ownership},
    };
    const Test concurrent[] = {
        {"SPSC exact FIFO", single_producer_fifo},
        {"MPSC per-producer FIFO", multiple_producer_order},
        {"MPMC no loss or duplication", multiple_producers_and_consumers},
    };
    int passed = 0;
    int failed = 0;
    const auto run = [&](const Test& test) {
        try {
            test.run();
            ++passed;
            std::cout << "PASS: " << test.name << '\n';
        } catch (const std::exception& error) {
            ++failed;
            std::cerr << "FAIL: " << test.name << ": " << error.what() << '\n';
        }
    };
    for (const auto& test : sequential) run(test);
    // Fail fast for a broken/scaffold API before starting retry-based workers.
    if (failed == 0) {
        for (const auto& test : concurrent) run(test);
    }
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

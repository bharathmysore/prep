#include "circular_queue.h"

#include <deque>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>

// Check observable behavior even when compiled with NDEBUG.
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("check failed: " #condition); \
        } \
    } while (false)

static_assert(!std::is_copy_constructible_v<CircularQueue>);
static_assert(!std::is_copy_assignable_v<CircularQueue>);
static_assert(!std::is_move_constructible_v<CircularQueue>);
static_assert(!std::is_move_assignable_v<CircularQueue>);

namespace {

// Catches missing zero-capacity validation.
void zero_capacity_rejected() {
    bool rejected = false;
    try {
        CircularQueue queue(0);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    CHECK(rejected);
}

// Catches incorrect empty state and underflow on failed removals.
void empty_operations_preserve_state() {
    CircularQueue queue(3);
    CHECK(queue.empty());
    CHECK(!queue.full());
    CHECK(queue.size() == 0);
    CHECK(queue.capacity() == 3);
    for (int i = 0; i < 3; ++i) {
        CHECK(!queue.front().has_value());
        CHECK(!queue.dequeue().has_value());
        CHECK(queue.empty());
        CHECK(queue.size() == 0);
    }
    CHECK(queue.enqueue(42));
    CHECK(queue.dequeue() == 42);
}

// Catches advancing the head during peek, including through a const view.
void front_does_not_remove() {
    CircularQueue queue(2);
    CHECK(queue.enqueue(10));
    const CircularQueue& view = queue;
    CHECK(view.front() == 10);
    CHECK(view.front() == 10);
    CHECK(view.size() == 1);
    CHECK(queue.dequeue() == 10);
    CHECK(queue.empty());
}

// Catches reserving a slot accidentally or confusing head==tail with empty.
void all_slots_usable_fifo() {
    CircularQueue queue(3);
    CHECK(queue.enqueue(10));
    CHECK(queue.enqueue(20));
    CHECK(queue.enqueue(30));
    CHECK(queue.full());
    CHECK(!queue.empty());
    CHECK(queue.size() == 3);
    CHECK(queue.dequeue() == 10);
    CHECK(queue.dequeue() == 20);
    CHECK(queue.dequeue() == 30);
    CHECK(queue.empty());
}

// Catches an overflow insertion overwriting or changing existing contents.
void full_rejection_preserves_fifo() {
    CircularQueue queue(2);
    CHECK(queue.enqueue(7));
    CHECK(queue.enqueue(8));
    CHECK(!queue.enqueue(99));
    CHECK(!queue.enqueue(100));
    CHECK(queue.size() == 2);
    CHECK(queue.front() == 7);
    CHECK(queue.dequeue() == 7);
    CHECK(queue.dequeue() == 8);
}

// Catches missing/incorrect wrap and using a power-of-two mask for capacity 3.
void wrapped_fifo() {
    CircularQueue queue(3);
    CHECK(queue.enqueue(10));
    CHECK(queue.enqueue(20));
    CHECK(queue.enqueue(30));
    CHECK(queue.dequeue() == 10);
    CHECK(queue.enqueue(40));
    CHECK(queue.full());
    CHECK(!queue.enqueue(50));
    CHECK(queue.dequeue() == 20);
    CHECK(queue.dequeue() == 30);
    CHECK(queue.dequeue() == 40);
    CHECK(queue.enqueue(50));
    CHECK(queue.dequeue() == 50);
    CHECK(queue.empty());
}

// Catches incorrect wrap at the smallest supported capacity.
void capacity_one_repeated_reuse() {
    CircularQueue queue(1);
    for (int value = 0; value < 100; ++value) {
        CHECK(queue.enqueue(value));
        CHECK(queue.full());
        CHECK(queue.front() == value);
        CHECK(!queue.enqueue(-1));
        CHECK(queue.dequeue() == value);
        CHECK(queue.empty());
        CHECK(!queue.dequeue().has_value());
    }
}

// Catches sentinel-based empty detection that loses valid int values.
void every_int_value_is_data() {
    CircularQueue queue(6);
    const int values[] = {0, -1, -1, std::numeric_limits<int>::min(),
                          std::numeric_limits<int>::max(), 0};
    for (int value : values) {
        CHECK(queue.enqueue(value));
    }
    for (int value : values) {
        CHECK(queue.dequeue() == value);
    }
    CHECK(queue.empty());
}

// Catches returning a borrowed slot instead of an independent value.
void returned_value_survives_reuse_and_destruction() {
    std::optional<int> saved;
    {
        CircularQueue queue(1);
        CHECK(queue.enqueue(55));
        saved = queue.front();
        CHECK(queue.dequeue() == 55);
        CHECK(queue.enqueue(66));
        CHECK(saved == 55);
    }
    CHECK(saved == 55);
}

// Independent deque model catches count/index drift across many mixed operations.
void matches_deque_model() {
    std::mt19937 random(20260917);
    const std::size_t capacities[] = {1, 2, 3, 7, 16};
    for (std::size_t capacity : capacities) {
        CircularQueue queue(capacity);
        std::deque<int> model;
        for (int step = 0; step < 2000; ++step) {
            const auto operation = random() % 3;
            if (operation == 0) {
                const int value = static_cast<int>(random() % 2001) - 1000;
                const bool accepted = model.size() < capacity;
                CHECK(queue.enqueue(value) == accepted);
                if (accepted) model.push_back(value);
            } else if (operation == 1) {
                const std::optional<int> expected = model.empty()
                    ? std::nullopt : std::optional<int>(model.front());
                CHECK(queue.dequeue() == expected);
                if (!model.empty()) model.pop_front();
            } else {
                const std::optional<int> expected = model.empty()
                    ? std::nullopt : std::optional<int>(model.front());
                CHECK(queue.front() == expected);
            }
            CHECK(queue.size() == model.size());
            CHECK(queue.empty() == model.empty());
            CHECK(queue.full() == (model.size() == capacity));
            CHECK(queue.capacity() == capacity);
        }
        while (!model.empty()) {
            CHECK(queue.dequeue() == model.front());
            model.pop_front();
        }
        CHECK(queue.empty());
        CHECK(!queue.dequeue().has_value());
    }
}

}  // namespace

int main() {
    struct Test { const char* name; void (*run)(); };
    const Test tests[] = {
        {"zero capacity", zero_capacity_rejected},
        {"empty operations", empty_operations_preserve_state},
        {"non-destructive front", front_does_not_remove},
        {"all slots and FIFO", all_slots_usable_fifo},
        {"full rejection", full_rejection_preserves_fifo},
        {"wrapped FIFO", wrapped_fifo},
        {"capacity one", capacity_one_repeated_reuse},
        {"all integer values", every_int_value_is_data},
        {"returned value lifetime", returned_value_survives_reuse_and_destruction},
        {"deque reference model", matches_deque_model},
    };
    int passed = 0;
    int failed = 0;
    for (const auto& test : tests) {
        try {
            test.run();
            ++passed;
            std::cout << "PASS: " << test.name << '\n';
        } catch (const std::exception& error) {
            ++failed;
            std::cerr << "FAIL: " << test.name << ": " << error.what() << '\n';
        }
    }
    std::cout << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

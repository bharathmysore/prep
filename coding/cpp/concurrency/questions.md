# Concurrency Coding Questions

Solve each question in C++ with explicit shared state, synchronization, safety invariants, liveness expectations, and shutdown behavior.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Implement a bounded blocking queue with multiple producers and consumers.
   - Expected pattern: mutex, condition variables, circular buffer.
   - Pattern tags: `bounded-blocking-queue`, `condition-variable`.
   - Solution: [Bounded Blocking Queue](./solutions.md#1-bounded-blocking-queue).
   - Complexity target: time `O(1)` per operation under lock, space `O(capacity)`.

2. Implement a thread pool that supports task submission and graceful shutdown.
   - Expected pattern: worker threads plus guarded task queue.
   - Pattern tags: `thread-pool`, `work-queue`.
   - Solution: [Thread Pool](./solutions.md#2-thread-pool).
   - Complexity target: submit `O(1)` under lock, space `O(queued tasks)`.

<a id="3-concurrent-token-bucket"></a>
3. Implement a token bucket rate limiter safe for concurrent callers.
   - Expected pattern: mutex-protected lazy refill or carefully justified atomics.
   - Pattern tags: `token-bucket`, `rate-limiter`.
   - Solution: [Concurrent Token Bucket](./solutions.md#3-concurrent-token-bucket).
   - Complexity target: time `O(1)` per request, space `O(1)`.

4. Implement single-flight duplicate suppression so only one concurrent caller computes a value for a key.
   - Expected pattern: map from key to shared future or condition state.
   - Pattern tags: `single-flight`, `future`.
   - Solution: [Single-Flight Duplicate Suppression](./solutions.md#4-single-flight-duplicate-suppression).
   - Complexity target: average coordination time `O(1)`, space `O(inflight keys)`.

5. Implement a readers-writer cache for frequent reads and rare writes.
   - Expected pattern: `std::shared_mutex` or immutable snapshot swap.
   - Pattern tags: `readers-writer-lock`, `shared-mutex`.
   - Solution: [Readers-Writer Cache](./solutions.md#5-readers-writer-cache).
   - Complexity target: average access `O(1)` plus lock cost, space `O(n)` or `O(active snapshots)`.

6. Implement a countdown latch or reusable barrier.
   - Expected pattern: mutex, condition variable, count, and generation.
   - Pattern tags: `barrier`, `condition-variable`.
   - Solution: [Reusable Barrier](./solutions.md#6-reusable-barrier).
   - Complexity target: arrive/wait `O(1)` plus wake cost, space `O(1)`.

7. Code a deadlock-free transfer between two account objects.
   - Expected pattern: global lock ordering or `std::scoped_lock`.
   - Pattern tags: `deadlock-prevention`, `lock-ordering`.
   - Solution: [Deadlock-Free Account Transfer](./solutions.md#7-deadlock-free-account-transfer).
   - Complexity target: time `O(1)` per transfer, space `O(1)`.

8. Implement a producer-consumer pipeline with cancellation and backpressure.
   - Expected pattern: bounded queues plus shutdown propagation.
   - Pattern tags: `producer-consumer`, `backpressure`, `cancellation`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: time `O(items + coordination)`, space bounded by queue capacities.

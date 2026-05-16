# Concurrency Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Bounded Blocking Queue

* **Question**: Implement a bounded blocking queue with multiple producers and consumers.
* **Solution**: [Bounded Blocking Queue](./solutions.md#1-bounded-blocking-queue).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Producer blocks when full | capacity 1; producer pushes two items before consumer pops | Second push waits until capacity is available. |
| Consumer blocks when empty | consumer pops before producer pushes | Pop waits and returns produced value. |
| Close wakes waiters | close while producers/consumers are waiting | Waiters return failure/nullopt without deadlock. |

## 2. Thread Pool

* **Question**: Implement a thread pool that supports task submission and graceful shutdown.
* **Solution**: [Thread Pool](./solutions.md#2-thread-pool).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Drain tasks | submit 100 increments then destroy pool | All accepted tasks finish before destructor returns. |
| Submit after stop | submit during/after shutdown | Submit returns false or rejects according to API. |
| No lock during task | task submits/blocks independently | Worker does not hold queue mutex while executing task. |

## 3. Concurrent Token Bucket

* **Question**: Implement a token bucket rate limiter safe for concurrent callers.
* **Solution**: [Concurrent Token Bucket](./solutions.md#3-concurrent-token-bucket).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Initial burst | capacity 5; consume 5 immediately | All 5 allowed; sixth denied until refill. |
| Refill | wait enough time for 2 tokens | Two more requests are allowed. |
| Concurrent callers | many threads consume simultaneously | Total successful consumes never exceeds available tokens. |

## 4. Single-Flight Duplicate Suppression

* **Question**: Implement single-flight duplicate suppression so only one concurrent caller computes a value for a key.
* **Solution**: [Single-Flight Duplicate Suppression](./solutions.md#4-single-flight-duplicate-suppression).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Duplicate callers | two threads request same key simultaneously | Underlying computation runs once; both receive same result. |
| Different keys | two keys requested | Computations can run independently. |
| Failure cleanup | computation throws/fails | Inflight entry is removed so later retry can run. |

## 5. Readers-Writer Cache

* **Question**: Implement a readers-writer cache for frequent reads and rare writes.
* **Solution**: [Readers-Writer Cache](./solutions.md#5-readers-writer-cache).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Concurrent reads | many readers access existing key | Reads succeed concurrently. |
| Write visibility | writer updates key then reader reads | Reader sees old or new snapshot consistently, never partial state. |
| Rare write contention | readers active while writer waits | No data race; starvation policy is explicit. |

## 6. Reusable Barrier

* **Question**: Implement a countdown latch or reusable barrier.
* **Solution**: [Reusable Barrier](./solutions.md#6-reusable-barrier).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| One generation | 3 parties call wait | All release only after third arrives. |
| Reuse | same barrier used for two rounds | Generation prevents early release from prior round. |
| Single party | barrier size 1 | Wait returns immediately. |

## 7. Deadlock-Free Account Transfer

* **Question**: Code a deadlock-free transfer between two account objects.
* **Solution**: [Deadlock-Free Account Transfer](./solutions.md#7-deadlock-free-account-transfer).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Opposite transfers | thread A transfers X->Y while thread B transfers Y->X | Both complete without deadlock. |
| Insufficient funds | transfer more than balance | No balances change or failure returned. |
| Self transfer | from and to same account | No deadlock and balance unchanged. |

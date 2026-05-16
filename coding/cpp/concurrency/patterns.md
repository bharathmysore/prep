# Concurrency Coding Patterns

Concurrency questions are about correctness under interleavings. In C++, prefer simple mutex and condition-variable designs unless atomics make the invariant clearer.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Bounded blocking queue | Producers block when full, consumers block when empty | `0 <= size <= capacity` and no item is lost | Time `O(1)` per op under lock, space `O(capacity)` | Runtime: notify one waiter after state change. Memory: circular buffer. |
| Producer-consumer pipeline | Work moves through stages | Each accepted item is processed once or explicitly cancelled | Time `O(items + coordination)`, space bounded by queues | Runtime: batch where safe. Memory: bounded queues provide backpressure. |
| Thread pool | Execute submitted tasks on workers | Task queue plus shutdown flag defines worker lifecycle | Submit `O(1)`, execution depends on tasks, space `O(queue)` | Runtime: avoid waking all workers for one task. Memory: bounded queue option. |
| Futures/promises executor | Need result propagation | Each task completes promise exactly once | Time task-dependent, space `O(tasks)` | Runtime: move callables. Memory: release packaged tasks after completion. |
| Readers-writer cache | Many reads, fewer writes | Readers see a coherent value protected by shared/exclusive lock | Time `O(1)` average per access, space `O(n)` | Runtime: `std::shared_mutex` for read-heavy cases. Memory: avoid duplicate snapshots unless needed. |
| Token bucket rate limiter | Limit average rate with bursts | Tokens never exceed capacity and refill with elapsed time | Time `O(1)` per request, space `O(1)` | Runtime: lazy refill on request. Memory: scalar state only. |
| Semaphore resource limiter | Bound concurrent access | Permits in use plus available permits equals capacity | Time `O(1)` under lock, space `O(waiters)` | Runtime: condition variable. Memory: no per-resource object if resources are identical. |
| Countdown latch / barrier | Wait for N events or threads | Waiters unblock only when count reaches zero or phase completes | Time `O(1)` arrive plus wake cost, space `O(1)` | Runtime: notify all at phase completion. Memory: generation counter for reusable barrier. |
| Single-flight duplicate suppression | Coalesce concurrent identical work | One owner computes each key while followers wait | Time `O(1)` average coordination plus work, space `O(inflight keys)` | Runtime: share future per key. Memory: erase inflight state after completion. |
| Deadlock prevention by lock ordering | Multiple locks required | Locks are acquired in a global order | Time depends on critical section, space `O(locks)` | Runtime: `std::scoped_lock` for known lock set. Memory: avoid extra lock graph unless detecting deadlocks. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `bounded-blocking-queue`, `condition-variable` | mutex, condition variables, circular buffer | [questions](./questions.md): Q1 |
| `thread-pool`, `work-queue` | worker threads plus guarded task queue | [questions](./questions.md): Q2 |
| `token-bucket`, `rate-limiter` | mutex-protected lazy refill or carefully justified atomics | [questions](./questions.md): Q3 |
| `single-flight`, `future` | map from key to shared future or condition state | [questions](./questions.md): Q4 |
| `readers-writer-lock`, `shared-mutex` | `std::shared_mutex` or immutable snapshot swap | [questions](./questions.md): Q5 |
| `barrier`, `condition-variable` | mutex, condition variable, count, and generation | [questions](./questions.md): Q6 |
| `deadlock-prevention`, `lock-ordering` | global lock ordering or `std::scoped_lock` | [questions](./questions.md): Q7 |
| `producer-consumer`, `backpressure`, `cancellation` | bounded queues plus shutdown propagation | [questions](./questions.md): Q8 |

## L7 Follow-Ups

- What are the safety and liveness guarantees?
- Where can lost wakeups, spurious wakeups, or shutdown races occur?
- When are atomics sufficient, and when is a mutex clearer?
- How would you test with stress loops and thread sanitizers?

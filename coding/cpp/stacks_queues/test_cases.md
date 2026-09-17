# Stacks Queues Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Valid Parentheses

* **Question**: Validate whether a string of brackets is balanced.
* **Solution**: [Valid Parentheses](./solutions.md#1-valid-parentheses).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Balanced | `()[]{}` | Return true. |
| Wrong order | `(]` | Return false. |
| Unclosed | `((` | Return false. |

## 2. Min Stack

* **Question**: Implement a stack that supports `push`, `pop`, `top`, and `getMin`.
* **Solution**: [Min Stack](./solutions.md#2-min-stack).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Min changes | push -2,0,-3; getMin; pop; top; getMin | Returns -3, then 0, then -2. |
| Duplicate min | push 1,1; pop one | Min remains 1. |
| Empty policy | top/getMin on empty | Reject or return sentinel per API contract. |

## 3. Next Greater Element

* **Question**: Given an array, return the next greater element for every index.
* **Solution**: [Next Greater Element](./solutions.md#3-next-greater-element).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[2,1,2,4,3]` | Return `[4,2,4,-1,-1]` for next greater to right. |
| Descending | `[5,4,3]` | Return all `-1`. |
| Duplicates | `[2,2,3]` | Return `[3,3,-1]`. |

## 4. Largest Rectangle In Histogram

* **Question**: Given histogram bar heights, return the largest rectangle area.
* **Solution**: [Largest Rectangle In Histogram](./solutions.md#4-largest-rectangle-in-histogram).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[2,1,5,6,2,3]` | Return `10`. |
| Increasing | `[1,2,3]` | Return `4`. |
| Empty | `[]` | Return `0`. |

## 5. Sliding Window Maximum

* **Question**: Given an array and window size `k`, return the maximum value in every sliding window.
* **Solution**: [Sliding Window Maximum](./solutions.md#5-sliding-window-maximum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | nums `[1,3,-1,-3,5,3,6,7]`, k=3 | Return `[3,3,5,5,6,7]`. |
| k=1 | any array, `k=1` | Return original array. |
| All equal | `[2,2,2]`, k=2 | Return `[2,2]`. |

## 6. Queue Using Two Stacks

* **Question**: Implement a queue using two stacks and explain amortized cost.
* **Solution**: [Queue Using Two Stacks](./solutions.md#6-queue-using-two-stacks).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| FIFO | push 1, push 2, pop | Pop returns 1. |
| Interleaved | push 1, pop, push 2, peek | Peek returns 2. |
| Empty pop | pop empty | Reject or return sentinel per API contract. |

## 7. Fixed-Capacity Circular Queue

* **Question**: Implement single-threaded and thread-safe fixed-capacity integer FIFOs with non-destructive rejection on full or empty.
* **Solution**: [Fixed-Capacity Circular Queue](./solutions.md#7-fixed-capacity-circular-queue).
* **Executable tests**: [single-threaded](./circular_queue_test.cpp) and [thread-safe](./thread_safe_circular_queue_test.cpp).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Zero capacity | Construct with 0 | Throw `std::invalid_argument`. |
| Empty operations | Capacity 3; repeated front/dequeue, then enqueue/dequeue 42 | Empty optional before insertion; size stays 0; final dequeue returns 42. |
| Non-destructive front | Capacity 2; enqueue 10; peek twice through const reference | Both return 10; size stays 1; dequeue returns 10. |
| All slots usable | Capacity 3; enqueue 10, 20, 30 | All accepted; full true, empty false; drain returns 10, 20, 30. |
| Full rejection | Capacity 2; enqueue 7, 8; try 99 and 100 | Both extra inserts return false; size remains 2; drain remains 7, 8. |
| Wraparound | Capacity 3; enqueue 10, 20, 30; dequeue; enqueue 40 | Removed 10; queue full again; next drain is 20, 30, 40. |
| Capacity one | Repeat enqueue(value), rejected extra enqueue, dequeue for 100 cycles | Exactly one slot usable; returned value always matches; empty afterward. |
| Sentinel collisions | Capacity 6; enqueue 0, -1, -1, INT_MIN, INT_MAX, 0 | All six values preserved in FIFO order. |
| Returned-value lifetime | Save optional from front containing 55; remove it, insert 66, destroy queue | Saved optional still contains 55. |
| Reference-model trace | Seed 20260917; 2,000 mixed operations at each capacity 1, 2, 3, 7, 16 | Admission, returned values, size, empty, full, and final drains match a capacity-limited `std::deque<int>`. |
| Copy/move contract | Compile-time type-trait checks | Copy/move construction and assignment are disabled. |

### Worked wraparound trace

Capacity 3; logical order differs from physical array order after wrapping. Stale slot bytes are not live elements.

| Operation | Head | Tail | Count | Logical FIFO |
| --- | --- | --- | --- | --- |
| Construct | 0 | 0 | 0 | empty |
| enqueue(10) | 0 | 1 | 1 | 10 |
| enqueue(20) | 0 | 2 | 2 | 10, 20 |
| enqueue(30) | 0 | 0 | 3 | 10, 20, 30 |
| dequeue() returns 10 | 1 | 0 | 2 | 20, 30 |
| enqueue(40) | 1 | 1 | 3 | 20, 30, 40 |
| enqueue(50) rejected | 1 | 1 | 3 | 20, 30, 40 |

### Run the local software tests

From the workspace root, use an existing C++17 compiler; no Linux host, storage device, service, or network lab is involved:

```bash
queue_test_dir=$(mktemp -d /private/tmp/circular-queue-check.XXXXXX)
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror coding/cpp/stacks_queues/circular_queue_test.cpp -o "$queue_test_dir/circular_queue_test"
"$queue_test_dir/circular_queue_test"
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer -g coding/cpp/stacks_queues/circular_queue_test.cpp -o "$queue_test_dir/circular_queue_sanitized"
"$queue_test_dir/circular_queue_sanitized"
```

Verification on 2026-09-17: all ten behavioral tests first failed against an unimplemented API scaffold, then passed with the implementation; the sanitizer build also passed. These results verify this single-threaded software exercise, not concurrency, hardware queues, or learner confidence.

### Thread-safe scenarios

The thread-safe runner has five sequential contract tests plus three concurrent tests. No test-side mutex serializes queue operations. Each consumer records into its own vector; verification happens after joining. Assertions inside workers are caught and reported as test failure rather than escaping a thread.

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid capacity | Construct with 0 | Throw `std::invalid_argument`. |
| Const observers and empty removal | Capacity 3; const front/size/capacity/empty/full, then dequeue | Capacity 3, size 0, empty true, full false, absent values, unchanged size. |
| Full rejection and wrap | Capacity 3; insert 10, 20, 30; reject 99; peek twice; remove 10; insert 40 | All slots usable; peek preserves 10; drain is 20, 30, 40. |
| Capacity-one reuse | 100 insert/reject/peek/remove cycles | One live value at a time, no overwrite, empty after each removal. |
| Integer values and ownership | Insert 0, -1, -1, INT_MIN, INT_MAX, 0; save front; drain, reuse, destroy | FIFO preserves all integers; saved optional retains 0 after slot reuse and destruction. |
| SPSC exact FIFO | 1 producer, 1 consumer; 10,000 values each at capacities 1 and 7 | All values received once in exact sequence; queue drains. |
| MPSC per-producer FIFO | 4 producers, 1 consumer; 3,000 values each; capacity 3 | All 12,000 values received once; each producer's sequence preserved; cross-producer interleaving unspecified. |
| MPMC no loss/duplication | 4 producers, 4 consumers; 2,000 values per producer at each capacity 1, 3, 64 | All 8,000 values received once per capacity; queue drains. Do not infer removal order from different consumers' post-return logging. |
| Concurrent snapshots | An observer runs alongside each transfer | Each size is within capacity; copied front values are in range. Separate observer calls need not describe the same instant. |
| Harness lifecycle design (reviewed, not fault-injected) | Worker exception, stalled retry loop, or thread creation failure | Cancel cooperating retries and join started workers before destroying referenced state. The 15-second retry deadline cannot interrupt a call deadlocked inside a mutex; use an outer process timeout for deliberate deadlock experiments. |

Run these after the preceding commands (which set `queue_test_dir`):

```bash
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -pthread coding/cpp/stacks_queues/thread_safe_circular_queue_test.cpp -o "$queue_test_dir/thread_safe_test"
"$queue_test_dir/thread_safe_test"
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -pthread -fsanitize=address,undefined -fno-omit-frame-pointer -g coding/cpp/stacks_queues/thread_safe_circular_queue_test.cpp -o "$queue_test_dir/thread_safe_asan"
"$queue_test_dir/thread_safe_asan"
clang++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -pthread -fsanitize=thread -g coding/cpp/stacks_queues/thread_safe_circular_queue_test.cpp -o "$queue_test_dir/thread_safe_tsan"
"$queue_test_dir/thread_safe_tsan"
```

Verification on 2026-09-17: the five new sequential contract tests failed against the unimplemented thread-safe scaffold, then all eight thread-safe tests passed after implementation. Fresh normal, `-O2 -DNDEBUG`, and AddressSanitizer/UndefinedBehaviorSanitizer builds passed for both suites (10 single-threaded + 8 thread-safe). The thread-safe suite also passed ThreadSanitizer with no reports. Its concurrent tests transfer 56,000 values per suite run. Stress/sanitizer results are evidence, not a proof for every possible interleaving or a confirmation of learner mastery; the lock/invariant argument is part of the correctness reasoning.

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

## 9. Writer-Preference Reader-Writer Lock

* **Question**: Implement writer-preference shared/exclusive locking with a mutex and condition variables.
* **Solution**: [Teaching explanation](./solutions.md#9-writer-preference-reader-writer-lock).
* **Executable tests**: [reader_writer_lock_test.cpp](./reader_writer_lock_test.cpp), against [reader_writer_lock.h](./reader_writer_lock.h).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Overlapping readers | One thread retains shared ownership while another acquires it | Second reader completes without waiting for the first to release |
| Writer blocks reader | Reader attempts entry while writer holds lock | No observed entry during the hold; reader completes after release |
| Writer blocks writer | First writer holds lock; second writer's enrollment is confirmed | Second writer waits, then completes after release |
| Last-reader wakeup | Two distinct threads hold shared ownership; writer is enrolled | Releasing one reader is insufficient; releasing the last enables writer |
| Writer preference | Hold a reader, enroll two writers, then introduce a late reader | Both writers enter before the late reader; either writer order is valid |
| Reader broadcast | Four readers contend behind a writer and remain held after acquisition | All four can acquire before any reader releases |
| Exclusive RAII | Throw inside an exclusive critical section | A later reader acquires successfully |
| Shared RAII | Throw inside a shared critical section | A later writer acquires successfully |
| Contention / visibility | Three writers each increment 2,000 times; four readers each check 2,000 times | Final value and mirror both equal 6,000; no conflicting holders or inconsistent reads |
| Compile-time ownership | Attempt type-trait checks for copy and move | Lock is neither copyable nor movable |

The runner has nine runtime tests and compile-time ownership checks. The friend accessor is defined only in the test file and reads writer enrollment under the real internal mutex; it adds no public observer API. It does not claim that starting a writer thread means that writer is already enrolled. Bounded negative future waits are timing-based observations, not proofs of scheduler progress. Positive handshakes and a per-test watchdog bound hangs. Each thread releases its own locks; async workers are joined by completion/get before lock destruction. The relaxed diagnostic occupancy counter intentionally adds no payload synchronization that could conceal a missing lock edge from ThreadSanitizer.

### Run the local software tests

From `/Users/bmysoren/prep` (use `/tmp` instead of `/private/tmp` on Linux):

```bash
rwlock_test_dir=$(mktemp -d /private/tmp/rwlock-tests.XXXXXX)
c++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -pthread coding/cpp/concurrency/reader_writer_lock_test.cpp -o "$rwlock_test_dir/basic"
"$rwlock_test_dir/basic"
c++ -std=c++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror -pthread coding/cpp/concurrency/reader_writer_lock_test.cpp -o "$rwlock_test_dir/optimized"
"$rwlock_test_dir/optimized"
c++ -std=c++17 -g -Wall -Wextra -Wpedantic -Werror -pthread -fsanitize=address,undefined -fno-omit-frame-pointer coding/cpp/concurrency/reader_writer_lock_test.cpp -o "$rwlock_test_dir/asan_ubsan"
"$rwlock_test_dir/asan_ubsan"
c++ -std=c++17 -g -Wall -Wextra -Wpedantic -Werror -pthread -fsanitize=thread coding/cpp/concurrency/reader_writer_lock_test.cpp -o "$rwlock_test_dir/tsan"
"$rwlock_test_dir/tsan"
```

Verification on 2026-10-03, Apple Clang 21, macOS arm64: all nine tests first failed against the unimplemented API scaffold, then passed in normal, optimized/NDEBUG, ASan/UBSan, and separate TSan builds with no sanitizer reports. This is local software evidence, not a proof of fairness/all possible interleavings, a Linux-kernel test, or a learner-confidence confirmation. Critical-section exception tests verify RAII release, not an injected CV wait failure; untimed wait with these non-throwing predicates has no recoverable wait-exception branch. Misuse and concurrent destruction are precondition violations, not supported recovery cases.

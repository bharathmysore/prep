# Concurrency Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Bounded Blocking Queue

* **Pattern / Idea**: Mutex, condition variables, circular buffer, shutdown flag.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `OpenAI: High`, `Anthropic: High`, `CoreWeave: High`, `NVIDIA: Medium`, `Microsoft: Medium`, `Amazon/AWS: Medium`.
* **Question**: Implement a bounded blocking queue with multiple producers and consumers.
* **Test Cases**: [Test cases](./test_cases.md#1-bounded-blocking-queue).
* **C++ Code**
  ```cpp
  template <class T>
  class BoundedBlockingQueue {
      mutex mu;
      condition_variable notFull, notEmpty;
      deque<T> q;
      size_t cap;
      bool closed = false;
  public:
      explicit BoundedBlockingQueue(size_t capacity) : cap(capacity) {}

      bool push(T value) {
          unique_lock<mutex> lk(mu);
          notFull.wait(lk, [&] { return closed || q.size() < cap; });
          if (closed) return false;
          q.push_back(move(value));
          notEmpty.notify_one();
          return true;
      }

      optional<T> pop() {
          unique_lock<mutex> lk(mu);
          notEmpty.wait(lk, [&] { return closed || !q.empty(); });
          if (q.empty()) return nullopt;
          T value = move(q.front());
          q.pop_front();
          notFull.notify_one();
          return value;
      }

      void close() {
          lock_guard<mutex> lk(mu);
          closed = true;
          notFull.notify_all();
          notEmpty.notify_all();
      }
  };
  ```
* **Code Explanation**: Producers wait for capacity; consumers wait for data; close wakes both sides.
* **Invariants**: `0 <= q.size() <= cap`; after close, no new values are accepted.
* **Complexity**: Time `O(1)` per operation under lock, space `O(capacity)`.
* **Optimizations**: Runtime: `notify_one` for normal state transitions. Memory: circular buffer can replace deque.
* **Edge Cases To Consider**: Full queue, empty queue, close while waiting, multiple producers/consumers.
* **L7 Follow-ups**: Discuss spurious wakeups, backpressure, and graceful shutdown.

## 2. Thread Pool

* **Pattern / Idea**: Worker threads consume a guarded task queue.
* **Company Frequency Tags**: Public signal: `OpenAI: High (all 67.5)`, `Databricks: Medium (all 54.1)`; Domain fit: `Anthropic: High`, `CoreWeave: High`, `NVIDIA: Medium`, `Microsoft: Medium`, `Amazon/AWS: Medium`.
* **Question**: Implement a thread pool that supports task submission and graceful shutdown.
* **Test Cases**: [Test cases](./test_cases.md#2-thread-pool).
* **C++ Code**
  ```cpp
  class ThreadPool {
      mutex mu;
      condition_variable cv;
      queue<function<void()>> tasks;
      vector<thread> workers;
      bool stopping = false;
  public:
      explicit ThreadPool(int n) {
          for (int i = 0; i < n; ++i) {
              workers.emplace_back([this] {
                  while (true) {
                      function<void()> task;
                      {
                          unique_lock<mutex> lk(mu);
                          cv.wait(lk, [&] { return stopping || !tasks.empty(); });
                          if (stopping && tasks.empty()) return;
                          task = move(tasks.front());
                          tasks.pop();
                      }
                      task();
                  }
              });
          }
      }

      bool submit(function<void()> task) {
          {
              lock_guard<mutex> lk(mu);
              if (stopping) return false;
              tasks.push(move(task));
          }
          cv.notify_one();
          return true;
      }

      ~ThreadPool() {
          {
              lock_guard<mutex> lk(mu);
              stopping = true;
          }
          cv.notify_all();
          for (thread& t : workers) if (t.joinable()) t.join();
      }
  };
  ```
* **Code Explanation**: Workers exit only after shutdown is requested and all accepted tasks are drained.
* **Invariants**: Tasks are removed from the queue by exactly one worker.
* **Complexity**: Submit `O(1)` under lock, space `O(queued tasks + workers)`.
* **Optimizations**: Runtime: avoid holding lock while running task. Memory: bounded queue prevents unbounded backlog.
* **Edge Cases To Consider**: Submit after shutdown, task throws policy, zero workers, many short tasks.
* **L7 Follow-ups**: Add futures, cancellation, queue bounds, and metrics for production.

## 3. Concurrent Token Bucket

* **Pattern / Idea**: Lazy refill under mutex.
* **Company Frequency Tags**: Public signal: `Databricks: High (6m 100.0)`, `Apple: High (6m 62.9)`, `Snowflake: Medium (6m 49.9)`; Domain fit: `OpenAI: High`, `Anthropic: High`, `CoreWeave: High`, `NVIDIA: Medium`, `Microsoft: Medium`, `Amazon/AWS: High`, `Stripe: High`.
* **Question**: Implement a token bucket rate limiter safe for concurrent callers.
* **Test Cases**: [Test cases](./test_cases.md#3-concurrent-token-bucket).
* **C++ Code**
  ```cpp
  class TokenBucket {
      mutex mu;
      double capacity, tokens, ratePerSecond;
      chrono::steady_clock::time_point last;
  public:
      TokenBucket(double cap, double rate)
          : capacity(cap), tokens(cap), ratePerSecond(rate), last(chrono::steady_clock::now()) {}

      bool allow(double cost = 1.0) {
          lock_guard<mutex> lk(mu);
          auto now = chrono::steady_clock::now();
          double elapsed = chrono::duration<double>(now - last).count();
          tokens = min(capacity, tokens + elapsed * ratePerSecond);
          last = now;
          if (tokens < cost) return false;
          tokens -= cost;
          return true;
      }
  };
  ```
* **Code Explanation**: Tokens are replenished based on elapsed monotonic time whenever a request arrives.
* **Invariants**: `0 <= tokens <= capacity` after refill and decision.
* **Complexity**: Time `O(1)`, space `O(1)`.
* **Optimizations**: Runtime: lazy refill avoids background thread. Memory: scalar state.
* **Edge Cases To Consider**: Burst capacity, no tokens, elapsed refill, concurrent callers.
* **L7 Follow-ups**: Distributed rate limits need shared counters or partitioned quotas.

## 4. Single-Flight Duplicate Suppression

* **Pattern / Idea**: One owner computes, followers wait on a shared future.
* **Company Frequency Tags**: Public signal: `OpenAI: High (all 67.5)`, `Databricks: Medium (all 54.1)`; Domain fit: `Anthropic: High`, `CoreWeave: High`, `NVIDIA: Medium`, `Microsoft: Medium`, `Amazon/AWS: Medium`, `Stripe: High`.
* **Question**: Implement single-flight duplicate suppression so only one concurrent caller computes a value for a key.
* **Test Cases**: [Test cases](./test_cases.md#4-single-flight-duplicate-suppression).
* **C++ Code**
  ```cpp
  template <class K, class V>
  class SingleFlight {
      mutex mu;
      unordered_map<K, shared_future<V>> inflight;
  public:
      template <class Fn>
      V doOnce(const K& key, Fn fn) {
          shared_ptr<promise<V>> promise;
          shared_future<V> fut;
          bool owner = false;
          {
              lock_guard<mutex> lk(mu);
              auto it = inflight.find(key);
              if (it != inflight.end()) fut = it->second;
              else {
                  promise = make_shared<std::promise<V>>();
                  fut = promise->get_future().share();
                  inflight[key] = fut;
                  owner = true;
              }
          }
          if (owner) {
              try {
                  promise->set_value(fn());
              } catch (...) {
                  promise->set_exception(current_exception());
              }
              {
                  lock_guard<mutex> lk(mu);
                  inflight.erase(key);
              }
          }
          return fut.get();
      }
  };
  ```
* **Code Explanation**: The first caller creates the shared computation; concurrent callers reuse the same future.
* **Invariants**: At most one in-flight future exists for a key.
* **Complexity**: Average coordination `O(1)`, space `O(inflight keys)`.
* **Optimizations**: Runtime: owner computes outside the map lock. Memory: erase state after completion.
* **Edge Cases To Consider**: Concurrent same key, different keys, exception cleanup, slow computation.
* **L7 Follow-ups**: Avoid detached threads in production; integrate with a managed executor.

## 5. Readers-Writer Cache

* **Pattern / Idea**: `shared_mutex` for read-heavy map.
* **Company Frequency Tags**: Public signal: `OpenAI: High (6m 66.7)`, `Apple: Medium (6m 54.0)`, `Snowflake: Medium (6m 49.9)`, `Oracle: Medium (6m 47.0)`, `Databricks: High (all 79.9)`; Domain fit: `Anthropic: High`, `CoreWeave: High`, `NVIDIA: Medium`, `Microsoft: High`, `Amazon/AWS: Medium`.
* **Question**: Implement a readers-writer cache for frequent reads and rare writes.
* **Test Cases**: [Test cases](./test_cases.md#5-readers-writer-cache).
* **C++ Code**
  ```cpp
  template <class K, class V>
  class RWCache {
      mutable shared_mutex mu;
      unordered_map<K, V> data;
  public:
      optional<V> get(const K& key) const {
          shared_lock<shared_mutex> lk(mu);
          auto it = data.find(key);
          if (it == data.end()) return nullopt;
          return it->second;
      }
      void put(K key, V value) {
          unique_lock<shared_mutex> lk(mu);
          data[move(key)] = move(value);
      }
      bool erase(const K& key) {
          unique_lock<shared_mutex> lk(mu);
          return data.erase(key) > 0;
      }
  };
  ```
* **Code Explanation**: Multiple readers may proceed concurrently; writes take exclusive access.
* **Invariants**: All map mutations occur under exclusive lock.
* **Complexity**: Average access `O(1)` plus lock cost, space `O(n)`.
* **Optimizations**: Runtime: shared locks help read-heavy workloads. Memory: return copies; use shared pointers for large values.
* **Edge Cases To Consider**: Concurrent reads, read during write, erase missing key, large value copy cost.
* **L7 Follow-ups**: Watch writer starvation depending on implementation and workload.

## 6. Reusable Barrier

* **Pattern / Idea**: Count plus generation to separate phases.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `OpenAI: High`, `Anthropic: High`, `CoreWeave: High`, `NVIDIA: High`, `Microsoft: Medium`, `Amazon/AWS: Medium`.
* **Question**: Implement a countdown latch or reusable barrier.
* **Test Cases**: [Test cases](./test_cases.md#6-reusable-barrier).
* **C++ Code**
  ```cpp
  class Barrier {
      mutex mu;
      condition_variable cv;
      int parties, count, generation = 0;
  public:
      explicit Barrier(int n) : parties(n), count(n) {}
      void arriveAndWait() {
          unique_lock<mutex> lk(mu);
          int gen = generation;
          if (--count == 0) {
              ++generation;
              count = parties;
              cv.notify_all();
              return;
          }
          cv.wait(lk, [&] { return generation != gen; });
      }
  };
  ```
* **Code Explanation**: The last arriving thread advances the generation and releases all waiters.
* **Invariants**: Waiters only resume when the barrier generation changes.
* **Complexity**: Time `O(1)` arrive plus wake cost, space `O(1)`.
* **Optimizations**: Runtime: generation avoids cross-phase wake confusion. Memory: scalar state.
* **Edge Cases To Consider**: Reuse many phases, one party, slow last thread, spurious wakeups.
* **L7 Follow-ups**: Add timeout or broken-barrier semantics for failures.

## 7. Deadlock-Free Account Transfer

* **Pattern / Idea**: Acquire both locks atomically or in a global order.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `OpenAI: High`, `Anthropic: High`, `CoreWeave: High`, `NVIDIA: Medium`, `Microsoft: Medium`, `Amazon/AWS: Medium`, `Stripe: High`.
* **Question**: Code a deadlock-free transfer between two account objects.
* **Test Cases**: [Test cases](./test_cases.md#7-deadlock-free-account-transfer).
* **C++ Code**
  ```cpp
  struct Account {
      mutable mutex mu;
      long long balance = 0;
  };

  bool transfer(Account& from, Account& to, long long amount) {
      if (&from == &to) return true;
      scoped_lock lock(from.mu, to.mu);
      if (from.balance < amount) return false;
      from.balance -= amount;
      to.balance += amount;
      return true;
  }
  ```
* **Code Explanation**: `scoped_lock` locks both mutexes using deadlock-avoidance behavior.
* **Invariants**: Total balance across the two accounts is unchanged after a successful transfer.
* **Complexity**: Time `O(1)` plus lock wait, space `O(1)`.
* **Optimizations**: Runtime: avoid nested unknown lock order. Memory: no extra lock graph.
* **Edge Cases To Consider**: Same account, insufficient funds, concurrent opposite transfers, negative amount policy.
* **L7 Follow-ups**: Real transfers need idempotency, ledger records, audit, and transactional durability.

## 9. Writer-Preference Reader-Writer Lock

* **Pattern / Idea**: A monitor: one mutex protects admission state; separate condition variables park readers and writers. Recognize this when many readers may overlap, writers need exclusive access, and the preference policy must be explicit. A plain mutex is the simpler baseline but serializes readers too.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `NVIDIA: Medium` (editorial systems/concurrency relevance, not a reported interview-frequency score).
* **Question**: Implement a nonrecursive reader-writer lock that prevents new readers from bypassing an enrolled writer, without using `std::shared_mutex` internally.
* **Pattern Tags**: `readers-writer-lock`, `condition-variable`.
* **Test Cases**: [Scenarios, executable suite, and run instructions](./test_cases.md#9-writer-preference-reader-writer-lock).
* **Requirements / API**: `lock_shared()` / `unlock_shared()` for readers and `lock()` / `unlock()` for writers. Blocking RAII usage works with `std::shared_lock<ReaderWriterLock>` and `std::unique_lock<ReaderWriterLock>`. No try-lock, timed/cancellable acquisition, recursion, upgrade, downgrade, or close. This is not a complete replacement for the standard `SharedMutex` interface. Calls must be properly paired by the acquiring thread; do not move ownership to another thread. All users, including waiters, must finish before destruction. Counter sizes assume the number of simultaneous callers fits in `std::size_t`.
* **C++ Code**: The complete, tested implementation is [reader_writer_lock.h](./reader_writer_lock.h); it is the canonical source rather than a second copy of the algorithm. Example consumer:

  ```cpp
  #include "reader_writer_lock.h"
  #include <mutex>
  #include <shared_mutex>

  class SharedValue {
  public:
      int read() const {
          std::shared_lock<ReaderWriterLock> guard(lock_);
          return value_; // Return a value, not an unprotected borrowed reference.
      }

      void write(int value) {
          std::unique_lock<ReaderWriterLock> guard(lock_);
          value_ = value;
      }

  private:
      mutable ReaderWriterLock lock_;
      int value_ = 0;
  };
  ```

* **Code Explanation**:
  1. `mutex_` protects `active_readers_`, `waiting_writers_`, and `writer_active_`. It is not held throughout the caller's critical section; logical ownership is represented by those fields.
  2. A reader waits for `!writer_active_ && waiting_writers_ == 0`, increments the reader count, and releases the bookkeeping mutex on return. Shared ownership continues until `unlock_shared()`.
  3. A writer increments `waiting_writers_` before waiting. This is the enrollment boundary that closes the gate to later readers. Merely starting a thread or calling `lock()` does not yet establish priority.
  4. The writer waits for `!writer_active_ && active_readers_ == 0`. Decrementing its waiting count and setting the writer flag happen under the same mutex, so a reader cannot slip between them.
  5. The last departing reader wakes one writer. A departing writer wakes one queued writer, or broadcasts to readers when no writers remain queued. Notification is not ownership transfer: every woken thread reacquires the mutex and rechecks its predicate.
  6. Predicate waits handle spurious wakeups and avoid a lost-wakeup gap between checking the condition and sleeping. Ordinary protected payload accesses become visible through the internal mutex's release/acquire synchronization; the fields need not be atomics.
  7. Acquiring the internal mutex can fail before any admission state changes. The untimed CV wait and our `noexcept` scalar predicates have no recoverable exception path after writer enrollment; failed mutex reacquisition terminates under the standard contract. If adding cancellation, timeout, or a throwing predicate, explicitly undo enrollment and wake newly eligible callers. Exceptions in the user's critical section are handled by the external RAII guard, not by cancelling a wait.
* **Invariants**:
  - `writer_active_` implies `active_readers_ == 0`; there is at most one active writer.
  - `active_readers_ > 0` implies no active writer. Shared holders may read, not mutate the protected data without separate synchronization.
  - A new reader increments its count only when there is no active writer and no enrolled writer.
  - All predicate reads, state transitions, and notifications use the same internal mutex.
* **Complexity**: `O(1)` bookkeeping per call and `O(1)` lock-object state. Actual acquisition time is unbounded under contention or an unfair scheduler. Broadcasting can wake `r` readers and induce `O(r)` scheduling/recheck work; blocked callers also consume `O(r + w)` aggregate thread/waiter resources outside the object. This is a blocking lock, not lock-free or wait-free.
* **Optimizations**:
  - **Runtime**: Separate CVs avoid waking an ineligible reader instead of the only eligible writer. Reader broadcasting enables concurrent work. Benchmark against a plain mutex and `std::shared_mutex`: shared counter/cache-line contention and bookkeeping can outweigh parallelism for tiny reads. Sharding or immutable snapshots can reduce contention, at a cost in semantics and memory.
  - **Memory**: Counters avoid an application-level per-waiter FIFO queue. Explicit FIFO/phase-fair policies need additional scheduling state; do not add them unless the fairness contract requires them.
* **Edge Cases To Consider**: Last-reader wakeup, multiple queued writers, writer-to-reader broadcast, spurious wakeups, RAII unwinding, and quiescent destruction. Assertions diagnose some misuse but do not track individual thread ownership. Reacquiring a shared lock while already holding one can deadlock when a writer is queued. Upgrading while retaining a read lock can deadlock even with one upgrading reader because the writer waits for that reader count to reach zero.
* **L7 Follow-ups**:
  - **Fairness**: Writer preference prevents new readers from extending an existing reader cohort indefinitely after writer enrollment. It does not promise FIFO ordering, bounded waiting for an individual writer, or starvation freedom. Continuous writers can starve readers; thread scheduling and mutex acquisition remain outside this policy.
  - **Failure matrix**: A spurious wake rechecks the predicate; a critical-section exception releases through RAII; a holder that never releases can block others indefinitely; concurrent destruction, wrong-thread unlock, and unbalanced calls violate the contract. This lock cannot recover protected state after a process crash.
  - **Observability**: Measure read/write acquisition and hold-time distributions separately, plus contention and writer backlog using low-overhead instrumentation. Do not invoke arbitrary callbacks while holding the bookkeeping mutex.
  - **Rollout / rollback**: Compare correctness, throughput, and reader/writer P99 against the existing synchronization primitive on representative workloads. Switch implementations only with all users quiescent; two different locks do not protect the same data against each other. Roll back the policy at a drained lifecycle boundary.
  - **Leadership decision**: Is writer preference acceptable, or does the product require reader latency bounds or FIFO/phase fairness? This decides whether the simple implementation fits the contract.
* **Useful Public References**: [C++17 condition-variable semantics](https://timsong-cpp.github.io/cppwp/n4659/thread.condition.condvar), [shared-lock RAII](https://eel.is/c++draft/thread.lock.shared). These establish library behavior, not company interview frequency. Prefer a standard primitive in production unless a measured policy requirement justifies a custom one.

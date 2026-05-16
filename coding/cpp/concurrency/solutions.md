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

# Stacks And Queues Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Valid Parentheses

* **Pattern / Idea**: Stack of unmatched opening brackets.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: High (6m 73.6)`, `Oracle: High (6m 69.5)`, `Meta: High (6m 66.2)`, `NVIDIA: High (6m 66.0)`, `Google: High (6m 60.8)`, `Microsoft: Medium (6m 56.5)`, `Apple: Medium (6m 54.0)`.
* **Question**: Validate whether a string of brackets is balanced.
* **Test Cases**: [Test cases](./test_cases.md#1-valid-parentheses).
* **C++ Code**
  ```cpp
  bool validParentheses(const string& s) {
      unordered_map<char, char> close{{')', '('}, {']', '['}, {'}', '{'}};
      vector<char> st;
      for (char c : s) {
          if (c == '(' || c == '[' || c == '{') st.push_back(c);
          else if (close.count(c)) {
              if (st.empty() || st.back() != close[c]) return false;
              st.pop_back();
          }
      }
      return st.empty();
  }
  ```
* **Code Explanation**: A closing bracket must match the most recent unmatched opening bracket.
* **Invariants**: `st` contains exactly the unmatched opening brackets for the processed prefix.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: use a switch instead of map for tiny constants. Memory: reserve stack size if needed.
* **Edge Cases To Consider**: Empty, starts with close, nested, interleaved mismatch, non-bracket chars policy.
* **L7 Follow-ups**: Extend to parser tokens with line/column diagnostics.

## 2. Min Stack

* **Pattern / Idea**: Store current minimum with each stack depth.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 74.2)`, `Microsoft: Medium (6m 52.4)`, `Oracle: Medium (6m 47.0)`, `Snowflake: Medium (all 55.0)`, `NVIDIA: Medium (all 38.4)`.
* **Question**: Implement a stack that supports `push`, `pop`, `top`, and `getMin`.
* **Test Cases**: [Test cases](./test_cases.md#2-min-stack).
* **C++ Code**
  ```cpp
  class MinStack {
      vector<pair<int, int>> st;
  public:
      void push(int x) {
          int mn = st.empty() ? x : min(x, st.back().second);
          st.push_back({x, mn});
      }
      void pop() { st.pop_back(); }
      int top() const { return st.back().first; }
      int getMin() const { return st.back().second; }
      bool empty() const { return st.empty(); }
  };
  ```
* **Code Explanation**: Each entry remembers the minimum for the prefix ending at that entry.
* **Invariants**: `st.back().second` is the minimum of all values currently in the stack.
* **Complexity**: Time `O(1)` per operation, space `O(n)`.
* **Optimizations**: Runtime: no scanning. Memory: compressed auxiliary min stack stores only min changes.
* **Edge Cases To Consider**: Duplicate minimums, pop minimum, negative values, empty operation contract.
* **L7 Follow-ups**: Make API exception-safe or return optional values.

## 3. Next Greater Element

* **Pattern / Idea**: Monotonic decreasing stack of unresolved indices.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given an array, return the next greater element for every index.
* **Test Cases**: [Test cases](./test_cases.md#3-next-greater-element).
* **C++ Code**
  ```cpp
  vector<int> nextGreater(const vector<int>& nums) {
      vector<int> ans(nums.size(), -1), st;
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          while (!st.empty() && nums[i] > nums[st.back()]) {
              ans[st.back()] = nums[i];
              st.pop_back();
          }
          st.push_back(i);
      }
      return ans;
  }
  ```
* **Code Explanation**: When a larger value arrives, it resolves all smaller values waiting on the stack.
* **Invariants**: Values at stacked indices are monotonically decreasing from bottom to top.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: every index is pushed and popped once. Memory: store indices only.
* **Edge Cases To Consider**: Decreasing input, increasing input, duplicates, empty input.
* **L7 Follow-ups**: Circular version scans `2n` positions with modulo.

## 4. Largest Rectangle In Histogram

* **Pattern / Idea**: Monotonic increasing stack.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (6m 54.5)`, `Amazon/AWS: Medium (6m 52.8)`, `Meta: Medium (6m 32.3)`, `Apple: Medium (all 52.1)`.
* **Question**: Given histogram bar heights, return the largest rectangle area.
* **Test Cases**: [Test cases](./test_cases.md#4-largest-rectangle-in-histogram).
* **C++ Code**
  ```cpp
  long long largestRectangleArea(const vector<int>& h) {
      vector<int> st;
      long long best = 0;
      for (int i = 0; i <= static_cast<int>(h.size()); ++i) {
          int cur = (i == static_cast<int>(h.size())) ? 0 : h[i];
          while (!st.empty() && cur < h[st.back()]) {
              int height = h[st.back()];
              st.pop_back();
              int left = st.empty() ? -1 : st.back();
              best = max(best, 1LL * height * (i - left - 1));
          }
          st.push_back(i);
      }
      return best;
  }
  ```
* **Code Explanation**: Popping a bar discovers its first smaller boundary on both sides.
* **Invariants**: Stack heights are nondecreasing.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: sentinel zero flushes all bars. Memory: stack of indices.
* **Edge Cases To Consider**: Empty, all equal, increasing, decreasing, large area overflow.
* **L7 Follow-ups**: For online histograms, exact answer may require retaining unresolved bars.

## 5. Sliding Window Maximum

* **Pattern / Idea**: Monotonic deque.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 84.2)`.
* **Question**: Given an array and window size `k`, return the maximum value in every sliding window.
* **Test Cases**: [Test cases](./test_cases.md#5-sliding-window-maximum).
* **C++ Code**
  ```cpp
  vector<int> maxSlidingWindow(const vector<int>& nums, int k) {
      deque<int> dq;
      vector<int> ans;
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          while (!dq.empty() && dq.front() <= i - k) dq.pop_front();
          while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
          dq.push_back(i);
          if (i >= k - 1) ans.push_back(nums[dq.front()]);
      }
      return ans;
  }
  ```
* **Code Explanation**: The deque front is always the largest active window element.
* **Invariants**: Deque indices are in increasing order and values are in decreasing order.
* **Complexity**: Time `O(n)`, space `O(k)`.
* **Optimizations**: Runtime: each index is inserted and removed once. Memory: store indices, not values.
* **Edge Cases To Consider**: `k = 1`, `k = n`, duplicates, negative values.
* **L7 Follow-ups**: For distributed windows, define event-time vs processing-time semantics.

## 6. Queue Using Two Stacks

* **Pattern / Idea**: Amortized transfer from input stack to output stack.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 38.9)`, `Oracle: Medium (all 33.6)`.
* **Question**: Implement a queue using two stacks and explain amortized cost.
* **Test Cases**: [Test cases](./test_cases.md#6-queue-using-two-stacks).
* **C++ Code**
  ```cpp
  class TwoStackQueue {
      vector<int> in, out;
      void refill() {
          if (!out.empty()) return;
          while (!in.empty()) {
              out.push_back(in.back());
              in.pop_back();
          }
      }
  public:
      void push(int x) { in.push_back(x); }
      int pop() {
          refill();
          int x = out.back();
          out.pop_back();
          return x;
      }
      int front() {
          refill();
          return out.back();
      }
      bool empty() const { return in.empty() && out.empty(); }
  };
  ```
* **Code Explanation**: Reversing `in` into `out` restores FIFO order when needed.
* **Invariants**: If `out` is non-empty, its back is the queue front; otherwise queue order is reverse of `in`.
* **Complexity**: Amortized time `O(1)` per operation, worst transfer `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: transfer only when `out` is empty. Memory: no duplicate storage after transfer.
* **Edge Cases To Consider**: Alternating push/pop, popping after bulk push, empty operation contract.
* **L7 Follow-ups**: For real concurrency, use a proper synchronized queue instead.


## 7. Fixed-Capacity Circular Queue

* **Pattern / Idea**: Reuse a fixed array as a ring instead of shifting elements after removing the front. Head, tail, and an explicit count give constant-time queue work and use every slot; a mutex serializes complete operations when callers share the queue.
* **Company Frequency Tags**: Public signal: not assessed for this exercise. Domain fit: `NVIDIA: Medium` — a storage-queue foundation, not a claim of interview frequency or a hardware NVMe implementation.
* **Pattern Tags**: `circular-buffer`, `backpressure`, `mutex`.
* **Question**: Implement single-threaded and thread-safe fixed-capacity integer FIFOs with enqueue, dequeue, front, size, capacity, empty, and full operations; reject overflow without overwriting data.
* **Test Cases**: [Concrete scenarios and wraparound trace](./test_cases.md#7-fixed-capacity-circular-queue); [single-threaded tests](./circular_queue_test.cpp); [thread-safe tests](./thread_safe_circular_queue_test.cpp).
* **Contract**: C++17; positive `std::size_t` capacity; zero throws `std::invalid_argument`. Enqueue returns false when full; dequeue/front return an empty optional when empty. Failed operations leave state unchanged. Queue instances are noncopyable/nonmovable; allocation failure propagates from the constructor. Neither variant resizes, overwrites live data, persists data, or models hardware DMA. The base has no synchronization. The MPMC wrapper may wait for its mutex, but does not wait for queue space/data; it is not lock-free or wait-free. Callers must stop/join all users before destruction.
* **C++ Code — single-threaded**: Canonical runnable source: [circular_queue.h](./circular_queue.h).
  ```cpp
  #include <cstddef>
  #include <optional>
  #include <stdexcept>
  #include <vector>

  // Fixed-capacity, single-threaded FIFO. Full queues reject new values.
  class CircularQueue {
  public:
      explicit CircularQueue(std::size_t capacity) : buffer_(capacity) {
          if (capacity == 0) {
              throw std::invalid_argument("capacity must be positive");
          }
      }

      // Keep queue instances in place; copying/moving is outside this exercise.
      CircularQueue(const CircularQueue&) = delete;
      CircularQueue& operator=(const CircularQueue&) = delete;
      CircularQueue(CircularQueue&&) = delete;
      CircularQueue& operator=(CircularQueue&&) = delete;

      bool enqueue(int value) noexcept {
          if (full()) return false;

          buffer_[tail_] = value;
          tail_ = next(tail_);
          ++count_;
          return true;
      }

      std::optional<int> dequeue() noexcept {
          if (empty()) return std::nullopt;

          const int value = buffer_[head_];
          head_ = next(head_);
          --count_;
          return value;
      }

      std::optional<int> front() const noexcept {
          if (empty()) return std::nullopt;
          return buffer_[head_];
      }

      bool empty() const noexcept { return count_ == 0; }
      bool full() const noexcept { return count_ == capacity(); }
      std::size_t size() const noexcept { return count_; }
      std::size_t capacity() const noexcept { return buffer_.size(); }

  private:
      std::size_t next(std::size_t index) const noexcept {
          return index == capacity() - 1 ? 0 : index + 1;
      }

      std::vector<int> buffer_;
      std::size_t head_ = 0;   // Oldest live element, when nonempty.
      std::size_t tail_ = 0;   // Next insertion slot, when nonfull.
      std::size_t count_ = 0;
  };
  ```
* **C++ Code — thread-safe**: Canonical runnable source: [thread_safe_circular_queue.h](./thread_safe_circular_queue.h). Composition keeps the ring algorithm in one place; the base queue is private and never exposed to callers.
  ```cpp
  #include "circular_queue.h"

  #include <mutex>

  // Fixed-capacity MPMC FIFO. May wait for the mutex, never for space or data.
  // Stop/join all users before destroying the queue. Observers are snapshots.
  class ThreadSafeCircularQueue {
  public:
      explicit ThreadSafeCircularQueue(std::size_t capacity) : queue_(capacity) {}

      ThreadSafeCircularQueue(const ThreadSafeCircularQueue&) = delete;
      ThreadSafeCircularQueue& operator=(const ThreadSafeCircularQueue&) = delete;
      ThreadSafeCircularQueue(ThreadSafeCircularQueue&&) = delete;
      ThreadSafeCircularQueue& operator=(ThreadSafeCircularQueue&&) = delete;

      bool enqueue(int value) {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.enqueue(value);
      }

      std::optional<int> dequeue() {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.dequeue();
      }

      std::optional<int> front() const {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.front();
      }

      bool empty() const {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.empty();
      }

      bool full() const {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.full();
      }

      std::size_t size() const {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.size();
      }

      std::size_t capacity() const {
          std::lock_guard<std::mutex> lock(mutex_);
          return queue_.capacity();
      }

  private:
      CircularQueue queue_;
      mutable std::mutex mutex_;
  };
  ```
* **Code Explanation**:
  - `buffer_(capacity)` constructs all array slots once. Merely calling `reserve` would not make indexed writes into nonexistent elements valid.
  - `head_` identifies the oldest element when nonempty; `tail_` identifies the next insertion position when nonfull.
  - Enqueue checks full before storing, advances tail with wraparound, and increments count. Dequeue checks empty before reading, advances head, and decrements count.
  - Front returns a value without changing state. Returning `optional<int>` by value avoids both sentinel collisions and borrowed-buffer lifetime problems.
  - `next` wraps using a comparison, so arbitrary positive capacities work. It never forms `head + count`; the invariant below is mathematical rather than an overflow-prone implementation expression.
  - Slots need not be cleared after dequeue: stale integers are outside the logical queue. A generic resource-owning type would require explicit lifetime design.
  - The wrapper locks before checking capacity/data, and keeps the lock through the slot access and head/tail/count update. Locking only the updates allows two producers to both observe one free slot.
  - Each wrapper method calls the nonlocking base, not another public wrapper method. Reacquiring the same nonrecursive mutex from its owner is invalid.
  - `mutable` allows const observers to synchronize. `lock_guard` releases the mutex on scope exit; locking methods deliberately omit `noexcept` because acquisition can throw. [C++ mutex requirements](https://eel.is/c++draft/thread.mutex.requirements.mutex) and [RAII lock guard](https://eel.is/c++draft/thread.lock.guard).
  - Successful enqueue/dequeue takes effect at its state update while locked; failed admission/removal takes effect at the locked full/empty check; an observer takes effect at its locked read. This is a linearizable FIFO, with overlapping calls ordered by their serialized operations, not arrival time or caller logging order. No fairness or bounded lock wait is promised.
  - `front`, `empty`, and `size` are snapshots, not reservations. Another consumer can remove an item after `empty()` returns false. Call `dequeue()` once and inspect its optional result. A `front()` value may not be the value a later dequeue returns.
* **Invariants**: Capacity is positive and unchanged; `head, tail < capacity`; `0 <= count <= capacity`; mathematically `tail = (head + count) mod capacity`. Logical contents occupy exactly count successive positions starting at head. Both full and empty can have equal head/tail; count distinguishes them. In the thread-safe variant, every operation accesses this state under the same mutex, so intermediate updates are not visible to another queue operation.
* **Complexity**: Construction time `O(capacity)`; base operations and wrapper critical-section work are worst-case `O(1)`. Thread-safe wall time also includes contention and scheduler delays; it has no bounded-time guarantee. Storage is `O(capacity)`, with `O(1)` metadata, mutex state, and per-operation auxiliary space. The ring performs no per-operation allocation.
* **Optimizations**: Runtime: contiguous storage and no shifting or per-operation allocation. A bitmask needs an enforced power-of-two capacity. Under measured contention, compare batching (less locking but longer critical sections), sharding (weaker global order), and a separate SPSC design when ownership permits. Memory: count uses one extra word but enables all N slots; a head/tail-only alternative reserves a slot. Both variants reuse the same preallocated ring.
* **Edge Cases To Consider**: Zero/one capacity, full/empty rejection, wraparound, non-power-of-two capacity, duplicates, every integer value, saved return values, allocation/lock failure, contention, snapshot races, and destruction before users stop. The queue owns no worker threads and provides no cancellation protocol for caller retry loops.
* **Failure Matrix**:

  | Condition | Behavior / responsibility |
  | --- | --- |
  | Full / empty | Return false / empty optional without changing contents; caller chooses retry, reject, or shed. |
  | Competing operations | Serialize with mutex; progress depends on scheduling, with no fairness guarantee. |
  | Allocation or lock failure | Propagate exception; do not report successful admission. |
  | Process crash | In-memory entries are lost; this queue is not a durability or exactly-once processing mechanism. |
  | Destruction while in use | Unsupported lifetime violation; stop/join all callers first. |

* **L7 Follow-ups**:
  - Blocking producer/consumer variant: add condition variables and predicate waits, define close/drain/cancellation semantics, and wake all waiters on close. This is a different API, not required for thread safety.
  - SPSC variant: redesign ownership and release/acquire publication; making these three fields atomic is not a correctness proof. MPMC needs a separate algorithm.
  - Generic type: define construction/destruction, throwing moves, and copy/move semantics before templating.
  - NVMe connection: this software queue stores integer values, not hardware command descriptors. Its count-based full-capacity rule is not the conventional reserved-slot NVMe ring protocol; no DMA/MMIO ordering is modeled.
  - Observability: measure queue depth/high-water mark, admission rejection rate, enqueue/dequeue latency, and mutex wait/hold time. Avoid holding the queue mutex while exporting telemetry or invoking callbacks.
  - Rollout: validate the concurrency/ownership contract and stress/sanitizer results, then canary representative producer/consumer loads and compare CPU and P99 latency.
  - Rollback: quiesce users and drain or explicitly account for pending work before replacing a queue. A rollback to a single-threaded queue is safe only if exclusive ownership or external synchronization is restored.
  - Leadership decision: agree on overload behavior (reject, retry, or wait) and whether global FIFO is required before trading simplicity for sharding or lock-free complexity. No production rollout is performed by this exercise.
* **Reference context**: [C++ vector construction and initialization cost](https://eel.is/c++draft/vector.cons), [optional value ownership](https://eel.is/c++draft/optional.optional), and [SPDK command/completion background](https://spdk.io/doc/nvme_spec.html). Company relevance is an editorial domain-fit assessment, not measured frequency.

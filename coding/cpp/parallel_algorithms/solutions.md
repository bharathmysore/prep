# Parallel Algorithms Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Parallel Map

* **Pattern / Idea**: Partition output indices by chunk.
* **Company Frequency Tags**: Public signal: `OpenAI: High (all 67.5)`, `Databricks: Medium (all 54.1)`; Domain fit: `NVIDIA: High`, `CoreWeave: High`, `Snowflake: Medium`, `Google: Medium`.
* **Question**: Implement parallel map over a vector with a fixed number of workers.
* **Test Cases**: [Test cases](./test_cases.md#1-parallel-map).
* **C++ Code**
  ```cpp
  template <class T, class Fn>
  auto parallelMap(const vector<T>& input, Fn fn, int workers) {
      using R = decltype(fn(input[0]));
      vector<R> out(input.size());
      vector<thread> ts;
      int n = input.size();
      int p = max(1, workers);
      for (int w = 0; w < p; ++w) {
          int l = w * n / p, r = (w + 1) * n / p;
          ts.emplace_back([&, l, r] {
              for (int i = l; i < r; ++i) out[i] = fn(input[i]);
          });
      }
      for (thread& t : ts) t.join();
      return out;
  }
  ```
* **Code Explanation**: Each worker owns a disjoint index range and writes only that output range.
* **Invariants**: Every output index is assigned by exactly one worker.
* **Complexity**: Work `O(n)`, span about `O(n/p + overhead)`, space `O(n + p)`.
* **Optimizations**: Runtime: chunking reduces scheduling overhead. Memory: contiguous writes improve locality.
* **Edge Cases To Consider**: Empty input contract, workers greater than `n`, throwing function policy.
* **L7 Follow-ups**: Discuss false sharing and when thread startup dominates.

## 2. Parallel Reduce

* **Pattern / Idea**: Local partial reductions plus final combine.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `NVIDIA: High`, `CoreWeave: High`, `Databricks: High`, `Snowflake: High`, `Google: Medium`, `OpenAI: Medium`.
* **Question**: Implement parallel reduce for an associative operation.
* **Test Cases**: [Test cases](./test_cases.md#2-parallel-reduce).
* **C++ Code**
  ```cpp
  template <class T, class Op>
  T parallelReduce(const vector<T>& input, T identity, Op op, int workers) {
      int n = input.size(), p = max(1, workers);
      vector<T> partial(p, identity);
      vector<thread> ts;
      for (int w = 0; w < p; ++w) {
          int l = w * n / p, r = (w + 1) * n / p;
          ts.emplace_back([&, w, l, r] {
              T acc = identity;
              for (int i = l; i < r; ++i) acc = op(acc, input[i]);
              partial[w] = acc;
          });
      }
      for (thread& t : ts) t.join();
      T ans = identity;
      for (const T& x : partial) ans = op(ans, x);
      return ans;
  }
  ```
* **Code Explanation**: Workers reduce disjoint ranges independently, then combine partials.
* **Invariants**: Each partial summarizes exactly one input partition.
* **Complexity**: Work `O(n)`, span `O(n/p + p)`, space `O(p)`.
* **Optimizations**: Runtime: tree-combine partials for large `p`. Memory: align partials to avoid false sharing.
* **Edge Cases To Consider**: Empty input, non-associative operation warning, one worker, many workers.
* **L7 Follow-ups**: Parallel reduction requires associative operations for deterministic regrouping.

## 3. Parallel Prefix Sum

* **Pattern / Idea**: Block scan, scan block totals, add offsets.
* **Company Frequency Tags**: Public signal: `Google: Low (all 5.0)`; Domain fit: `NVIDIA: High`, `CoreWeave: High`, `Databricks: High`, `Snowflake: Medium`, `OpenAI: Medium`.
* **Question**: Implement parallel prefix sum.
* **Test Cases**: [Test cases](./test_cases.md#3-parallel-prefix-sum).
* **C++ Code**
  ```cpp
  vector<long long> parallelPrefixSum(const vector<int>& a, int workers) {
      int n = a.size(), p = max(1, workers);
      vector<long long> out(n), totals(p, 0);
      vector<thread> ts;
      for (int w = 0; w < p; ++w) {
          int l = w * n / p, r = (w + 1) * n / p;
          ts.emplace_back([&, w, l, r] {
              long long sum = 0;
              for (int i = l; i < r; ++i) {
                  sum += a[i];
                  out[i] = sum;
              }
              totals[w] = sum;
          });
      }
      for (thread& t : ts) t.join();
      long long offset = 0;
      for (int w = 0; w < p; ++w) {
          long long block = totals[w];
          totals[w] = offset;
          offset += block;
      }
      ts.clear();
      for (int w = 0; w < p; ++w) {
          int l = w * n / p, r = (w + 1) * n / p;
          ts.emplace_back([&, w, l, r] {
              for (int i = l; i < r; ++i) out[i] += totals[w];
          });
      }
      for (thread& t : ts) t.join();
      return out;
  }
  ```
* **Code Explanation**: Local scans are adjusted by the sum of all previous blocks.
* **Invariants**: After offset add, `out[i]` equals sum of `a[0..i]`.
* **Complexity**: Work `O(n + p)`, span about `O(n/p + p)`, space `O(n + p)`.
* **Optimizations**: Runtime: parallelize the totals scan for very large `p`. Memory: in-place variant if input can be overwritten.
* **Edge Cases To Consider**: Empty input, negative values, workers > n, large sums.
* **L7 Follow-ups**: Work-efficient tree scan has better theoretical span but more complex implementation.

## 4. Parallel Top K

* **Pattern / Idea**: Local top-K heaps then merge candidates.
* **Company Frequency Tags**: Public signal: `Meta: High (6m 93.6)`, `Apple: High (6m 69.3)`, `Microsoft: Medium (6m 52.4)`, `Oracle: Medium (6m 47.0)`, `NVIDIA: Medium (all 59.6)`; Domain fit: `CoreWeave: High`, `Databricks: High`, `Snowflake: High`, `Google: Medium`, `OpenAI: Medium`.
* **Question**: Given a very large log split into chunks, compute top `k` error codes in parallel.
* **Test Cases**: [Test cases](./test_cases.md#4-parallel-top-k).
* **C++ Code**
  ```cpp
  vector<int> parallelTopK(const vector<int>& a, int k, int workers) {
      int n = a.size(), p = max(1, workers);
      vector<vector<int>> local(p);
      vector<thread> ts;
      for (int w = 0; w < p; ++w) {
          int l = w * n / p, r = (w + 1) * n / p;
          ts.emplace_back([&, w, l, r] {
              priority_queue<int, vector<int>, greater<int>> pq;
              for (int i = l; i < r; ++i) {
                  pq.push(a[i]);
                  if (static_cast<int>(pq.size()) > k) pq.pop();
              }
              while (!pq.empty()) {
                  local[w].push_back(pq.top());
                  pq.pop();
              }
          });
      }
      for (thread& t : ts) t.join();
      priority_queue<int, vector<int>, greater<int>> global;
      for (auto& part : local) for (int x : part) {
          global.push(x);
          if (static_cast<int>(global.size()) > k) global.pop();
      }
      vector<int> ans;
      while (!global.empty()) { ans.push_back(global.top()); global.pop(); }
      reverse(ans.begin(), ans.end());
      return ans;
  }
  ```
* **Code Explanation**: The global top `k` must be contained in the union of local top `k` candidates.
* **Invariants**: Each local heap contains the best `k` values from its partition.
* **Complexity**: Work `O(n log k)`, space `O(p*k)`.
* **Optimizations**: Runtime: local heaps avoid shared contention. Memory: only keep candidates, not full partitions.
* **Edge Cases To Consider**: `k = 0` policy, duplicates, workers > n, negative values.
* **L7 Follow-ups**: For skewed data, dynamic work distribution may improve utilization.

## 5. Parallel Merge Sort With Cutoff

* **Pattern / Idea**: `parallel-sort`, `fork-join`; recursively split both the range and a finite execution budget, then merge sorted children with one reusable scratch buffer.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Sort an integer vector using bounded parallel merge sort, switching small ranges to sequential sorting.
* **Test Cases**: [Test cases](./test_cases.md#5-parallel-merge-sort-with-cutoff).
* **API Contract / Failure Model**: `parallelMergeSort(a, workers, cutoff)` sorts ascending in place. Both limits must be positive, even for empty input. The caller exclusively owns `a` during the call. At most `min(workers, a.size())` execution threads, including the calling thread, participate; the cutoff can reduce this further. Allocation and thread-creation failures propagate after every started child is joined. On failure the vector remains valid but may be partially reordered; there is no rollback. The integer implementation has no user callbacks and does not promise stability.
* **C++ Code**
  ```cpp
  class SortThreadJoin {
      thread& child_;
  public:
      explicit SortThreadJoin(thread& child) : child_(child) {}
      SortThreadJoin(const SortThreadJoin&) = delete;
      SortThreadJoin& operator=(const SortThreadJoin&) = delete;
      // Unwinding the other branch must still wait for this child.
      ~SortThreadJoin() { if (child_.joinable()) child_.join(); }
  };

  void mergeSortRange(vector<int>& a, vector<int>& scratch,
                      size_t lo, size_t hi, size_t budget, size_t cutoff) {
      // Stop spawning when work is small or this branch has no spare thread.
      if (hi - lo <= cutoff || budget == 1) {
          sort(a.begin() + lo, a.begin() + hi);
          return;
      }
      const size_t mid = lo + (hi - lo) / 2;
      exception_ptr leftError;
      {
          // Split both index ownership and the finite execution budget.
          thread left([&] {
              try {
                  mergeSortRange(a, scratch, lo, mid, budget / 2, cutoff);
              } catch (...) {
                  leftError = current_exception();
              }
          });
          SortThreadJoin join(left);
          mergeSortRange(a, scratch, mid, hi, budget - budget / 2, cutoff);
      }
      // Both disjoint halves are finished before the parent reads or merges them.
      if (leftError) rethrow_exception(leftError);
      merge(a.begin() + lo, a.begin() + mid,
            a.begin() + mid, a.begin() + hi, scratch.begin() + lo);
      copy(scratch.begin() + lo, scratch.begin() + hi, a.begin() + lo);
  }

  void parallelMergeSort(vector<int>& a, size_t workers, size_t cutoff) {
      if (workers == 0 || cutoff == 0)
          throw invalid_argument("workers and cutoff must be positive");
      if (a.size() < 2) return;
      vector<int> scratch(a.size());
      mergeSortRange(a, scratch, 0, a.size(), min(workers, a.size()), cutoff);
  }
  ```
* **Code Explanation**: Each branch owns disjoint input and scratch ranges. The left branch records its exception; the right branch runs on the current thread. The RAII guard joins the left branch both on success and while unwinding a right-branch failure. Only then may the parent merge. If both branches fail, the right-branch exception wins. A failed constructor creates no child to join; enclosing guards still join all previously started children.
* **Invariants**: Child budgets sum to the parent's budget, so live execution threads never exceed the root budget. No parent touches a child range until both children finish. Each successful merge produces a sorted permutation of its input range. `leftError` is read only after its writer is joined.
* **Complexity**: Time/work `O(n log n + q)`, where `q <= min(workers, n)` is the actual number of leaf tasks; at most `q-1` threads are created. Balanced compute span is `O((n/q) log(n/q + 1) + n)`, plus thread startup/join costs along the recursion. Sequential allocation/initialization of scratch is `O(n)` and the final sequential merge is `O(n)`, so span is not `O(log n)` even with many workers. Space `O(n + q)` for scratch and thread/recursion metadata (sum of balanced recursion depths is `O(q)`); OS thread stacks add platform-dependent storage for at most `q` threads.
* **Optimizations**: Runtime: tune cutoff to amortize thread creation; a persistent task pool avoids repeated startup, while parallel merging can reduce the serial merge bottleneck. Memory: reuse scratch across all recursion levels; ping-pong source/destination ranges remove a copy at the cost of more complex ownership.
* **Edge Cases To Consider**: Empty and singleton vectors, duplicate and extreme integers, cutoff larger than input, workers larger than input, invalid limits, and thread-creation failure at nested forks.
* **L7 Follow-ups**: Compare this bounded fork/join model with work stealing, discuss memory-bandwidth limits and NUMA placement, and measure thread count, cutoff sensitivity, and merge time separately. An iterative bottom-up sort avoids recursion but still needs bounded scheduling and exception-safe joins.

## 6. Parallel Histogram

* **Pattern / Idea**: `parallel-histogram`, `local-aggregation`; count disjoint chunks into private histograms and reduce them after joining.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Count integer keys in a fixed bucket range using a bounded number of workers without shared counter contention.
* **Test Cases**: [Test cases](./test_cases.md#6-parallel-histogram).
* **API Contract / Failure Model**: `parallelHistogram(keys, buckets, workers)` returns `size_t` counts for keys in `[0, buckets)`. `buckets` and `workers` must be positive; invalid limits throw `invalid_argument`, and any negative or too-large key throws `out_of_range` before threads start. Empty input returns a zero histogram. Input stays immutable. At most `min(workers, keys.size())` worker threads run. Allocation or thread-creation failures propagate after all started threads finish; no partial output escapes. Counts cannot overflow `size_t` because their sum cannot exceed the representable vector size. There are no callbacks.
* **C++ Code**
  ```cpp
  class HistogramThreads {
  public:
      vector<thread> threads;
      HistogramThreads() = default;
      HistogramThreads(const HistogramThreads&) = delete;
      HistogramThreads& operator=(const HistogramThreads&) = delete;
      ~HistogramThreads() {
          for (auto& t : threads) if (t.joinable()) t.join();
      }
  };

  vector<size_t> parallelHistogram(const vector<int>& keys,
                                   size_t buckets, size_t workers) {
      if (buckets == 0 || workers == 0)
          throw invalid_argument("buckets and workers must be positive");
      for (int key : keys)
          if (key < 0 || static_cast<size_t>(key) >= buckets)
              throw out_of_range("key outside bucket range");
      vector<size_t> result(buckets, 0);
      if (keys.empty()) return result;
      const size_t p = min(workers, keys.size());
      // Private rows avoid atomic increments on hot histogram buckets.
      vector<vector<size_t>> local(p, vector<size_t>(buckets, 0));
      {
          HistogramThreads group;
          group.threads.reserve(p);
          size_t lo = 0;
          for (size_t w = 0; w < p; ++w) {
              // Distribute the remainder without computing a large w*n product.
              const size_t hi = lo + keys.size() / p + (w < keys.size() % p);
              group.threads.emplace_back([&, w, lo, hi] {
                  for (size_t i = lo; i < hi; ++i)
                      ++local[w][static_cast<size_t>(keys[i])];
              });
              lo = hi;
          }
      }
      // The RAII group's joins publish every worker's private counts.
      for (const auto& part : local)
          for (size_t b = 0; b < buckets; ++b) result[b] += part[b];
      return result;
  }
  ```
* **Code Explanation**: Validate all keys before allocation of worker state. Quotient/remainder partitioning avoids multiplying large indices. Each worker exclusively writes one ordinary `vector<size_t>`; no atomic counter or packed `vector<bool>` is needed. All worker allocations happen before launch and the worker loop cannot throw for valid immutable input. The RAII group also joins already-created threads if a later launch fails.
* **Invariants**: Chunks are disjoint and cover the input exactly. Each local bucket is written by only one worker. Reduction starts after every count is visible through join; the sum of all final buckets equals the input length.
* **Complexity**: Time/work `O(n + p*b + b)`, with `b = buckets`, `p = min(workers, n)` for nonempty input. Counting alone takes `O(n/p)` per worker; actual span includes serial key validation `O(n)`, local-table initialization and serial reduction `O(p*b)`, output initialization `O(b)`, and `O(p)` launches/joins. Thus this implementation does not achieve end-to-end `O(n/p)` time. Space `O(p*b + b + p)` plus worker stacks; for empty input time/space are `O(b)`.
* **Optimizations**: Runtime: parallelize validation and bucket reduction and reuse workers/tables between batches; alignment can reduce false sharing for tiny rows. Memory: use sparse per-worker maps when the domain is large and sparse, accepting hash overhead and less predictable locality.
* **Edge Cases To Consider**: Empty input, a single bucket, severe key skew, boundary keys, invalid bucket/worker limits, oversized worker requests, and allocation/launch failure.
* **L7 Follow-ups**: Compare local tables against shared atomic counters under uniform versus skewed keys. Track memory bandwidth, reduction cost, NUMA locality, and counter-width requirements across streamed batches.

## 7. Bounded Fanout/Fanin With Cooperative Cancellation

* **Pattern / Idea**: `fanout-fanin`, `bounded-concurrency`; a mutex assigns each task once, and the first recorded error publishes a shared cancellation token.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Execute independent integer-returning tasks with bounded concurrency, preserving result order and requesting early cancellation on failure.
* **Test Cases**: [Test cases](./test_cases.md#7-bounded-fanoutfanin-with-cooperative-cancellation).
* **API Contract / Failure Model**: `boundedFanoutFanin(tasks, workers)` requires positive workers and nonempty callables; invalid arguments throw before any task runs. An empty task list succeeds with an empty result. At most `min(workers, tasks.size())` worker threads run. A task is `int(const atomic<bool>& stop)` and must return or throw in finite time, polling `stop` during long work for prompt cancellation. Distinct tasks run concurrently: shared captures and external effects must be synchronized and retry-safe by the caller. Tasks may throw; the first exception recorded under the scheduler mutex, including a launch exception, is rethrown after every thread joins. No more tasks are claimed after cancellation is published; tasks already claimed may still begin or finish. No rollback, timeout, retry, forced interruption, or cancellation-latency guarantee is provided. Neither task arguments nor the token may be retained past the call, and the caller must not mutate the task list during execution.
* **C++ Code**
  ```cpp
  class FanoutThreads {
  public:
      vector<thread> threads;
      FanoutThreads() = default;
      FanoutThreads(const FanoutThreads&) = delete;
      FanoutThreads& operator=(const FanoutThreads&) = delete;
      ~FanoutThreads() {
          for (auto& t : threads) if (t.joinable()) t.join();
      }
  };

  vector<int> boundedFanoutFanin(
      const vector<function<int(const atomic<bool>&)>>& tasks, size_t workers) {
      if (workers == 0) throw invalid_argument("workers must be positive");
      for (const auto& task : tasks)
          if (!task) throw invalid_argument("empty task");
      vector<int> result(tasks.size());
      if (tasks.empty()) return result;
      const size_t p = min(workers, tasks.size());
      mutex gate;
      size_t next = 0;
      atomic<bool> stop{false};
      exception_ptr firstError;
      auto fail = [&](exception_ptr error) {
          lock_guard<mutex> lock(gate);
          if (!firstError) firstError = error;
          // The token carries no result data; joins publish completed results.
          stop.store(true, memory_order_relaxed);
      };
      auto run = [&] {
          try {
              for (;;) {
                  size_t index;
                  {
                      lock_guard<mutex> lock(gate);
                      // Checking cancellation and claiming work share one lock.
                      if (stop.load(memory_order_relaxed) || next == tasks.size())
                          return;
                      index = next++;
                  }
                  // Each index has one writer; slow tasks never hold the gate.
                  result[index] = tasks[index](stop);
              }
          } catch (...) {
              fail(current_exception());
          }
      };
      {
          FanoutThreads group;
          group.threads.reserve(p);
          try {
              for (size_t w = 0; w < p; ++w)
                  group.threads.emplace_back(run);
          } catch (...) {
              // Stop already-started workers if a later launch fails.
              fail(current_exception());
          }
      }
      // The group has joined all workers, including on the error path.
      if (firstError) rethrow_exception(firstError);
      return result;
  }
  ```
* **Code Explanation**: Claiming a task and checking stop use the same mutex as error publication, defining a clear cancellation boundary. Callbacks run outside the lock. Each claimed index has one writer; integer result slots are separate memory locations. Relaxed atomics suffice because the token only requests cancellation; exception state uses the mutex and result publication uses joins. Thread-launch failure also sets stop before the group joins. Ordinary callback exceptions are caught inside each worker, never allowed to escape a thread entry point. Synchronization primitives are assumed operational; process crashes and `terminate` inside callbacks are outside this model.
* **Invariants**: `next` increases once per claim under the mutex; every task is invoked at most once. No claim occurs after stop publication. Active callbacks never exceed `p`; all callbacks have ended before return or rethrow, and successful output follows input order regardless of completion order.
* **Complexity**: Time/work on success `O(n + p + sum(task work))`; serial callable validation and result initialization cost `O(n)`, thread startup/fanin `O(p)`, and the mutex serializes `O(n)` claims. With bounded scheduling costs, the usual load-balance estimate is `O(n + p + sum(task time)/p + max(task time))`; lock contention and external I/O can worsen it. Span includes setup, serialized claims, and the longest dependent task, not just the slowest callback. On failure, work includes all claimed tasks and fanin waits for the longest active task's remaining work. Space `O(n + p)` for results and thread metadata plus callback-owned state and worker stacks.
* **Optimizations**: Runtime: reuse a pool, batch claims for tiny tasks, and add deadlines to external I/O; batching permits more already-claimed work at cancellation. Memory: stream tagged completions to a consumer when ordered materialization is unnecessary, keeping a bounded queue for backpressure.
* **Edge Cases To Consider**: No tasks, empty callable, zero/oversized worker count, out-of-order completion, simultaneous exceptions, failure during partial thread startup, and callbacks that delay or ignore cancellation.
* **L7 Follow-ups**: Add explicit per-task outcome records for partial results, admission limits across concurrent requests, cancellation-latency metrics, and an external idempotency contract. Returning quickly requires tasks that cooperate or bounded external I/O; detaching still-running threads would invalidate captured state.

## 8. Parallel BFS

* **Pattern / Idea**: `parallel-bfs`, `frontier`; expand one BFS level at a time, atomically claim each vertex, then join and combine private next-frontier lists.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Compute shortest hop distances from one source in an unweighted directed graph with bounded parallel frontier expansion.
* **Test Cases**: [Test cases](./test_cases.md#8-parallel-bfs).
* **API Contract / Failure Model**: `parallelBfs(graph, source, workers)` uses vertex IDs `[0, graph.size())`, returns `size_t` distances, and represents unreachable vertices by `bfsUnreachable`. Workers must be positive (`invalid_argument`); an invalid source, including any source in an empty graph, or any invalid edge endpoint anywhere in the graph throws `out_of_range` before launching. Self-loops, duplicate edges, and disconnected components are allowed. Graph storage must stay immutable. Each level starts at most `min(workers, frontier.size())` worker threads. Allocation, worker, or launch failures propagate only after all started threads join; partial internal state is discarded. No callbacks or concurrent graph mutation are supported; frontier order is deliberately unspecified.
* **C++ Code**
  ```cpp
  constexpr size_t bfsUnreachable = numeric_limits<size_t>::max();

  class BfsThreads {
  public:
      vector<thread> threads;
      BfsThreads() = default;
      BfsThreads(const BfsThreads&) = delete;
      BfsThreads& operator=(const BfsThreads&) = delete;
      ~BfsThreads() {
          for (auto& t : threads) if (t.joinable()) t.join();
      }
  };

  vector<size_t> parallelBfs(const vector<vector<size_t>>& graph,
                             size_t source, size_t workers) {
      if (workers == 0) throw invalid_argument("workers must be positive");
      if (source >= graph.size()) throw out_of_range("invalid source");
      for (const auto& edges : graph)
          for (size_t v : edges)
              if (v >= graph.size()) throw out_of_range("invalid endpoint");
      vector<atomic<size_t>> distance(graph.size());
      for (auto& d : distance) d.store(bfsUnreachable, memory_order_relaxed);
      distance[source].store(0, memory_order_relaxed);
      vector<size_t> frontier{source};
      size_t depth = 0;
      while (!frontier.empty()) {
          const size_t p = min(workers, frontier.size());
          // Only worker w appends to local[w] or writes errors[w].
          vector<vector<size_t>> local(p);
          vector<exception_ptr> errors(p);
          {
              BfsThreads group;
              group.threads.reserve(p);
              size_t lo = 0;
              for (size_t w = 0; w < p; ++w) {
                  const size_t hi = lo + frontier.size() / p
                                      + (w < frontier.size() % p);
                  group.threads.emplace_back([&, w, lo, hi] {
                      try {
                          for (size_t i = lo; i < hi; ++i) {
                              for (size_t v : graph[frontier[i]]) {
                                  size_t expected = bfsUnreachable;
                                  // One winner discovers v, even with duplicate edges.
                                  // All contenders belong to the same BFS level.
                                  if (distance[v].compare_exchange_strong(
                                          expected, depth + 1, memory_order_relaxed))
                                      local[w].push_back(v);
                              }
                          }
                      } catch (...) {
                          errors[w] = current_exception();
                      }
                  });
                  lo = hi;
              }
          }
          // Joins form the level barrier before depth/frontier can change.
          for (const auto& error : errors)
              if (error) rethrow_exception(error);
          size_t count = 0;
          for (const auto& part : local) count += part.size();
          vector<size_t> next;
          next.reserve(count);
          for (const auto& part : local)
              next.insert(next.end(), part.begin(), part.end());
          frontier = std::move(next);
          ++depth;
      }
      vector<size_t> result(graph.size());
      for (size_t v = 0; v < graph.size(); ++v)
          result[v] = distance[v].load(memory_order_relaxed);
      return result;
  }
  ```
* **Code Explanation**: Complete validation includes unreachable components. Atomic distances combine visited marking and distance assignment without packed-bit races. Only the winner of the unreachable-to-distance CAS enqueues a vertex, using its worker-owned vector. Joining provides the level barrier before combining lists or changing `depth`/`frontier`; relaxed CAS is sufficient for claiming because graph data is immutable and all other publication uses joins. A failed local allocation is caught; other workers finish and the whole traversal throws instead of returning incomplete distances. Launch failure unwinds through the same joining guard.
* **Invariants**: Every current frontier vertex has distance `depth`. Each vertex enters a frontier at most once, and its first successful discovery has shortest hop distance. Adjacent levels never overlap execution. Every non-source reachable distance is at most `V-1`, so `depth + 1` is representable and never assigned the unreachable sentinel to a newly discovered vertex.
* **Complexity**: Time/work `O(V + E)` under the conventional unit-cost atomic model, including serial validation, distance initialization, and copying final distances. For level `l`, let `p_l = min(workers, frontier size)` and `E_l,w` be the edges assigned to worker `w`. Expansion span includes `sum_l max_w E_l,w`, plus contention, `O(sum_l p_l)` launches/joins, and sequential frontier combination `O(V)`. Validation alone is `O(V + E)` serial, so this sample has no end-to-end ideal `O((V+E)/p)` guarantee. A high-degree vertex is not split and can dominate a level; atomic operations are not guaranteed lock-free by C++17. Space `O(V + p_max)` auxiliary, excluding `O(V+E)` input and platform worker stacks; uniqueness bounds the combined local frontiers by `V`.
* **Optimizations**: Runtime: reuse a pool with level barriers, validate trusted graph storage once, split high-degree edge ranges, or use direction-optimizing BFS on dense frontiers. Memory: reuse frontier buffers and use CSR graph storage for locality; a compact atomic bitmap requires explicit word-level synchronization and is not interchangeable with `vector<bool>`.
* **Edge Cases To Consider**: Empty graph, invalid source/edge in any component, zero/oversized workers, isolated source, cycles, self-loops, duplicate edges, diamond-shaped contention, long chains, and allocation/launch failures.
* **L7 Follow-ups**: Measure level widths, edge skew, CAS contention, startup overhead, and memory bandwidth. Discuss deterministic parent selection, concurrent graph snapshots, and distributed frontier exchange with partition ownership and per-level termination detection.

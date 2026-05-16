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

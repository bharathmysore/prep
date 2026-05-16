# Parallel Algorithms Coding Questions

Solve each question in C++ with work, span, coordination overhead, contention points, and speedup limits.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Implement parallel map over a vector with a fixed number of workers.
   - Expected pattern: chunking plus worker threads.
   - Pattern tags: `parallel-map`, `chunking`.
   - Solution: [Parallel Map](./solutions.md#1-parallel-map).
   - Complexity target: work `O(n)`, span roughly `O(n/p + overhead)`, space `O(n)` output.

2. Implement parallel reduce for an associative operation.
   - Expected pattern: local partials plus final combine.
   - Pattern tags: `parallel-reduce`, `local-partials`.
   - Solution: [Parallel Reduce](./solutions.md#2-parallel-reduce).
   - Complexity target: work `O(n)`, span `O(n/p + log p)`, space `O(p)`.

3. Implement parallel prefix sum.
   - Expected pattern: block scan plus scan of block totals.
   - Pattern tags: `parallel-prefix-scan`, `block-scan`.
   - Solution: [Parallel Prefix Sum](./solutions.md#3-parallel-prefix-sum).
   - Complexity target: work `O(n)`, practical span `O(n/p + p)`, space `O(p)`.

4. Implement parallel merge sort with a cutoff to sequential sort.
   - Expected pattern: fork/join divide and conquer.
   - Pattern tags: `parallel-sort`, `fork-join`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: work `O(n log n)`, space `O(n)` scratch, span depends on merge strategy.

<a id="4-parallel-top-k"></a>
5. Given a very large log split into chunks, compute top `k` error codes in parallel.
   - Expected pattern: local maps plus top-k merge.
   - Pattern tags: `parallel-top-k`, `local-aggregation`.
   - Solution: [Parallel Top K](./solutions.md#4-parallel-top-k).
   - Complexity target: work `O(n)`, merge `O(unique log k)`, space `O(p * localUnique)`.

6. Implement a parallel histogram for integer keys.
   - Expected pattern: per-worker histograms plus reduction.
   - Pattern tags: `parallel-histogram`, `local-aggregation`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: work `O(n + p*buckets)`, space `O(p*buckets)`.

7. Implement fanout/fanin execution with bounded concurrency and early cancellation on first failure.
   - Expected pattern: task queue, futures, cancellation flag.
   - Pattern tags: `fanout-fanin`, `bounded-concurrency`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: work sum of tasks, span bounded by slowest active tasks and scheduling overhead.

8. Implement parallel BFS for an unweighted graph level by level.
   - Expected pattern: frontier expansion plus atomic or synchronized visited marking.
   - Pattern tags: `parallel-bfs`, `frontier`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: work `O(V + E)`, space `O(V)`, synchronization once per level.

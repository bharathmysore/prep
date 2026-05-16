# Parallel Algorithms Coding Patterns

Parallel algorithms test decomposition, coordination overhead, and speedup limits. State the work, span, contention points, and merge cost.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Parallel map | Independent transform per element | Each output slot is written by exactly one worker | Work `O(n)`, span `O(n/p + overhead)`, space `O(n)` output | Runtime: chunk work to reduce scheduling overhead. Memory: write contiguous chunks. |
| Parallel reduce | Associative combine operation | Partial results summarize disjoint partitions | Work `O(n)`, span `O(n/p + log p)`, space `O(p)` | Runtime: tree reduction. Memory: one partial per worker avoids contention. |
| Parallel prefix scan | Need prefix values with associative op | Up-sweep/down-sweep preserve segment aggregates | Work `O(n)`, span `O(log n)` ideal, space `O(n)` or `O(p)` | Runtime: block scan plus prefix of block sums. Memory: in-place scan when allowed. |
| Parallel merge sort | Divide and conquer sorting | Each subarray is sorted before merge | Work `O(n log n)`, span depends on merge, space `O(n)` | Runtime: cutoff to sequential sort. Memory: reuse scratch buffer. |
| Parallel quicksort | Partition then sort subranges | Partition places pivot in final position | Average work `O(n log n)`, space `O(log n)` tasks plus array | Runtime: random pivot and task cutoff. Memory: in-place partition. |
| Parallel top K | Top K from many partitions | Global top K is top K of partition top K candidates | Work `O(n log k)`, space `O(p * k)` | Runtime: local heaps then merge. Memory: keep only local top K. |
| Parallel histogram | Count frequencies over large data | Local histograms partition writes by worker | Work `O(n + p*buckets)`, space `O(p*buckets)` | Runtime: reduce local histograms. Memory: sparse local maps for large key spaces. |
| Parallel BFS | Frontier expansion by levels | All nodes in current frontier have same distance | Work `O(V + E)`, span per level plus sync, space `O(V)` | Runtime: atomic visited or frontier dedupe. Memory: bitsets for visited/frontier. |
| Fanout/fanin task execution | Independent remote or CPU tasks | Aggregator waits for all required results or cancellation | Work sum of tasks, span max task plus fanin, space `O(tasks)` | Runtime: bounded parallelism. Memory: stream results instead of retaining all. |
| Work stealing scheduler model | Irregular recursive tasks | Each task is owned by one deque or worker at a time | Work task-dependent, space `O(tasks)` | Runtime: steal from opposite end. Memory: task granularity cutoff. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `parallel-map`, `chunking` | chunking plus worker threads | [questions](./questions.md): Q1 |
| `parallel-reduce`, `local-partials` | local partials plus final combine | [questions](./questions.md): Q2 |
| `parallel-prefix-scan`, `block-scan` | block scan plus scan of block totals | [questions](./questions.md): Q3 |
| `parallel-sort`, `fork-join` | fork/join divide and conquer | [questions](./questions.md): Q4 |
| `parallel-top-k`, `local-aggregation` | local maps plus top-k merge | [questions](./questions.md): Q5 |
| `parallel-histogram`, `local-aggregation` | per-worker histograms plus reduction | [questions](./questions.md): Q6 |
| `fanout-fanin`, `bounded-concurrency` | task queue, futures, cancellation flag | [questions](./questions.md): Q7 |
| `parallel-bfs`, `frontier` | frontier expansion plus atomic or synchronized visited marking | [questions](./questions.md): Q8 |

## L7 Follow-Ups

- What is the theoretical speedup, and what limits it?
- Where do false sharing and memory bandwidth dominate?
- How do you choose task granularity?
- How do you cancel outstanding work after a failure?

# Parallel Algorithms Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Parallel Map

* **Question**: Implement parallel map over a vector with a fixed number of workers.
* **Solution**: [Parallel Map](./solutions.md#1-parallel-map).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Order preservation | input `[1,2,3]`, square function, workers 2 | Return `[1,4,9]` in input order. |
| Empty input | `[]` | Return empty output without starting unnecessary work. |
| Exception policy | mapper throws for one item | Error is propagated or captured according to API. |

## 2. Parallel Reduce

* **Question**: Implement parallel reduce for an associative operation.
* **Solution**: [Parallel Reduce](./solutions.md#2-parallel-reduce).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Sum | input `[1,2,3,4]`, identity 0 | Return `10`. |
| Empty | empty input | Return identity. |
| Associativity requirement | non-associative combine | Document that result may differ by partitioning. |

## 3. Parallel Prefix Sum

* **Question**: Implement parallel prefix sum.
* **Solution**: [Parallel Prefix Sum](./solutions.md#3-parallel-prefix-sum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[1,2,3,4]` | Return `[1,3,6,10]`. |
| Single element | `[5]` | Return `[5]`. |
| Large input | many values split across workers | Matches sequential prefix exactly. |

## 4. Parallel Top K

* **Question**: Given a very large log split into chunks, compute top `k` error codes in parallel.
* **Solution**: [Parallel Top K](./solutions.md#4-parallel-top-k).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Basic | input `[5,1,9,3,7]`, k=3 | Return top values `[9,7,5]`. |
| k larger than n | k > input size | Return all values sorted by rank. |
| Skewed partitions | largest values concentrated in one partition | Global merge still returns correct top k. |

## 5. Parallel Merge Sort With Cutoff

* **Question**: Sort an integer vector using bounded parallel merge sort with a sequential cutoff.
* **Solution**: [Parallel Merge Sort With Cutoff](./solutions.md#5-parallel-merge-sort-with-cutoff).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Boundaries | `[]`, `[7]`, positive workers/cutoff | No change and no child launches. |
| Values and permutation | `[INT_MAX,0,-1,INT_MIN,0]`, workers 3, cutoff 1 | `[INT_MIN,-1,0,0,INT_MAX]`; no lost/duplicated elements. |
| Sequential paths | Reverse-sorted values; workers 1 or cutoff greater than length | Matches `std::sort`; no child launches. |
| Excess workers | Two values, workers 100, cutoff 1 | Sorted result; at most two execution threads including caller. |
| Invalid configuration | Workers 0 or cutoff 0, including empty input | `invalid_argument` before any launch. |
| Nested launch failure | Inject failure on first or later thread construction | `system_error` reaches caller only after every started child finishes; vector remains valid, ordering unspecified. |
| Stress | Random duplicate-heavy arrays, varying lengths, workers and cutoff | Exact equality with sequential sort; peak children at most root budget minus one. |

## 6. Parallel Histogram

* **Question**: Count integer keys using private per-worker histograms and a final reduction.
* **Solution**: [Parallel Histogram](./solutions.md#6-parallel-histogram).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Empty | `[]`, buckets 3, workers 2 | `[0,0,0]`; no worker launches. |
| Bucket boundaries | `[0,2,0,1,2]`, buckets 3, workers 2 | `[2,1,2]`; counts sum to 5. |
| Severe skew | 10,000 zero keys, buckets 1, workers 7 | `[10000]`; no shared-counter lost updates. |
| Excess workers | `[1]`, buckets 2, workers 100 | `[0,1]`; one worker. |
| Invalid limits | Buckets 0 or workers 0, including empty input | `invalid_argument` before launch. |
| Invalid key | `[-1]` or `[3]`, buckets 3 | `out_of_range` before launch, input unchanged. |
| Partial startup failure | Inject launch failure after one or more starts | Exception propagated after all started workers join; no output returned. |
| Impossible allocation size | Empty input, buckets `SIZE_MAX`, workers 1 | `length_error` before launch; no output returned. |
| Randomized reduction | Random in-range keys across bucket counts and worker counts | Every bucket matches sequential counts; input unchanged. |

## 7. Bounded Fanout/Fanin With Cooperative Cancellation

* **Question**: Execute ordered tasks with bounded concurrency and cooperative early cancellation.
* **Solution**: [Bounded Fanout/Fanin With Cooperative Cancellation](./solutions.md#7-bounded-fanoutfanin-with-cooperative-cancellation).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Empty | No tasks, workers 4 | Empty result, no launches. |
| Ordering | Tasks returning 0 through 19 finish out of order, workers 3 | Output `[0,1,...,19]`; each task invoked once, peak active callbacks at most 3. |
| Invalid arguments | Workers 0 or an empty `std::function` among valid tasks | `invalid_argument`; no task invoked, including valid preceding tasks. |
| Single-worker failure | First task throws, followed by ten side-effect counters, workers 1 | Original exception; later tasks never invoked. |
| Cooperative stop | Two active tasks synchronize; one throws, the other polls stop; more tasks queued | Poller observes stop and exits; queued tasks never claimed; no callback active at rethrow. |
| Simultaneous failure | Two callbacks throw concurrently | One recorded exception propagated, all workers joined; wall-clock ordering is not promised. |
| Callback allocation failure | Callback throws `bad_alloc` | Same exception type propagated after joins; no partial result. |
| Partial startup failure | First callback polls stop; second thread construction throws | Stop is published, poller exits, launch exception propagates after join. |
| Delayed cooperation | A claimed task finishes despite stop | Failure still waits for that task; no partial result or rollback promised. |
| Excess workers | One task, workers 100 | Exactly one invocation and at most one worker. |

## 8. Parallel BFS

* **Question**: Compute shortest hop distances in an immutable unweighted graph using level frontiers.
* **Solution**: [Parallel BFS](./solutions.md#8-parallel-bfs).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Isolated source | Graph `[[],[]]`, source 0 | `[0,bfsUnreachable]`. |
| Diamond and cycle | Graph `[[1,2],[3],[3],[0],[]]`, source 0, workers 3 | `[0,1,1,2,bfsUnreachable]`; shared child discovered once. |
| Duplicate/self edges | Graph `[[0,1,1],[1,2,2],[]]`, source 0 | `[0,1,2]`; no duplicate frontier entries or infinite loop. |
| Long chain | 50-vertex directed chain, source 0, workers 8 | Distances 0 through 49; one frontier vertex per level. |
| Invalid source | Empty graph with source 0; source equal to graph size | `out_of_range` before launch. |
| Unreachable invalid endpoint | Graph `[[],[2]]`, source 0 | `out_of_range` even though invalid edge is unreachable. |
| Invalid workers | Workers 0 on any graph | `invalid_argument` before launch. |
| Launch failure | Inject a failure during a multi-worker level after earlier levels succeeded | Exception propagated after started workers join; partial distances are not returned. |
| Random/skewed graphs | Directed graphs with disconnected vertices, high-degree hubs, repeated edges | Exact distance equality with sequential BFS; peak worker count bounded by requested workers and level width. |

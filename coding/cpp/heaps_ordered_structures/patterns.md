# Heaps And Ordered Structures Coding Patterns

Use heaps for repeated best-candidate extraction. Use ordered maps and sets when order, predecessor, successor, or range queries matter.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Kth largest element | Need rank, not full sort | Min-heap keeps largest `k` values seen | Time `O(n log k)`, space `O(k)` | Runtime: quickselect gives average `O(n)`. Memory: heap bounds state. |
| Merge k sorted arrays/lists | Many sorted streams | Heap contains next candidate from each stream | Time `O(N log k)`, space `O(k)` | Runtime: push only next from popped stream. Memory: store indices instead of copies. |
| Streaming median | Need median after each update | Lower heap has max lower half, upper heap has min upper half | Time `O(log n)` update, space `O(n)` | Runtime: rebalance by at most one item. Memory: store all values unless approximate median allowed. |
| Task scheduler with cooldown | Repeated tasks with cooldown | Max-heap selects best available task, cooldown queue delays reuse | Time `O(n log k)`, space `O(k)` | Runtime: jump idle time when possible. Memory: count frequencies only. |
| Meeting rooms II | Intervals requiring capacity | Min-heap contains active meeting end times | Time `O(n log n)`, space `O(n)` | Runtime: sort starts once. Memory: heap size equals max rooms. |
| Sliding window median | Ordered multiset over moving window | Data structure contains exactly current window | Time `O(n log k)`, space `O(k)` | Runtime: two multisets or lazy heap deletion. Memory: lazy deletion map can grow if not cleaned. |
| Skyline problem | Sweep line with active heights | Ordered multiset contains heights of buildings crossing current x | Time `O(n log n)`, space `O(n)` | Runtime: events sorted by x with tie rules. Memory: count duplicate heights. |
| Calendar booking | Need detect interval overlap | Ordered starts let predecessor and successor bound overlap | Time `O(log n)` per booking, space `O(n)` | Runtime: `std::map` lower_bound. Memory: store only accepted intervals. |
| Top K frequent words | Frequency plus lexical tie-break | Heap ordering reflects worst retained candidate | Time `O(n log k)`, space `O(unique)` | Runtime: custom comparator. Memory: count map dominates. |
| Dijkstra with heap | Repeated nearest unsettled node | Heap minimum has smallest tentative distance | Time `O((V + E) log V)`, space `O(V + E)` | Runtime: lazy stale-entry skip. Memory: no decrease-key structure needed. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `heap`, `top-k` | min-heap of size `k` or quickselect | [questions](./questions.md): Q1 |
| `two-heaps`, `streaming-median` | max-heap for lower half and min-heap for upper half | [questions](./questions.md): Q2 |
| `heap`, `k-way-merge` | min-heap of stream heads | [questions](./questions.md): Q3 |
| `min-heap`, `sweep-line` | sort by start and min-heap of end times | [questions](./questions.md): Q4 |
| `ordered-map`, `intervals` | ordered map with predecessor and successor checks | [questions](./questions.md): Q5 |
| `heap`, `top-k`, `custom-comparator` | hash counts plus custom heap or ordered set | [questions](./questions.md): Q6 |
| `sliding-window-median`, `ordered-multiset` | ordered active window split into lower and upper halves | [questions](./questions.md): Q7 |

## L7 Follow-Ups

- When does quickselect beat a heap, and when is heap stability preferable?
- How do duplicate keys affect ordered sets and maps?
- What are the memory costs of lazy deletion?
- How would you shard or approximate top-K for streaming data?

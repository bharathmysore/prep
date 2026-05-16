# Advanced Data Structures Coding Patterns

These patterns are high-signal for L6/L7 because they combine API design, invariants, and operational tradeoffs. Implement in C++ with clear ownership and small helper methods.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| LRU cache | Bounded cache evicts least recently used key | List order matches recency and map points to list nodes | Time `O(1)` average per op, space `O(capacity)` | Runtime: splice list nodes. Memory: store key in node for eviction. |
| LFU cache | Evict least frequently used, tie by recency | Each key belongs to exactly one frequency bucket | Time `O(1)` average per op, space `O(capacity)` | Runtime: track min frequency. Memory: bucket lists plus key metadata. |
| TTL cache | Values expire by time | Returned entries are unexpired and latest for key | Average time `O(1)` get, `O(log n)` expiry heap update, space `O(n)` | Runtime: lazy expiry. Memory: heap may hold stale versions unless compacted. |
| Segment tree range query | Many range queries and point/range updates | Each node summarizes exactly its interval | Build `O(n)`, query/update `O(log n)`, space `O(n)` | Runtime: iterative tree improves constants. Memory: `2n` iterative layout. |
| Fenwick tree | Prefix sums with point updates | Tree node stores range ending at index based on lowbit | Update/query `O(log n)`, space `O(n)` | Runtime: tight loops. Memory: one array. |
| Disjoint set union | Dynamic connectivity | Parent root represents component identity | Amortized `O(alpha(n))`, space `O(n)` | Runtime: path compression and union by size. Memory: parent and size arrays. |
| Trie or radix tree | Prefix-heavy key operations | Path from root represents key prefix | Time `O(key length)`, space `O(total chars)` | Runtime: arrays for dense alphabets. Memory: sparse maps or compressed edges. |
| Ordered interval map | Need assign/query ranges compactly | Adjacent intervals with same value are merged | Time `O(log n + changed intervals)`, space `O(intervals)` | Runtime: split only boundaries touched. Memory: coalesce neighbors. |
| Rolling metrics window | Need moving counts/sums over time | Buckets cover the active time window exactly once | Update `O(1)`, query `O(buckets)`, space `O(buckets)` | Runtime: fixed buckets. Memory: bucket granularity trades accuracy for space. |
| Sharded counter/map | High write concurrency | Each shard owns disjoint subset of keys or increments | Time `O(1)` average with lower contention, space `O(shards + keys)` | Runtime: shard by hash. Memory: tune shard count to avoid overhead. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `lru-cache`, `hash-map`, `linked-list` | hash map plus doubly linked list | [questions](./questions.md): Q1 |
| `lfu-cache`, `frequency-buckets` | key metadata plus frequency buckets | [questions](./questions.md): Q2 |
| `ttl-cache`, `expiry-heap` | hash map plus expiry heap or ordered expiry map | [questions](./questions.md): Q3 |
| `fenwick-tree`, `range-query` | Fenwick tree or segment tree | [questions](./questions.md): Q4 |
| `interval-map`, `ordered-map` | ordered map with boundary splitting and coalescing | [questions](./questions.md): Q5 |
| `rolling-window`, `time-buckets` | fixed time buckets | [questions](./questions.md): Q6 |

## L7 Follow-Ups

- What are the API guarantees and eviction semantics?
- Which operations are average-case because of hashing?
- How would you make the structure thread-safe without one giant lock?
- How do you test invariants after random operation sequences?

# Advanced Data Structures Coding Questions

Solve each question in C++ with a clear API, representation invariant, and randomized or adversarial test plan.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

<a id="1-lru-cache"></a>
1. Implement an LRU cache with `get` and `put`.
   - Expected pattern: hash map plus doubly linked list.
   - Pattern tags: `lru-cache`, `hash-map`, `linked-list`.
   - Solution: [LRU Cache](./solutions.md#1-lru-cache).
   - Complexity target: average time `O(1)` per operation, space `O(capacity)`.

2. Implement an LFU cache with LRU tie-breaking.
   - Expected pattern: key metadata plus frequency buckets.
   - Pattern tags: `lfu-cache`, `frequency-buckets`.
   - Solution: [LFU Cache](./solutions.md#2-lfu-cache).
   - Complexity target: average time `O(1)` per operation, space `O(capacity)`.

3. Implement an in-memory TTL cache with `put`, `get`, and opportunistic cleanup.
   - Expected pattern: hash map plus expiry heap or ordered expiry map.
   - Pattern tags: `ttl-cache`, `expiry-heap`.
   - Solution: [TTL Cache](./solutions.md#3-ttl-cache).
   - Complexity target: average get `O(1)`, put `O(log n)` with heap, space `O(n)`.

4. Implement a range-sum structure with point updates.
   - Expected pattern: Fenwick tree or segment tree.
   - Pattern tags: `fenwick-tree`, `range-query`.
   - Solution: [Fenwick Tree](./solutions.md#4-fenwick-tree).
   - Complexity target: build `O(n)`, update/query `O(log n)`, space `O(n)`.

5. Implement an interval assignment map that stores non-overlapping ranges compactly.
   - Expected pattern: ordered map with boundary splitting and coalescing.
   - Pattern tags: `interval-map`, `ordered-map`.
   - Solution: [Interval Assignment Map](./solutions.md#5-interval-assignment-map).
   - Complexity target: time `O(log n + changed intervals)`, space `O(intervals)`.

6. Implement a rolling metrics window for counts and sums over the last five minutes.
   - Expected pattern: fixed time buckets.
   - Pattern tags: `rolling-window`, `time-buckets`.
   - Solution: [Rolling Metrics Window](./solutions.md#6-rolling-metrics-window).
   - Complexity target: update `O(1)`, query `O(buckets)`, space `O(buckets)`.

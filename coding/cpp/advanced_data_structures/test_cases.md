# Advanced Data Structures Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. LRU Cache

* **Question**: Implement an LRU cache with `get` and `put`.
* **Solution**: [LRU Cache](./solutions.md#1-lru-cache).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Eviction | capacity 2; put(1,1), put(2,2), get(1), put(3,3), get(2) | `get(2)` returns `-1`; key 2 evicted. |
| Update recency | put existing key then insert another key | Updated key remains most recent and value changes. |
| Capacity one | capacity 1; put A, put B | Only B remains. |

## 2. LFU Cache

* **Question**: Implement an LFU cache with LRU tie-breaking.
* **Solution**: [LFU Cache](./solutions.md#2-lfu-cache).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| LFU eviction | capacity 2; put(1,1), put(2,2), get(1), put(3,3) | Key 2 is evicted. |
| Tie by recency | Two keys same frequency, insert third | Least recently used among min frequency is evicted. |
| Zero capacity | capacity 0; put/get | No value is stored. |

## 3. TTL Cache

* **Question**: Implement an in-memory TTL cache with `put`, `get`, and opportunistic cleanup.
* **Solution**: [TTL Cache](./solutions.md#3-ttl-cache).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Unexpired read | put key with TTL 10s, read before expiry | Return stored value. |
| Expired read | read after TTL deadline | Return miss and remove or ignore expired entry. |
| Overwrite TTL | put same key with new TTL | New value and expiry win. |

## 4. Fenwick Tree

* **Question**: Implement a range-sum structure with point updates.
* **Solution**: [Fenwick Tree](./solutions.md#4-fenwick-tree).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Prefix sums | array `[1,2,3,4]`, query prefix 3 | Return `6` for first three elements. |
| Point update | add `+5` at index 1 | Affected prefix/range sums increase by 5. |
| Single element | array `[7]` | Query returns `7`. |

## 5. Interval Assignment Map

* **Question**: Implement an interval assignment map that stores non-overlapping ranges compactly.
* **Solution**: [Interval Assignment Map](./solutions.md#5-interval-assignment-map).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Assign middle | default A; assign `[2,5)=B` | Lookups in `[2,5)` return B; outside returns A. |
| Overwrite overlap | assign `[1,4)=B`, then `[3,6)=C` | `[3,6)` returns C and `[1,3)` returns B. |
| Adjacent same value | assign adjacent ranges to same value | Canonical representation merges boundaries. |

## 6. Rolling Metrics Window

* **Question**: Implement a rolling metrics window for counts and sums over the last five minutes.
* **Solution**: [Rolling Metrics Window](./solutions.md#6-rolling-metrics-window).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Window average | window 3; add `1,2,3` | Average is `2`. |
| Evict old | add `4` to window 3 | Average uses `2,3,4`. |
| Empty window | query before data | Return empty/zero according to API contract. |

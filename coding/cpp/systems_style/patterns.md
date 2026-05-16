# Systems-Style Coding Patterns

Systems-style coding problems combine data structures, API design, concurrency, and operational behavior. For L7, name the contract before coding.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| In-memory TTL cache | Get/put plus expiry | Returned value is the latest unexpired value for key | Average get `O(1)`, put `O(log n)` with heap, space `O(n)` | Runtime: lazy heap expiry. Memory: stale heap entries need cleanup. |
| Job scheduler | Execute tasks at or after due time | Min-heap top is next task eligible to run | Insert `O(log n)`, pop due `O(log n)`, space `O(n)` | Runtime: condition variable waits until next due time. Memory: store task ids and payload refs. |
| Retry queue | Retry failed work with backoff | Each task has next eligible time and attempt count | Push/pop `O(log n)`, space `O(n)` | Runtime: jitter prevents synchronized retries. Memory: cap attempts and queue size. |
| Rolling metrics window | Query recent counts/sums | Active buckets cover exactly the configured window | Update `O(1)`, query `O(buckets)`, space `O(buckets)` | Runtime: pre-aggregate buckets. Memory: bucket granularity controls footprint. |
| Log aggregator top K | Aggregate events and query heavy hitters | Counts reflect ingested events within retention window | Ingest `O(1)` average, query `O(unique log k)`, space `O(unique)` | Runtime: maintain heap if queries dominate. Memory: evict by time/window. |
| Rate-limited API component | Enforce request budget | Token or leaky bucket state never permits more than configured rate | Time `O(1)` per request, space `O(clients)` | Runtime: lazy refill. Memory: expire inactive client state. |
| Sharded key-value simulator | Route keys across shards | Each key has exactly one owning shard for a version | Get/put `O(1)` average plus routing, space `O(keys + shards)` | Runtime: consistent hash routing. Memory: shard maps bound rehash costs. |
| Message broker model | Enqueue, consume, ack, retry | Acked message is not redelivered; unacked message can be retried | Depends on queue, typically `O(log n)` for delayed retry, space `O(messages)` | Runtime: visibility timeout heap. Memory: compact ack state. |
| Deduplicating event processor | Process events exactly once in local model | Processed id set contains all side-effected ids in retention window | Average `O(1)`, space `O(window)` | Runtime: check id before side effect. Memory: TTL dedupe eviction. |
| Consistent hashing router | Route requests to backend pool | Same key maps deterministically while membership is fixed | Lookup `O(log V)`, space `O(V)` | Runtime: cache hot key routing. Memory: tune virtual nodes. |
| Config snapshot manager | Readers need stable config while updates happen | Each reader sees one immutable version | Read `O(1)`, update `O(size)`, space `O(active versions)` | Runtime: atomic shared pointer swap. Memory: old versions released after readers. |
| Backpressure controller | Protect downstream from overload | Admitted work remains under capacity or target latency | Time `O(1)` per decision, space `O(1)` or buckets | Runtime: EWMA or token bucket. Memory: compact counters. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `delayed-scheduler`, `min-heap` | min-heap plus worker wait condition | [questions](./questions.md): Q1 |
| `message-broker`, `visibility-timeout` | ready queue, inflight map, timeout heap | [questions](./questions.md): Q2 |
| `deduplication`, `ttl-window` | hash set plus expiry queue | [questions](./questions.md): Q3 |
| `sharding`, `key-value-store` | routing plus per-shard map | [questions](./questions.md): Q4 |
| `immutable-snapshot`, `atomic-publish` | immutable snapshots and atomic pointer swap | [questions](./questions.md): Q5 |
| `rolling-window`, `top-k` | time buckets plus frequency maps | [questions](./questions.md): Q6 |
| `load-balancing`, `strategy` | strategy object or clear mode switch plus backend state | [questions](./questions.md): Q7 |
| `backpressure`, `admission-control` | bounded queue plus admission policy | [questions](./questions.md): Q8 |
| `per-customer-ordering`, `scheduler` | per-customer queues plus active customer set | [questions](./questions.md): Q9 |
| `leaderboard`, `ordered-set` | hash map plus ordered set | [questions](./questions.md): Q10 |

## L7 Follow-Ups

- What is the API contract under retries, shutdown, and partial failure?
- Where do you need idempotency?
- What metrics and logs would make this debuggable in production?
- Which parts should be sharded or made thread-safe first?

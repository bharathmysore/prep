# Systems-Style Coding Questions

Solve each question in C++ by first naming the API contract, failure behavior, concurrency assumptions, and observability hooks.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Implement a delayed job scheduler that runs jobs at or after their due time.
   - Expected pattern: min-heap plus worker wait condition.
   - Pattern tags: `delayed-scheduler`, `min-heap`.
   - Solution: [Delayed Job Scheduler](./solutions.md#1-delayed-job-scheduler).
   - Complexity target: schedule `O(log n)`, dispatch `O(log n)`, space `O(n)`.

2. Implement a message broker with enqueue, consume, ack, and visibility timeout.
   - Expected pattern: ready queue, inflight map, timeout heap.
   - Pattern tags: `message-broker`, `visibility-timeout`.
   - Solution: [Message Broker With Visibility Timeout](./solutions.md#3-message-broker-with-visibility-timeout).
   - Complexity target: enqueue/dequeue average `O(1)`, timeout handling `O(log n)`, space `O(messages)`.

3. Implement a deduplicating event processor with TTL-based dedupe state.
   - Expected pattern: hash set plus expiry queue.
   - Pattern tags: `deduplication`, `ttl-window`.
   - Solution: [TTL Deduplication Store](../hashing/solutions.md#5-ttl-deduplication-store).
   - Complexity target: average `O(1)` per event, space `O(window)`.

4. Implement a sharded in-memory key-value store simulator.
   - Expected pattern: routing plus per-shard map.
   - Pattern tags: `sharding`, `key-value-store`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: average get/put `O(1)` plus route cost, space `O(keys + shards)`.

5. Implement a config snapshot manager that allows lock-light reads and atomic updates.
   - Expected pattern: immutable snapshots and atomic pointer swap.
   - Pattern tags: `immutable-snapshot`, `atomic-publish`.
   - Solution: [Config Snapshot Manager](./solutions.md#4-config-snapshot-manager).
   - Complexity target: read `O(1)`, update `O(size of config)`, space `O(active snapshots)`.

6. Implement a log aggregator that supports ingest and query top `k` messages over a time window.
   - Expected pattern: time buckets plus frequency maps.
   - Pattern tags: `rolling-window`, `top-k`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: ingest `O(1)` average, query `O(activeUnique log k)`, space `O(windowUnique)`.

7. Implement a load balancer that supports round-robin, weighted round-robin, and least-loaded modes.
   - Expected pattern: strategy object or clear mode switch plus backend state.
   - Pattern tags: `load-balancing`, `strategy`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: round-robin `O(1)`, weighted/least-loaded `O(log n)` if heap-backed, space `O(backends)`.

8. Implement a backpressure controller that rejects or delays work when queue depth exceeds thresholds.
   - Expected pattern: bounded queue plus admission policy.
   - Pattern tags: `backpressure`, `admission-control`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: time `O(1)` per decision, space `O(1)` policy state.

9. Implement a scheduler that assigns tasks to workers while preserving per-customer ordering.
   - Expected pattern: per-customer queues plus active customer set.
   - Pattern tags: `per-customer-ordering`, `scheduler`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: enqueue `O(1)`, dispatch `O(log customers)` or `O(1)`, space `O(tasks + customers)`.

10. Implement a compact in-memory leaderboard with update score and query top `k`.
    - Expected pattern: hash map plus ordered set.
    - Pattern tags: `leaderboard`, `ordered-set`.
    - Solution: [Leaderboard](./solutions.md#5-leaderboard).
    - Complexity target: update `O(log n)`, top-k `O(k)`, space `O(n)`.

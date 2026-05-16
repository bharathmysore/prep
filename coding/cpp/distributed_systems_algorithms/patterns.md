# Distributed Systems Algorithms Coding Patterns

Implement these as single-process C++ simulations unless explicitly asked to use networking. The goal is to demonstrate correctness, failure-model clarity, and tradeoff judgment.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Consistent hashing ring | Assign keys to changing node set | Key maps to first ring point clockwise | Lookup `O(log V)`, add/remove `O(vnodes log V)`, space `O(V)` | Runtime: virtual nodes improve balance. Memory: tune vnode count. |
| Rendezvous hashing | Pick best node per key without ring state | Highest score among live nodes owns key | Lookup `O(N)`, space `O(N)` | Runtime: top-K with partial selection for replicas. Memory: no vnode ring. |
| Quorum read/write simulator | Replicated storage with `R + W > N` | Read and write quorums intersect | Operation messages `O(N)`, decision `O(N log N)` or `O(N)`, space `O(N)` | Runtime: stop after quorum replies. Memory: store version per replica. |
| Heartbeat failure detector | Detect suspected failed nodes | Node is suspected only after missed heartbeat threshold | Time `O(N)` per sweep, space `O(N)` | Runtime: monotonic timestamps. Memory: compact per-node state. |
| Gossip membership | Cluster membership spreads peer to peer | Newer incarnation overrides older membership state | Rounds `O(log N)` typical, messages per round `O(fanout)`, space `O(N)` | Runtime: bounded fanout. Memory: prune tombstones after retention. |
| Leader election by term | Need single leader per term in model | Term number only increases and votes are single-use per term | Messages `O(N)` per election, space `O(N)` | Runtime: randomized timeout simulation. Memory: per-node term and vote. |
| Lease-based ownership | Temporary exclusive ownership | Owner is valid only before lease expiry under clock assumption | Acquire messages `O(N)` or quorum, space `O(N)` | Runtime: renew before expiry. Memory: store epoch plus expiry. |
| Lamport timestamps | Need total order compatible with happens-before | Local clock increases before send and after receive | Time `O(events)`, space `O(nodes)` | Runtime: scalar clocks. Memory: smaller than vector clocks. |
| Vector clocks | Need detect causality vs concurrency | Clock vector dominates only if event causally includes another | Update `O(N)`, compare `O(N)`, space `O(N)` per timestamp | Runtime: sparse vectors for active nodes. Memory: prune inactive node entries. |
| Idempotency and dedup store | Retry-safe side effects | Same idempotency key maps to one final result | Average time `O(1)`, space `O(window)` | Runtime: atomic check-and-set. Memory: TTL expiry bounds state. |
| CRDT G-counter or OR-set | Concurrent updates must merge without coordination | Merge is associative, commutative, and idempotent | Merge `O(N)` or entries, space `O(N)` or elements | Runtime: delta-state merge. Memory: compaction and tombstone GC. |
| Merkle tree anti-entropy | Compare replicas efficiently | Equal subtree hashes imply equal covered data | Build `O(n)`, compare `O(changed * log n)`, space `O(n)` | Runtime: compare hashes top down. Memory: choose chunk size carefully. |
| Circuit breaker | Stop calls to unhealthy dependency | State transitions are driven by failure counts and timers | Time `O(1)` per call, space `O(1)` | Runtime: sliding window counters. Memory: compact buckets. |
| Retry with exponential backoff and jitter | Transient failures | Each retry has bounded attempts and increasing delay | Time `O(attempts)`, space `O(1)` | Runtime: cap max delay. Memory: scalar attempt state. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `consistent-hashing`, `virtual-nodes` | sorted ring map | [questions](./questions.md): Q1 |
| `rendezvous-hashing`, `replica-selection` | score every live node and select highest scores | [questions](./questions.md): Q2 |
| `quorum`, `versioning` | quorum intersection and version resolution | [questions](./questions.md): Q3 |
| `heartbeat`, `failure-detector` | last-seen timestamps and suspicion threshold | [questions](./questions.md): Q4 |
| `gossip`, `membership` | membership records with incarnation numbers and tombstones | [questions](./questions.md): Q5 |
| `leader-election`, `term` | monotonic terms, one vote per node per term, majority wins | [questions](./questions.md): Q6 |
| `lamport-clock`, `logical-clock` | scalar logical clocks | [questions](./questions.md): Q7 |
| `vector-clock`, `causality` | vector timestamp dominance | [questions](./questions.md): Q8 |
| `idempotency`, `deduplication` | check-and-record with final result caching | [questions](./questions.md): Q9 |
| `crdt`, `g-counter` | per-node counters and max merge | [questions](./questions.md): Q10 |
| `merkle-tree`, `anti-entropy` | hash tree and recursive compare | [questions](./questions.md): Q11 |
| `retry`, `backoff` | bounded retry policy | [questions](./questions.md): Q12 |
| `circuit-breaker`, `state-machine` | state machine plus rolling failure window | [questions](./questions.md): Q13 |

## L7 Follow-Ups

- What failure model does the simplified C++ implementation assume?
- Which guarantees require real consensus and cannot be proven by a local simulator?
- How do you bound memory for tombstones, idempotency records, and logs?
- What metrics show imbalance, retry storms, stale leases, or convergence lag?

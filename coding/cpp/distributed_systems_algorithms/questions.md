# Distributed Systems Algorithms Coding Questions

Implement these as single-process C++ simulations unless explicitly asked to use networking. State the failure model and what the simulation does not prove.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

<a id="1-consistent-hashing-ring"></a>
1. Implement a consistent hashing ring with virtual nodes and lookup.
   - Expected pattern: sorted ring map.
   - Pattern tags: `consistent-hashing`, `virtual-nodes`.
   - Solution: [Consistent Hashing Ring](./solutions.md#1-consistent-hashing-ring).
   - Complexity target: lookup `O(log V)`, add/remove `O(vnodes log V)`, space `O(V)`.

2. Implement rendezvous hashing to choose the top `r` replicas for a key.
   - Expected pattern: score every live node and select highest scores.
   - Pattern tags: `rendezvous-hashing`, `replica-selection`.
   - Solution: [Rendezvous Hashing](./solutions.md#2-rendezvous-hashing).
   - Complexity target: lookup `O(N log r)` with min-heap or `O(N)` for one owner, space `O(r)`.

<a id="3-quorum-readwrite-simulator"></a>
3. Simulate quorum reads and writes over `N` replicas with versions.
   - Expected pattern: quorum intersection and version resolution.
   - Pattern tags: `quorum`, `versioning`.
   - Solution: [Quorum Read/Write Simulator](./solutions.md#3-quorum-readwrite-simulator).
   - Complexity target: messages `O(N)` worst case, decision `O(N)`, space `O(N)`.

<a id="4-heartbeat-failure-detector"></a>
4. Implement a heartbeat-based failure detector.
   - Expected pattern: last-seen timestamps and suspicion threshold.
   - Pattern tags: `heartbeat`, `failure-detector`.
   - Solution: [Heartbeat Failure Detector](./solutions.md#4-heartbeat-failure-detector).
   - Complexity target: update `O(1)`, sweep `O(N)`, space `O(N)`.

5. Implement a gossip membership simulator.
   - Expected pattern: membership records with incarnation numbers and tombstones.
   - Pattern tags: `gossip`, `membership`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: each round messages `O(N * fanout)` if all nodes gossip, space `O(N)` per node in full simulation.

6. Implement leader election in a simplified term-based cluster model.
   - Expected pattern: monotonic terms, one vote per node per term, majority wins.
   - Pattern tags: `leader-election`, `term`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: messages `O(N)` per election round, space `O(N)`.

7. Implement Lamport timestamps for a set of send and receive events.
   - Expected pattern: scalar logical clocks.
   - Pattern tags: `lamport-clock`, `logical-clock`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: time `O(events)`, space `O(nodes)`.

8. Implement vector clocks and compare two events as before, after, equal, or concurrent.
   - Expected pattern: vector timestamp dominance.
   - Pattern tags: `vector-clock`, `causality`.
   - Solution: [Vector Clock Comparison](./solutions.md#5-vector-clock-comparison).
   - Complexity target: update/compare `O(nodes)`, space `O(nodes)` per timestamp.

9. Implement an idempotency key store for retry-safe request handling.
   - Expected pattern: check-and-record with final result caching.
   - Pattern tags: `idempotency`, `deduplication`.
   - Solution: _Pending implementation in [solutions](./solutions.md)._
   - Complexity target: average `O(1)` per request, space `O(retention window)`.

10. Implement a CRDT grow-only counter and merge operation.
    - Expected pattern: per-node counters and max merge.
    - Pattern tags: `crdt`, `g-counter`.
    - Solution: _Pending implementation in [solutions](./solutions.md)._
    - Complexity target: increment `O(1)`, merge `O(nodes)`, space `O(nodes)`.

11. Implement a Merkle tree comparison to find differing chunks between two replicas.
    - Expected pattern: hash tree and recursive compare.
    - Pattern tags: `merkle-tree`, `anti-entropy`.
    - Solution: _Pending implementation in [solutions](./solutions.md)._
    - Complexity target: build `O(n)`, compare `O(changed * log n)` typical, space `O(n)`.

12. Implement retry with exponential backoff, jitter, and max attempts.
    - Expected pattern: bounded retry policy.
    - Pattern tags: `retry`, `backoff`.
    - Solution: _Pending implementation in [solutions](./solutions.md)._
    - Complexity target: time `O(attempts)`, space `O(1)`.

13. Implement a circuit breaker with closed, open, and half-open states.
    - Expected pattern: state machine plus rolling failure window.
    - Pattern tags: `circuit-breaker`, `state-machine`.
    - Solution: _Pending implementation in [solutions](./solutions.md)._
    - Complexity target: time `O(1)` per request, space `O(window buckets)` or `O(1)` counters.

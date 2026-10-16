# Distributed Systems Algorithms Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Consistent Hashing Ring

* **Question**: Implement a consistent hashing ring with virtual nodes and lookup.
* **Solution**: [Consistent Hashing Ring](./solutions.md#1-consistent-hashing-ring).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Lookup stable | ring with A,B,C; lookup key K twice | Returns same node. |
| Add node | add D to ring | Only some keys move; most assignments remain. |
| Remove node | remove assigned node | Keys owned by removed node move to next ring node. |

## 2. Rendezvous Hashing

* **Question**: Implement rendezvous hashing to choose the top `r` replicas for a key.
* **Solution**: [Rendezvous Hashing](./solutions.md#2-rendezvous-hashing).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Highest score wins | nodes A,B,C; key K | Return node with maximum hash score. |
| Add node | add D | Existing key moves only if D scores highest. |
| Remove node | remove selected node | Key moves to next-highest node. |

## 3. Quorum Read/Write Simulator

* **Question**: Simulate quorum reads and writes over `N` replicas with versions.
* **Solution**: [Quorum Read/Write Simulator](./solutions.md#3-quorum-readwrite-simulator).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Write quorum | N=3, W=2 write succeeds on two replicas | Write succeeds. |
| Read quorum intersects | R=2 after W=2 | Read sees at least one latest replica under quorum assumptions. |
| Insufficient acks | only one write ack with W=2 | Write fails or remains uncommitted. |

## 4. Heartbeat Failure Detector

* **Question**: Implement a heartbeat-based failure detector.
* **Solution**: [Heartbeat Failure Detector](./solutions.md#4-heartbeat-failure-detector).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Healthy heartbeat | node heartbeats before timeout | Node remains alive. |
| Timeout | no heartbeat past threshold | Node marked suspect/dead. |
| Late heartbeat | heartbeat after suspect state | Node transitions back to alive if policy allows. |

## 5. Vector Clock Comparison

* **Question**: Implement vector clocks and compare two events as before, after, equal, or concurrent.
* **Solution**: [Vector Clock Comparison](./solutions.md#5-vector-clock-comparison).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Happens-before | clock A `{x:1}`, B `{x:2}` | A < B. |
| Concurrent | A `{x:2}`, B `{y:1}` | Return concurrent. |
| Equal | same components | Return equal. |

## 6. Simplified Term-Based Leader Election

* **Question**: Implement a single-process term-based leader-election simulator.
* **Solution**: [Simplified Term-Based Leader Election](./solutions.md#6-simplified-term-based-leader-election).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Delayed vote | Candidate advances from term 1 to 2 before a granted term-1 reply arrives | Reply is ignored; candidate remains follower in term 2. |
| Stale heartbeat | Receiver is in term 2; former leader heartbeats in term 1 | Heartbeat is rejected; receiver does not restore the former leader. |
| Partition | Five nodes divide into `{A,B}` and `{C,D,E}` | A cannot lead with two votes; C leads with three. |
| Crash after self-vote | A persists term 1 and its vote, then restarts | A remains unable to vote for B in term 1. |
| Former-leader write | B becomes leader in term 2 after A led term 1 | Replica rejects A's term-1 write and accepts B's term-2 write. |

## 7. Gossip Membership Simulator

* **Question**: Simulate membership propagation with retained tombstones and monotonic incarnations.
* **Solution**: [Gossip Membership Simulator](./solutions.md#7-gossip-membership-simulator).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid registry | Empty registry; duplicate A; empty ID; self Z absent | Constructor throws `invalid_argument`. |
| Frozen round | A marks B dead; edges A->B, B->C in one round | B sees tombstone; C does not see it until a later round. |
| Delay and duplicates | Save alive incarnation 0; merge dead incarnation 0, then replay alive twice | Dead remains; duplicate tombstone has no effect. |
| Rejoin then stale death | B calls `rejoin()` to incarnation 1; delayed incarnation-0 death arrives | Incarnation 1 remains alive. |
| Equal-incarnation conflict | Merge alive and dead incarnation 1 in both orders | Dead incarnation 1 wins both orders. |
| Partition healing | Isolate C, spread a tombstone across A/B, later reconnect directed edges repeatedly | All three snapshots converge. |
| One-way non-convergence | Two peers A/B; B marks A dead; repeatedly deliver only A->B | A never learns B's update despite weak connectivity; snapshots remain different. |
| Invalid delivery | Unknown member Z; mixed valid/unknown packet; valid edge followed by out-of-range edge; mismatched registries | Throws before the packet/round changes membership. |
| Incarnation exhaustion | Restore own incarnation `UINT64_MAX`, then rejoin | Throws `overflow_error`; incarnation and dead bit unchanged. |
| Empty round | No peers and no edges | No effect; no error. |

## 8. Lamport Timestamps

* **Question**: Timestamp local/send/receive events without wrapping logical time.
* **Solution**: [Lamport Timestamps](./solutions.md#8-lamport-timestamps).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Initial/local/send | Initial clock 0; tick then send | Returns 1 then 2. |
| Receive newer | Local 2 receives remote 9 | Returns 10, greater than both inputs. |
| Receive stale/duplicate | Local 10 receives remote 2 twice | Returns 11 then 12; deliveries are separate events. |
| Causal chain | A sends, B receives and sends, C receives | Strictly increasing timestamps along the chain. |
| Local overflow | Initial `UINT64_MAX - 1`; two ticks | First returns maximum; second throws without mutation. |
| Remote overflow | Initial 3 receives `UINT64_MAX` | Throws without changing local 3. |

## 9. Idempotency Key Store

* **Question**: Bound reservation/cache storage while distinguishing duplicates and conflicts.
* **Solution**: [Idempotency Key Store](./solutions.md#9-idempotency-key-store).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid limits | Capacity 0, TTL 0, or byte limit 0 | Throws `invalid_argument`. |
| Reservation/duplicate/conflict | `begin("k", "req", 0)` twice; then request `"other"` under k | Started with nonzero ticket; InFlight with no ticket; Conflict. |
| Owner completion | Wrong ticket, right ticket with empty response, repeated completion | False, true, false; cached empty response remains distinguishable from in-flight. |
| Completed conflict | Reuse cached k with different request | Conflict, no cached response exposed. |
| Completion-based expiry | Begin at 0, complete at 100, TTL 10, reads at 109 then 110 | Cached at 109; Started at 110 with a new ticket; duplicate read does not extend TTL. |
| Old completion fencing | After expiry/new reservation, complete using old ticket | False; current request stays in-flight. |
| Capacity and explicit sweep | Capacity 1; unrelated expired cached key still present | New key returns Full until sweep; size never exceeds 1. |
| Uncertain owner | In-flight record, sweep at `UINT64_MAX` | Record retained; duplicate remains InFlight; unrelated key Full. |
| Time boundary | Complete at `UINT64_MAX - 1`, TTL 2; sweep at maximum | Not expired, with no expiry-addition overflow. |
| Validation | Empty/oversized key, oversized request/result, backward begin/complete/sweep time | Appropriate exception; no duplicate execution is admitted. |

## 10. Grow-Only CRDT Counter

* **Question**: Merge grow-only component counters without duplicate increments or arithmetic wraparound.
* **Solution**: [Grow-Only CRDT Counter](./solutions.md#10-grow-only-crdt-counter).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Registry validation | Zero nodes, owner outside registry, merge different sizes | Throws `invalid_argument`; rejected merge leaves destination unchanged. |
| Local increments | Three owners increment by 2, 3, 5; zero increment | Individual components reflect ownership; zero does nothing. |
| Merge algebra | Merge the three snapshots in different orders/groupings; repeat/self merge | Same components and value 10; associative, commutative, idempotent. |
| Old snapshot | Merge a snapshot preceding another increment | Destination components never decrease. |
| Component boundary | Single owner increments by `UINT64_MAX`, then by 1 | Value maximum, then overflow exception without mutation. |
| Aggregate boundary | Merge components `[UINT64_MAX, 0]` and `[0, 1]` | Merge succeeds; `value()` throws; components remain monotonic. |

## 11. Merkle Tree Comparison

* **Question**: Find differing aligned chunk indices under the explicit no-hash-collision assumption.
* **Solution**: [Merkle Tree Comparison](./solutions.md#11-merkle-tree-comparison).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Empty/single | Two empty trees; `{""}` against itself and `{"x"}` | Empty, empty, and index `[0]`, respectively. |
| Identical | Five identical aligned chunks in each snapshot | No differing indices; padding excluded. |
| Sparse differences | `{"a","b","c","d","e"}` vs `{"a","B","c","d","E"}` | Sorted indices `[1,4]`. |
| Dense differences | Every aligned chunk changed | Every real index returned exactly once. |
| Byte framing | Binary strings with embedded NUL or high-bit bytes; chunk permutations | Detect changed leaves under the no-collision assumption. |
| Size mismatch | Empty vs one empty chunk; two vs three chunks | Throws `invalid_argument`; differing layouts are not silently compared. |
| Deterministic oracle | For lengths 0 through 64, compare generated snapshots against direct string inequality | Identical index vectors for all generated cases. |
| Collision limit | Two distinct contents collide in the demonstration 64-bit hash | May falsely report equal; deliberately no cryptographic or adversarial guarantee. |

## 12. Bounded Exponential Retry With Jitter

* **Question**: Bound transient retries with injected randomness, waiting, and cancellation.
* **Solution**: [Bounded Exponential Retry With Jitter](./solutions.md#12-bounded-exponential-retry-with-jitter).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid policy | Zero attempts; base 5 greater than cap 4 | Throws before operation/random/wait callbacks. |
| Immediate terminal | First operation returns Success or Permanent | One operation; no random sample or wait. |
| One attempt | One allowed transient attempt | Exhausted with one attempt; no trailing sleep. |
| Exponential cap | Five transient attempts; base 2, cap 5; RNG returns ceiling | Ceilings/waits `[2,4,5,5]`; Exhausted with five calls. |
| Zero jitter | RNG returns 0, then operation succeeds on third attempt | Two zero waits; Succeeded with three attempts. |
| Cancellation | Cancel before first call; cancel after transient result; wait returns false | Cancelled with attempt counts 0, 1, 1; no extra operation. |
| Poll after wait | Wait returns true but sets cancellation flag | Cancelled before the next call. |
| Arithmetic boundary | Base `UINT64_MAX / 2 + 1`, cap maximum; RNG returns ceiling | Next ceiling saturates to maximum, never wraps. |
| Zero delays | Base/cap 0, transient attempts | All requested waits exactly 0. |
| Callback errors | RNG returns ceiling+1; operation or wait throws | Exception propagates; no hidden retry. |

## 13. Circuit Breaker With Rolling Failure Window

* **Question**: Bound rolling-window failures and allow only one fenced recovery probe.
* **Solution**: [Circuit Breaker With Rolling Failure Window](./solutions.md#13-circuit-breaker-with-rolling-failure-window).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid config | Any zero threshold/window/cooldown/in-flight limit | Throws `invalid_argument`. |
| Admission bound | Closed breaker, limit 2, three admissions before completion | First two get distinct tickets; third rejects; completion releases capacity. |
| Rolling threshold | Threshold 2, window 10; failures at 0 and 9 | Opens on second failure. |
| Exact expiry | Failures at 0 and 10 with window 10 | First failure expired; stays Closed with count 1. |
| Success preserves history | Failure, success, failure within window at threshold 2 | Success does not reset window; last failure opens. |
| Cooldown/probe gate | Open at 9, cooldown 5; admit at 13, then two admissions at 14 | Reject before 14; one probe at 14; second rejects. |
| Failed probe | Probe completes unsuccessfully at 14 | Reopens at 14; next probe only at 19. |
| Successful probe | Current probe succeeds | Closes with zero failures; duplicate probe completion ignored. |
| Stale completion | Keep old closed tickets; trip; admit probe; deliver old success/failure before and after recovery | Old completions return false and never close/retrip the new generation. |
| Lost probe | Probe remains incomplete; repeated admissions long after cooldown | All reject until owner completes probe or reports timeout failure. |
| Time validation | Admit/complete with a timestamp below the last API timestamp | Throws before changing ticket/state. |
| Maximum ticks | Fail near `UINT64_MAX`; cooldown tested using elapsed subtraction | Boundary decision is correct without computing an overflowing expiry. |

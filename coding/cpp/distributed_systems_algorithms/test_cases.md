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

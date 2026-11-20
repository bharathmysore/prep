# Replication, consensus, and transactions

[Contents](../README.md) · Previous: [Local engines](02-local-engines.md) · Next: [Distributed placement](04-placement-and-file-object-storage.md)

Distribution introduces partial failure: a peer can be alive but unreachable, and a client can time out after a successful commit. Treat ordering, durability, visibility, and atomicity as separate contracts.

## 1. Replication has several purposes

Copies can increase read capacity, survive device loss, and support failover. An asynchronous primary may reply before a follower has received its change. A synchronous protocol waits for defined peer conditions, but “synchronous” alone does not name the failure domains or disk persistence required.

Replication factor is the number of copies, not a consistency guarantee. A system can store three copies and still serve stale data or lose acknowledged writes during a failover if its protocol permits it.

## 2. Consensus and the replicated state machine

Consensus selects a compatible decision despite crash failures and message delays under its assumptions. Raft organizes replicated logs around terms, elected leaders, log matching, quorum decisions and state-machine application. Paxos uses proposal/ballot ordering and intersecting quorums to preserve decisions. A usable storage service also needs durable protocol state, membership change, log truncation, snapshots and recovery.

For a conventional majority group of `N = 2f + 1` voters, a majority is `f + 1`; the group can tolerate up to `f` unavailable voters while retaining a majority. This assumes the voting configuration and protocol rules actually preserve quorum intersection. It is not an arbitrary rule that any `f + 1` disks are enough for every storage system.

A committed log entry and an applied state-machine entry are different positions. A read must use a protocol that proves it observes an appropriate committed state. Reading any follower's local database is not automatically linearizable.

Read [cornerstone](../systems/cornerstone.md) and [paxos](../systems/paxos.md) as protocol implementations with explicit durability limitations. Then study [etcd](../systems/etcd.md) as a service that combines consensus, MVCC and a persistent backend. The small libraries are not substitutes for the complete service.

## 3. Leases, epochs and fencing

A lease grants temporary authority under timing assumptions. An epoch or term orders generations of authority. A fencing token allows a downstream system to reject requests from an old owner. A distributed lock that expires does not stop an old process from continuing to send writes; the protected resource must enforce authority where required.

Membership changes and stale routing are architecture topics, not deployment details. An old leader, an old placement map, and an old client can all coexist temporarily. Identify how the protocol rejects or redirects each stale actor.

## 4. Consistency vocabulary

| Contract | What it constrains | What it does not automatically give |
|---|---|---|
| Linearizability | Operations appear atomic and respect real-time precedence | Multi-key transaction atomicity unless that is the operation |
| Serializability | Transactions equivalent to some serial execution | Real-time order |
| Strict serializability | Serializable transactions with real-time order | Unlimited availability during partitions |
| Snapshot isolation | Reads use a snapshot; conflicting writes constrained by implementation | Protection from all write-skew anomalies |
| Eventual convergence | Replicas converge under stated conditions | A bounded stale-read window or real-time order |
| Read-your-writes | A session observes its prior changes | Global ordering across unrelated clients |

A transaction may offer snapshot reads or relaxed read modes alongside a stronger default path. Name API, mode, configuration and failure assumptions when assigning a guarantee. TiKV's README claims and protocol variations require more precise reading than labeling every API identically.

## 5. Why a replicated shard is not a distributed transaction

Suppose an invariant says `A + B = 100`. `A` and `B` are on different shards. Each shard can use Raft and preserve its own log perfectly while a client changes only `A` before crashing. Per-shard consensus has not made the pair atomic.

A cross-shard transaction needs a commit decision spanning participants, with recovery if the coordinator fails. Two-phase commit separates preparation from the decision; replication can preserve participant/coordinator state, but it does not erase uncertainty or contention. Transaction protocols optimize the common path with mechanisms such as early resolution, asynchronous work or single-participant shortcuts, each requiring its own safety conditions.

MVCC gives historical visibility; concurrency control determines which overlapping transactions may commit. A timestamp service orders versions, but does not by itself validate conflict freedom.

## 6. Compare two distributed database compositions

**TiKV:** ordered key ranges become Regions; each Region has replicated ownership through Raft; local engines materialize versions; distributed transactions coordinate across keys/Regions; PD supplies cluster management and timestamp-related services. SQL is provided by a separate upper system such as TiDB, which is not a checkout in this directory.

**FoundationDB:** clients obtain read versions and read storage servers; commit proxies, resolvers and transaction logs cooperate on transaction ordering, conflict checks and durable mutation logging; storage servers consume committed changes. It is a mistake to redraw this as merely one Raft group per key range. Its heterogeneous roles deliberately separate responsibilities.

**OceanBase:** SQL planning/execution, transaction management, tablets, log streams and replicated logging interact inside a distributed database. A tablet is not simply another name for TiKV's Region: ownership, grouping and protocol boundaries differ.

The [system dossiers](../README.md#system-dossiers) provide implementation anchors and qualifications for these summaries.

## 7. Watches, revisions and coordination

A revision can order changes and give a resume point to watchers. History compaction removes old versions; a client trying to resume before retained history needs a resynchronization strategy. A watch is not a durable queue with arbitrary retention unless the service explicitly provides that contract. Leases often implement ephemeral membership, but their expiration and observation have defined timing behavior.

Etcd is useful for learning these contracts because its coordination API combines revisions, compare-and-swap transactions, watches and leases. Do not treat its coordination role as evidence that it is intended to carry bulk object bytes.

## 8. Recovery is a protocol

On restart, recover durable local state, recover the consensus generation/log, determine committed versus uncommitted work, replay/apply safely, then admit reads and writes according to authority. A snapshot must be paired with a position in the mutation history. Truncating a log before the snapshot is durable and usable can destroy recoverability.

During network isolation, ask which side has the necessary quorum and authority. During recovery, ask whether the node can prove freshness before serving. During client retries, ask how the application distinguishes a failed operation from an unknown outcome.

## Check your understanding

Draw a three-voter log with one lagging replica and a client timeout. Mark persisted, committed and applied positions. Then place two keys in different groups and show the extra state needed for an atomic transfer.

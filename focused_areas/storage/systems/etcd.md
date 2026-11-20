# etcd: a replicated MVCC coordination database

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

## Scope and evidence

Local snapshot `2b7b001a90b4be38e6b1b54021ea3327666e3793`, clean when inspected. Read-only architectural review; no daemon startup, mutation, benchmark, or claim about current upstream release status. This snapshot's roadmap is evidence of local intent, not a release promise. **Inference** identifies derived reasoning.

## Build from the smallest concepts

- **Key and revision:** user keys have version history. A main revision identifies an atomic set of changes; subrevisions order individual changes inside that set. Revision order is separate from Raft log index. [revision definition](/Users/bmysoren/tpt/etcd/server/storage/mvcc/revision.go:35)
- **MVCC index and backend:** the in-memory key index maps keys/history to revisions, while the backend stores durable records. The store separately tracks current revision and compacted revision. [store fields](/Users/bmysoren/tpt/etcd/server/storage/mvcc/kvstore.go:53)
- **bbolt:** the local storage backend opens a bbolt database. This is distinct from the replicated Raft write-ahead log: one materializes KV state, the other preserves ordered consensus history. [backend opening](/Users/bmysoren/tpt/etcd/server/storage/backend/backend.go:233), [Raft persistence](/Users/bmysoren/tpt/etcd/server/etcdserver/raft.go:245)
- **Consensus group:** etcd uses Raft for its highly available replicated log. This architecture is aimed at a coherent replicated keyspace; this tree does not expose TiKV-style automatic user-key range sharding into many independent Raft groups. The latter statement is a bounded architectural inference, not a claim that external partitioning is impossible. [README](/Users/bmysoren/tpt/etcd/README.md:28)
- **Watch and lease:** watches consume revisioned changes, with explicit unsynced and blocked/victim watcher groups. Leases associate liveness/time-based expiry with keys; keepalive forwarding and leader authority need separate treatment from ordinary KV writes. [watch groups](/Users/bmysoren/tpt/etcd/server/storage/mvcc/watchable_store.go:63), [lease renewal](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:489)

## Write, read, watch, and recovery paths

**Write:** Put/Delete/Txn request → internal Raft request → propose → replicated durable log → ordered state-machine apply → MVCC/backend update → waiting client result. Atomic compare/branch operations are represented by the Txn endpoint rather than arbitrary long-running client-side database transactions. [Put routing](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:304), [Txn routing](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:376), [Raft proposal](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:1115)

**Current linearizable read:** Range defaults to invoking `LinearizableReadNotify` unless the request chooses `Serializable`. The read loop gets a confirmed ReadIndex and waits until the local applied index reaches it before unblocking readers. Both current authority and sufficiently applied state matter; simply reading a local bbolt page would omit those checks. [Range decision](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:139), [ReadIndex/apply barrier](/Users/bmysoren/tpt/etcd/server/etcdserver/read/read.go:117)

**Serializable flag nuance:** in the etcd API this selects a local read path that can return stale state. The flag's name should not be confused with the full strict-serializable transaction contract in FoundationDB. Revision-pinned reads also depend on retained history; compacted or future revisions produce explicit errors. [revision errors](/Users/bmysoren/tpt/etcd/server/storage/mvcc/kvstore.go:37)

**Watch:** stream changes in revision order and catch up unsynced watchers from retained history; a slow consumer can enter a blocked group. Watch delivery is not the same operation as a linearizable latest-state Range. Applications need resynchronization when their desired history has been compacted. The catch-up/resynchronization conclusion follows from watch grouping and the MVCC compaction boundary. [watch state](/Users/bmysoren/tpt/etcd/server/storage/mvcc/watchable_store.go:63)

**Recovery:** restore local state/snapshot and replay needed Raft history; lagging replicas can install a snapshot. The Raft loop explicitly requires saving snapshot files and the WAL snapshot entry before later entries/hardstate so restore remains possible, and syncing hardstate before releasing old WAL history. These ordering obligations explain why recovery correctness cannot be reduced to copying the backend file alone. [snapshot/WAL ordering](/Users/bmysoren/tpt/etcd/server/etcdserver/raft.go:245), [WAL sync](/Users/bmysoren/tpt/etcd/server/etcdserver/raft.go:265)

## Control/data-plane boundary and lifecycle

Raft membership, leader election, health, authentication, quota, and maintenance govern the service; Range/Put/Txn/Watch/Lease operate on application state. Unlike TiKV's separate PD component, much of this management is embedded in the etcd server. This is a synthesis of server interfaces, not an inventory of every administrative endpoint. [server interface](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:61)

Compaction removes old logical versions; defragmentation rewrites live backend state to reclaim physical space. They are different operations with different costs. The backend contains a live-database copy phase and transaction reset steps during defrag. [defrag lifecycle](/Users/bmysoren/tpt/etcd/server/storage/backend/backend.go:723), [logical compaction request](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:385)

## Delete and historical garbage collection

`DeleteRange` finds live keys and records tombstones as revisioned backend records; it also tombstones the in-memory index. Older values remain a history concern rather than disappearing through an in-place overwrite. Scheduled MVCC compaction first computes which indexed revisions to retain, then deletes eligible backend records in batches. This is distinct from the backend defragmentation needed for physical file-space reclamation. [delete/tombstone implementation](/Users/bmysoren/tpt/etcd/server/storage/mvcc/kvstore_txn.go:293), [compaction retention set](/Users/bmysoren/tpt/etcd/server/storage/mvcc/kvstore_compaction.go:28). Exercise: explain why aggressively compacting history reduces a watch consumer's available catch-up interval even when its current keys remain present.

## Bottlenecks and evolution

**Derived hypotheses:** write throughput is bounded by a shared replicated-log/apply path and its persistence latency; adding voting replicas does not automatically shard application write work. Slow WAL sync, backend transaction contention, large history, and watch fanout create different pressure modes. Linearizable reads may queue behind an apply lag even with fast bbolt point lookup. These are explanations to test with metrics, not measured limitations of this checkout.

**Observed local roadmap:** focuses on technical debt, v2-state removal, an official performance validation suite, and tentative integration of Raft asynchronous writes. Range-stream support is listed completed in its v3.7 table. [roadmap scope](/Users/bmysoren/tpt/etcd/Documentation/contributor-guide/roadmap.md:3), [roadmap items](/Users/bmysoren/tpt/etcd/Documentation/contributor-guide/roadmap.md:29)

**Forecast, medium confidence:** the design is likely to improve operability, maintenance behavior, protocol simplicity, and pipeline efficiency while retaining its coordination-store role. Async Raft writes could improve overlap, but correctness depends on preserving WAL/apply/snapshot ordering. No local evidence justifies forecasting conversion into an automatically sharded general-purpose database. This forecast is conditional on roadmap continuity and should be revisited after source/roadmap refresh.

## Reading route and exercises

API protobuf semantics → `v3_server.go` → `read/read.go` → `raft.go` → MVCC revision/index/store → backend batch/read transactions → watchable store → lease handling → compaction/defrag and robustness tests.

1. Draw one Txn with two writes and explain why both share a main revision but have different subrevisions.
2. Find the exact barrier preventing a linearizable Range from seeing unapplied committed entries.
3. Explain why client timeout after proposal leaves the outcome uncertain, then design a revision/compare-based retry strategy.
4. Distinguish MVCC compaction, Raft log truncation, backend defrag, and an application backup.
5. For a watch consumer paused beyond retention, describe a revision-consistent re-list/watch recovery protocol and verify its race handling in client documentation/tests next.

# TiKV: transactional ranges over replicated local engines

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

## Scope and evidence

Read-only local snapshot: `2be7aafd3d04a128100c5df69cb43fc99ad14b04`; checkout clean when inspected. This is a bounded architectural dossier, not an exhaustive correctness audit or description of a live deployment. Source links refer to the local snapshot. Statements marked **inference** are deductions rather than release commitments.

## Build the model from primitives

1. **Ordered key and timestamp:** logical keys have multiple versions. TiKV separates locks, values, and write records. In `MvccTxn`, locks go to `CF_LOCK`, timestamp-suffixed values to `CF_DEFAULT`, and timestamp-suffixed write records to `CF_WRITE`. These distinctions let the system represent an uncommitted intent separately from committed visibility. Read the actual mutations in [txn.rs](/Users/bmysoren/tpt/tikv/src/storage/mvcc/txn.rs:127), [value mutations](/Users/bmysoren/tpt/tikv/src/storage/mvcc/txn.rs:175), and [write mutations](/Users/bmysoren/tpt/tikv/src/storage/mvcc/txn.rs:187).
2. **Region:** a bounded key range is the unit of replication and data movement. Region replicas form a Raft group. Many Regions share a Store; a node can host Stores. PD manages placement and balance. [README](/Users/bmysoren/tpt/tikv/README.md:69)
3. **Replicated state machine:** log agreement precedes state-machine application. Region epoch, leader/read state, term/index, applied/truncated state, and snapshot state are different metadata contracts; treating all as one progress counter hides correctness bugs. [metadata map](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:181)
4. **Local engine:** RocksDB handles persistent KV data and a raft-log engine handles replication log storage in the documented default composition. Engine selection also admits a tablet-based RaftKv2 path. [engine map](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:315)
5. **Distributed transaction:** Region consensus makes an individual replicated state machine safe; it does not atomically commit changes across all Regions. The transaction layer supplies timestamped intents and commit/rollback logic above replication. TiKV describes its model as Percolator-inspired, with SI and locking options. Do not infer SQL serializability merely from Raft or the README's externally consistent transaction wording. [transaction contract](/Users/bmysoren/tpt/tikv/README.md:25)

## Write, read, and recovery paths

**Write path:** RPC service → storage scheduler/transaction action → engine bridge → Region router/peer FSM → Raft replication/persistence → committed-log application → completion callback. The storage scheduler owns transactional command semantics; raftstore owns Region replication. A multi-Region transaction coordinates prewrite and commit across those independent pipelines. [documented pipeline](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:211)

For a transaction, prewrite checks lock/conflict state and installs intents; commit gives those intents committed visibility. Optimizations change the protocol shape: duplicate prewrite handling examines `use_async_commit`, and pessimistic/shared-lock branches have separate conditions. Read those conditions before treating all transactions as the same two round trips. [prewrite implementation](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/prewrite.rs:58), [async-commit retry handling](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/prewrite.rs:554)

**Read path:** RPC → MVCC/raw storage snapshot → engine. A local read is permitted only under valid lease/read-progress conditions; a locally present value alone cannot prove current authority. Coprocessor requests execute pushed-down scans/aggregations over snapshots. Cache participation remains subordinate to snapshot correctness. [read path](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:245)

**Recovery:** surviving Region peers elect leaders; missing replication history can be replaced through snapshot generation/application. Persistent Raft state must align with apply state; callback completion and applied progress must remain ordered. A failed node need not imply a global transaction-system generation change as in FoundationDB. The final sentence is a comparison inference from TiKV's Region-scoped ownership. [raftstore ownership](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/components/raftstore.md:110), [read and state invariants](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/components/raftstore.md:160)

## Control plane and data plane

PD supplies metadata, timestamps, placement decisions, and configuration coordination. TiKV nodes serve reads, execute MVCC commands, replicate Region changes, and materialize persistent state. SQL planning generally sits above TiKV in TiDB. Region movement and splits are control decisions that become data-plane state transitions; epochs protect clients against stale routing. [system boundaries](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:17), [contracts](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:204)

## Delete and garbage collection

A transactional delete produces MVCC delete state rather than immediately erasing every historical value. GC walks versions with a safe point; deleting an old Put write also deletes its separately stored value when it is not inlined. Its state machine preserves a latest delete until removal of everything older completes, preventing older data from reappearing. This couples tombstone removal to historical-version eligibility, distinct from RocksDB physically reclaiming obsolete bytes. [GC delete pairing](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/gc.rs:50), [safe-point walk](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/gc.rs:67), [delete preservation state](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/gc.rs:87). Questions for a deeper pass: which services pin the safe point, and how much compaction work remains after logical GC?

## Bottlenecks and diagnosis hypotheses

- A hot key or Region concentrates leadership, conflict checks, latches, and apply work. Adding nodes only helps if hot work can be redistributed. **Inference** from Region ownership and transactional scheduling.
- Durable write latency includes scheduler queueing, replication/quorum delay, log persistence, apply, and callback delivery. Diagnose stages separately using scheduler/MVCC and raftstore metrics. [observability map](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:439)
- LSM compaction, snapshots, imports, backups, and cache population can compete with foreground work. A faster read cache does not remove replication, timestamp, or transaction-conflict limits. **Inference** from [background-work map](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:264).

## Local evolution signals and conditional forecast

**Observed:** RaftKv2/tablet-based persistence exists beside classic raftstore; hybrid/in-memory engines provide Region-scoped optional cache reads; coprocessor v2 introduces dynamically loaded execution plugins. These are present code paths, not proof of deployment defaults. [alternate pipeline](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:224), [coprocessors](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:255)

**Forecast, medium confidence:** further work is likely around isolating per-Region persistence cost, cache admission/eviction, and controlling contention between tenants and background work. The key constraint is maintaining identical snapshot semantics across engines. The hybrid guide explicitly requires aligned disk/cache snapshot sequence numbers. [cache contract](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/components/hybrid_engine.md:38). Evidence that would weaken this forecast: removal/deprecation of these paths or benchmarks showing another bottleneck dominates.

## Code reading route and exercises

Read the repo overview → storage scheduler → `MvccTxn` and prewrite/commit actions → `raftkv` → peer/apply paths → Region snapshot/lease logic → alternate RaftKv2 and hybrid-engine paths. Use the [pipeline hot-file list](/Users/bmysoren/tpt/tikv/doc/maintenance-guides/repo-overview.md:237).

1. Draw a two-Region transaction and mark which steps are transaction coordination and which are Raft agreement.
2. Explain what remains in each CF if a client disappears after prewrite. Locate the recovery/status-check path next; this dossier has not audited that entire path.
3. Trace a stale Region-epoch error and prove why retrying routing is different from retrying an uncertain commit.
4. Compare lease-valid reads, read-index reads, and historical MVCC visibility. Which authority and visibility checks does each need?
5. Identify the callback point that permits success, then ask whether an optimization can acknowledge before apply without weakening the API contract.

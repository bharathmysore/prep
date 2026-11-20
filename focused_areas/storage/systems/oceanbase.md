# OceanBase: SQL and transactions over tablets and log streams

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

## Scope and evidence

Local snapshot `5e8a63d29dae9fde5e962b8fcc7b167d438fc343`, clean when inspected. Bounded source-grounded architecture map, not an audit of every SQL/isolation mode or a live deployment. No builds or remote operations. **Inference** denotes a conclusion drawn from code structure; forecasts have no implied release date.

## Fundamental building blocks

1. **SQL row and schema:** SQL execution, transactions, and physical storage coexist inside each OBServer. The checked-in architecture document describes a shared-nothing baseline across servers; that scope does not characterize every OceanBase architecture or storage mode. [overview](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:7)
2. **Partition and tablet:** a table can be horizontally partitioned by range/hash/list and subpartitioned. A physical partition has a tablet holding ordered records. Logical partitioning and physical replication must be distinguished. [partition model](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:13)
3. **Log stream (LS):** multiple tablets share a replication/log stream. A leader accepts modifications; Multi-Paxos replicates redo across followers. Tablet movement between log streams changes placement without requiring SQL table identity to change. [LS model](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:17)
4. **Memtable, SSTable, and snapshots:** mutable state and immutable sorted storage are represented by tablet APIs; reads obtain the appropriate tables for a snapshot, and background scheduling freezes/merges tablet data. These compose an LSM-like storage lifecycle. The lifecycle description is a synthesis, not a verified exhaustive ordering of every DML operation. [tablet object and memtable linkage](/Users/bmysoren/tpt/oceanbase/src/storage/tablet/ob_tablet.h:217), [read-table interfaces](/Users/bmysoren/tpt/oceanbase/src/storage/tablet/ob_tablet.h:1011), [compaction scheduling](/Users/bmysoren/tpt/oceanbase/src/storage/compaction/ob_tenant_tablet_scheduler.h:172)
5. **SCN and transaction state:** transaction snapshots and commit versions are separate from physical log positions and tablet identity. The transaction service exposes weak-read snapshots, timestamp acquisition, readable SCNs, and decided progress. [transaction service](/Users/bmysoren/tpt/oceanbase/src/storage/tx/ob_trans_service.h:220)
6. **Tenant and resource unit:** a tenant is an isolated logical database environment; units reserve CPU/memory/disk, grouped into resource pools and placed across zones. System metadata and per-user-tenant Meta tenants have different management roles. [tenant model](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:25), [units](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:31)

## Architecture scope: baseline versus shared storage

The local tree also defines `SHARED_STORAGE_MODE_STR = "shared_storage"`. This is evidence of a mode identifier, not proof of end-to-end implementation completeness, OSS support, or deployed configuration. [mode constant](/Users/bmysoren/tpt/oceanbase/src/share/ob_define.h:594)

The OceanBase-authored February 27, 2026 paper describes **Bacchus**, an object-storage-oriented LSM design with shared PALF logging, shared block caching, and independently scalable compute/cache/storage. This is a separate published architecture signal that prevents a blanket “OceanBase is only shared-nothing” conclusion. Neither the paper nor the local constant establishes that this OSS checkout supports or deploys the published system. [Bacchus paper](https://arxiv.org/abs/2602.23571)

## Compose the write and read paths

**Write, high-level synthesis:** application → optional obproxy routing → OBServer SQL parse/plan/execute → transaction service and participant context → tablet/memtable mutations plus redo → LS/PALF replication → transaction commit-state completion. Consensus within an LS and distributed atomic commit across participants solve different problems. The local implementation explicitly has two-phase-commit message, log, timeout, reboot, and leader-transition handlers. [routing](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:35), [commit state machine](/Users/bmysoren/tpt/oceanbase/src/storage/tx/ob_two_phase_committer.h:41), [PALF append boundary](/Users/bmysoren/tpt/oceanbase/src/logservice/palf/palf_handle.h:78)

The 2PC code contract requires protected atomic transitions, retries for best-effort asynchronous messages, and different callbacks for applied versus replayed logs. A consensus-backed prepare does not by itself mean every participant has globally committed. [state-machine atomicity](/Users/bmysoren/tpt/oceanbase/src/storage/tx/ob_two_phase_committer.h:25), [message and log handlers](/Users/bmysoren/tpt/oceanbase/src/storage/tx/ob_two_phase_committer.h:63)

**Read:** route SQL to an OBServer, choose transaction/statement snapshot according to consistency/isolation semantics, locate tablet tables, and merge visible versions for execution. The header explicitly distinguishes current reads from bounded-staleness reads and transaction snapshots from statement/participant snapshots. Do not equate all follower/weak reads with current leader-authoritative reads or infer a universal SQL isolation level from Multi-Paxos. The matrix is implementation commentary, and exact supported SQL mode defaults require a separate end-to-end check. [snapshot matrix](/Users/bmysoren/tpt/oceanbase/src/storage/tx/ob_trans_define.h:426), [read tables](/Users/bmysoren/tpt/oceanbase/src/storage/tablet/ob_tablet.h:1011)

## Recovery, placement, and control plane

Replica failure and leader changes operate at the log-stream group; replay reconstructs transaction state from logged decisions. The 2PC layer has explicit `handle_reboot`, `leader_takeover`, and `leader_revoke` paths, so recovering commit state is a protocol obligation rather than merely restoring bytes. Tablet migration/transfer has separate metadata and SSTable parameters. [transaction recovery hooks](/Users/bmysoren/tpt/oceanbase/src/storage/tx/ob_two_phase_committer.h:87), [migration and transfer](/Users/bmysoren/tpt/oceanbase/src/storage/tablet/ob_tablet.h:597)

Zones express failure domains; placement of replicas across zones supports availability. Tenant metadata/resource pools and location caches guide serving placement. The user-facing obproxy is stateless with partition routing caches, while OBServer carries the SQL/storage/transaction execution. This dossier has not traced the complete RootService management protocol; do not treat that omission as absence of a control plane. [zones](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:9), [OBServer](/Users/bmysoren/tpt/oceanbase/docs/docs/en/architecture.md:21)

## Delete and reclamation

Row deletion is a transactional DML operation through `delete_rows`; memtable code distinguishes a delete flag. Logical absence therefore has a transaction/visibility lifetime, separate from when obsolete versions and physical files can be reclaimed. [delete API](/Users/bmysoren/tpt/oceanbase/src/storage/ls/ob_ls_tablet_service.h:425), [memtable delete handling](/Users/bmysoren/tpt/oceanbase/src/storage/memtable/ob_memtable.cpp:3563)

The tablet exposes `multi_version_start`, recycle-version calculation, and checks for SSTable GC; compaction scheduling also checks tombstone pressure. These establish separate retention/reclamation machinery. **Inference:** delete-heavy workloads may retain bytes until snapshot/version eligibility and merge/GC progress permit removal. This dossier has not proved every backup/replication pin or exact tombstone-elision predicate; trace those before predicting time to reclaim space. [recycle version](/Users/bmysoren/tpt/oceanbase/src/storage/tablet/ob_tablet.h:649), [GC eligibility](/Users/bmysoren/tpt/oceanbase/src/storage/tablet/ob_tablet.h:667), [tombstone freeze checks](/Users/bmysoren/tpt/oceanbase/src/storage/compaction/ob_tenant_tablet_scheduler.h:63)

## Bottlenecks and evolution

**Derived hypotheses:** a hot tablet can concentrate row-lock, cache, and storage pressure; several tablets sharing an LS can share replication and recovery queues. Distributed transactions add participant coordination beyond local execution, and cross-zone log latency affects durable progress. Tenant resource limits, SQL execution, and compaction contention can dominate before raw disk capacity is exhausted. These are architectural hypotheses, not measurements.

**Observed signals:** the tree contains column-oriented merge execution and column-group scanners alongside row/tablet storage, plus explicit fast-freeze checks for hotspot/tombstone conditions. [column merge implementation](/Users/bmysoren/tpt/oceanbase/src/storage/column_store/ob_co_merge_dag.cpp:464), [freeze heuristics](/Users/bmysoren/tpt/oceanbase/src/storage/compaction/ob_tenant_tablet_scheduler.h:55)

**Forecast, medium confidence:** mixed transactional/analytical layouts will increase pressure to coordinate row/column storage, compaction budgets, and tenant isolation. Separating tablet placement from LS replication should make balancing more flexible, but LS-level log/recovery contention remains a tradeoff. Evidence that would strengthen this forecast: benchmarks, supported-feature documentation, and recent changes tying column-group work to analytical query paths. Existence of a source file alone does not prove production maturity or future priority.

## Reading route and exercises

Architecture → SQL executor to transaction-service boundary → participant context → two-phase committer → PALF append/leadership → tablet table store/memtable → compaction → migration/tenant resource controls.

1. Draw three tablets in two log streams and mark SQL partition identity, consensus group, and transaction participant boundaries separately.
2. Trace a transaction touching two LS leaders. What must survive a leader change after prepare?
3. Explain why bounded-staleness reads can remain useful while being unsuitable for a latest-state invariant.
4. Compare moving a tablet to changing Paxos membership: what data, identity, and authority change in each?
5. Derive which workload benefits from column groups and which might pay extra merge cost; locate measurements before treating that derivation as a performance result.

# Research gaps and next discriminating studies

[Contents](README.md)

The current knowledge base covers every discovered top-level project and connects core mechanisms to selected source paths. It is not an exhaustive implementation audit. The following gaps distinguish architectural understanding from operational proof.

## Shared gaps

No live cluster configuration, benchmark, service startup, failure injection, crash-consistency test, upgrade, or backup restore was performed. Durability and consistency depend on exact APIs/options and deployed failure domains. Source presence and README claims do not establish released support or production maturity. Repositories are pinned to local revisions; only selected evolution signals were checked against current primary publications.

## Targeted next studies

| System | Most valuable next trace | Evidence that would resolve it |
|---|---|---|
| LevelDB | Crash ordering during file-set publication and obsolete-file removal | Implementation plus fault-injection recovery tests for each boundary |
| RocksDB | One representative configuration under sustained compaction debt | Pinned options, workload, cache accounting and steady-state performance evidence |
| LMDB | Long reader plus writer growth and sync-flag behavior | Reader table/freeDB trace and isolated crash/reuse tests |
| Redis | Exact persistence/replication/failover contract for one application | Fixed AOF/fsync config, WAIT/WAITAOF handling and failover history |
| Paxos | Persistent promise/accept/commit ordering | Crash/restart tests and a protocol-level safety argument |
| Cornerstone | Production state/log-store adapter contract | Durable implementation of flush, truncation, snapshot and state metadata |
| RocksLab | Bridge from toy merging to persistent recovery | A separate learning exercise adding WAL/publication, with explicit crash behavior |
| etcd | Watch recovery races and maintenance under load | Client resume algorithm/tests, revision barriers and backend measurements |
| TiKV | Precise transaction variants and engine configurations | End-to-end API trace through prewrite/commit/read/GC and feature support evidence |
| FoundationDB | Generation recovery and chosen engine/version-vector path | Current code, simulation scenarios, selected configuration and feature maturity |
| OceanBase | Isolation defaults, RootService, shared-storage edition boundary | SQL-visible tests, control-plane trace and edition/version support documents |
| Ceph | One frontend's writeback/flush through replicated or EC OSD commit | Full RBD/RGW/CephFS trace, selected pool policy and crash tests |
| CubeFS | Client fsync flags, concurrent writers and EC acknowledgment | API-to-packet path, cache rules, configured code mode and recovery tests |
| SeaweedFS | Filer metadata transaction and chunk publication/reclamation | Selected backend behavior, volume fsync settings and interrupted-write/delete tests |

## Coverage standard for future additions

For every deeper topic, add the invariant, operation sequence, source anchors, relevant configuration, a failure timeline, resource cost and a falsifier. Keep evidence, inference and unresolved claims visibly separate. A source-linked paragraph with an explicit limit is more useful than a confident undocumented assertion.

## Continuing without losing the thread

Start at the stage matching the prerequisite gap in [Learning path](learning-path.md). For a new deep dive, select one exact system, operation and source revision. Add new findings to its existing dossier and update the shared comparison only when the new evidence changes it. Use [Maintenance](maintenance.md) to refresh anchors and snapshots.

## Expertise coverage added

The [expertise path](expertise/README.md) adds dedicated time-series, constrained/offline and engine implementation tracks with pass criteria. Prometheus, SQLite, littlefs and Automerge are supplemental primary-source study examples, not new source checkouts or locally audited dossiers. Their exact code/release traces remain a gap if you choose them for implementation work.

The [lab portfolio](expertise/labs.md) is specified but unimplemented. Future competency evidence must include actual crash/recovery histories, resource bounds, transaction/merge invariants and reproducible measurements. No study-time or prior-experience assumption has been substituted for demonstrated mastery.

## Research readiness

The [research program](expertise/research-program.md) proposes milestones and candidate questions. No candidate's novelty has been established. No paper result has been reproduced or new contribution evaluated. The seed literature is foundational and selective; a current focused related-work search, independent criticism and executed evidence are necessary before a publishable claim. Leadership also requires applied architectural outcomes and influence beyond the learning artifacts.

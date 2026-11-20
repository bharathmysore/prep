# Storage systems architecture knowledge base

Build architectural understanding of every project under `/Users/bmysoren/tpt`: start with machines and durable bytes, learn the local data structures, add replication and transactions, then study placement, namespaces and higher-level query models. Finally, use the resulting cost and failure models to make testable predictions about evolution.

The central idea is that storage systems compose **representation, durable publication, ownership, visibility and reclamation** in different ways. Their tradeoffs become clearer when those responsibilities are traced separately and then connected through a complete operation.

This is a source-grounded architecture study of all 14 discovered projects at the local snapshot on **2026-10-27**. It includes selected code traces and documented limits; it is not an exhaustive code audit or a measured comparison. [Provenance and maintenance](maintenance.md) explains evidence levels and update rules. [Research gaps](research-gaps.md) distinguishes the current coverage from expert operational validation.

## Start here

This directory is the canonical home for this storage learning material. Use [exercises](exercises/README.md) for the implementation agenda and [samples](samples/README.md) for saved code, beginning with [RocksLab](samples/rockslab-week1/README.md).

Implementation is central to the learning goal. The [implementation workbook](expertise/implementation-workbook.md) provides an incremental build sequence and a concrete first exercise in the existing LSM teaching model.

For the goal of industry-level architectural leadership and writing original papers, start with the [research and leadership program](expertise/research-program.md). It adds mathematical foundations, paper critique, reproduction, research questions, evidence standards and production architecture practice to the existing tracks.

Your deeper goal is expertise in engines, databases, large-scale infrastructure, time-series storage and constrained/disconnected systems. Start with the [expertise path](expertise/README.md): it prioritizes LSM and B+ tree internals, then develops time-series and offline specialties through a connected lab portfolio.

- [Deep engine internals](expertise/engine-internals.md)
- [Time-series storage](expertise/time-series.md)
- [Constrained and disconnected systems](expertise/constrained-and-disconnected.md)
- [Lab portfolio and telemetry capstone](expertise/labs.md)

Read [Learning path](learning-path.md) for the prerequisite sequence and mastery checks, or use [Architecture map](architecture-map.md) to orient yourself. Each system dossier links to the source files supporting its claims and ends with further reading/exercises.

## Fundamental building blocks

| Read | What it builds |
|---|---|
| [1 Machines and durability](foundations/01-machine-and-durability.md) | Interfaces, hardware hierarchy, buffered I/O, acknowledgment, WAL, COW, checksums, failure models |
| [2 Local storage engines](foundations/02-local-engines.md) | Ordered keys, memtables, SSTs, manifests, caches, Bloom filters, compaction, MVCC, tombstones, reclamation |
| [3 Replication and transactions](foundations/03-distribution-and-transactions.md) | Raft/Paxos, quorum, authority, fencing, commit/apply, read barriers, isolation and distributed commit |
| [4 Placement and file/object storage](foundations/04-placement-and-file-object-storage.md) | Names, inodes, chunks/extents, partitions, CRUSH, replication, EC, publication, GC and repair |
| [5 Queries and resource control](foundations/05-query-models-and-resource-control.md) | Indexes, row/column layouts, execution, vectors, compression, tenants, admission and catalog layers |

## System dossiers

| Layer | Projects |
|---|---|
| Learning model | [RocksLab Week 1](systems/rockslab-week1.md) |
| Embedded engines | [LevelDB](systems/leveldb.md), [RocksDB](systems/rocksdb.md), [LMDB](systems/lmdb.md) |
| Memory/data structure service | [Redis](systems/redis.md) |
| Consensus libraries | [Paxos](systems/paxos.md), [Cornerstone](systems/cornerstone.md) |
| Coordination store | [etcd](systems/etcd.md) |
| Distributed transactional KV | [TiKV](systems/tikv.md), [FoundationDB](systems/foundationdb.md) |
| Distributed SQL | [OceanBase](systems/oceanbase.md) |
| Distributed file/object/block systems | [Ceph](systems/ceph.md), [CubeFS](systems/cubefs.md), [SeaweedFS](systems/seaweedfs.md) |

Dossiers cover primitives, composition, operation/recovery paths, durability and consistency boundaries, bottleneck hypotheses, source reading routes, and conditional evolution. Smaller protocol examples and the educational lab are explicitly separated from complete production storage services.

## Connect the systems

- [Architecture map](architecture-map.md): conceptual prerequisites, actual composition examples and ten questions for reviewing any system.
- [Comparisons](comparison.md): engines, distributed databases and file/object architectures under common assumptions.
- [Performance and failure reasoning](performance-and-failure-reasoning.md): critical paths, resource budgets, maintenance debt, failure invariants and falsifiable hypotheses.
- [Evolution](evolution.md): evidence-backed scenarios for roughly 2027–2029, with confidence, counterpressure and tests that could weaken each forecast.
- [Glossary](glossary.md): vocabulary shared across the projects, with warnings where names have different meanings.

## A first complete reading session

Begin with machines/durability and the LSM model, then read RocksLab and LevelDB. Explain a write, snapshot, overwrite, delete, flush and compaction without looking at the text. Draw the crash boundaries. Next compare LMDB's page reuse with LSM version retention. That gives you a foundation for the replicated services rather than a list of product facts to memorize.

## Evidence and continuing study

- [Source snapshot](source-snapshot.json): all project revisions and worktree state.
- [Source index](source-index.json): cited files, lines, hashes and referring chapters.
- [Validation report](validation-report.md): coverage and local citation checks.
- [Research gaps](research-gaps.md): the most valuable next trace for each project.
- [Maintenance](maintenance.md): refreshing the knowledge base without losing provenance.

Full architectural fluency means being able to derive behavior for a new workload or failure, not merely recognize component names. Use the capstone in the learning path to test that ability, and preserve uncertainty wherever the source or configuration does not settle a claim.

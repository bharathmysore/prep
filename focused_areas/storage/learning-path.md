# Learning path from primitives to architectural judgment

[Contents](README.md)

For your expertise goal, use the [expertise path](expertise/README.md) as the priority guide. It adds deeper [engine work](expertise/engine-internals.md), [time-series](expertise/time-series.md), [constrained/offline systems](expertise/constrained-and-disconnected.md), and [implementation/failure labs](expertise/labs.md). The stages below remain the shared architectural prerequisites; complete them to the depth your mastery evidence requires.

Follow prerequisites rather than reading repositories alphabetically. Each stage ends with an observable explanation or diagram. Reading a chapter is progress; being able to derive a failure or cost consequence is the mastery check. Suggested exercises are not changes already made or tests already executed.

## Stage 1 Understand the persistence boundary

Read [Machines and durability](foundations/01-machine-and-durability.md). Draw application memory, kernel buffers, device state, replica state, commit state and backup. Place acknowledgment points on that drawing.

**Mastery check:** explain a process crash, power loss and client timeout without treating them as the same failure. Explain why checksums and replication solve different problems.

## Stage 2 Build the LSM model

Read [Local engines](foundations/02-local-engines.md), [RocksLab](systems/rockslab-week1.md), then [LevelDB](systems/leveldb.md). Trace internal versions from memtable through flush and compaction. Use the lab's existing exercises as a future isolated learning activity; its retained tombstones and lack of persistent files are deliberate limits to understand.

**Mastery check:** draw three versions of one key in different files, add a snapshot and a deletion, and state which records may be removed. Explain the manifest's role during a crash.

## Stage 3 Compare local engine costs

Read [RocksDB](systems/rocksdb.md), [LMDB](systems/lmdb.md) and [Redis](systems/redis.md). Keep the workload constant while changing the representation and persistence mode.

**Mastery check:** explain why RocksDB can stall writes, why a read-only LMDB transaction can cause growth, and why a slow Redis command or persistence event can affect unrelated requests. Separate local transaction, batch, snapshot and command atomicity.

## Stage 4 Learn protocol safety before distribution at scale

Read [Replication and transactions](foundations/03-distribution-and-transactions.md), [Paxos](systems/paxos.md) and [Cornerstone](systems/cornerstone.md). Study their adapter and persistence limitations as part of the lesson.

**Mastery check:** walk through an old leader returning after a partition. Mark ballot/term, persisted state, quorum and fencing. Explain why a consensus algorithm name does not prove a storage implementation is crash-safe.

## Stage 5 Study one complete replicated service

Read [etcd](systems/etcd.md). Trace one transaction and a subsequent linearizable read through proposal, persistence, apply, MVCC and read barrier. Then consider watches and history compaction.

**Mastery check:** distinguish revision from log index, history compaction from defrag, and a stale local read from a linearizable read. Describe how a watcher recovers after losing retained history.

## Stage 6 Add sharding and cross-shard transactions

Read [TiKV](systems/tikv.md), then [FoundationDB](systems/foundationdb.md). Draw the same two-key transfer in both, labeling ordering, conflict detection, logging and materialization.

**Mastery check:** explain why one replicated shard is insufficient for a multi-shard transaction, why a local engine replacement does not replace the commit protocol, and how hotspot/conflict pressure differs between these architectures.

## Stage 7 Put SQL and tenants above storage

Read [Queries and resource control](foundations/05-query-models-and-resource-control.md), then [OceanBase](systems/oceanbase.md). Add schema, secondary indexes, SQL execution, tablets, log streams and tenants to the picture.

**Mastery check:** distinguish table partition, tablet, log stream and transaction participant. Explain how a query can be slow with healthy storage and how index maintenance changes write amplification.

## Stage 8 Add namespaces, placement and bulk payload

Read [Placement and object storage](foundations/04-placement-and-file-object-storage.md), then [SeaweedFS](systems/seaweedfs.md), [CubeFS](systems/cubefs.md) and [Ceph](systems/ceph.md). Start with a single file, then a large namespace, then failure and repair.

**Mastery check:** draw naming, allocation, payload persistence and metadata publication separately. Explain the difference between a master quorum and a payload write protocol. Compare replication and EC under degraded operation.

## Stage 9 Derive performance and failure behavior

Read [Performance and failure reasoning](performance-and-failure-reasoning.md) and [Comparisons](comparison.md). Choose one write-heavy, one read-heavy and one metadata-heavy workload. Predict bottlenecks before looking for measurements.

**Mastery check:** budget logical/physical bytes, CPU, network round trips and maintenance debt. State one observation that would disprove each prediction. Separate measured behavior from source-derived hypotheses.

## Stage 10 Make and revise architectural forecasts

Read [Evolution](evolution.md). Choose one pressure—capacity cost, CPU per I/O, repair time, tenant isolation or new query models. Trace its likely effect on component boundaries.

**Mastery check:** produce a one-page forecast with evidence, causal mechanism, counterpressure, confidence, horizon and falsifier. Update it when source or release evidence changes.

## Capstone architecture review

For each production system, produce a single diagram and answer the [ten architectural questions](architecture-map.md#a-common-architectural-interview). Compare two systems using the same workload, durability and failure assumptions. Explain why neither is universally best.

A strong capstone includes one successful write, one fresh read, one deletion, one partial failure, one repair/rebalance and one unknown client outcome. Include the resource budgets and the location of every authoritative state transition.

## Depth still needed for expert operation

This knowledge base supplies a connected architecture foundation and selected code traces. Expert-level understanding also requires implementation-specific crash histories, compatibility modes, supported deployment configurations, performance experiments, upgrades and restoration drills. Track those in [Research gaps](research-gaps.md), without confusing unexecuted exercises with validated behavior.

## Research and leadership progression

The higher target is to create and defend architecture and write credible papers. Continue from the mastery checks into the [research and leadership program](expertise/research-program.md), [scientific method](expertise/research-method.md) and [paper study sequence](expertise/paper-study.md). The first research artifact is a technical study and reproduction; originality requires a focused literature review and new evidence, not simply more reading or a larger implementation.

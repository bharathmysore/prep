# Predicting architectural evolution

[Contents](README.md) · [Reasoning method](performance-and-failure-reasoning.md)

These are conditional architectural forecasts as of 2026-10-27, not project commitments. The horizon is roughly 2027–2029. **Evidence** names an observed implementation or published design; **forecast** explains a plausible response to cost pressure. Confidence is qualitative, not a measured probability. Local capability, released support, actual deployment and future priority are different claims.

## The prediction method

Start with a pressure that survives changes in fashion: bytes cost money, tail latency limits usefulness, failures require reserve capacity, and operators must preserve correctness during change. Locate the architectural boundary that concentrates that pressure. Predict a specific change to that boundary and the new cost it introduces. Finally name an observation that could weaken the forecast.

A useful chain is `workload or hardware change → bottleneck → architectural response → displaced cost → falsification test`. “AI will change storage” is too broad. “Filtered vector search increases pressure on resident index memory and recall-aware admission” is a mechanism that can be examined.

## 1. Separate maintenance compute from serving where economics justify it

**Evidence:** RocksDB provides remote compaction callbacks in the [local API](/Users/bmysoren/tpt/rocksdb/include/rocksdb/options.h:535). Its [official remote-compaction guide](https://github.com/facebook/rocksdb/wiki/Remote-Compaction) describes a worker producing output for the primary to install, with communication/integration supplied by the user.

**Forecast, medium confidence:** systems with bursty compaction or uneven CPU demand will increasingly move some maintenance to independent workers. The hard part becomes safe file publication, stale-worker rejection, transfer cost and failure handling. It will not help every workload: small databases or constrained networks may favor local work.

**Test:** measure foreground p99 and total CPU/bytes/cost at equal durability and steady-state debt. Weaken the forecast if transport and integration costs dominate the avoided interference. The API already exists; the prediction concerns wider architectural use, not invention of remote compaction.

## 2. Separate durable capacity, caches and query compute selectively

**Evidence:** FoundationDB already separates transaction/log/storage roles; its [local feature-status document](/Users/bmysoren/tpt/foundationdb/design/feature-status.md:8) discusses file/range-oriented work. The February 2026 [OceanBase Bacchus paper](https://arxiv.org/abs/2602.23571) describes a shared-storage design with independently elastic compute, cache and storage resources and shared log services. That paper is evidence of a published design, not proof that the checked-out Community Edition exposes every feature.

**Forecast, medium confidence:** cold durable data increasingly sits behind independently sized caches/compute, while hot ordering and commit state retain stronger locality or specialized services. Elasticity shifts difficulty into cache warmup, ownership transfer, metadata durability and service dependencies.

**Test:** compare scale-out time and cost including cold-cache periods and recovery. The forecast weakens where remote reads, egress, or latency targets overwhelm capacity savings. Shared-nothing and shared-storage designs can coexist rather than one replacing the other everywhere.

## 3. Faster media moves attention toward CPU and concurrency

**Evidence:** Ceph's [SeaStore design goals](/Users/bmysoren/tpt/ceph/doc/dev/crimson/seastore.rst:10) target NVMe, asynchronous execution and reduced copying; [Crimson status](/Users/bmysoren/tpt/ceph/doc/dev/crimson/index.rst:12) must be considered separately from default backend support.

**Forecast, medium confidence:** more work will target CPU cycles per I/O, cross-core traffic, allocation, batching and shard ownership as media latency falls. Lock-free or reactor designs introduce their own lifetime, fairness and recovery complexity.

**Test:** compare CPU/request, p99, feature completeness and recovery under faults at equal workload. Fast microbenchmarks alone do not demonstrate a viable replacement. HDD-dominated deployments may continue to prioritize seek and rebuild efficiency.

## 4. Metadata and recovery become more visible design constraints

**Evidence:** Ceph separates placement/maps and interface metadata; CubeFS separates namespace, replica and EC subsystems; SeaweedFS separates volume placement and filer metadata. Their [dossiers](README.md#system-dossiers) show these boundaries. The [official Ceph architecture overview](https://docs.ceph.com/en/latest/architecture/) also emphasizes recovery and rebalancing work on OSD hosts; that page describes a development version.

**Forecast, medium-high confidence:** scaling object/file counts and device capacities increases pressure on metadata partitioning, indexed listing, incremental repair, and explicit foreground/recovery budgets. More payload capacity can worsen recovery time unless rebuild bandwidth grows with it.

**Test:** track recovery duration at fixed failure fraction, metadata ops per object, repair bytes per lost byte and p99 under repair. This is an inference about pressure, not a promise of a particular algorithm or release.

## 5. Specialized query models add new maintenance work

**Evidence:** Redis has [local vector-set code/docs](/Users/bmysoren/tpt/redis/modules/vector-sets/README.md:1); [official vector-set documentation](https://redis.io/docs/latest/develop/data-types/vector-sets/) describes similarity queries with attribute filtering. SeaweedFS's [local README](/Users/bmysoren/tpt/seaweedfs/README.md:224) describes Iceberg/Lance catalogs and maintenance; OceanBase has row/column storage paths.

**Forecast, medium confidence:** more systems will expose workload-specific indexes and table metadata over established storage primitives. Memory accounting, recall/latency tradeoffs, index rebuilds, catalog transactions and orphan cleanup become part of the storage architecture. The presence of a new API does not remove these costs.

**Test:** require representative workloads and failure/rebuild evidence. Weaken the forecast where specialization compromises the core system's latency or operability enough that an independent service is preferable.

## Project watchlist

These rows synthesize the evidence and qualifications in each dossier. They are study hypotheses, not verified maintainer roadmaps.

| Project | Conditional direction | Confidence | Watch for / disconfirm with |
|---|---|---|---|
| LevelDB | Stable minimal engine and portability/correctness work | Medium | Scope expansion versus continued narrow API |
| RocksDB | Better maintenance isolation, cache economics and offload integration | Medium-high | Actual integration use, total cost and correctness tests |
| LMDB | Incremental page/portability work while retaining mapped COW core | Medium | Evidence of a deliberate change to writer/reclamation model |
| Redis | Richer query structures with stricter memory and persistence budgets | Medium | Recall/cost measurements and supported execution paths |
| Paxos example | Most valuable as a protocol/persistence study | Low | Sustained modernization and durable-state fixes; no roadmap inferred |
| Cornerstone | Integration correctness remains more important than demo throughput | Low | Production-grade storage adapter evidence and maintenance activity |
| RocksLab | Learning progression to WAL, persistent tables and safe GC | Educational proposal | This is a curriculum suggestion, not an upstream forecast |
| etcd | Pipeline, maintenance, testing and operability improvements | Medium | Local roadmap continuity and preserved coordination role |
| TiKV | Isolation of Region/engine costs, movement efficiency and transaction paths | Medium | Supported engine modes and under-load balancing results |
| FoundationDB | Range/file movement and role/fanout efficiency | Medium | Feature status, ownership and release support, not directories alone |
| OceanBase | Hybrid query layouts and selective storage/compute separation | Medium | Edition-specific support and end-to-end operational evidence |
| Ceph | CPU-efficient OSD/backend paths, recovery scheduling and heterogeneous placement | Medium | Defaults, completeness, real recovery tests and adoption |
| CubeFS | Placement and coding/recovery economics | Medium | Namespace consistency, EC quorum policy and repair exposure |
| SeaweedFS | More catalog/gateway features with metadata and maintenance pressure | Medium | Code/tests beyond README claims; sustainable filer/backend scale |

## Forces that probably do not disappear

Ordered access requires indexes or ordered representations. Durable mutation requires a recoverable publication rule. Replicated authority requires a protocol that excludes incompatible decisions. Historical reads delay reclamation. Extra redundancy consumes resources. A timeout can leave an unknown outcome. Evolution usually moves these responsibilities rather than making them vanish.

## Updating a prediction

Record the baseline, new evidence, changed mechanism, confidence and a falsifier. Do not upgrade confidence because a feature was announced; look for a usable path, support statement, validation and cost evidence. No monitoring or recurring automation is installed by this document.

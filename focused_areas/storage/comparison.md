# Architectural comparisons and tradeoffs

[Contents](README.md) · [System details](README.md#system-dossiers)

Compare systems at the same layer and under the same workload. The tables summarize the source-linked dossiers; they are not benchmark rankings or product selection advice.

## Local engines

| Axis | LevelDB | RocksDB | LMDB | Redis |
|---|---|---|---|---|
| Primary representation | LSM | LSM with many tunable policies | Mapped copy on write B+ tree | Typed in-memory structures |
| Primary API boundary | Embedded KV | Embedded KV and related extensions | Embedded transactions/cursors | Network commands/data types |
| Foreground write cost | Logging and memory mutation; flush/merge later | Similar base, with more concurrency/resource controls | Serialized writer updates copied pages and publishes transaction | Command execution plus configured persistence/replication |
| Stable historical view | Snapshot sequence | Snapshots and versioned engine state | Read transaction holds an older page view | Do not assume arbitrary engine-style MVCC snapshots |
| Reclamation pressure | Old versions, tombstones, file sets | Same plus richer policies/features | Old readers delay page reuse | Expiry, deletion, allocator/persistence behavior |
| Common performance trap | Young-file buildup and compaction debt | Memory/cache contention and excessive tuning without measurement | Long readers and writer serialization | Large/expensive commands, memory growth and persistence overhead |
| Distributed semantics | Supplied above engine | Supplied above engine | Supplied above engine | Depend on replication/cluster mode and commands |

The engines can all serve key lookup workloads, but their costs are structurally different. A RocksDB write-heavy test, LMDB read-mostly test and Redis cache test cannot be ranked by requests per second without fixing durability, dataset size, value distribution and failure requirements.

## Distributed databases and coordination

| Axis | etcd | TiKV | FoundationDB | OceanBase |
|---|---|---|---|---|
| Main abstraction | Coordination KV | Transactional KV | Transactional ordered KV | SQL database |
| Principal distribution model | Replicated coordination state through a shared Raft log | Many Regions, each with replicated Raft ownership | Separate transaction, log and storage roles | Tablets grouped in replicated log streams, distributed SQL |
| Transaction layer | Compare-and-mutate operations in service API | MVCC plus distributed transaction protocols | Optimistic conflict validation and ordered commit versions | Transaction engine and cross-participant coordination |
| Read nuance | Linearizable barrier vs serializable local read mode | API, transaction mode and follower-read conditions matter | Normal conflict-tracked vs snapshot reads matter | Isolation, snapshot/SCN, plan and replica choice matter |
| Scaling question | Leader/log/backend and watch work | Region distribution, hot keys, engines and PD interactions | Role-specific load, conflicts, log and storage lag | Tenant resources, SQL execution, log streams and compaction |
| Wrong simplification | “A tiny general object store” | “RocksDB with a network port” | “One Raft group per shard” | “TiKV with a SQL parser” |

In particular, SI, serializability and linearizability are different contracts. See [the consistency vocabulary](foundations/03-distribution-and-transactions.md#4-consistency-vocabulary), then verify the exact API in the system dossier.

## Distributed file and object storage

| Axis | Ceph | CubeFS | SeaweedFS |
|---|---|---|---|
| Base organization | RADOS objects over placement groups and OSDs | Partitioned namespace and extent data; blobstore paths | Packed blob volumes plus filer namespace |
| Placement/control | Cluster maps and CRUSH; MON quorum | Master manages topology/partition placement | Master manages volumes and allocation/location |
| Namespace | CephFS MDS; RGW has its own object metadata paths | Metanodes/metapartitions | Filer and configurable metadata backend |
| Payload path | OSD object operations; frontends map into RADOS | Datanodes/extents and blobstore components by path | Volume servers store needles/chunks |
| Redundancy | Replicated and erasure-coded pools | Replicated extent subsystem; selectable EC BlobStore subsystem | Replicated volumes and EC paths |
| First bottleneck hypothesis | Metadata/PG/OSD/backend/repair depending on workload | Namespace/partition hotspots, extent or blob path | Filer/backend metadata, allocation or volume I/O |
| Important caution | CRUSH placement is not the whole commit protocol | Append, overwrite and EC paths have different protocols and acknowledgment conditions | Raft in master management is not proof of Raft per blob write |

These systems differ in how much naming and placement state is centralized, replicated, cached or computed. All still need authoritative state transitions during movement and failure.

## Six comparisons that teach architecture

### LevelDB versus LMDB

Hold the logical workload constant, then compare where updates become physical work: sequential log and later sorted merging versus copying changed pages and committing a new root. Add a long-lived reader. Predict retained SST versions in one case and unreusable pages in the other. Neither design escapes version retention.

### RocksDB versus TiKV

Start with one atomic engine batch. Then distribute its keys across two Regions. Identify which new state belongs to transaction coordination, which to Raft replication, and which still belongs to RocksDB recovery. This reveals why an embedded engine benchmark cannot predict distributed transaction latency.

### TiKV versus FoundationDB

Both provide transactional KV, but the placement of ordering and conflict checking differs. Ask how a hotspot stresses a Region leader versus transaction roles and conflict ranges. A shared API abstraction does not imply a shared bottleneck or recovery protocol.

### Ceph versus SeaweedFS

Compare object placement through CRUSH/PGs with volume indirection and filer metadata. Then add a new server: explain what data moves, which metadata changes and which component coordinates it. Avoid equating placement lookup complexity with end-to-end latency.

### Redis versus etcd

Both have network KV operations, but the main execution model and replicated commit guarantees differ. Compare an ordinary asynchronous Redis replicated write with an etcd write through its consensus path, then separately inspect Redis WAIT/WAITAOF behavior. Matching acknowledgment conditions is necessary before interpreting performance.

### Replicated versus erasure-coded payload

Start with equal useful capacity and failure-domain requirements. Account for writes, small-object padding, degraded reads, reconstruction traffic and repair reserve. Then change the workload from immutable large objects to frequent small overwrites. The capacity winner may become the latency or repair loser.

## Avoid these false equivalences

A local WAL is not a distributed consensus log. A timestamp is not conflict detection. Replication is not backup. A control-plane quorum is not proof of payload durability. A snapshot is not a transaction's isolation contract. More shards do not split one hot key. More threads do not create device bandwidth. More replicas do not automatically improve write availability. These distinctions should appear in every design review.

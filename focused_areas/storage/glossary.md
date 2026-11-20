# Storage glossary

[Contents](README.md)

Definitions are conceptual. Project-specific semantics belong to the linked system dossiers and source contracts.

| Term | Meaning |
|---|---|
| Acknowledgment | Reply that a protocol-defined completion condition has been met |
| Admission control | Limiting work entering a system to protect capacity and latency |
| Applied index | Position through which committed log entries have affected a state machine |
| Atomicity | An operation's changes become visible together or not at all under its contract |
| Backfill | Copying data to bring a destination into the required placement/state |
| Backup | Separately recoverable data plus the means to restore it |
| Ballot or term | Generation identifier used to order protocol authority |
| Batching | Amortizing fixed overhead across multiple operations |
| B+ tree | Ordered page-based search tree with records at leaves |
| Blob | A binary payload; the exact unit varies by system |
| Block | Fixed or bounded I/O/format unit; distinguish device, cache and file-format blocks |
| Bloom filter | Probabilistic membership filter with false positives and no false negatives under correct construction/use |
| Cache | Reusable copy of data whose authority and invalidation policy must be defined |
| Checkpoint | Persisted state tied to a recoverable history position |
| Checksum | Integrity code used to detect corruption, not recreate lost data |
| Chunk | Piece/container of payload; exact size and role depend on system |
| Column family | Engine namespace/configuration grouping, not a SQL columnar layout |
| Column store | Layout grouping attribute values to favor selected-column scans |
| Commit | Protocol decision that an operation/transaction is accepted as authoritative |
| Commit unknown result | Outcome ambiguous to a client despite a possible successful commit |
| Compaction | Reorganization/reclamation; distinguish LSM merging from MVCC history removal |
| Consensus | Protocol for compatible decisions despite failures under explicit assumptions |
| Control plane | Management of topology, policy, authority and lifecycle |
| Copy on write | Update by writing new state and publishing a new reference/root |
| CRUSH | Ceph's topology-aware deterministic placement mechanism |
| Data plane | Path that serves user data operations |
| Dentry | Directory entry mapping a parent/name to file identity |
| Durable | Survives a specified class of failure; the class must be named |
| Epoch | Ordered generation of configuration, ownership or authority |
| Erasure coding | Encoding data into fragments permitting recovery from specified losses |
| Extent | Mapping/container for a range of payload bytes; project-specific |
| Failure domain | Set of resources that may fail together, such as host/rack/zone |
| Fencing | Rejecting stale owners using enforceable generations/tokens |
| Flush | Moving buffered state onward; specify memtable-to-SST versus filesystem/device sync |
| fsync | OS synchronization request with platform/filesystem/device-specific persistence contract |
| Garbage collection | Reclaiming state proven no longer required |
| HNSW | Hierarchical navigable small world graph used for approximate nearest-neighbor search |
| Idempotency | Repetition preserves the intended single-operation effect |
| Inode | File identity and attributes, separate from directory names |
| Isolation | Rules governing interaction among concurrent transactions |
| Lease | Time-bounded authority/liveness relationship under explicit timing assumptions |
| Linearizability | Atomic operations respecting real-time precedence |
| Log stream | OceanBase grouping for ordered replicated logging; not a synonym for tablet |
| LSM | Log structured merge organization of memory state and immutable sorted runs |
| Manifest | Authoritative metadata describing engine file sets or table snapshots, depending on context |
| Materialization | Building a queryable representation from authoritative changes |
| Memtable | In-memory recent-version representation in an LSM engine |
| MVCC | Multi-version concurrency control using versioned visibility |
| Namespace | Logical organization of keys, files, buckets, tables or tenants |
| OSD | Ceph object storage daemon, managing local storage and PG work |
| Page cache | OS-managed cached file pages, distinct from an engine's block cache |
| Participant | Entity taking part in a distributed transaction decision |
| Peering | Ceph process establishing compatible authoritative PG history |
| Placement group | Ceph unit grouping objects for placement and recovery coordination |
| Primary-backup | Replication organized around one primary and backup replicas |
| Quorum | Required subset of participants satisfying protocol rules |
| RADOS | Ceph distributed object substrate |
| Read amplification | Extra I/O/bytes/work per logical read at a stated boundary |
| Read barrier | Proof/coordination that a read observes a sufficiently current applied state |
| Region | TiKV key-range and replicated ownership unit |
| Repair | Restoring correctness or redundancy after loss/divergence |
| Replica | Another copy/state-machine instance under a replication protocol |
| Revision | Ordered version of changes; etcd revisions differ from Raft indexes |
| SCN | OceanBase version/time concept used for snapshots and progress |
| Serializable | Transactions equivalent to some serial order |
| Shard | Partition of logical data or serving work |
| Snapshot | Stable view or saved state at a defined version/history point |
| Snapshot isolation | Snapshot reads with implementation-defined write conflict checks; permits some anomalies |
| Space amplification | Physical allocated bytes divided by live logical bytes with scope named |
| SST | Immutable sorted table used by LSM engines |
| Strict serializability | Serializability respecting real-time order |
| Tablet | OceanBase storage object for a physical table partition |
| Tail latency | Slow-end request latency, often described by high percentiles |
| Tenant | Logical administrative/security/resource boundary with implementation-specific isolation |
| Tombstone | Marker suppressing an older value or representing deletion |
| Two-phase commit | Prepare/decision protocol for atomic commit across participants |
| Version retention | Preserving old state needed by readers, recovery or policy |
| WAL | Write-ahead log preserving recoverable mutation/protocol state |
| Watch | Subscription to ordered change events under service retention/delivery rules |
| Write amplification | Physical writes per logical write at a stated boundary |
| Write skew | Concurrent snapshot-based decisions violating a shared invariant without direct write conflict |

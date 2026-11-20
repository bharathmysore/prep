# LevelDB: the compact durable LSM reference

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: local checkout `/Users/bmysoren/tpt/leveldb`, HEAD `bb74ef739973a70ca9f0d90788a1da89b788c0ad`, read on 2026-10-27. Static source reading; no build or fault tests. This is a production embedded ordered key/value engine, not a distributed database or SQL server. Its README limits access to one process per database and describes limited maintenance ([scope](/Users/bmysoren/tpt/leveldb/README.md:1)).

## Build it from fundamentals

An ordered map gives point lookups and range iteration. A sequence number adds version visibility. A write-ahead log makes a mutable memory map recoverable. Immutable sorted tables move data to disk. A manifest records the live table set. Compaction bounds lookup fanout and discards obsolete versions. Bloom filters and a block cache reduce physical reads, but do not decide correctness.

The resulting path is `WriteBatch -> WAL -> mutable MemTable -> immutable MemTable -> L0 SST -> lower levels`. These are different roles: a WAL is recovery history, an SST is searchable data, and the manifest is database metadata. An old file becoming unnecessary is separate from a user deleting a key.

## Trace the lifecycle

- **Write:** `DBImpl::Write` queues writers, groups compatible batches, assigns consecutive sequence numbers, appends a log record, optionally syncs the log, then inserts into the MemTable ([entry](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1206), [ordering](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1231)). A successful asynchronous write does not promise power-loss durability.
- **Read:** choose the snapshot sequence, retain references to current structures, then search mutable memory, immutable memory, and the current on-disk version ([Get](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1121)). References allow disk reads outside the main mutex. Newest visibility, rather than arbitrary first physical match, is the invariant.
- **Flush:** a full MemTable becomes immutable; a sorted table is created and its metadata installed through a version edit ([flush](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:505), [background transition](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:549)).
- **Recovery:** reconstruct version metadata and replay relevant logs into memory/tables ([Recover](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:292), [log replay](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:385)). Acknowledgment, WAL persistence, and manifest publication are distinct crash boundaries.
- **Delete:** write a tombstone. Compaction drops hidden versions only when snapshots permit; a deletion marker also requires proof that older data cannot survive in deeper levels ([drop rules](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:966)). Otherwise deleting the marker could resurrect a value.

Snapshots are sequence-number views, not independent durable backups ([creation](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1187)). Atomic batches do not supply general multi-operation serializable transactions or replication.

## Internal structures: why the parts fit

A MemTable is not simply a hash map overwritten on every update. It stores encoded internal keys containing the user key plus a sequence number and value/deletion type. `MemTable::Add` allocates one encoded record from an arena and inserts it into a skip list ([encoding](/Users/bmysoren/tpt/leveldb/db/memtable.cc:79)). The internal comparator sorts user keys first and newer sequence numbers ahead of older ones ([comparison](/Users/bmysoren/tpt/leveldb/db/dbformat.cc:47)). Seeking with a snapshot sequence therefore skips versions that are too new, then lands on the newest visible version. This is how a single ordered structure supports both current reads and older snapshots.

The arena and skip list simplify lifetime management: nodes are not individually removed while readers may be traversing them. Writes require external synchronization; reads need the structure to remain alive, then can traverse without the skip list taking internal locks ([concurrency contract](/Users/bmysoren/tpt/leveldb/db/skiplist.h:8)). `Get` retains the MemTable and current Version before releasing the database mutex. A Version is the immutable description of which SST files participate in a read; keeping that description alive prevents a background compaction from invalidating the read's file set.

Within an SST, an index identifies candidate data blocks. The table reader first attempts the block cache and otherwise reads the block from the file ([block lookup](/Users/bmysoren/tpt/leveldb/table/table.cc:156)). Configurable filters can eliminate unnecessary candidate reads ([filter option](/Users/bmysoren/tpt/leveldb/include/leveldb/options.h:147)). A cache miss changes latency, not the visible answer. Keeping separate concepts for the table/file cache, data-block cache and operating-system page cache helps explain why a dataset larger than RAM can still have fast hot-key reads.

## Worked history: overwrite, snapshot, delete, crash

Assume an initially empty database and illustrative sequence numbers; actual numbering depends on all writes in that database.

1. `Put(a,old)` receives sequence 1. Its WAL record precedes its MemTable insertion. With synchronous writes, log sync is part of successful completion.
2. Create snapshot S at sequence 1, then `Put(a,new)` at sequence 2. Both internal versions may coexist. A current Get selects `new`; a Get using S selects `old`.
3. Flush the MemTable. The same visibility information now lives in an SST. Flushing changes location, not which version either read sees.
4. `Delete(a)` at sequence 3 inserts a tombstone into the new MemTable. Current Get returns absent immediately and does not continue into the SST to resurrect `new`. S still sees `old`.
5. Compact while S exists. The compactor cannot discard the history S still needs. After releasing S, compaction can remove obsolete versions; removing the tombstone additionally requires proving that no deeper-level older value can reappear.
6. Crash after a successfully synced delete but before its MemTable flush. Recovery replays the WAL, restoring deletion visibility. If the delete used asynchronous logging and the machine lost unpersisted data, its successful reply alone would not prove that the delete survives.

The central invariant is that moving data among structures and reclaiming files must preserve every supported visible view. That invariant explains both the read order and the apparently conservative compaction rules.

## Cost model and evolution hypotheses

Foreground writes are amortized through sequential logging, but compaction repeatedly reads and rewrites bytes. L0 overlap raises read fanout; falling behind eventually delays or stops writers ([backpressure](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1331)). Large caches reduce read I/O while consuming memory. Long snapshots retain versions and delay reclamation.

Conditional forecast: workloads needing many independent flushes, parallel compaction, transactions, or richer tuning push toward a RocksDB-like engine or a wrapper. This follows from the pipeline bottlenecks; it is not an upstream roadmap claim. LevelDB remains useful as the smallest production-scale example for learning the invariants.

## Reading exercises and unresolved work

1. Follow `Put(a,1); snapshot; Delete(a)` through Get and the compaction drop conditions. Explain why the snapshot still reads `1`.
2. Draw crashes immediately before log append, after append but before sync, after sync, and after MemTable insertion. State which facts recovery can trust.
3. Inspect `MakeRoomForWrite` and explain how a background backlog becomes foreground latency.

Not established here: filesystem-specific directory sync guarantees, recovery under injected torn writes, comparator compatibility across restarts, or measured amplification for your workloads. These require deeper implementation tracing or experiments; no benchmark conclusions are implied.

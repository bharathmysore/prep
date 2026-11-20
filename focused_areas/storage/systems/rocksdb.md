# RocksDB: a configurable LSM engine

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: `/Users/bmysoren/tpt/rocksdb`, HEAD `4c38c45ac10a73ff933b20610925da828e52c8ae`, read on 2026-10-27. Static reading only. Repository `AGENTS.md` delegates to `CLAUDE.md`; both were read. This is a production embedded storage library, descended from LevelDB, with explicit read/write/space amplification tradeoffs and multithreaded compaction ([README](/Users/bmysoren/tpt/rocksdb/README.md:8)). It needs a service layer for network access, replication, membership, and distributed transactions.

## Foundations and composition

Start with LevelDB's sorted records, sequence-number visibility, WAL, MemTables, SSTs, versions and compaction. Add column families for separate logical ordered spaces, concurrent write coordination, multiple immutable MemTables, parallel flush/compaction, configurable table formats/caches/filters, and optional transaction and blob facilities. Shared process resources mean column families are not independent failure or resource-isolation domains.

The local component documentation provides a reading map for point reads, MultiGet, iterators, write modes and crash recovery ([index](/Users/bmysoren/tpt/rocksdb/docs/components/index.md:1)). Configuration changes the actual algorithm; there is no single RocksDB latency or durability profile.

## Lifecycle and guarantees

- **Write:** APIs converge on `WriteImpl`, which coordinates writers, allocates sequence visibility, writes WAL data and inserts into MemTables. Alternative modes alter ordering and concurrency ([WriteImpl](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl_write.cc:891), [MemTable insertion](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl_write.cc:1484)). Do not assume every optional mode has identical snapshot behavior.
- **Durability:** `WriteOptions::sync` defaults false; its comment distinguishes process crash from machine crash. `disableWAL` separately defaults false; enabling it can lose unflushed writes even after a process crash ([contract](/Users/bmysoren/tpt/rocksdb/include/rocksdb/options.h:2705)). These two flags cannot be reduced to a single durable/not-durable switch without stating the failure model.
- **Read:** the point-read API enters `GetImpl`; version visibility and the live MemTable/table set govern which value wins ([entry](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl.cc:3186)). Caches and filters accelerate searching, while sequence numbers and deletion semantics determine the answer.
- **Flush:** immutable memory is persisted to output files, then incorporated into version metadata ([flush implementation](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl_compaction_flush.cc:185)). Flush is different from syncing a WAL and from compacting existing SSTs.
- **Recover:** reconstruct the version state and replay eligible WALs; replay inserts batches into MemTables ([Recover](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl_open.cc:442), [WAL recovery](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl_open.cc:1237), [insertion](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl_open.cc:1693)).
- **Delete/reclaim:** point/range deletions become visibility information carried through reads and compaction. Snapshot-aware compaction is explicit in the iterator interface ([snapshot machinery](/Users/bmysoren/tpt/rocksdb/db/compaction/compaction_iterator.h:261)). Logical absence is not immediate disk-space reclamation or secure erasure.

## Internal structures, concurrency and memory hierarchy

A column family has its own mutable and immutable MemTables and SST Version. RocksDB packages references to those structures in a `SuperVersion` ([layout](/Users/bmysoren/tpt/rocksdb/db/column_family.h:208)). Holding a coherent set of these references matters because a flush or compaction may publish a replacement while a reader is still traversing the previous structures. It is the structural view of the data; a sequence-number snapshot is the logical visibility boundary. Confusing the two obscures why a read needs both lifetime safety and version filtering.

The default MemTable representation is a skip list, but the representation is pluggable ([representation contract](/Users/bmysoren/tpt/rocksdb/include/rocksdb/memtablerep.h:22)). A skip list supports ordered insertion and traversal without rebuilding a contiguous sorted array after every write. Writer coordination sits above this representation in `WriteThread` ([coordinator](/Users/bmysoren/tpt/rocksdb/db/write_thread.h:32)). Grouping several callers amortizes WAL overhead, while optional concurrent insertion and pipelined modes change which phases overlap. Consequently, throughput tuning must preserve the configured publication and snapshot contract, not merely increase thread counts.

For block-based SSTs, a lookup uses index/filter information to select candidate blocks and a block cache to avoid repeated decompression and file reads. Index and filter blocks can themselves be cached, partitioned and prioritized ([table options](/Users/bmysoren/tpt/rocksdb/include/rocksdb/table.h:218), [block cache](/Users/bmysoren/tpt/rocksdb/include/rocksdb/table.h:380), [partition filters](/Users/bmysoren/tpt/rocksdb/include/rocksdb/table.h:519)). A filter negative can avoid reading a candidate table's data; a positive means “may exist,” not “found.” The OS page cache can still cache file bytes beneath the engine cache. Memory budgeting therefore includes MemTables, cache entries, table metadata, temporary compaction buffers and application objects, rather than one advertised cache size.

## Worked history: two column families and a restart

Assume ordinary WAL-enabled writes and default ordered publication, with column families `accounts` and `audit`. This example illustrates engine atomicity, not a complete banking transaction protocol.

1. Prepare one WriteBatch that changes `accounts/alice` and inserts an `audit/event7` record. A single engine batch expresses their atomic publication; two separate Put calls would leave an observable/crash boundary between them.
2. Write with `sync=true`. The write path coordinates the batch's WAL and MemTable work before successful completion. If a machine crashes afterward, recovery should reconstruct the durable batch without needing both MemTables to have flushed.
3. Create a snapshot, then overwrite `accounts/alice`. A snapshot read observes the earlier account value while current reads observe the overwrite. Atomic publication does not mean that arbitrary separate reads made without a common snapshot form one consistent application view.
4. Flush one family before the other. Its records may now reside in SSTs while the other family's records still require WAL replay. Different physical locations do not invalidate the batch's logical visibility contract.
5. Delete `accounts/alice` and compact. The snapshot may require retention of the prior value. The audit record is unaffected because its key and family have separate logical identity.
6. Repeat the experiment conceptually with `disableWAL=true`. A successful unflushed write now lacks the recovery history assumed above. Faster completion would be a changed durability contract, not a free implementation optimization.

This history shows why column families, atomic batches, snapshots, flushing and syncing answer different questions. A transaction wrapper is still needed when correctness requires checking reads for conflicts before committing dependent writes.

## Bottlenecks and architectural direction

Compaction debt competes with foreground I/O and CPU. Filters and caches trade memory for read work. Compression trades CPU for bytes; write grouping trades individual latency for throughput. A slow column family or shared WAL retention can affect shared resources. Long snapshots, large values and range tombstones complicate reclamation.

Conditional forecast: if payload rewriting dominates, value separation or blob storage becomes attractive; if point-read CPU dominates, batched reads and cache/filter efficiency matter more; if tenants interfere, resource isolation becomes architectural. These are hypotheses from cost structure, not release predictions. The local tree already has lazy wide-column/blob paths ([example](/Users/bmysoren/tpt/rocksdb/db/db_impl/db_impl.cc:3200)); their presence is evidence of this checkout's features, not universal deployed availability.

## Exercises and unknowns

Trace the same batch with `sync=false`, `sync=true`, and `disableWAL=true`, recording the exact crash boundary each option changes. Compare point Get with an iterator and explain their shared visibility requirement. Identify the compaction picker and calculate a byte budget before changing any tuning knob.

Still to investigate: selected compaction style, table format, transaction mode, timestamp semantics, cache hierarchy, blob garbage collection and exact write-stall thresholds for an actual configuration. No configuration, performance result, or production deployment was inferred from this checkout.

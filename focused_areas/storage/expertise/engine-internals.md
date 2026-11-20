# Deep track for LSM and B tree internals

[Expertise path](README.md) · [Labs](labs.md)

Learn representation and publication together. An elegant index that loses acknowledged data or exposes freed memory is not a working storage engine. The local [LevelDB](../systems/leveldb.md), [RocksDB](../systems/rocksdb.md), and [LMDB](../systems/lmdb.md) dossiers contain verified source routes; this track makes the required depth explicit.

## Layer 1 Bytes and physical I/O

Implement a binary record with explicit endianness, lengths, record type, version and integrity check. Validate lengths before allocation or traversal. Specify handling of truncated records, invalid checksums and unsupported format versions. Distinguish detectable corruption from recoverable corruption.

Understand buffered I/O, mmap/page faults, direct-I/O constraints where relevant, device queueing, alignment and persistence barriers. Define the failure model before choosing a sync policy. A process-kill test does not reproduce loss of unflushed kernel/device state.

**Gate:** given a file hex dump and a truncation point, identify which records recovery may trust without executing application logic.

## Layer 2 LSM representation

Study internal-key encoding and ordering, sequence visibility, arena/skiplist lifetimes, WAL framing, block restart points, sparse indexes, Bloom filters and iterators. Then study immutable memtables, SST construction and Version/MANIFEST publication.

Trace the existing LevelDB path from `DBImpl::Write` to WAL and memtable, `Get` to the relevant Version, and compaction to snapshot-aware drop rules. The [LevelDB dossier](../systems/leveldb.md) links those symbols. Draw structural lifetime and logical version visibility separately.

**Gate:** implement point reads and range scans over overlapping sorted runs with overwrite/delete histories. Compare every operation against a simple reference model, including old snapshots. Prove why the tombstone cannot disappear while an older value remains outside compaction inputs.

## Layer 3 Compaction as constrained scheduling

Derive costs for leveled and tiered layouts using the actual file overlap and workload, rather than memorizing a universal amplification formula. Account for flush bytes, WAL, compaction input/output, retained versions, temporary space and manifest metadata.

Vary key skew, value size, update rate, scans and snapshot age. Explain where value separation might save rewrite bandwidth and introduce an extra dereference/GC problem. Compare filters, indexes and cache budgets before assuming the only solution is faster disks.

**Gate:** predict maintenance debt and stable goodput under a fixed write budget; show where compaction or flush falls behind and why adding threads eventually stops helping.

## Layer 4 B+ tree pages and publication

Study page headers, branch/leaf/overflow pages, slot arrays, comparator behavior, splits, merges and free-space accounting. Start with a single writer and immutable reader views. In a COW design, identify which ancestor pages must change when a child is replaced and when a root becomes authoritative.

Use LMDB for COW/root publication and reader-driven reuse. For a different recovery architecture, read SQLite's [atomic commit explanation](https://www.sqlite.org/atomiccommit.html), which describes rollback-journal ordering and storage assumptions. This is supplemental reading: SQLite is not in the local project inventory. It supplies a useful contrast to both WAL/SST and COW-root designs.

**Gate:** split a leaf while an older reader remains active; demonstrate that both roots find their correct values. After deletion, distinguish page reuse from shrinking the underlying file.

## Layer 5 Concurrency and database semantics

Study writer grouping, publication sequence, structural references, memory ordering, iterator lifetimes and cancellation. RocksDB's SuperVersion and snapshot solve different problems; the [RocksDB dossier](../systems/rocksdb.md) explains that boundary. Ask what happens when compaction publishes while a reader is paused inside an old table.

Add secondary-index mutations and transactional read dependencies. Contrast atomic batches, snapshot isolation and serializable conflict validation. Use anomalies such as lost update and write skew as concrete histories, not vocabulary quizzes. SQL planning/execution and index maintenance from [query foundations](../foundations/05-query-models-and-resource-control.md) come above the engine, with distinct costs.

**Gate:** identify which interleavings the engine prevents, which the transaction layer prevents, and which remain the application's responsibility.

## Layer 6 Recovery as the design review

For every persistent transition, list the before-state, writes, synchronization, publication and cleanup. Inject interruption at each modeled boundary. Ensure recovery is repeatable: crashing again during recovery must not create an incompatible state.

Compare against an oracle that separately records the model's durable acknowledgment boundary. Include torn tails, partial outputs, orphan files, invalid metadata, disk-full returns and failed syncs. Model only failures the harness actually supports; document the gap to real power-loss behavior.

**Gate:** present a small matrix of crash points and allowed recovered states, with no unsupported claims that successful reopen proves correctness.

## What to postpone

Learn a complete single-writer durable path before lock-free tree research, custom allocators or a new distributed protocol. They become valuable when an observed limit calls for them. Keep the initial goal an inspectable engine with a precise contract and strong failure evidence.

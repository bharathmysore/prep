# Storage implementation samples

[Storage learning hub](../README.md) · [Exercises](../exercises/README.md)

[Rockslab Week 1](rockslab-week1/README.md) is a copy of the existing C++17 in-memory LSM teaching model, including its CMake configuration, header, implementation, CLI and tests. The source is `/Users/bmysoren/tpt/rockslab-week1`; the save manifest records file hashes. No new mechanism has been implemented by copying it, and no build/test result is implied.

The [implementation workbook](../expertise/implementation-workbook.md) begins with an explainable-lookup extension and progresses through durable engines, database mechanisms, time series and disconnected systems. Keep subsequent sample implementations under this directory. Preserve the distinction between educational models and production guarantees.

## Saved sample code

| Concept | Entry point | What to inspect |
|---|---|---|
| Public API and state | [MiniLsm](rockslab-week1/include/mini_lsm.h:12) | Map-based memory/table model and tombstone representation |
| Put and delete | [Put](rockslab-week1/src/mini_lsm.cc:12), [Delete](rockslab-week1/src/mini_lsm.cc:17) | Mutations and automatic flush check |
| Lookup resolution | [Get](rockslab-week1/src/mini_lsm.cc:22), [FindNewest](rockslab-week1/src/mini_lsm.cc:100) | Newest source wins; a tombstone stops older-value lookup |
| Range scans | [Scan](rockslab-week1/src/mini_lsm.cc:30) | Merge/resolution and omission of deleted keys |
| Flush | [Flush](rockslab-week1/src/mini_lsm.cc:55) | Moving the memory table into newest-first immutable tables |
| Compaction | [Compact](rockslab-week1/src/mini_lsm.cc:63) | Preserve newest entries, including tombstones |
| Behavior checks | [Test entry point](rockslab-week1/tests/mini_lsm_test.cc:18) | Flush, overwrite, deletion, scan and compaction expectations |
| Interactive commands | [CLI main](rockslab-week1/src/main.cc:23) | Parsing commands and inspecting state |

## Production examples and internals

- [RocksDB simple API example](/Users/bmysoren/tpt/rocksdb/examples/simple_example.cc:28): open, Put/Get and an atomic WriteBatch. It demonstrates library use rather than engine implementation.
- [LevelDB write path](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1206): writer coordination, WAL and memory insertion.
- [LMDB transaction commit](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:4652): COW persistence and root publication; follow its callees and surrounding contracts.
- [etcd read barrier](/Users/bmysoren/tpt/etcd/server/etcdserver/read/read.go:117): confirmed read index and applied-state waiting.
- [Exercise-by-exercise code references](../expertise/implementation-workbook.md#the-implementation-ladder): direct implementation anchors for the full ladder, with explicit gaps where no local sample exists yet.

The external repository examples are referenced in place, not copied into this directory. No new sample build or execution is implied by these references.

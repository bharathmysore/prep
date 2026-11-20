# RocksLab Week 1: the deliberately incomplete LSM model

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: `/Users/bmysoren/tpt/rockslab-week1`, read on 2026-10-27. This directory was not a Git repository in the inspected environment. It is a single-threaded in-memory educational lab, not RocksDB and not a durable engine. The README explicitly omits WAL, real SST serialization, indexes, caches, Bloom filters and concurrent compaction ([scope](/Users/bmysoren/tpt/rockslab-week1/README.md:1)). No code was built or executed.

## Foundations

The model uses `std::map<Key, Entry>` for both mutable and immutable tables; an entry contains an optional value, where absence represents a tombstone ([types](/Users/bmysoren/tpt/rockslab-week1/include/mini_lsm.h:35)). “SSTable” is a teaching name for an in-memory sorted map. Newest-table order substitutes for the production sequence-number/MVCC machinery.

## Trace every operation

- **Put:** replace the key's entry in the MemTable, then check the threshold ([Put](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:12)). The threshold counts distinct MemTable keys, not bytes or total writes ([threshold](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:116)). Repeatedly overwriting one key need not trigger a flush.
- **Delete:** store `std::nullopt` so the newest deletion masks older values ([Delete](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:17)). Removing a key only from the newest map would permit old values to reappear.
- **Get:** search the MemTable, then immutable tables newest to oldest; a tombstone stops the search and returns absence ([Get](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:22), [FindNewest](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:100)).
- **Scan:** resolve first-seen entries in that same order, then emit live entries sorted over the half-open interval `[begin,end)` ([Scan](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:30)). Tombstones participate in resolution before being filtered out.
- **Flush:** move the MemTable to the front of the table vector ([Flush](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:55)). No bytes reach disk, and a process crash loses everything.
- **Compact:** first flush, then merge tables newest first using `emplace` so the first version wins ([Compact](/Users/bmysoren/tpt/rockslab-week1/src/mini_lsm.cc:63)). **The implementation keeps surviving tombstones**; it does not implement tombstone garbage collection. It also does not retain snapshot versions.

## What it explains, and what it cannot

The lab demonstrates version precedence, overlapping sorted runs and compaction preserving visible answers. It cannot demonstrate crash recovery, write acknowledgment durability, real disk write amplification, cache hit rates, concurrency or production compaction scheduling. Lookups grow with table count; scans build a temporary merged map; compaction merges everything, so none of those costs directly models a tuned RocksDB deployment.

Exercise: predict the results and map contents after `put a old; flush; delete a; flush; compact; get a`. Explain why the tombstone survives this implementation. Next, overwrite the same key ten times with a threshold of three and predict the table count. Finally compare the tombstone drop conditions in LevelDB with this lab's unconditional retention.

Conditional evolution is a learning sequence: add durable record framing and recovery, real immutable files and indexes, explicit versioning/snapshots, then bounded compaction and concurrency. Each introduces a new invariant; adding only file output does not create a correct database. Existing tests are a source-reading exercise at [test file](/Users/bmysoren/tpt/rockslab-week1/tests/mini_lsm_test.cc:1), not evidence that production concerns are covered.

# Implementation workbook for storage architecture

[Expertise path](README.md) · [Lab portfolio](labs.md) · [Research program](research-program.md)

Implementation is a central learning method. Work through a sequence of small mechanisms: predict the behavior, specify the invariant, implement it, challenge it with a counterexample, measure its costs, and trace its counterpart in a production system. Keep correctness and resource accounting visible as features accumulate.

This workbook specifies implementation work; it does not claim the mechanisms have been newly implemented. The existing RocksLab is already implemented and serves as a starting reference. The preferred collaboration mode is pending: learner implementation with review, pair implementation, or reference implementations. The sequence works for each mode; the amount of solution code supplied should follow that preference.

## Two complementary implementations

**Build a small mechanism yourself.** Keep it inspectable enough to explain every state transition. This teaches the invariant and design choices.

**Trace and instrument a production counterpart.** Identify the additional concurrency, formats, compatibility and failure handling. This prevents a toy model from becoming the whole mental model. Use the existing local source dossiers for reading routes; changes to those checkouts are separate work.

A toy engine does not need to be production ready to teach something important. Its omissions must be explicit. An in-memory simulator cannot establish disk durability, and a deterministic network simulator cannot establish behavior on real intermittent devices.

## The implementation ladder

| Step | Implement | Invariant or question to test | Production study |
|---|---|---|---|
| 0 | Explainable lookup in the existing LSM model | Newest visible entry wins; tombstone stops older-value search | [RocksLab FindNewest](/Users/bmysoren/prep/focused_areas/storage/samples/rockslab-week1/src/mini_lsm.cc:100) → [LevelDB Get](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1121) |
| 1 | Ordered memory records and merging iterator | Point lookup and scan agree under overwrite/delete histories | [MemTable records](/Users/bmysoren/tpt/leveldb/db/memtable.cc:79); [merge iterator](/Users/bmysoren/tpt/leveldb/table/merger.cc:179) |
| 2 | Binary record codec and checksums | Invalid lengths/tails cannot become valid unintended records | [WAL record writer](/Users/bmysoren/tpt/leveldb/db/log_writer.cc:34); [record reader](/Users/bmysoren/tpt/leveldb/db/log_reader.cc:56) |
| 3 | Append log and replay | Recovery reconstructs exactly the records allowed by the declared persistence model | [RecoverLogFile](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:385) |
| 4 | Persistent sorted file and sparse index | Lookup/scan preserve order and values after reopen | [SST creation](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:505); [block lookup](/Users/bmysoren/tpt/leveldb/table/table.cc:156) |
| 5 | Manifest and file-set publication | Partial outputs cannot become committed state by mere existence | [VersionSet LogAndApply](/Users/bmysoren/tpt/leveldb/db/version_set.cc:777); [Recover](/Users/bmysoren/tpt/leveldb/db/version_set.cc:861) |
| 6 | Multi-version keys and snapshots | An old reader keeps the right view while current state changes | [GetSnapshot](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1187); [internal-key comparator](/Users/bmysoren/tpt/leveldb/db/dbformat.cc:47) |
| 7 | Compaction and safe reclamation | No required version disappears; deletion never resurrects old data | [LevelDB compaction drop rules](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:966); [RocksDB snapshot inputs](/Users/bmysoren/tpt/rocksdb/db/compaction/compaction_iterator.h:261) |
| 8 | Bloom filter and block cache | Optimization changes work, not the returned answer | [Bloom creation](/Users/bmysoren/tpt/leveldb/util/bloom.cc:28); [membership test](/Users/bmysoren/tpt/leveldb/util/bloom.cc:56); [RocksDB cache options](/Users/bmysoren/tpt/rocksdb/include/rocksdb/table.h:380) |
| 9 | Page-based B+ tree | Search/order/reachability survive splits and deletion policy | [LMDB page types](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1075); [node layout](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1230); [page search](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:7580) |
| 10 | COW root publication and reader epochs | No active root reaches an incorrectly reused page | [page touch](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:3028); [commit](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:4652); [reader table](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:802) |
| 11 | Atomic record/index updates and conflict model | Record and indexes agree; claimed isolation matches observed histories | [RocksDB atomic batch example](/Users/bmysoren/tpt/rocksdb/examples/simple_example.cc:50); [TiKV prewrite](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/prewrite.rs:58); [transaction architecture](../systems/foundationdb.md) |
| 12 | Replicated-log simulator and read barrier | No incompatible committed decisions; reads meet the stated freshness contract | [etcd Raft proposal](/Users/bmysoren/tpt/etcd/server/etcdserver/v3_server.go:1115); [read/apply barrier](/Users/bmysoren/tpt/etcd/server/etcdserver/read/read.go:117) |
| 13 | Time-series registry, postings and chunks | Selection and temporal scans return the documented sample set | No dedicated local TSDB implementation; [time-series study](time-series.md) and [primary references](paper-study.md) |
| 14 | Encoding, lateness, retention and rollups | Compression preserves defined values; corrections/aggregation preserve defined meaning | No local Gorilla codec/rollup implementation; [encoding study](time-series.md#4-chunks-compression-and-recovery-serve-different-purposes) |
| 15 | Bounded outbox and deduplicating receiver | Uncertain replies do not duplicate effects within the supported retry contract | No local outbox sample yet; [protocol specification](constrained-and-disconnected.md#4-store-and-forward-with-a-durable-outbox) |
| 16 | Rejoin, deletion history and resource policy | Stale peers cannot silently resurrect deleted state; overflow has an explicit outcome | [TiKV safe-point GC](/Users/bmysoren/tpt/tikv/src/storage/txn/actions/gc.rs:67); [offline rejoin differences](constrained-and-disconnected.md#6-long-absences-make-deletion-and-history-expensive) |
| 17 | Placement, redundancy and repair simulator | Failure-domain losses and repair costs match the model | [Ceph replication commit](/Users/bmysoren/tpt/ceph/src/osd/ReplicatedBackend.cc:638); [CubeFS EC writes](/Users/bmysoren/tpt/cubefs/blobstore/access/stream/stream_put.go:205); [placement and repair](../systems/ceph.md) |
| 18 | Admission and maintenance budgeting | Sustainable goodput and backlog bounds match resource assumptions | [LevelDB write backpressure](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1331); [RocksDB WriteController](/Users/bmysoren/tpt/rocksdb/db/write_controller.h:24) |

This is a prerequisite ladder, not a deadline. Steps can be skipped only when an equivalent implementation and explanation demonstrate the competency. Single-writer correctness should precede concurrent optimization. The existing [five labs](labs.md) group these mechanisms into complete projects.

## How to use the code references

Read the saved teaching sample first, then the production implementation linked in the ladder. Links point to verified one-based lines in the current local files. The source revisions are recorded in [the snapshot](../source-snapshot.json); [the source index](../source-index.json) records anchor text and hashes. These are code-reading entry points, not claims that one function implements the entire mechanism.

For steps 13–15, the local inventory does not yet contain the proposed time-series or outbox sample. The links lead to specifications/primary readings, not an invented implementation. TiKV's centralized safe-point GC is useful comparison material for step 16; it is not a complete offline-peer deletion/rejoin protocol.

## The first exercise Explain where the answer came from

### Starting point

The saved [RocksLab README](../samples/rockslab-week1/README.md) describes a single-threaded, in-memory LSM model. Its [header](../samples/rockslab-week1/include/mini_lsm.h) exposes Put, Delete, Get, Scan, Flush and Compact. It already implements these operations, so the first new task is a small extension rather than a claim to rebuild them from scratch.

A learning copy is saved under `../samples/rockslab-week1/`. The original reference remains under `/Users/bmysoren/tpt/rockslab-week1`. Use C++17 initially because that is the existing lab's language; this is a proposed starting choice, not a requirement for every later implementation.

### Sample functions to inspect

Read [Get](../samples/rockslab-week1/src/mini_lsm.cc:22) and [FindNewest](../samples/rockslab-week1/src/mini_lsm.cc:100), then [Delete](../samples/rockslab-week1/src/mini_lsm.cc:17), [Flush](../samples/rockslab-week1/src/mini_lsm.cc:55), [Compact](../samples/rockslab-week1/src/mini_lsm.cc:63) and the [existing tests](../samples/rockslab-week1/tests/mini_lsm_test.cc:18). The [CLI](../samples/rockslab-week1/src/main.cc:23) connects commands to the model. Start the production comparison at [LevelDB Get](/Users/bmysoren/tpt/leveldb/db/db_impl.cc:1121).

### Prediction before editing

Use a memtable limit of 100 so the following steps do not auto-flush. On paper predict both the value and the source of each Get:

```text
Put("a", "old")
Flush()
Put("a", "new")
Get("a")
Delete("a")
Get("a")
Flush()
Get("a")
Compact()
Get("a")
```

Also predict a lookup for a never-written key. Explain the distinction between “no entry found” and “entry found indicating deletion,” even though ordinary Get returns absence in both cases.

### Implement one small feature

Add an explainable lookup API, tentatively `GetWithSource`. Specify its result before coding: lookup outcome (value/deleted/missing), optional value, and source (memtable or immutable-table position). A source position describes this instant's layout, not a permanent table identifier.

Keep existing Get behavior unchanged. Do not add persistence, snapshots or concurrency in this exercise. Decide how a caller can observe a tombstone without accidentally treating it as a live value. Reuse the existing newest-entry resolution where possible so the diagnostic API cannot disagree with Get.

### Targeted checks

1. A mutable overwrite wins over an older immutable value.
2. A mutable tombstone hides an older value and reports deletion from memory.
3. After flush, the deletion is resolved from the newest immutable table.
4. A never-written key reports missing rather than a deletion entry.
5. Compaction may change the reported source but preserves the visible answer.
6. Scan still omits deleted keys and agrees with point lookups.

The current teaching model retains tombstones during compaction. Do not write an expectation that compaction automatically removes every deletion marker. Later reclamation work must justify the conditions for removal.

### Explain the result

Show the before/after state, explain why search stops at a tombstone, and identify which logic is correctness versus diagnostics. Then compare the toy lookup order with LevelDB's sequence-aware visibility. The toy model does not yet support retained historical snapshots.

## A shared completion record

For each step record:

- Contract and intentionally omitted behavior.
- Data structures and operation sequence.
- One normal history and one adversarial history.
- Reference oracle and failure model where applicable.
- Measured bytes, memory, operations or time relevant to the mechanism.
- A production source comparison and one remaining question.

Use tests that challenge the invariant rather than duplicate implementation decisions. Keep failed histories and seeds so later changes cannot silently reintroduce them.

## Moving from implementation to research

After a mechanism is correct, vary one assumption: available memory, key skew, late data, snapshot age, outage length or repair budget. Predict the effect before measurement. Explain the result and examine an alternative design. A surprising, reproducible boundary can become a research question; a working tutorial implementation alone does not establish novelty.

## Current execution status

The source/header, CMake configuration and existing test were inspected on 2026-10-27. This workstation exposes a C++ compiler, but `cmake` was not found on the inspected shell PATH. The documented CMake build/test sequence was therefore not executed, and no compiler installation or alternative build procedure was attempted. No source files were edited. Build prerequisites should be resolved within the chosen implementation workflow before claiming a passing baseline.

# RocksLab — Week 1: Build an LSM Model

This lab teaches the shape of an LSM tree before using RocksDB. It is a
single-threaded, in-memory model, deliberately omitting production concerns
such as a WAL, real SST serialization, block indexes, caching, Bloom filters,
and concurrent compaction.

## Learning outcomes

After completing this lab, you should be able to trace:

1. A write into the sorted mutable **MemTable**.
2. A flush that turns the MemTable into an immutable sorted **SSTable**.
3. A lookup through the MemTable and immutable tables from newest to oldest.
4. A deletion represented as a **tombstone**.
5. Compaction merging tables while retaining the newest entry per key.

## Day-by-day plan

### Day 1 — The model (60–90 minutes)

Read `include/mini_lsm.h`. Draw the lifetime of `put(a, 1)`, then
`put(a, 2)`, through a flush. Answer: why must the read path prefer newer
structures?

### Day 2 — MemTable and flush (60–90 minutes)

Read `Put`, `Delete`, and `Flush` in `src/mini_lsm.cc`. Run three writes:
the third distinct key flushes the MemTable. Use `state` before and after.

### Day 3 — Read correctness (60–90 minutes)

Read `FindNewest` and `Get`. Make a prediction before each command:

```text
put a old
put b two
put a new
get a
state
```

Explain why returning `old` from the SSTable would be incorrect.

### Day 4 — Range scans and tombstones (60–90 minutes)

Read `Scan`. Run a scan spanning both the MemTable and an SSTable, then delete
a key and scan again. Explain why a tombstone must participate in scans rather
than simply disappearing.

### Day 5 — Compaction (60–90 minutes)

Create at least three SSTables by flushing repeatedly. Inspect `state`, run
`compact`, and inspect it again. Write down which old versions disappeared and
why the live answer to each key did not change.

### Day 6 — Extend the model (90 minutes)

Choose one extension:

- Add a `GetWithSource` command that says `MemTable` or `SSTable[n]`.
- Add a compaction counter for input and output entries; use it to estimate
  write amplification in this toy model.
- Add an explicit `flush` threshold argument to the CLI.

Add a test before implementing the extension.

### Day 7 — Explain it without code (45–60 minutes)

Write one page answering:

- Why do LSM trees make writes cheap?
- Why can one logical key occupy more than one physical record?
- Why are compactions necessary?
- What is this toy model missing before it can be a durable database?

## Build and run

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/rockslab
```

Try this session:

```text
put apple red
put banana yellow
put apple green
state
get apple
delete banana
state
scan a z
compact
state
quit
```

## What differs from production RocksDB

Real RocksDB writes a WAL for recovery; uses skip-list-like MemTables; creates
SST files on disk with data/index/filter blocks; runs background compactions;
supports snapshots and concurrent access; and uses multiple levels, not one
flat SSTable list. Week 2 replaces this model with the real RocksDB API.

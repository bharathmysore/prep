# Machines, bytes, and durable state

[Contents](../README.md) · Next: [Local engines](02-local-engines.md)

A storage system turns a logical operation into a recoverable change in bytes. Before studying algorithms, identify the hardware resource consumed, the state that becomes authoritative, and the failures that authority survives. The principles below are explanatory models; the system dossiers link them to implementation evidence.

## 1. Interfaces determine the problem

| Interface | Addressing unit | Required interpretation | Local examples |
|---|---|---|---|
| Block | Device offset and length | Caller owns filesystem/database format | Ceph RBD |
| File | Path, inode, offset | Directories, permissions, names and often POSIX behavior | CephFS, CubeFS, SeaweedFS mount |
| Object | Bucket and key | Whole object, metadata, multipart and version semantics vary | Ceph RGW, SeaweedFS S3, CubeFS object/blob paths |
| Ordered key value | Byte key and value | Point lookup, ordered range scan, batches or transactions | LevelDB, RocksDB, LMDB, TiKV, FoundationDB, etcd |
| Structured data | SQL row, data structure, vector | Indexing and execution above storage | OceanBase, Redis |

An S3 endpoint does not imply a particular durability model. A POSIX mount does not prove every POSIX semantic. A key-value engine does not imply a networked database. First specify the contract; then inspect its implementation.

## 2. The hardware hierarchy

Registers and CPU caches serve computation; DRAM holds mutable state and caches; persistent devices hold recoverable data; networks connect independent failure domains. SSDs reduce seek costs but still have finite write bandwidth, endurance, internal garbage collection, and tail latency. HDD workloads benefit strongly from sequential access. Remote storage adds transport, service queues, and failure ambiguity.

The useful question is not simply whether an access is O(1) or O(log n). Ask how many cache misses, device operations, bytes transferred, CPU cycles, and dependent round trips it requires. A hash lookup that faults a page can be slower than several cached tree traversals. A Bloom filter can save I/O while consuming CPU and memory.

## 3. Process memory is not durable storage

A normal buffered file write may copy data into the operating system page cache. The application receiving success is distinct from the device durably preserving those bytes. Synchronization operations request stronger ordering and persistence, subject to the filesystem, device, and platform contracts. File creation or rename can also require directory metadata durability. Do not generalize one operating system's behavior to every platform.

LevelDB exposes this distinction explicitly: [WriteOptions](/Users/bmysoren/tpt/leveldb/include/leveldb/options.h:167) documents process-crash versus machine-crash behavior and defaults `sync` to false. This is the first source contract to read before calling an engine durable.

A useful ladder is:

1. Accepted into application memory.
2. Copied into the kernel or an asynchronous output queue.
3. Persisted on one device.
4. Persisted on the required replicas or coding fragments.
5. Committed according to the system's ordering protocol.
6. Applied to a queryable representation.
7. Included in a separately recoverable backup.

This is a checklist, not a universal ordering: some systems apply before replying, some query a materialized state that lags a committed log, and some do not have replicas. Name exactly which conditions a successful reply guarantees.

## 4. Atomicity requires a publication rule

A machine can stop between any two writes. A multi-page update therefore needs a method to distinguish the old committed state from the new committed state.

**Write-ahead logging:** append enough recovery information before publishing dependent state. Recovery replays valid committed records; torn or incomplete tails need detection and handling. The WAL is not the engine's diagnostic log.

**Copy on write:** write new pages without overwriting pages reachable by existing readers, then publish a new committed root. Recovery finds a valid root; obsolete pages become reusable only when no protected snapshot needs them. LMDB is the local example.

**Immutable files plus manifest:** construct a new sorted file, then durably publish the file-set edit. Until the manifest refers to the new file, it is not authoritative merely because it exists. LevelDB's [MANIFEST and CURRENT](/Users/bmysoren/tpt/leveldb/doc/impl.md:44) illustrate this split. Read code for exact ordering; old prose is not an executable crash-safety specification.

Each approach has metadata that is as important as the data: log positions, transaction IDs, root pages, file sets, sequence numbers and checksums.

## 5. Checksums and identity

A checksum detects many forms of corruption; it does not recreate the correct bytes. Recovery needs another valid copy, sufficient erasure fragments, or a backup. A checksum that covers only payload may miss misdirected writes unless identity and ordering are also protected. Ask which layer checks which unit: network frame, log record, block, object or whole file.

Scrubbing checks stored state proactively. Read repair reacts to disagreement observed during reads. Replica repair restores redundancy. These are different mechanisms and have different resource costs.

## 6. Failure models

| Failure | Key question |
|---|---|
| Process crash | Can state be reconstructed from kernel/device state? |
| Power or host failure | Which acknowledged operations reached persistent media? |
| Device corruption | Can bad bytes be identified and reconstructed? |
| Network partition | Who may continue making authoritative decisions? |
| Slow node | Is it required for the request, quorum, or recovery? |
| Zone loss | Were copies or fragments placed in independent zones? |
| Client timeout | Did the operation fail, commit, or remain uncertain? |
| Operator error | Can a deletion be recovered outside the replication domain? |

Replication also replicates accidental deletion. A snapshot may share the failure domain of its source. A backup needs a restorable copy and a tested restoration procedure.

## 7. Worked durability example

Suppose a client writes `balance = 40` and times out. A retry cannot safely assume the first attempt failed: it may have committed and the reply may have been lost. Repeating an assignment can be idempotent; repeating `decrement by 10` is not. An operation ID, transactional deduplication, or protocol-specific retry rule is needed to preserve intent. A returned error and an unknown commit outcome are different facts.

## Check your understanding

Explain what happens if a crash occurs after a new SST is written but before its manifest edit, after a WAL record is appended but before sync, and after a replicated commit but before a client reply. For each case, name the authoritative recovery state and whether a retry is safe.

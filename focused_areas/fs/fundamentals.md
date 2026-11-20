# Filesystem fundamentals

## The abstraction

A filesystem maps names and byte ranges to stored data while defining permissions, sharing, and failure behavior. Block storage exposes addressable blocks; object storage exposes named objects and API operations; file storage exposes a namespace and file operations. A mounted object gateway does not automatically acquire every POSIX guarantee.

```text
Application
  → syscall / VFS
  → pathname lookup and filesystem metadata
  → page cache / filesystem allocation and mapping
  → block layer / driver
  → device controller and persistent media
```

A distributed filesystem adds client caches, network protocols, remote metadata ownership, remote data placement, and distributed recovery.

## Namespace and metadata

Directory entries map names to inode identifiers. An inode holds attributes and data mapping; a file descriptor refers to an open-file description, with access state such as an offset. Hard links share an inode; symbolic links contain a path. Unlink removes a name; storage reclamation may wait for the last link and open reference.

Lookup, stat, create, rename, and unlink consume metadata resources even when little payload moves. Directory layout, hot parents, permission checks, and distributed locks can dominate a small-file workload.

Exercise: draw a directory with two hard links and one open descriptor. Remove both names. Explain why the open descriptor may still access data.

## Data layout

A block map identifies individual blocks. An extent maps a contiguous logical range to a contiguous physical range, reducing mapping overhead when locality exists. Sparse files can have holes that read as zeros without allocating every logical block. Delayed allocation can improve placement but delays some space-allocation failures until writeback.

Allocation groups and partitioned metadata can improve concurrency; hot objects still require coordination. Fragmentation adds requests and mapping work. SSDs remove seek mechanics, not all locality effects: request processing, mapping, garbage collection, and network overhead remain.

## Caching and I/O

Buffered reads can hit the page cache. Readahead predicts future access. Buffered writes dirty cached pages, and writeback moves them toward storage. Metadata caches can avoid repeated pathname and attribute work.

Direct I/O has filesystem-specific alignment and coherence constraints. It does not by itself imply durability. mmap changes how the process accesses pages; it does not remove persistence and ordering questions.

Measure both cache-resident and cache-miss behavior. A process restart usually does not clear the OS page cache.

## Persistence and crash consistency

Successful write means bytes were accepted under the interface contract; it does not generally mean power-loss persistence. fsync requests synchronization of a file's modified data and required metadata, subject to filesystem, device, and remote-service guarantees. Directory-entry persistence is a separate concern.

A common local-filesystem replacement sequence is:

1. Create a temporary file in the destination directory.
2. Write all contents, handling short writes and errors.
3. fsync the temporary file and check the result.
4. Rename it to the destination on the same filesystem.
5. fsync the parent directory and check the result.

Atomic namespace replacement and durability are distinct. This sequence is not a transaction across multiple files or filesystems. Verify supported directory-sync and remote semantics for the actual deployment.

Journaling records enough information to restore a consistent state after interruption. Metadata-only journaling does not automatically make arbitrary application data updates atomic. Copy-on-write builds replacement state and publishes a new root; reference management, free-space accounting, and write ordering remain difficult.

## Four different guarantees

| Guarantee | Question |
| --- | --- |
| Consistency | What can concurrent clients observe? |
| Durability | What acknowledged state survives the stated failure? |
| Availability | Which operations complete during the failure? |
| Integrity | Can corruption be detected and repaired? |

Replication is not a substitute for independent backups: it can replicate an accidental deletion. Checksums detect certain corruption; repair also needs a valid redundant source. Snapshots share failure domains with their source unless separately protected.

## Concurrency

Define the atomicity boundary of each operation. Locks protect state transitions but can limit throughput and cause deadlock. In a distributed filesystem, leases/capabilities can permit local caching; revocation and fencing are required to handle ownership changes safely. Timeouts indicate suspicion, not proof that a server is dead.

## Capacity and write amplification

Track raw bytes, protected usable bytes, reserved headroom, metadata, snapshots, and actual available capacity separately. Compression ratios depend on data and should be measured. Logical write bytes can become more physical bytes due to journals, replication/parity, metadata, and device-level amplification.

## Explain these without notes

- Why can a 4 KiB write require several other writes?
- Why can a directory-heavy workload saturate a filesystem at low GB/s?
- What must be ordered before a journal commit is durable?
- Why does a checksum mismatch not identify the correct replica by itself?
- Why can full-capacity behavior be much worse than steady-state behavior?

## References

See [OSTEP, Linux VFS, ext4, XFS, OpenZFS, and fsync sources](references.md). Product and protocol behavior must be checked against the deployed version.

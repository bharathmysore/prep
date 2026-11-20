# Implementation exercises

## Working contract

Build one educational system in stages. Use C++17, a regular disk-image file, explicit serialization, and deterministic tests. Keep the first version single-threaded with fixed limits. Define the supported behavior before extending it. The [starter samples](samples/README.md) illustrate E1, E3, and E7; all full milestone implementations below remain learner work.

For every exercise submit code, tests, a diagram, invariants, time/space analysis, measurements where relevant, and a short explanation of unsupported guarantees. These are storage-learning exercises, not claims of company interview frequency.

## E1. Virtual block device

**Question:** implement bounded block reads and writes over a regular file.

**Idea/tags:** `block-io`, `bounds-checking`, `raii`. Use a fixed block size; checked multiplication maps block IDs to offsets. Loop over interrupted/short I/O and distinguish EOF from zeros in a valid allocated block.

**API/state:** create an exclusive new image; open an existing validated image; read/write a block; sync; report geometry. A descriptor owns the resource; geometry defines valid offsets. Do not serialize native C++ structs as a permanent format.

**Invariants:** writes cannot extend the configured image; reads cannot silently accept truncated images; failures are visible. One caller at a time initially.

**Tests:** first/last block, exact reopen, out-of-range IDs, wrong write length, invalid geometry, truncated backing file and injected short operations. See the working sample's [test cases](samples/test_cases.md#block-device).

**Complexity:** O(B) bytes per block operation; O(B) transfer buffer, O(1) device metadata. Count physical I/O separately from CPU work.

**Optimize/follow up:** batch adjacent blocks for throughput; reuse buffers for memory. Add a format header, checksums, allocation bitmap, and deterministic fault injection. Explain what POSIX fsync establishes and what a model cannot prove about hardware.

## E2. Minimal persistent filesystem

**Question:** implement format/create/read/write/list/unlink/check with a root directory and bounded filenames.

**Idea/tags:** `inode`, `bitmap`, `namespace`, `block-mapping`. Baseline: fixed inode table and direct block pointers, then introduce extents after correctness.

**State:** superblock with magic/version/geometry; inode and block bitmaps; inode table; directory records; data area. Define byte order, field widths, maximum sizes and error behavior. Reconstruct in-memory indexes from durable state.

**Invariants:** reserved blocks stay reserved; allocated file blocks have valid owners; directory entries reference live inodes; sizes fit mapping limits. The checker compares reachable blocks/inodes to allocation state. Initially reject hard links, sparse writes, and nested directories unless implemented deliberately.

**Tests:** empty file; boundary-crossing read/write; duplicate create; nonexistent path; disk/inode exhaustion; delete/reuse; reopening; malformed inode pointer. Document partial-write behavior under exhaustion.

**Complexity:** direct directory scan O(D); bitmap search O(M) worst case; file transfer O(K+B) for K mapped blocks and B copied bytes. State the actual representation and limits.

**Optimize/follow up:** hashed directories, next-fit allocation, extents, sparse files, hard links and open-but-unlinked files. Revisit every invariant as features change.

## E3. Crash-consistent metadata

**Question:** recover atomic metadata transactions after any modeled persistent-write boundary.

**Idea/tags:** `write-ahead-log`, `recovery`, `idempotency`. Start with one outstanding redo transaction. Order journal payload persistence before commit persistence, and commit persistence before home updates. Clear/reuse the journal only after home state is durable.

**State:** framed transaction records, sequence/generation, target identifiers, payload, integrity field, commit record, replay state. The starter uses atomic in-memory fields to expose ordering; the exercise must define record validation on an image.

**Invariants:** no uncommitted home update; committed work remains recoverable; replay is idempotent; a corrupt journal is not replayed blindly.

**Tests:** crash before/after every stage; crash during replay; run replay twice; incomplete/torn commit; invalid length/checksum; journal wrap/reuse. Distinguish structural consistency from durability of application bytes.

**Complexity:** replay O(J) journal bytes plus home writes; memory O(max record) for streaming or O(J) if buffered. Count extra write traffic.

**Optimize/follow up:** group commit and bounded log space; compare metadata journaling with data journaling and copy-on-write. Define recovery when an I/O error prevents progress.

## E4. Cache and extent allocation

**Question:** add a bounded block cache and compare individual-block mapping to extents.

**Idea/tags:** `lru-cache`, `writeback`, `extent-allocation`. Start with read-through caching; add dirty pages only after persistence behavior is explicit.

**Invariants:** dirty data is never silently evicted; cache entries match the intended file generation; capacity is bounded; sync errors reach callers.

**Tests:** hit/miss; eviction; capacity one; overwrite then read; dirty eviction failure; stale entry after delete/reuse; scans larger than cache. Sweep sequential/random reads and fragmented allocation.

**Complexity:** expected O(1) LRU lookup/update with hash+list, O(C×B) payload memory. Extent lookup depends on representation (O(E) vector scan or O(log E) ordered index).

**Optimize/follow up:** readahead, scan-resistant admission, coalescing, buffer reuse. Distinguish runtime optimizations from reduced metadata/buffer memory. Later specify lock ordering and cancellation for background writeback.

## E5. Remote operation retry simulator

**Question:** preserve mutation semantics when requests or responses are lost, duplicated, delayed or reordered.

**Idea/tags:** `rpc`, `deduplication`, `idempotency`. Use an event queue before real networking. Identify clients by session and request sequence; retain the request fingerprint and result.

**Invariants:** same request ID and same payload replay the prior result; same ID with different payload is rejected. A durable effect and deduplication record must recover consistently. Otherwise a server restart reintroduces duplicate effects.

**Tests:** lost request; lost response; duplicate delivery; restart after effect/before response; ID collision; dedupe eviction; stale client session. Retries have a budget; timeout is an unknown outcome.

**Complexity:** expected O(1) dedupe lookup; O(R) retained results; O(M) scheduled-message memory. State retention and garbage-collection policies.

**Optimize/follow up:** batching, bounded result windows, backpressure. Discuss why this does not give arbitrary external side effects exactly-once behavior.

## E6. Replicated chunk store simulator

**Question:** replicate chunks to three nodes and define which acknowledged writes survive one-node loss.

**Idea/tags:** `replication`, `epochs`, `checksums`, `repair`. Start with a fixed leader, ordered writes, durable copies, explicit two-copy acknowledgment, and no automatic failover.

**State:** node ID/failure domain, chunk ID/version/checksum, epoch, sequence, pending acknowledgments and repair queue. Use a deterministic event scheduler; clocks are not correctness authorities.

**Invariants:** acknowledgments reflect the required durable copies; old epochs cannot mutate current ownership; repair never overwrites a newer valid version with stale data. Liveness depends on required replicas/network being reachable.

**Tests:** node loss before/after acknowledgment; delayed old write; partition; corrupt copy; repair plus foreground traffic; failed leader. Document which operations stop. Do not claim quorum intersection alone implements consensus.

**Complexity:** O(r×B) network bytes per replicated B-byte write; O(r×B) protected storage; repair proportional to reconstructed bytes plus metadata. Track message count and queued work.

**Optimize/follow up:** batch replication, bound repair bandwidth/queue memory, and compare erasure coding. Study a separate replicated-log/consensus model before automatic election, membership change and safe committed-prefix recovery.

## E7. Parallel layout and metadata service

**Question:** map file byte ranges to striped targets and serve data without routing every byte through metadata.

**Idea/tags:** `striping`, `metadata-partitioning`, `range-mapping`. Metadata owns identity, access, length and a versioned layout. Clients obtain a layout and contact targets directly.

**Invariants:** mapping covers a requested range exactly once; inverse mapping reconstructs offsets; stale layouts cannot silently update a new file generation. The sample implements only immutable arithmetic.

**Tests:** zero length; stripe boundaries; target-cycle wrap; range overflow; zero stripe/target count; reconstruction from returned fragments. See [sample cases](samples/test_cases.md#striping).

**Complexity:** O(F) for F returned fragments and O(F) result memory; mapping one offset is O(1).

**Optimize/follow up:** coalesce target requests, vary stripe width, bound outstanding requests. Add metadata partitioning only after measuring hotspots; specify cross-directory rename and lock ownership.

## E8. AI/ML input and checkpoint harness

**Question:** demonstrate how record layout and persistence policy affect useful workload progress.

**Idea/tags:** `dataset-layout`, `checkpoint`, `manifest-publication`. Read identical logical records from individual files versus indexed shards. Write immutable checkpoint generations and publish a complete manifest.

**Invariants:** sampled records are equivalent across layouts; a published checkpoint refers only to complete durable shards; restore rejects missing/corrupt/incompatible state; failed work cannot replace a valid generation.

**Tests:** seeded sampling; incomplete shards; slow writer; storage full; writer restart; failure before/after manifest publication; restore on another client. Specify measured cache state and whether write rates include final sync.

**Complexity:** payload work O(B) plus O(F) file operations; manifest O(S) shard entries. Measure peak staging memory for asynchronous checkpoints.

**Optimize/follow up:** prefetch/batching, compression, cache policy, asynchronous snapshots, fairness between training and checkpoints. Assess CPU decoding, shuffle quality and overfetch rather than reporting only filesystem GB/s.

## Final architecture review

Use [the review template](architecture.md#architecture-review-template) and [AI/ML capstone](ai_ml.md#capstone). Submit normal and degraded results, exact guarantees, operational controls, capacity/cost models, alternatives, and unresolved risks.

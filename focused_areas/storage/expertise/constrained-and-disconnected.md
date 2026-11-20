# Storage under bounded resources and intermittent connectivity

[Expertise path](README.md) · [Time series](time-series.md) · [Labs](labs.md)

Treat resource constraints and disconnection as explicit operating conditions. Start by distinguishing a small Linux machine, a phone/browser client and a microcontroller with raw flash. They have different OS, memory, power and storage contracts. This chapter supplies architectural reasoning and supplemental primary readings, not a claim that the existing server checkouts run on every such device.

## 1 Write the envelope before selecting an engine

Specify RAM peak, persistent capacity, erase/program geometry where exposed, CPU, energy, acceptable loss, maximum outage, write rate and recovery deadline. Include metadata, caches, temporary output, replication/outbox backlog and reserve space. A mapped database is not automatically constant-memory: page residency, page faults, address space and writer allocations still matter.

Use LMDB to study reader-driven page reuse and LevelDB/RocksDB to study maintenance debt. Ask whether the engine can remain within the envelope during compaction, index rebuild or restart, not just when idle.

**Gate:** account for peak resources in normal ingestion, longest supported outage, reconnection and recovery.

## 2 Flash and power loss change the cost model

Raw NOR flash exposes erase/program restrictions; managed SSD/eMMC devices hide additional translation and wear behavior. Distinguish application, engine and device write amplification. Repeatedly changing a tiny metadata location can be costly even if payload volume is low.

The [littlefs design](https://github.com/littlefs-project/littlefs/blob/master/DESIGN.md) treats power-loss resilience, wear leveling and bounded memory as primary goals for microcontrollers. Its metadata-pair and copy-on-write approach is useful supplemental study beside a database engine. littlefs is a filesystem, not a SQL transaction or replica synchronization protocol; its presence cannot prove an application's multi-file update is atomic.

**Gate:** identify where logical publication depends on device ordering, and how interrupted erase/program operations are represented in a test model.

## 3 Local database recovery is distinct from synchronization

SQLite offers a complementary local study: its [WAL documentation](https://www.sqlite.org/wal.html) explains log frames, readers and checkpointing. Readers and a writer can overlap, while long readers can impede checkpoint progress. Study local journal/checkpoint state separately from an upload outbox. The database WAL is generally not an application replication protocol or an indefinitely retained change feed.

Define local success independently of remote success. If the user may act while disconnected, the critical operation cannot require a reachable remote quorum unless failure/rejection is the intended behavior.

**Exercise:** draw `stored locally`, `queued for transfer`, `accepted remotely` and `reconciled locally` as separate states. Mark which survive a device restart and which require a network.

## 4 Store and forward with a durable outbox

A telemetry design can atomically record a measurement and its pending-transfer entry in local transactional state. Give each event stable identity, such as device identity plus a persistent generation/sequence scheme. Reboot or device cloning must not accidentally reuse identity.

Send bounded batches, retry after uncertain outcomes, and persist progress only at defined acknowledgments. At the receiver, make deduplication and the intended state change atomic within the relevant scope. A lost reply otherwise lets one accepted batch be applied twice.

This can deliver an exactly-once effect within a bounded application contract; it does not make network delivery universally exactly once. Receiver dedup retention must cover the supported retry horizon, or the protocol must force resynchronization before older identities are replayed.

**Gate:** survive a crash between local append and send, receiver commit and reply, reply and local progress update, and deletion of acknowledged outbox entries.

## 5 Some problems require merging, others require authority

Append-only sensor events, mutable device configuration and inventory allocation are different data models. Independent append events can be reconciled by identity. Concurrent configuration changes need a conflict policy. Allocating one unique resource to two disconnected users may require a designated authority, preallocated rights, or rejecting some offline actions.

Last-writer-wins based solely on wall-clock time can silently discard intent when clocks drift. Causal metadata can distinguish concurrent changes, but preserving that metadata and deciding business meaning still costs resources.

For a concrete merge-oriented reference, Automerge separates a CRDT document/sync core from repository storage/network plumbing, with a transport-agnostic protocol. See [Automerge concepts](https://automerge.org/docs/reference/concepts/). Convergence does not mean arbitrary business invariants are preserved. [Ink & Switch's local-first principles](https://www.inkandswitch.com/local-first-software/) provide motivation for offline work and user-held data, rather than a universal implementation recipe.

**Gate:** demonstrate a conflict that merges mechanically but violates the application's intended rule.

## 6 Long absences make deletion and history expensive

A device returning after months may still hold values deleted elsewhere. Tombstones or version knowledge can prevent resurrection, but retaining them forever can violate a bounded-space requirement. Safe reclamation needs a membership/acknowledgment rule, a bounded absence policy with forced snapshot resync, or another protocol-specific proof.

If a peer can disappear indefinitely and later replay arbitrary old state, do not promise both unrestricted history-free rejoin and safe deletion without additional constraints. Name the constraint in the contract.

## 7 Derive the outage budget

Illustrative workload: 200 events/s at 80 encoded bytes/event is 16,000 bytes/s. A 24-hour outage accumulates 1,382,400,000 bytes, about 1.38 GB decimal before indexes, journals and reserve. A device with 256 MB free cannot buffer that losslessly; changing the policy is necessary.

If effective post-reconnect upload service is 40,000 bytes/s while new encoded input remains 16,000 bytes/s, backlog drains at 24,000 bytes/s. The ideal drain time is 57,600 seconds, or 16 hours. Real transfer overhead, energy limits and local contention can increase it. At or below the incoming rate, there is no positive drain rate.

Choose explicitly between admission/backpressure, bounded data loss, aggregation, additional storage and a shorter supported outage. Compression ratios are workload-dependent inputs, not guaranteed capacity.

## 8 Access control and schema evolution survive disconnection too

A disconnected client may miss credential revocation or schema updates. Define what it may do locally, what the server later accepts, and how rejected data is surfaced. Encryption and authentication do not by themselves decide conflict policy or revocation timing.

Version envelopes and migrations must coexist with queued old records. A server upgrade that rejects all buffered old messages can break the promised outage horizon. Include identity rotation, replay, partial batch errors and incompatible schema in the test matrix.

## Exit artifact

Write a resource envelope, acknowledgment contract, retry/dedup state machine, conflict policy, deletion/rejoin rule and schema-evolution policy. Demonstrate each with a deterministic small history before choosing a more elaborate protocol.

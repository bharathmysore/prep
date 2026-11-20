# Reasoning about performance and failure

[Contents](README.md)

Use a path-and-budget model to turn architecture knowledge into predictions. This chapter supplies reasoning exercises, not measured results or an instruction to operate live clusters.

## 1. Specify a workload before naming a bottleneck

Record key/value sizes, live dataset, working set, read/write/delete mix, range-scan lengths, concurrency, skew, arrival pattern and transaction conflict rate. Name durability options, replication/coding, network distances, cache state and failure conditions. A requests-per-second number without these inputs cannot explain architecture.

Separate offered load from completed goodput. Count retries and errors. Closed-loop clients can reduce offered load when latency rises, masking overload; open-loop tests can expose queue growth but need careful accounting. Percentiles describe distributions, not causes.

## 2. Draw the serial critical path

For an illustrative replicated write:

`client routing → leader queue → log work → required peer persistence → commit decision → apply/publication → reply`

Actual systems overlap or reorder stages and have different response conditions. Mark parallel fan-out and identify which responses gate success: all replicas, a quorum, or a configured subset. Tail latency is influenced by the order statistic of required responses and by queues shared across requests.

For a read, separate routing lookup, version/barrier acquisition, local cache/index work, device reads and deserialization. A cache hit can still pay a distributed read barrier. A fast local engine cannot remove required network round trips.

## 3. Apply resource ceilings

For each request estimate CPU time, device bytes/operations, network bytes and serialized coordination work. Divide each resource's available capacity by cost per request to get rough ceilings. The lowest ceiling is the first bottleneck candidate; queues and interference often make usable capacity lower.

Little's law, `L = λW`, relates average in-system work `L`, stable arrival/completion rate `λ` and average residence time `W`. It is not a formula for p99 or a license to ignore overload. If arrival exceeds sustainable service, the stationarity assumption breaks down.

Amdahl's law explains why parallelizing background work cannot remove a serialized leader, writer or hot metadata lock. A single hot key also defeats a design that balances only by key-range count.

## 4. Measure maintenance debt

| Architecture | Debt to watch | Potential foreground symptom |
|---|---|---|
| LSM | Immutable memtables, young files, pending compaction bytes | Stalls, lookup fan-out, I/O contention |
| MVCC | Oldest active snapshot and retained versions | Disk growth, longer scans/compactions |
| Copy on write | Old reader epochs and reusable-page backlog | File growth or allocation pressure |
| Replication | Persisted/committed/applied lag and retained logs | Slow reads, failover recovery, log growth |
| File/object | Orphan/garbage backlog, metadata queues, vacuum | Capacity leak or delete latency |
| Redundant storage | Degraded objects/fragments and repair backlog | Lower resilience and contention |
| Memory server | Expiry/eviction backlog, allocator overhead, snapshot COW | Latency spikes, memory pressure |

These are conceptual measurements. The dossier reading routes locate the relevant implementation; metric names are not assumed to exist.

## 5. Reason about a failure using invariants

Write the invariant first: no committed value lost under the stated failure model; no two owners publish conflicting state; no object reclaimed while referenced; no transaction exposes half its update. Then list the durable evidence used to reestablish the invariant after restart.

For a crash at each boundary, record what is definitely persisted, possibly persisted, authoritative, and visible. Distinguish a protocol guarantee from a test observation. Passing many crash tests is useful evidence; it is not a proof that every timing is safe.

## 6. A comparative thought experiment

Assume a uniformly distributed 100 GB dataset, small values, 80% reads, 20% writes, and no scans. This is a reasoning input, not an observed workload.

- LevelDB/RocksDB: predict cache sensitivity and whether compaction keeps up with the write stream.
- LMDB: predict page working-set behavior and serialized writer cost.
- Redis: predict resident memory cost, command CPU and the chosen persistence overhead; do not assume the dataset fits.
- TiKV: add Region routing, transaction mode, Raft and distributed timestamps.
- FoundationDB: add version acquisition, role-specific capacity and conflict checking.
- OceanBase: add SQL plan/index work and participant count.
- etcd: examine whether the coordination workload and backend limits fit the scenario before extrapolating.

Now make 50% of writes target one key. Identify what splitting, balancing and caching can no longer solve. Then replace writes with deletes and hold an old snapshot: the reclamation problem becomes central.

## 7. Failure experiments to design before executing

For each production system, design an isolated test for process crash, host loss, partition, slow device, coordinator loss, metadata unavailability and repair under load. Define expected visible results, acknowledgment semantics and recovery bounds. Include a client timeout/retry case and a backup restoration case.

No service, workload or fault injection was run while creating this knowledge base. The tests are a learning and validation agenda; operational execution requires the applicable environment scope and workflow authorization.

## 8. Evidence that changes an architectural prediction

If p99 increases with young-file count while device busy time rises, compaction interference is plausible. If p99 increases while disks are idle and a leader CPU saturates, investigate serialized work. If object bytes remain flat while create/list latency rises, inspect metadata paths. These are hypotheses, not diagnoses; correlate timing, bound alternative causes and inspect the actual wait path.

A useful conclusion states mechanism, evidence, confidence, and the next observation that could disprove it.

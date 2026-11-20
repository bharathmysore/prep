# Filesystem architecture and tradeoffs

## Study systems through the same questions

For each system identify workload assumptions, APIs and semantics, metadata ownership, data layout, cache coherence, acknowledgment point, failure domains, recovery, scaling limits, security, observability, and cost.

| Family | Architectural lesson | Questions to investigate |
| --- | --- | --- |
| ext4 | Inodes, extents, journal, block allocation | Which metadata is journaled? How do data ordering modes differ? |
| XFS | Allocation groups, metadata logging, parallel operations | Which shared objects remain serialization points? |
| ZFS | Copy-on-write, checksums, snapshots, pooled storage | How do record size, fragmentation, and free space affect behavior? |
| NFS | Remote operations, caches, protocol state | What does the selected protocol/version guarantee on retries and caching? |
| Lustre | Separate metadata and data services, layouts, parallel I/O | How do stripe size/count and directory distribution affect the workload? |
| CephFS | MDS capabilities, RADOS, CRUSH, protection and repair | When does the MDS limit metadata and when do OSDs/network limit data? |
| WEKA | Distributed metadata, parallel access, flash-oriented software | What client CPU, networking, licensing, and operational resources are required? |

## Parallel filesystem paths

```text
Metadata/control-related path:
Client → namespace/metadata service → file identity, permissions, layout

Bulk data path:
Client → storage target A
       → storage target B
       → storage target C
```

Metadata is still involved in opens, layout changes, locking, and other coordination. Separating paths does not eliminate coherence or recovery work. In real systems the placement of control and metadata responsibilities differs.

## Tradeoff matrix

| Choice | Benefit | Cost / failure pressure | Default reasoning |
| --- | --- | --- | --- |
| Replication | Redundant complete copies; simple repair sources | Capacity, network, write fanout | Useful baseline for a prototype; define acknowledgment rules |
| Erasure coding | Capacity efficiency | Encoding, partial stripes, repair reads and traffic | Evaluate by write pattern and rebuild objective |
| Wide striping | Parallel service of large files | Fanout, slow targets, coordination | Sweep stripe count against actual client concurrency |
| Strong cache coherence | Predictable shared access | Invalidations, recalls, contention | Pay for guarantees the application actually needs |
| Writeback cache | Batching and latency absorption | Dirty-data limits and recovery obligations | Document persistence boundaries and sustained drain rate |
| Shared storage | Shared access and centralized lifecycle | Network and shared contention | Budget aggregate and per-client performance |
| Local cache | Locality and reduced remote traffic | Eviction, duplicates, placement, stale data | Specify cache keys, versioning, invalidation, and node loss |
| Central metadata | Simple ordering and ownership | Hotspot and failover scope | Good educational baseline; partition after measuring |
| Partitioned metadata | More concurrent work | Cross-partition operations and skew | Define ownership and distributed transaction boundaries |

## Failure and replication exercise

Assume three replicas in distinct failure domains. A fixed leader assigns an epoch and sequence number. Compare acknowledgment after one, two, and all three durable copies. A two-copy acknowledgment alone is not a complete consensus or failover protocol: a replacement leader must recover the committed prefix and fence stale owners. Read/write quorum intersection alone does not solve concurrent ordering, stale leaders, or membership changes.

State separately: allowed failures, readable state during a partition, writable partition, repair source selection, and how an acknowledged write remains reachable after failover.

## Metadata versus data scaling

Many small files stress opens, stats, directory traversal, cache misses, and locks. A few huge files can stress streaming bandwidth and layout balance. A single hot directory, file, or lock can prevent otherwise distributed resources from scaling. Metadata servers may be scalable as a cluster while one indivisible operation remains serialized.

## Architecture review template

1. Problem and optimization target; what would make the design fail?
2. Functional operations and nonfunctional objectives.
3. Workload: file counts/sizes, access distribution, clients, bursts, working set.
4. Components: public APIs, in-memory/durable state, indexes, background work.
5. Read/write paths with exact acknowledgment and ownership boundaries.
6. Retry, concurrent mutation, stale-owner and recovery behavior.
7. Quantitative metadata, network, bandwidth, capacity and rebuild budgets.
8. Quotas, tenant isolation, authentication, authorization and audit.
9. Observability, upgrades, migration, rollback and backup restore.
10. Alternatives, why this choice fits, and conditions that change it.

## References

Architecture descriptions follow the [primary-source reading list](references.md). Recommendations are engineering judgments requiring workload-specific validation, not vendor rankings.

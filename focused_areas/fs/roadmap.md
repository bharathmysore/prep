# Filesystem architecture roadmap

## Goal and workload

Build broad storage architecture expertise, using implementation and measurement as evidence. Budget 6–8 hours/week over 24 weeks. This is a foundation-building schedule, not a promise of industry leadership after six months. Production incidents, design reviews, and sustained contributions deepen judgment afterward.

Use each week for roughly two hours of reading, three hours of coding/experiments, and one hour of explanation and design review. Keep remaining time for debugging and repetition.

## Six phases

| Weeks | Topics | Implementation and experiment | Exit evidence |
| --- | --- | --- | --- |
| 1–4 | VFS, directories, inodes, extents, allocation, caches, persistence | E1 block device; begin E2 namespace and checker | Trace a pathname and byte offset to disk blocks; explain write versus sync |
| 5–8 | ext4, XFS, ZFS; journaling, copy-on-write, integrity | Finish E2; E3 deterministic crash recovery | Invariants hold across every modeled crash point; compare design choices |
| 9–12 | NFS, RPC failure, metadata/data separation, placement, locks | E5 request retry simulator; study Lustre and CephFS | Draw read, write, retry, stale-owner, and recovery paths |
| 13–16 | Queueing, concurrency, caching, amplification, tail latency | E4 cache/extent experiments; performance lab | Predict and explain a scaling knee with reproducible data |
| 17–20 | Replication, checksums, repair, striping, AI input/checkpoints | E6 and E7 bounded simulators; E8 workload harness | State precisely what an acknowledgment guarantees under each failure |
| 21–24 | Failure domains, maintenance, isolation, capacity and cost | Capstone architecture and benchmark report | Defend a design during overload, rebuild, upgrade, and node loss |

## Week-by-week prompts

1. Trace open/read/write and distinguish file descriptors from inodes.
2. Implement exact block reads/writes and reject invalid ranges.
3. Add allocation and a minimal on-disk inode format.
4. Add directories and an offline consistency checker.
5. Compare inode/extent allocation in ext4 and XFS using documentation and traces.
6. Compare journaling with ZFS copy-on-write and checksum-based integrity.
7. Implement redo logging with explicit persistence boundaries.
8. Enumerate crashes; test recovery interrupted by another crash.
9. Trace NFS caching and explain why remote I/O adds ambiguity.
10. Implement duplicate request handling, including request-ID reuse errors.
11. Draw Lustre metadata lookup, layout acquisition, and direct data I/O.
12. Draw CephFS MDS capabilities, RADOS access, and placement/recovery roles.
13. Measure request size versus IOPS and bandwidth.
14. Sweep concurrency; identify the throughput knee and p99 growth.
15. Add bounded caching; compare cold, warm, and working-set-too-large cases.
16. Measure fragmentation and metadata pressure separately from data throughput.
17. Define replicated acknowledgment and recovery contracts.
18. Simulate delayed messages, partitions, repair, and stale epochs.
19. Implement striping and reconstruct ranges across stripe boundaries.
20. Compare small samples with packed shards and checkpoint manifests.
21. Specify a GPU-cluster workload and failure model.
22. Build metadata, bandwidth, capacity, network, and rebuild budgets.
23. Run normal, degraded, and competing-workload experiments.
24. Present a 45-minute design review with evidence and open risks.

## How to assess progress

For each phase, explain the design without notes, then support it with code and a result. If a measurement disagrees with your prediction, identify the missing assumption before adding optimizations. A correct explanation of a disappointing result is useful evidence.

## Longer-term architect practice

Write architecture decision records, investigate real incidents, review unfamiliar systems, and contribute a small correctness or observability improvement upstream. Build expertise in migration, security, quotas, multi-tenancy, and operations alongside the hot I/O path.

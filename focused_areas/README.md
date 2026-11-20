# Focused Areas

Focused areas collect coding and system-design drills that cut across company guides and C++ categories. Use them when a topic is likely to appear in both implementation and architecture rounds.

| Area | Coding Focus | Design Focus |
| --- | --- | --- |
| [Linux Storage Principal Ramp](./linux_storage_principal_ramp.md) ([detailed exercises](./linux_storage_principal_exercises.md)) | Teaching-first JR2024421 ramp; September 25, 2026 readiness target; guided C/C++, kernel C, I/O, ownership, recovery, and replication; three 90-minute rehearsals with strict mocks only by opt-in. | Taught Linux/NVMe/RDMA depth, seven guided storage/design/incident drills, and truthful project stories; September 26–27 reserved for review. |
| [USP and OCI Block Storage Project Evidence](./linux_storage_project_evidence.md) | Local source anchors for replication, asynchronous ownership, filesystem/device paths, and background compression; bridges to the existing coding exercises. | User-confirmed ownership areas, including leader election, snapshot, backup, XRR, and XRRVG; sanitized project stories and eight-part failure/operations cards. |
| [Rate Limiters](./rate_limiters.md) | Fixed window, sliding window log, sliding window counter, token bucket, leaky bucket, GCRA, keyed tenant buckets, and C++ concurrency tradeoffs. | Distributed quota enforcement, local vs global accuracy, quota leasing, tenant fairness, overload protection, observability, and storage/LLM/API variants. |

| [Storage Architecture and Research Learning Hub](./storage/README.md) | LSM/B+ tree internals, durability, transactions, time-series and offline implementation exercises; [samples](./storage/samples/README.md). | Source-grounded system architectures, resource/failure models and research/paper milestones; [exercises](./storage/exercises/README.md). |
| [Filesystem Architecture Learning Pack](./fs/README.md) | Eight implementation milestones; runnable C++17 block I/O, journal recovery and striping samples. | 24-week broad architecture roadmap, filesystem tradeoffs, performance labs and AI/ML storage evaluation. |

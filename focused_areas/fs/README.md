# Filesystems: engineer-to-architect learning pack

Prepared October 27, 2026 for an experienced engineer pursuing broad storage architecture expertise, with AI/ML as a specialization.

## Start here

1. Follow the [24-week roadmap](roadmap.md).
2. Read [fundamentals](fundamentals.md) and [architecture tradeoffs](architecture.md).
3. Work through the [implementation exercises](exercises.md).
4. Build and run the [C++17 starter samples](samples/README.md).
5. Use the [performance lab](performance_lab.md) and [AI/ML guide](ai_ml.md).
6. Record evidence in the [progress tracker](progress.md).
7. Consult the [primary-source reading list](references.md).

## Learning approach

Read a design → predict its behavior → implement a bounded model → measure → explain discrepancies → write a design decision.

The destination is architectural judgment: explain where time goes, which data survives a failure, what limits scaling, and why a guarantee costs resources.

## Implementation scope

The supplied samples implement a regular-file-backed block device, a deterministic redo-journal model, and stripe-range mapping. They are runnable teaching examples with tests. They do not constitute a complete filesystem, real replication protocol, or production storage system. Exercises E1–E8 describe that larger progression and remain learner work.

C++17 follows the prep workspace's language convention. Earlier discussion suggested Rust; the architecture and exercises also translate to Rust or Go. No packages, mounts, raw devices, services, or remote clusters are required for the starter samples. macOS or Linux with a C++17 compiler and POSIX file APIs is sufficient; kernel tracing and later FUSE labs should use a disposable Linux environment.

## First session: 90 minutes

- 20 min: read the namespace, cache, and durability sections in fundamentals.
- 15 min: build and run the starter samples.
- 25 min: trace the block sample through write, sync, close, and reopen.
- 20 min: predict every journal crash outcome before running the journal sample.
- 10 min: record one invariant and one unanswered question in the tracker.

Related existing material: [Linux storage ramp](../linux_storage_principal_ramp.md) and [guided exercises](../linux_storage_principal_exercises.md).

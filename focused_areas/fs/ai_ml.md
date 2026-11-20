# Distributed filesystems for AI/ML

## Recommendation and scope

For broad storage architecture learning, study Lustre deeply and compare CephFS. For a large shared Linux training environment, Lustre is a strong initial candidate. Include WEKA in a commercial evaluation for mixed small-file and bandwidth-intensive AI workloads. For AWS, evaluate FSx for Lustre when its supported deployments and S3 integration fit. These are conditional recommendations, not a claim of a universal winner.

The architecture facts and primary links below were researched in the accompanying discussion on October 27, 2026. No product was benchmarked here; feature, compatibility, support, and pricing details need checking for the selected deployment.

## Workload classes

| Workload | Pressure | Application metric |
| --- | --- | --- |
| Individual image/audio files | Lookup/open/stat plus small reads | Samples/sec and batch wait |
| Packed tokenized datasets | Concurrent streaming and range reads | Effective input rate and step time |
| Checkpoint writes | Bursty parallel writes with persistence | Time to durably recoverable checkpoint |
| Checkpoint restore | Large reads and coordinated restart | Time to useful resumed work |
| Preparation pipelines | Scans, temporary files, creates and renames | Pipeline duration and resource use |
| Inference / RAG | Model loading, index/database I/O, sometimes KV state | Startup, request tail latency, retrieval latency |

Identify whether an inference bottleneck is actually in a shared filesystem, a local SSD index, database, network, CPU, or GPU memory before choosing storage.

## Candidate comparison

| Candidate | Architectural fit | Validate before selecting |
| --- | --- | --- |
| Lustre | Separate metadata and data services; striped parallel data access | Small-file behavior, directory layouts, stripes, client support, recovery and operations |
| CephFS | Shared filesystem over RADOS; metadata coordinated by MDS | MDS pressure, pools/protection, client access mode, repair interference |
| WEKA | Distributed metadata and parallel access; flash-oriented architecture | Workload results, client CPU/network requirements, degraded mode, licensing and TCO |
| FSx for Lustre | Managed Lustre in AWS with supported S3-linked workflows | Deployment-specific integration/protection constraints, loading/export behavior and cost |

Sources: [Lustre architecture](https://wiki.lustre.org/images/6/64/LustreArchitecture-v4.pdf), [CephFS](https://docs.ceph.com/en/reef/cephfs/), [WEKA architecture](https://www.weka.io/wp-content/uploads/files/resources/WEKA-Architecture-Key-Concepts.pdf), [FSx repositories](https://docs.aws.amazon.com/fsx/latest/LustreGuide/fsx-data-repositories.html). Vendor descriptions establish architecture, not comparative superiority.

## Sizing examples

Required dataset bandwidth ≈ aggregate samples/sec × stored bytes/sample, adjusted for cache reuse, overfetch, repeated reads, and distribution skew. CPU decoding and augmentation may limit useful throughput even with sufficient storage bandwidth.

A 4 TiB checkpoint in 60 seconds requires 4096/60 ≈ 68.3 GiB/s effective durable throughput. Include model weights, optimizer and other restart state; checkpoint representation and parallelism determine actual bytes. Allow for simultaneous jobs and degraded service.

Physical storage consumption includes protection overhead, metadata, snapshots, headroom, and measured compression. Do not apply a vendor reduction ratio without representative data.

## Dataset layout experiment

Compare individual files to shards using identical logical records and seeded sampling. Record metadata operations, bytes fetched, CPU decompression, shuffle quality, first-epoch time and steady-state time. Packing reduces file-operation frequency but can increase overfetch or complicate updates and sampling.

## Checkpoint publication protocol exercise

Write uniquely versioned shards, synchronize them under the storage contract, and publish a manifest only after every required shard is durable. The manifest contains checkpoint ID, shard sizes/checksums, format version, and required training metadata. Persist manifest publication using the actual filesystem's documented semantics. Restore only complete published generations; ignore incomplete work.

Test writer failure before and after manifest publication, corrupt or missing shards, a slow rank, full storage, and restore from another client. Multi-file atomicity is an application protocol, not a consequence of renaming one shard. Asynchronous checkpointing also requires a consistent captured state and bounded memory/backpressure.

## Proof of concept acceptance

Set explicit targets for input stall time, training step distribution, checkpoint pause, restore time, simultaneous jobs, usable capacity and cost. Run normal, cold-cache, degraded and repair cases. Compare equivalent configurations and report hardware, protection, dataset, client count and durability boundaries.

Use [MLPerf Storage](https://mlcommons.org/benchmarks/storage/) for reproducible reference workloads; compare like workloads and inspect submission configuration. End-to-end application measurements decide the final architecture.

## Capstone

Design storage for an explicitly chosen GPU cluster. State GPU and client counts, dataset size/file distribution, input rate, checkpoint size/interval/pause, retention, failure domains, and recovery target. Produce read/write paths, namespace/metadata budget, network/bandwidth budget, usable-capacity model, security/isolation controls, lifecycle and backup plan, cost model and evidence. Explain which changed assumptions would make another filesystem preferable.

# Filesystem performance lab

## What to measure

Separate application outcome (samples/sec, batch wait, checkpoint pause) from filesystem behavior (bandwidth, operations/sec, latency) and device behavior (queueing, utilization, amplification). One utilization percentage cannot identify the bottleneck.

Approximate relationships:

- bandwidth = completed operations/sec × average bytes/operation
- outstanding requests ≈ throughput in operations/sec × mean latency in seconds (Little's law, using the same stable system boundary)
- checkpoint duration ≈ checkpoint bytes / effective durable bandwidth, plus coordination costs
- capacity-only rebuild lower bound ≈ missing bytes / effective reconstruction rate; foreground traffic and coding overhead can extend it

If throughput is 100,000 IOPS with 4 KiB operations, payload bandwidth is about 390.6 MiB/s. At a mean response time of 1 ms, about 100 requests are outstanding within that measurement boundary.

## Experiment protocol

Record hardware, OS/kernel, filesystem version, mount options, client mode, protection, usable capacity, fullness, network, CPU allocation, dataset, cache state, request size, concurrency, sync policy, duration, and repetitions.

1. State a hypothesis and expected bottleneck.
2. Change one main variable.
3. Run a warmup followed by a measured interval.
4. Record p50/p95/p99 and throughput; distinguish per-operation latency from job makespan.
5. Repeat enough to characterize variability; report the range and sample count.
6. Explain surprises before optimizing.

## Lab matrix

| Lab | Sweep | Expected observation to test |
| --- | --- | --- |
| Request size | 4 KiB, 64 KiB, 1 MiB | Per-request overhead versus bandwidth limits |
| Concurrency | 1, 2, 4, 8, ... until saturation | Throughput knee followed by rising latency |
| Working set | Fits cache versus exceeds cache | Warm-cache results differ from storage service rates |
| File layout | Many individual samples versus shards | Metadata cost and shuffle/indexing tradeoffs |
| Striping | One target versus several | Parallelism gains versus fanout and slow-target effects |
| Persistence | Buffered completion versus fsync boundary | Different acknowledgment costs and guarantees |
| Space use | Moderate versus high occupancy | Allocation, fragmentation, and recovery headroom |
| Recovery | Normal versus one unavailable target/repair | Tail latency and foreground/repair interference |
| Tenancy | One job versus training plus checkpoints | Fairness, burst handling, and QoS |

Results are hypotheses to test, not guaranteed trends on every system.

## Tools and constraints

Use an existing fio installation for controlled I/O, IOR for parallel I/O, MDTest for metadata, and MLPerf Storage alongside an actual application. Tool installation and remote cluster work are separate setup tasks. Read official tool documentation before running a job.

Confine tests to newly created disposable directories and ordinary test files. Do not benchmark raw devices or flush machine-wide caches as part of the starter work. On shared systems schedule resource-intensive tests with their owners.

The supplied C++ samples test correctness; they are not filesystem throughput benchmarks. They use tiny datasets and do not establish product performance.

## Result template

```text
Question and hypothesis:
Configuration/version:
Dataset and cache state:
Request size / concurrency / duration:
Completion and durability boundary:
Throughput / p50 / p95 / p99:
CPU / network / device and metadata observations:
Repetitions and spread:
Failure or competing work:
Explanation / alternative explanation:
Design consequence / next experiment:
```

## Architect follow-ups

Can you distinguish queueing from service time? Is the bottleneck a single client? Did compression or caching change the bytes moved? Does a training epoch hide first-read costs? Does a reported write rate include all final syncs? Can a second client restore the checkpoint after the original client is lost?

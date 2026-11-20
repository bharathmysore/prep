# Time series storage from first principles

[Expertise path](README.md) · [Constrained devices](constrained-and-disconnected.md) · [Labs](labs.md)

A time-series system must preserve the meaning of measurements while organizing ingestion, selection, temporal scans and lifecycle efficiently. The following is a design-study framework. The local inventory has engines and infrastructure but no dedicated Prometheus checkout; external readings supply missing examples rather than new locally audited systems.

## 1 Identity and time are separate design decisions

Define a sample as series identity, event time, value, and optionally source/sequence/quality metadata. Decide whether two samples with the same series/time are duplicates, replacements or distinct observations. Event time describes the measurement; ingestion time describes its arrival. A reconnecting device can produce old event times long after newer data has arrived.

Prometheus identifies a series using metric name and label set. Its [data-model documentation](https://prometheus.io/docs/concepts/data_model/) is a concrete example, not a universal TSDB schema. Design an explicit series registry and decide how identity changes when labels or device configuration change.

**Exercise:** model sensor reboot, clock correction, repeated upload and two devices claiming the same identity. Explain which state detects or tolerates each event.

## 2 Cardinality is an indexing and memory problem

Estimate simultaneously active series, new-series creation rate and retained inactive-series metadata separately. A small sample payload can coexist with a large series registry, postings lists and per-series mutable state. Unbounded identifiers in labels can make cardinality track events instead of useful measurement dimensions; Prometheus's [instrumentation guidance](https://prometheus.io/docs/practices/instrumentation/) explains the associated resource costs.

Illustrative budget: at 100,000 active series and an assumed 256 bytes of mutable metadata per series, metadata alone consumes about 25.6 MB decimal, before samples, allocator overhead, indexes and caches. The 256-byte assumption is a learning input, not a product measurement. Change it and recalculate.

**Gate:** predict which memory structures grow when sample rate stays fixed but series count increases tenfold.

## 3 Key layout follows query shape

Compare `(series ID, timestamp)` with `(time bucket, series ID, timestamp)`. The first favors contiguous per-series time scans; the second makes temporal lifecycle boundaries more explicit. Real systems can combine a series index with time-partitioned payload chunks rather than choose one global KV key order.

Label selection needs an access path. A postings index can map a label/value pair to candidate series IDs; intersections narrow candidates before reading time chunks. Account for index update cost, broad selectors and missing/selective predicates. Do not hide a full registry scan behind a compact query syntax.

**Gate:** count candidate series, chunks and decoded samples for one selective query and one broad aggregation.

## 4 Chunks, compression and recovery serve different purposes

Learn delta encoding, timestamp delta-of-delta, value XOR coding, bit packing and restart/checkpoint boundaries. The original [Gorilla paper](https://www.vldb.org/pvldb/vol8/p1816-teller.pdf) describes timestamp and floating-point compression for a monitoring workload that values recent aggregate analysis and accepts some write-path loss. Do not transfer that loss policy to accounting, control or safety-sensitive measurements.

As one concrete architecture, Prometheus uses a mutable head protected by recovery logging and persistent blocks containing chunks, index and metadata; deletion markers can precede physical removal. Its local storage is not itself a clustered replicated store. See [Prometheus storage](https://prometheus.io/docs/prometheus/latest/storage/). Study this beside an LSM: both have mutable/immutable lifecycle work, but time blocks and series chunks are not simply interchangeable with key-range SST levels.

**Gate:** explain what compression improves, what state is necessary to decode a middle point, and which recovery history protects data not yet in immutable blocks.

## 5 Lateness creates explicit policy choices

A late sample can enter a correction buffer, trigger rewriting, become a separate overlay, or be rejected under a documented rule. Each choice changes read merging, storage cost and user semantics. Define the tolerated lateness and how query results can change after initial publication.

A watermark is a statement about progress under a source/protocol policy, not proof that no disconnected device can ever return. Retention must say whether age is based on event time, ingestion time or another clock. Keep clock rollback and bad timestamps in the model.

**Exercise:** deliver samples at times 10, 12, 11, then redeliver 11 with a conflicting value. Specify the exact answer before choosing a structure.

## 6 Retention and aggregation have information costs

Dropping a time partition can be cheaper than issuing per-sample deletions, but only when all protected references and retention rules permit it. Late arrivals complicate closed partitions. Historical queries and snapshots may extend physical retention.

Downsampling is a derived data model. A mean of means is wrong when groups have different counts; preserve sum and count where that matches the required query. Percentiles generally cannot be reconstructed from averages. Define how missing values, counter resets and duplicate samples affect each aggregate.

**Gate:** identify which future queries become impossible after each proposed rollup.

## 7 Scale is more than ingestion throughput

Separate ingestion, series discovery/indexing, hot queries, historical scans, compaction and remote persistence. Choose sharding by the work it distributes: one hot series, one huge tenant and a broad cross-shard query stress different boundaries. An immutable object tier can reduce durable-capacity cost while adding metadata, caching, request and read-amplification costs.

Design a benchmark matrix varying active series, churn, sample rate, disorder, compression entropy, retention, query fan-out and outage replay. Report ingest goodput, query p99, peak memory, physical writes, recovery duration and maintenance backlog. A favorable bytes/sample number alone is insufficient.

## Exit artifact

Produce an end-to-end design for telemetry from intermittently connected devices: schema, index, chunk format, recovery boundary, lateness/dedup policy, retention, query plan and resource budget. Then use the [capstone specification](labs.md) to expose failures in that design.

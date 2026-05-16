# Snowflake L7 System Design Interview Prep

This guide uses Snowflake-style distributed systems prompts. They are representative preparation questions, not a claim about Snowflake's private interview bank.

Maintenance note: this is a living Snowflake-specific question catalog. Do not treat the current number of questions as fixed; future agents should add, remove, merge, reorder, and renumber prompts as public sources and the user's target role evolve.

Useful background:
- Snowflake key concepts and architecture: https://docs.snowflake.com/en/user-guide/intro-key-concepts
- Micro-partitions and clustering: https://docs.snowflake.com/en/user-guide/tables-clustering-micropartitions
- Multi-cluster warehouses: https://docs.snowflake.com/en/user-guide/warehouses-multicluster
- Snowflake SIGMOD paper: https://www.snowflake.com/wp-content/uploads/2019/06/Snowflake_SIGMOD.pdf
- Cortex Agents: https://docs.snowflake.com/en/user-guide/snowflake-cortex/cortex-agents
- Cortex Agents platform context: https://www.snowflake.com/en/blog/enterprise-ai-agent-platform/
- Snowflake Intelligence and Cortex Code update: https://www.snowflake.com/en/news/press-releases/snowflake-expands-snowflake-intelligence-and-cortex-code-to-power-the-control-plane-for-the-agentic-enterprise/
- Snowflake CoCo / Cortex Code: https://www.snowflake.com/en/product/snowflake-coco/

## 1. Design A Cloud Data Warehouse Like Snowflake

* Question

Design a cloud-native data warehouse that separates storage and compute, supports SQL analytics at petabyte scale, and serves many tenants.

* Answer

The key design is a multi-cluster shared-data architecture: durable immutable data in cloud object storage, elastic compute warehouses for query execution, and a control/cloud-services layer for metadata, auth, optimization, transactions, and billing.

** Scope

Support batch analytics, interactive BI, SQL, semi-structured data, multiple tenants, elastic compute, and cloud object storage. Exclude full OLTP workloads and sub-millisecond point reads.

** Functional Requirements

- Create databases, schemas, tables, views, and roles.
- Load data from cloud storage and query it with SQL.
- Scale compute independently from storage.
- Support concurrent workloads with workload isolation.
- Support transactions, snapshots, access control, and query history.

** Non Functional Requirements

- Petabyte-scale storage.
- Thousands to millions of queries per day.
- High availability across failures inside a region.
- Strong metadata consistency.
- Predictable tenant isolation and auditable security.
- Cost-efficient storage and elastic compute.

** High level design and diagram (at block level)

```text
Clients / BI / JDBC / ODBC
        |
        v
Cloud Services / Control Plane
 Auth | Catalog | Optimizer | Transactions | Billing | Governance
        |
        v
Warehouse Manager
        |
        v
Virtual Warehouses / MPP Compute Clusters
 Workers | Local Cache | Shuffle | Execution Engine
        |
        v
Cloud Object Storage
 Columnar Files | Micro-partitions | Table Versions
```

*** Explain the blocks

- Clients submit SQL, DDL, load, and admin requests.
- Cloud services authenticate, authorize, parse SQL, optimize plans, manage metadata, and commit transactions.
- Warehouse manager provisions, resumes, suspends, and scales compute clusters.
- Virtual warehouses execute distributed query fragments and cache hot data.
- Object storage keeps immutable table data and historical versions.

*** Explain the control flow

User requests enter cloud services. The control plane authenticates, authorizes, resolves metadata, builds an optimized plan, chooses a warehouse, provisions compute if needed, records query state, and later commits metadata changes or records query completion.

*** Explain the data flow

For reads, workers fetch only relevant columns and micro-partitions from object storage or local cache, execute filters, joins, and aggregations, exchange intermediate data through shuffle, and return results. For writes, workers create new immutable files and the control plane atomically publishes a new metadata version.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: storage/compute architecture.

- Shared-nothing cluster with local disks
  - Pros: very fast local reads, simpler execution locality.
  - Cons: storage and compute scale together, hard elasticity, harder multi-tenant isolation.
- Shared-disk architecture
  - Pros: any compute can access any data, easier elasticity.
  - Cons: shared storage latency and metadata bottlenecks.
- Multi-cluster shared-data architecture
  - Pros: independent scaling, workload isolation, cheap durable storage, fast clone/share semantics.
  - Cons: requires strong metadata services, caching, pruning, and object-store-aware execution.

L7 answer: choose multi-cluster shared-data, then mitigate object-store latency with micro-partition pruning, columnar format, local cache, result cache, and careful metadata design.

## 2. Design Distributed SQL Query Execution Over Object Storage

* Question

Design a distributed SQL execution engine that can query large columnar datasets stored in S3, GCS, or Azure Blob.

* Answer

Compile SQL into a distributed physical plan, split it into stages, schedule fragments on workers, scan columnar partitions from object storage, exchange data for joins and aggregations, and return streamed or materialized results.

** Scope

Support analytical SQL: scans, filters, projections, joins, aggregations, sorts, window functions, and writes. Exclude OLTP indexes as the primary access path.

** Functional Requirements

- Parse, validate, and optimize SQL.
- Push filters and column projections into scans.
- Run distributed joins and aggregations.
- Spill safely when memory is exceeded.
- Return progress, cancellation, errors, and profiles.

** Non Functional Requirements

- High throughput scans.
- Reasonable latency for interactive queries.
- Fault tolerance for worker failures.
- Isolation across tenants and warehouses.
- Efficient object-store access.

** High level design and diagram (at block level)

```text
SQL Request
   |
   v
Parser / Analyzer -> Optimizer -> Physical Plan
                                |
                                v
                         Stage Scheduler
                                |
                                v
          +---------------- MPP Workers ----------------+
          | Scan | Filter | Join | Aggregate | Sort      |
          | Local Cache | Spill | Shuffle | Metrics      |
          +-------------------+--------------------------+
                              |
                              v
                    Object Storage / Result Store
```

*** Explain the blocks

- Parser/analyzer validates SQL and resolves objects.
- Optimizer chooses join order, access paths, repartitioning, and stage boundaries.
- Scheduler maps fragments to workers based on capacity and locality hints.
- Workers run vectorized operators and exchange intermediate data.
- Object/result storage stores input files, spilled data, and final result sets.

*** Explain the control flow

Cloud services build the plan, ask the warehouse for capacity, create execution stages, track dependencies between stages, monitor health, retry failed fragments where safe, and update query state until completion or cancellation.

*** Explain the data flow

Workers scan only needed columns and partitions, pass batches through operators, shuffle rows by join or grouping keys, merge partial results, spill large intermediates, and stream final output to the client or result cache.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: join execution.

- Broadcast join
  - Pros: avoids repartitioning the large table, good for small dimensions.
  - Cons: can overload memory if the "small" side is misestimated.
- Repartition/hash join
  - Pros: scales to two large tables.
  - Cons: expensive shuffle and vulnerable to skew.
- Sort-merge join
  - Pros: good when inputs are already ordered or too large for hash tables.
  - Cons: sort cost can dominate.
- Adaptive join
  - Pros: can switch strategy using runtime cardinality.
  - Cons: more complex execution and harder debugging.

L7 answer: use cost-based planning plus runtime feedback. Build spillable joins, skew detection, and query profiles that explain why the strategy was chosen.

## 3. Design Snowflake-Style Virtual Warehouses

* Question

Design elastic compute warehouses that execute SQL workloads independently over shared storage.

* Answer

A virtual warehouse is an isolated pool of compute clusters. It can auto-resume, auto-suspend, resize vertically, and scale out with multiple clusters for concurrency.

** Scope

Support analytical workloads with different sizes and concurrency profiles. Exclude long-running always-on OLTP serving.

** Functional Requirements

- Create, resize, suspend, resume, and drop warehouses.
- Route queries to a selected warehouse.
- Scale up for larger individual queries.
- Scale out for concurrent query bursts.
- Meter compute usage.

** Non Functional Requirements

- Fast startup for interactive users.
- Isolation between workloads.
- Efficient utilization.
- Predictable billing.
- Safe failure recovery.

** High level design and diagram (at block level)

```text
Admin / SQL
   |
   v
Warehouse Control API
   |
   v
Warehouse Manager
 Policy | State | Metering | Autoscaler
   |
   v
Cluster Pool
 Cluster A | Cluster B | Cluster C
   |
   v
Workers with Execution Engine + Cache
```

*** Explain the blocks

- Warehouse control API handles DDL and configuration.
- Warehouse manager maintains desired state and lifecycle.
- Autoscaler decides when to add/remove clusters.
- Cluster pool contains active compute clusters of a configured size.
- Workers execute query fragments and maintain local caches.

*** Explain the control flow

An admin defines warehouse size, min/max clusters, and suspend policy. Query submission checks warehouse state. If suspended, the manager resumes it. Autoscaler observes queue depth, running queries, and utilization, then starts or drains clusters.

*** Explain the data flow

Queries assigned to the warehouse run on one cluster or across workers in a cluster. Workers read object-store data, use local caches, exchange intermediate results, and return query output.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: scale up vs scale out.

- Scale up: larger workers or cluster size
  - Pros: helps single large queries with more CPU, memory, and parallelism.
  - Cons: more expensive and may not improve many small concurrent queries.
- Scale out: add clusters
  - Pros: improves concurrency and queue time.
  - Cons: a single query may not become faster if it runs on one cluster.
- Serverless pool
  - Pros: easier user experience, better global utilization.
  - Cons: less predictable isolation and harder cost attribution.

L7 answer: expose warehouses for isolation and cost control, use multi-cluster scaling for concurrency, and consider serverless for maintenance tasks and bursty background features.

## 4. Design A Metadata And Catalog Service

* Question

Design the metadata service for a cloud data warehouse.

* Answer

The catalog stores object definitions, table versions, file lists, micro-partition statistics, grants, transactions, query history, and lineage. It is the source of truth for SQL correctness.

** Scope

Support many tenants and petabytes of data with high metadata read volume and strongly consistent writes.

** Functional Requirements

- Store databases, schemas, tables, views, functions, stages, roles, and grants.
- Store table snapshots and partition/file metadata.
- Support atomic DDL and DML commits.
- Serve optimizer metadata quickly.
- Audit metadata changes.

** Non Functional Requirements

- Strong consistency for commits and grants.
- Low-latency metadata reads.
- Horizontal scalability.
- High availability and disaster recovery.
- Strict tenant isolation.

** High level design and diagram (at block level)

```text
Cloud Services
   |
   v
Catalog API
 AuthZ | Object Resolver | Version Manager
   |
   +--> Metadata Cache
   |
   v
Metadata Store
 Objects | Grants | Table Versions | Partition Stats
   |
   v
Change Log / Audit / Replication Stream
```

*** Explain the blocks

- Catalog API validates requests and enforces authorization.
- Object resolver maps names to stable object IDs.
- Version manager maintains snapshot lineage.
- Metadata cache accelerates hot catalog reads.
- Metadata store persists strongly consistent state.
- Change log feeds audit, replication, and cache invalidation.

*** Explain the control flow

DDL and DML commits go through the catalog API. The API validates permissions, checks conflicts, writes a new version transactionally, publishes invalidation events, and records an audit entry.

*** Explain the data flow

During query planning, optimizer reads table stats, partition metadata, and grants from cache/store. During execution, workers use the chosen file and partition list to read object storage.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: metadata consistency model.

- Single primary relational metadata DB
  - Pros: simple strong consistency and SQL transactions.
  - Cons: scaling and regional availability limits.
- Sharded strongly consistent store
  - Pros: scales by account/table, preserves correctness.
  - Cons: cross-shard transactions are complex.
- Eventually consistent catalog
  - Pros: high availability and scale.
  - Cons: dangerous for permissions, DDL, and snapshot correctness.

L7 answer: use strong consistency for object identity, grants, and table commits; use caches and async derived metadata for performance.

## 5. Design Micro-Partitions And Partition Pruning

* Question

Design the storage layout and pruning metadata for large analytical tables.

* Answer

Store tables as immutable columnar micro-partitions. For each partition, track metadata such as row count, column min/max, null counts, size, value distributions, and clustering depth so queries can skip irrelevant data.

** Scope

Optimize analytical scans over huge tables. Exclude traditional B-tree indexes as the default for all columns.

** Functional Requirements

- Write data into immutable columnar partitions.
- Track per-partition stats.
- Prune partitions during query planning/execution.
- Support semi-structured data stats where feasible.
- Preserve versions for time travel.

** Non Functional Requirements

- Efficient object-store reads.
- High compression.
- Fast pruning decisions.
- Manageable metadata size.
- Good performance for common predicates.

** High level design and diagram (at block level)

```text
Ingestion / Query Writes
        |
        v
Partition Builder
 Columnar Encode | Compress | Stats
        |
        v
Object Storage Files
        |
        v
Partition Metadata Catalog
 min/max | nulls | row count | clustering | version
        |
        v
Optimizer Pruning
```

*** Explain the blocks

- Partition builder batches rows and writes columnar files.
- Stats collector records pruning metadata.
- Object storage stores immutable partition files.
- Catalog indexes partition metadata by table version.
- Optimizer uses stats to eliminate partitions.

*** Explain the control flow

On write, the system builds partitions, computes stats, writes files, and commits partition metadata. On query, the optimizer asks the catalog for candidate partitions and removes partitions impossible to satisfy predicates.

*** Explain the data flow

Only selected partition files and columns are read by workers. Irrelevant files are skipped entirely, reducing object-store reads, CPU, and network usage.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: pruning metadata richness.

- Min/max stats
  - Pros: cheap and compact.
  - Cons: poor for high-overlap or random distributions.
- Bloom filters / membership filters
  - Pros: better point lookup pruning.
  - Cons: more metadata and false positives.
- Histograms
  - Pros: better cardinality estimates.
  - Cons: expensive to maintain accurately.
- Search/index service
  - Pros: improves selective lookup and text-like predicates.
  - Cons: extra storage, maintenance, and consistency complexity.

L7 answer: default to compact stats for all data, then offer specialized indexes/search optimization for selective workloads where benefit exceeds maintenance cost.

## 6. Design Automatic Clustering / Reclustering

* Question

Design a service that continuously improves table clustering for faster pruning.

* Answer

Measure partition overlap for clustering keys and query predicates, then use background compute to rewrite poorly clustered partitions into better ordered immutable files.

** Scope

Support large mutable analytical tables. Exclude mandatory manual physical tuning for every table.

** Functional Requirements

- Let users define clustering keys or infer candidates.
- Measure clustering quality and query benefit.
- Schedule background reclustering.
- Rewrite partitions safely with snapshot isolation.
- Limit cost and avoid interfering with foreground queries.

** Non Functional Requirements

- Bounded background spend.
- No correctness impact on readers.
- Scalable scheduling.
- Fairness across tenants.
- Observable benefit and cost.

** High level design and diagram (at block level)

```text
Query History + Table Metadata
          |
          v
Clustering Analyzer
 overlap | depth | workload benefit
          |
          v
Reclustering Scheduler
 priority | budget | fairness
          |
          v
Background Warehouse / Serverless Workers
          |
          v
New Partition Files -> Metadata Commit -> Old File GC Later
```

*** Explain the blocks

- Analyzer computes overlap/depth and estimates benefit.
- Scheduler chooses tables and partition ranges under budget.
- Background workers sort/rewrite data.
- Metadata commit publishes new partition versions.
- Garbage collector removes obsolete files after retention.

*** Explain the control flow

The service periodically reads table and workload stats, creates reclustering tasks, enforces budgets, runs workers, and commits rewritten partitions as a new table version.

*** Explain the data flow

Workers read overlapping old partitions, sort or range-partition rows by clustering key, write new compact partitions, and update metadata. Future queries prune better.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: choosing clustering strategy.

- User-defined clustering key
  - Pros: explicit and predictable.
  - Cons: users may choose poorly and waste compute.
- Workload-aware recommendation
  - Pros: adapts to real predicates.
  - Cons: needs query history and can chase changing workloads.
- Fully automatic clustering
  - Pros: best user experience.
  - Cons: high risk of opaque cost and surprising rewrites.

L7 answer: support user intent plus recommendations. Put hard budgets, expose benefit metrics, and deprioritize low-benefit reclustering.

## 7. Design ACID Transactions Over Cloud Object Storage

* Question

Design ACID table updates where data lives in immutable object-store files.

* Answer

Use metadata transactions: writers create new immutable files, then atomically publish a new table version if conflict checks pass. Readers use snapshot isolation and never observe partial writes.

** Scope

Support SQL DML, DDL, batch inserts, deletes, updates, and merge. Exclude high-QPS row-level OLTP.

** Functional Requirements

- Atomic commit for writes.
- Snapshot reads.
- Conflict detection for concurrent writers.
- Rollback failed writes.
- Metadata-driven file visibility.

** Non Functional Requirements

- Correctness over object-store eventual behaviors.
- High write throughput.
- Bounded commit latency.
- Recoverability after failures.
- Idempotent retries.

** High level design and diagram (at block level)

```text
Writer Query
   |
   v
Execution Workers
 write new files to object storage
   |
   v
Commit Coordinator
 validate conflicts | assign version | publish metadata
   |
   v
Catalog Table Versions
   |
   v
Readers select stable snapshot
```

*** Explain the blocks

- Workers produce new data files and delete markers/replacement files.
- Commit coordinator performs validation and metadata commit.
- Catalog stores versions and visible file sets.
- Readers bind to one version for consistent execution.

*** Explain the control flow

The planner creates a transaction with a read snapshot. Execution writes files under a temporary transaction ID. Commit coordinator checks whether touched partitions/table versions changed, then publishes a new version or aborts.

*** Explain the data flow

New files are uploaded before commit but invisible. After commit, readers of newer snapshots see the new files and replacement set. Old files remain for active readers and time travel.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: isolation.

- Read committed
  - Pros: simpler and may allow more concurrency.
  - Cons: analytical queries can see inconsistent state.
- Snapshot isolation
  - Pros: stable long-running reads and practical implementation.
  - Cons: write skew possible without stricter checks.
- Serializable isolation
  - Pros: strongest user semantics.
  - Cons: higher conflict rate and more expensive validation.

L7 answer: use snapshot isolation for reads, strict metadata atomicity for commits, and targeted conflict detection for updates/merges.

## 8. Design Time Travel

* Question

Design Time Travel so users can query or restore historical table states.

* Answer

Keep append-only table version metadata and retain old immutable data files for a configured retention period. Historical queries bind to a past timestamp or version.

** Scope

Support historical reads, accidental recovery, audit, and clone-from-past. Exclude indefinite archival retention unless separately configured.

** Functional Requirements

- Query table as of timestamp/version.
- Restore table/database to previous state.
- Preserve old files during retention.
- Enforce retention policy.
- Support historical metadata and grants carefully.

** Non Functional Requirements

- Low overhead on normal writes.
- Predictable storage growth.
- Correct snapshot reconstruction.
- Efficient garbage collection.
- Clear compliance behavior.

** High level design and diagram (at block level)

```text
Table Version Log
 V1 -> V2 -> V3 -> V4
  |     |     |     |
  v     v     v     v
File Sets In Object Storage
        |
        v
Retention Manager / GC
```

*** Explain the blocks

- Version log maps timestamps to table snapshots.
- File sets represent visible partitions for each version.
- Retention manager determines which versions/files must remain.
- Garbage collector deletes unreferenced expired files.

*** Explain the control flow

Each commit creates a version entry. Historical query planning resolves the requested time to a version. GC periodically checks retention, active queries, clones, and fail-safe rules before deleting old files.

*** Explain the data flow

Historical reads use the file set from an older metadata snapshot. Restore creates a new current version pointing to the chosen historical file set.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: old data retention.

- Keep full copies per version
  - Pros: simple restore logic.
  - Cons: extremely expensive.
- Immutable files plus metadata versions
  - Pros: cheap for unchanged data and fast snapshots.
  - Cons: lineage and GC become more complex.
- Delta logs only
  - Pros: compact for small updates.
  - Cons: expensive reconstruction for long histories.

L7 answer: immutable files plus versioned metadata is the natural fit for analytical object-store systems.

## 9. Design Zero-Copy Cloning

* Question

Design zero-copy clone for databases, schemas, and tables.

* Answer

Create a new metadata object that points to the same source snapshot. Future writes use copy-on-write so only changed partitions consume additional storage.

** Scope

Support fast dev/test, recovery, experimentation, and branching. Exclude writable clones that mutate shared physical files in place.

** Functional Requirements

- Clone table/schema/database at current or historical version.
- Allow source and clone to diverge.
- Preserve access controls and ownership rules.
- Track storage ownership and references.
- Support drop and GC safely.

** Non Functional Requirements

- Clone creation in seconds.
- No data copy at clone time.
- Correct isolation after divergence.
- Accurate billing and lineage.
- Safe cleanup.

** High level design and diagram (at block level)

```text
Source Table Metadata ---- points to ---- File Set A
        |
        | clone metadata pointer
        v
Clone Table Metadata  ---- points to ---- File Set A
        |
        | later writes
        v
Clone Version points to File Set A + New Files B
```

*** Explain the blocks

- Source object owns the original metadata lineage.
- Clone object starts with a pointer to the same snapshot.
- Reference tracker counts file usage across objects.
- Copy-on-write creates new files for clone modifications.

*** Explain the control flow

Clone request validates permissions, resolves source snapshot, creates new object metadata, and increments file references. Later writes commit new clone-specific versions.

*** Explain the data flow

Reads from source and clone initially scan the same files. Writes to either object create new files and update only that object's metadata.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: storage accounting.

- Bill only original owner
  - Pros: simple for users initially.
  - Cons: unfair when clones diverge heavily.
- Bill per logical object
  - Pros: simple attribution.
  - Cons: double-counts shared bytes.
- Bill shared bytes once plus changed bytes to mutator
  - Pros: fairer and cost-reflective.
  - Cons: harder lineage and reference tracking.

L7 answer: track file references and charge incremental unique bytes where possible, with transparent storage reporting.

## 10. Design Batch Data Ingestion From Cloud Storage

* Question

Design a service that loads large files from S3/GCS/Azure Blob into warehouse tables.

* Answer

Use stages, file discovery, validation, parallel parsing, columnar partition building, load history for idempotency, and metadata commit.

** Scope

Support CSV, JSON, Parquet, Avro, and ORC from external/internal stages. Exclude low-latency streaming as the primary target.

** Functional Requirements

- Discover files in stages.
- Validate schema and format.
- Parallelize load.
- Deduplicate already loaded files.
- Record load history and errors.

** Non Functional Requirements

- High throughput.
- Idempotent retries.
- Clear error reporting.
- Cost-efficient parsing.
- Secure external storage access.

** High level design and diagram (at block level)

```text
External Stage / Internal Stage
        |
        v
File Discovery + Manifest
        |
        v
Load Coordinator
        |
        v
Parser Workers -> Partition Builder -> Object Storage Files
        |
        v
Load History + Table Metadata Commit
```

*** Explain the blocks

- Stage stores source files and credentials/integration info.
- Discovery lists files or reads manifests.
- Load coordinator assigns files and tracks progress.
- Parser workers decode source formats.
- Partition builder writes warehouse-native files.
- Load history prevents duplicate loads.

*** Explain the control flow

User runs load command. Control plane validates permissions and file format, checks load history, creates work items, monitors workers, records errors, and commits created partitions.

*** Explain the data flow

Workers read source files, parse rows, transform them to internal columnar representation, write partition files to object storage, and publish metadata.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: file sizing.

- Many small files
  - Pros: low producer latency and easy parallelism.
  - Cons: high listing, scheduling, and metadata overhead.
- Few huge files
  - Pros: efficient storage and metadata.
  - Cons: slower recovery and lower parallelism unless splittable.
- Compacted medium files
  - Pros: good balance for analytics.
  - Cons: requires staging/compaction step.

L7 answer: support all, but recommend file size ranges, use manifests, parallel readers for splittable formats, and optionally compact small-file workloads.

## 11. Design Streaming Ingestion / CDC

* Question

Design a low-latency ingestion path for events or change data capture into warehouse tables.

* Answer

Buffer events in durable queues, micro-batch them into columnar files, commit frequently, and expose exactly-once-ish semantics through offsets, idempotency, and transactional metadata commits.

** Scope

Support seconds-to-minutes freshness for append and CDC workloads. Exclude strict per-row transactional serving.

** Functional Requirements

- Ingest from Kafka, cloud streams, or API.
- Track source offsets.
- Handle inserts, updates, deletes, and schema evolution.
- Commit micro-batches to tables.
- Provide replay and dead-letter handling.

** Non Functional Requirements

- Low latency.
- High throughput.
- Idempotent replay.
- Backpressure.
- Schema and ordering robustness.

** High level design and diagram (at block level)

```text
CDC Source / Event Stream
        |
        v
Ingestion Gateway
        |
        v
Durable Buffer / Offset Store
        |
        v
Micro-batch Builder
        |
        v
Table Committer -> Warehouse Table Versions
        |
        v
DLQ / Monitoring / Lag Metrics
```

*** Explain the blocks

- Gateway authenticates producers and validates events.
- Buffer stores events durably and tracks offsets.
- Micro-batch builder compacts rows into efficient files.
- Committer atomically publishes batches.
- DLQ captures poison records.

*** Explain the control flow

The system assigns source partitions to consumers, monitors lag, controls batch size/time, commits offsets only after table commit, and rebalances consumers on failure.

*** Explain the data flow

Events flow from source to buffer, then into micro-batches. Workers write columnar files, commit a new table version, and record offsets so retries do not duplicate committed changes.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: freshness vs efficiency.

- Commit every event
  - Pros: lowest latency.
  - Cons: terrible metadata and small-file overhead.
- Fixed micro-batches
  - Pros: efficient and predictable.
  - Cons: adds latency.
- Adaptive micro-batches
  - Pros: balances latency and throughput under changing load.
  - Cons: more tuning and operational complexity.

L7 answer: use adaptive micro-batching with strict offset-to-table-version mapping.

## 12. Design A Distributed Query Optimizer

* Question

Design the optimizer for a distributed analytical SQL engine.

* Answer

Build a cost-based optimizer that uses catalog stats, partition metadata, query history, and runtime feedback to choose scan pruning, join order, join algorithm, repartitioning, and stage boundaries.

** Scope

Optimize complex analytical SQL. Exclude hand-written query hints as the main operating model.

** Functional Requirements

- Parse and normalize SQL.
- Resolve names and types.
- Estimate cardinality and costs.
- Choose physical operators.
- Explain plan and collect actual runtime metrics.

** Non Functional Requirements

- Low planning latency for simple queries.
- High-quality plans for expensive queries.
- Robustness to stale stats.
- Deterministic enough for debugging.
- Extensible rule framework.

** High level design and diagram (at block level)

```text
SQL
 |
 v
Logical Plan
 |
 v
Rule Optimizer -> Cost Optimizer -> Physical Plan
        ^              |
        |              v
Catalog Stats     Query History / Runtime Feedback
```

*** Explain the blocks

- Logical planner creates relational algebra.
- Rule optimizer applies predicate pushdown, projection pruning, decorrelation, and simplification.
- Cost optimizer estimates alternatives.
- Stats service provides table, column, and partition information.
- Runtime feedback updates future estimates.

*** Explain the control flow

Planning begins after authorization. The optimizer enumerates candidate plans, estimates costs, picks a physical plan, records plan metadata, and sends stages to the scheduler.

*** Explain the data flow

The optimizer itself moves metadata, not table data. Its output determines how execution workers later move table data through scans, joins, and shuffles.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: planning strategy.

- Rule-based optimizer
  - Pros: fast and predictable.
  - Cons: misses workload-specific cost differences.
- Cost-based optimizer
  - Pros: better for complex joins and large data.
  - Cons: depends on stats quality and can be expensive.
- Adaptive optimizer
  - Pros: corrects bad estimates at runtime.
  - Cons: harder to reason about and test.

L7 answer: combine rule-based simplification with cost-based optimization and targeted adaptive execution for skew or cardinality surprises.

## 13. Design Distributed Joins With Skew Handling

* Question

Design join execution for very large tables with skewed keys.

* Answer

Support multiple join strategies, detect skew at planning and runtime, split hot keys, salt partitions, broadcast small sides, spill safely, and expose query profile diagnostics.

** Scope

Support equi-joins and common analytical joins. Exclude arbitrary cross products as a primary optimization target.

** Functional Requirements

- Choose join algorithms.
- Repartition data by join key.
- Broadcast small inputs.
- Detect and mitigate skew.
- Spill and retry safely.

** Non Functional Requirements

- High throughput.
- Bounded memory use.
- Tail-latency control.
- Efficient network shuffle.
- Correctness under null and SQL semantics.

** High level design and diagram (at block level)

```text
Left Scan          Right Scan
   |                  |
   v                  v
Partition / Broadcast Decision
   |                  |
   +------ Shuffle ---+
              |
              v
        Join Workers
     hash | sort | spill
              |
              v
       Joined Output
```

*** Explain the blocks

- Scans read pruned partitions.
- Planner selects broadcast, repartition, or sort strategy.
- Shuffle service exchanges rows by key.
- Join workers build/probe hash tables or merge sorted streams.
- Spill manager handles oversized state.

*** Explain the control flow

Optimizer picks an initial strategy. Scheduler assigns partitions. Runtime monitors partition sizes and hot workers. If skew is detected, the engine can split hot partitions or fall back to spill/adaptive repartitioning.

*** Explain the data flow

Rows flow from scans into exchange operators. Matching keys are co-located on join workers, joined, and forwarded to downstream aggregation/sort/result stages.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: skew mitigation.

- Static salting
  - Pros: simple and effective for known hot keys.
  - Cons: can increase work and complicate aggregation.
- Runtime hot-key splitting
  - Pros: targets actual skew.
  - Cons: complex coordination.
- Broadcast small side
  - Pros: avoids shuffling large table.
  - Cons: memory blowup if estimate is wrong.
- Spill to disk/remote storage
  - Pros: prevents failure.
  - Cons: slower and can hide bad plans.

L7 answer: prevent common skew in planning, detect unexpected skew at runtime, and make spill a safety net rather than the normal path.

## 14. Design Caching For Warehouse Compute

* Question

Design caching across metadata, local data blocks, and query results.

* Answer

Use multiple caches: metadata cache in cloud services, local SSD block cache in warehouses, and permission-aware result cache for repeated deterministic queries.

** Scope

Accelerate repeated analytical workloads. Exclude caching as a substitute for correct metadata versioning.

** Functional Requirements

- Cache table/partition metadata.
- Cache object-store file blocks locally.
- Cache final query results.
- Invalidate or version caches correctly.
- Track cache hit rates.

** Non Functional Requirements

- Correctness before speed.
- Low-latency cache lookup.
- Tenant isolation.
- Bounded memory/disk.
- Resilience to cache loss.

** High level design and diagram (at block level)

```text
Query Planner
   |
   +--> Metadata Cache
   |
   v
Warehouse Workers
   |
   +--> Local SSD Data Cache
   |
   v
Object Storage

Completed Query -> Result Cache -> Future Query
```

*** Explain the blocks

- Metadata cache stores hot object and partition metadata.
- Local data cache stores column chunks or object ranges.
- Result cache stores final result sets keyed by query dependencies.
- Versioning/invalidation protects correctness.

*** Explain the control flow

Planner checks result cache first when allowed. If not, it fetches metadata through cache, generates a plan, and workers check local data cache before object storage. Commits publish invalidations or version changes.

*** Explain the data flow

Data may be served from local cache instead of object storage. Final results may be served directly from result cache if table versions, role permissions, and session settings match.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: result cache correctness.

- Query-text-only cache key
  - Pros: simple.
  - Cons: incorrect when data, roles, parameters, or nondeterministic functions differ.
- Dependency-aware key
  - Pros: correct for table versions and permissions.
  - Cons: more metadata tracking.
- No result cache
  - Pros: simpler correctness.
  - Cons: misses huge dashboard and repeated-query wins.

L7 answer: use dependency-aware result cache and disable it for nondeterministic or unsafe queries.

## 15. Design Query Result Cache

* Question

Design a result cache for repeated SQL queries in a data warehouse.

* Answer

Fingerprint normalized SQL plus session context, role, permissions, deterministic function state, and referenced object versions. Store results separately with expiration and access checks.

** Scope

Serve exact repeated queries and dashboard refreshes. Exclude approximate cache reuse for semantically similar queries.

** Functional Requirements

- Detect reusable query results.
- Store result sets and metadata.
- Validate permissions and object versions.
- Expire or invalidate entries.
- Support large result pagination.

** Non Functional Requirements

- Correctness and security.
- Very low lookup latency.
- Efficient storage.
- High cache hit rates for BI workloads.
- Auditability.

** High level design and diagram (at block level)

```text
Incoming Query
      |
      v
Normalizer / Fingerprinter
      |
      v
Cache Eligibility Check
      |
      +--> Hit: Access Check -> Return Result
      |
      +--> Miss: Execute Query -> Store Result + Dependencies
```

*** Explain the blocks

- Normalizer canonicalizes SQL and session parameters.
- Eligibility checker rejects nondeterministic/unsafe queries.
- Dependency tracker stores referenced object versions and grants.
- Result store contains data pages.
- Access checker verifies caller can still see the data.

*** Explain the control flow

Before planning fully, the service checks whether a valid cache entry exists. On hit, it verifies role and dependencies. On miss, normal execution runs and stores result metadata if eligible.

*** Explain the data flow

On hit, result pages flow from result store to client. On miss, workers generate output and write pages into result storage for future use.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: invalidation.

- Eager invalidation on every table change
  - Pros: simple correctness.
  - Cons: expensive fanout for popular tables.
- Version-based validation
  - Pros: no broad invalidation; check dependency versions at read time.
  - Cons: dependency metadata must be complete.
- Time-based TTL only
  - Pros: simple.
  - Cons: can return stale/unauthorized data.

L7 answer: use version-based validation with strict permission checks and TTL as cleanup, not correctness.

## 16. Design Multi-Tenant Isolation

* Question

Design tenant isolation for a cloud data platform serving many customers.

* Answer

Isolate tenants at account, metadata, encryption, access-control, compute, networking, and workload-management layers while sharing fleet resources where safe.

** Scope

Support enterprise tenants with sensitive data and variable workloads. Exclude single-tenant dedicated deployments as the only solution.

** Functional Requirements

- Account/org isolation.
- Role-based access control.
- Per-tenant encryption and key management.
- Warehouse-level compute isolation.
- Auditing and governance.

** Non Functional Requirements

- Strong security boundary.
- Noisy-neighbor protection.
- Compliance readiness.
- High utilization.
- Clear billing attribution.

** High level design and diagram (at block level)

```text
Tenant Accounts
   |
   v
Identity / RBAC / Policy Engine
   |
   v
Tenant-Scoped Catalog + Encryption Context
   |
   v
Warehouses / Resource Pools
   |
   v
Shared Cloud Storage With Tenant Isolation
```

*** Explain the blocks

- Identity service authenticates users and service principals.
- Policy engine enforces RBAC, masking, row policies, and network rules.
- Catalog scopes objects by tenant/account.
- Warehouses isolate compute workloads.
- Storage uses tenant-aware paths, metadata, and encryption.

*** Explain the control flow

Every request carries tenant and role context. Control plane checks policies before planning. Warehouse scheduling enforces resource boundaries. Audit logs record access and changes.

*** Explain the data flow

Workers read only files authorized through the plan. Data is decrypted using tenant/account context and returned only after policy enforcement.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: isolation model.

- Dedicated stack per tenant
  - Pros: strongest isolation.
  - Cons: poor utilization and high operational cost.
- Shared services with tenant-scoped metadata and compute isolation
  - Pros: efficient and scalable.
  - Cons: requires rigorous authorization and testing.
- Fully shared compute
  - Pros: maximum utilization.
  - Cons: noisy-neighbor and security concerns.

L7 answer: use shared control services with strict tenant scoping and isolated virtual warehouses for user workloads.

## 17. Design Cross-Region Replication And Failover

* Question

Design replication and failover for warehouse data and metadata across regions or clouds.

* Answer

Replicate object-store data and metadata change logs asynchronously to a secondary region, expose replication checkpoints, and support controlled failover/failback.

** Scope

Support disaster recovery and read locality. Exclude strict zero-latency global serializability for all writes.

** Functional Requirements

- Replicate databases/accounts to another region.
- Track RPO/RTO.
- Fail over to secondary.
- Reconcile or fail back.
- Validate data and metadata consistency.

** Non Functional Requirements

- Low RPO for critical data.
- Bounded RTO.
- Cost-aware bandwidth usage.
- Secure cross-region transfer.
- Clear operational controls.

** High level design and diagram (at block level)

```text
Primary Region
 Catalog Log -> Replication Service -> Secondary Catalog
 Object Files -> Object Replicator ----> Secondary Storage

Failover Controller
 DNS / Account Routing / Promotion
```

*** Explain the blocks

- Catalog log captures ordered metadata changes.
- Object replicator copies referenced files.
- Secondary catalog replays changes after files arrive.
- Failover controller promotes secondary and routes clients.

*** Explain the control flow

Primary commits emit replication events. Secondary applies them in order after verifying data availability. During failover, writes stop or fence on primary, secondary is promoted at a checkpoint, and clients are routed there.

*** Explain the data flow

Data files and metadata logs flow from primary to secondary. Queries after failover read from secondary object storage and catalog snapshots.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: replication consistency.

- Synchronous replication
  - Pros: near-zero RPO.
  - Cons: high write latency and lower availability.
- Asynchronous replication
  - Pros: lower latency and cost.
  - Cons: nonzero data loss window.
- Semi-synchronous checkpointing
  - Pros: better RPO for selected objects.
  - Cons: more complex and still not free.

L7 answer: default to async with visible lag/RPO, then offer stricter modes for critical accounts or metadata.

## 18. Design Secure Data Sharing

* Question

Design no-copy data sharing between provider and consumer accounts.

* Answer

Share metadata references to provider-managed data rather than copying files. Consumers query shared objects with their own compute while the provider controls grants, revocation, and policies.

** Scope

Support cross-account and possibly cross-region sharing. Exclude arbitrary export as the default sharing path.

** Functional Requirements

- Provider creates shares.
- Consumer imports shared databases.
- Enforce grants, masking, row policies, and revocation.
- Audit consumer access.
- Support versioned consistent reads.

** Non Functional Requirements

- No data copy for same-region sharing.
- Strong access control.
- Low-latency metadata resolution.
- Clear provider/consumer billing.
- Safe revocation.

** High level design and diagram (at block level)

```text
Provider Account
 Tables -> Share Object -> Grants / Policies
                         |
                         v
Consumer Account
 Imported Database Metadata
                         |
                         v
Consumer Warehouse reads Provider Data References
```

*** Explain the blocks

- Share object lists exposed databases/schemas/tables/views.
- Grant engine maps consumer accounts to shared objects.
- Imported database is a consumer-side metadata view.
- Consumer warehouse executes queries using authorized references.

*** Explain the control flow

Provider creates a share and grants it. Consumer imports it. Query planning checks both consumer role permissions and provider share permissions. Revocation updates metadata and invalidates future access.

*** Explain the data flow

For same-region sharing, consumer compute reads provider-owned files through authorized references. Results flow to consumer; provider data is not copied.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: sharing mechanism.

- Physical copy
  - Pros: simple isolation and offline use.
  - Cons: stale copies, high storage cost, hard revocation.
- Metadata reference sharing
  - Pros: fresh data, no copy, efficient.
  - Cons: complex authorization and cross-account metadata.
- API-mediated access
  - Pros: provider controls every request.
  - Cons: lower SQL flexibility and potential bottleneck.

L7 answer: use metadata reference sharing with strict policy enforcement and audit logs.

## 19. Design Warehouse Workload Management

* Question

Design workload management for concurrent analytical queries.

* Answer

Classify queries, estimate resource needs, queue/admit work by priority and fairness, autoscale clusters for concurrency, and protect the warehouse from runaway queries.

** Scope

Support BI dashboards, ETL jobs, ad hoc analysts, and background maintenance. Exclude hard real-time scheduling.

** Functional Requirements

- Queue queries.
- Enforce priority and resource classes.
- Admit based on memory/CPU/concurrency.
- Autoscale clusters.
- Cancel, timeout, or throttle runaway queries.

** Non Functional Requirements

- Low queue latency for interactive work.
- Fairness.
- High utilization.
- Predictable tail latency.
- Transparent query diagnostics.

** High level design and diagram (at block level)

```text
Incoming Queries
      |
      v
Classifier / Estimator
      |
      v
Admission Controller
 queues | priority | fairness | limits
      |
      v
Warehouse Scheduler -> Cluster Autoscaler
      |
      v
Execution Workers
```

*** Explain the blocks

- Classifier tags workload type and priority.
- Estimator predicts resource usage.
- Admission controller decides run vs queue.
- Scheduler assigns work to clusters/workers.
- Autoscaler adjusts compute for queue depth.

*** Explain the control flow

On query submission, the system estimates cost, checks limits, queues or admits the query, updates autoscaler signals, monitors execution, and enforces timeout/cancel policies.

*** Explain the data flow

Admitted queries move through normal execution. Queued queries do not consume data-plane resources until admitted.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: admission policy.

- FIFO
  - Pros: simple and fair by arrival.
  - Cons: head-of-line blocking.
- Shortest-job-first
  - Pros: good average latency.
  - Cons: can starve large jobs and depends on estimates.
- Priority queues
  - Pros: protects business-critical workloads.
  - Cons: can starve lower-priority work.
- Weighted fair queuing
  - Pros: balances fairness and priority.
  - Cons: more complex to explain and tune.

L7 answer: combine priority with weighted fairness, separate warehouses for strong isolation, and query acceleration/autoscaling for bursts.

## 20. Design Cost Metering And Billing

* Question

Design metering and billing for compute, storage, serverless services, data transfer, and cloud services usage.

* Answer

Emit immutable usage events from every billable subsystem, aggregate them through a reliable billing pipeline, reconcile with resource state, and expose near-real-time cost attribution.

** Scope

Support customer billing, internal cost attribution, budget alerts, and usage analytics. Exclude pricing strategy details.

** Functional Requirements

- Meter warehouse uptime and size.
- Meter serverless/background compute.
- Meter storage bytes over time.
- Meter data transfer and cloud-services usage.
- Provide usage reports and invoices.

** Non Functional Requirements

- Accurate and auditable.
- Idempotent event processing.
- Low overhead on critical paths.
- Tamper-resistant records.
- Timely cost visibility.

** High level design and diagram (at block level)

```text
Billable Services
 Warehouse | Storage | Serverless | Transfer | Cloud Services
        |
        v
Usage Event Collector
        |
        v
Durable Metering Stream
        |
        v
Aggregator / Rating / Reconciliation
        |
        v
Billing Store -> Reports / Alerts / Invoice
```

*** Explain the blocks

- Billable services emit usage events.
- Collector validates and deduplicates events.
- Stream provides durable ordered processing.
- Aggregator computes usage windows.
- Rating applies pricing.
- Reconciliation compares events to source-of-truth resource states.

*** Explain the control flow

Services register meters and emit events. Billing jobs aggregate by account, region, service, and time. Reconciliation detects missing or duplicate events. Reports and invoices are generated from immutable records.

*** Explain the data flow

Usage events flow from services into the metering stream, then into aggregated usage tables and customer-facing reports.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: metering accuracy.

- Synchronous billing writes in hot path
  - Pros: immediate accounting.
  - Cons: can hurt availability and latency.
- Async immutable usage events
  - Pros: resilient and low hot-path overhead.
  - Cons: requires reconciliation and late-event handling.
- Periodic state snapshots only
  - Pros: simple for storage.
  - Cons: poor for short-lived compute and fine attribution.

L7 answer: use async immutable events for compute plus periodic snapshots for storage, with reconciliation and customer-visible correction handling.

## 21. Design A Governed Enterprise Agent Data Platform

* Question

Design a Snowflake-style governed platform where business users and builders can create AI agents that reason over enterprise data, run code, call tools, and automate workflows without moving data outside the security boundary.

* Answer

The key design is an agent control plane on top of the data cloud: agent definitions, semantic tools, search services, MCP connectors, code sandboxes, evaluation traces, policy enforcement, and cost governance all share the same identity, catalog, lineage, and compute isolation model as the warehouse platform.

** Scope

Support agents that answer questions over structured and unstructured data, generate SQL, execute Python in an isolated runtime, call stored procedures or UDF tools, invoke approved MCP connectors, preserve conversation context, and expose monitoring, feedback, evaluations, and cost controls. Exclude general consumer chat and unrestricted autonomous actions.

** Functional Requirements

- Create versioned agent objects with model, instructions, tools, budgets, and sharing policy.
- Ground answers in governed tables, semantic views, search indexes, notebooks, code repos, and data lineage.
- Support structured SQL tools, unstructured search tools, code execution, charts, custom tools, packaged skills, and MCP connectors.
- Maintain thread state, run events, tool traces, approvals, feedback, and evaluation results.
- Enforce user role, object privileges, row access policies, network egress policy, and per-team usage budgets.
- Let builders use coding agents across CLI, desktop, IDE, Snowsight, and SDK/API surfaces while preserving the same governance semantics.

** Non Functional Requirements

- Strong tenant and role isolation.
- No implicit broadening of data access by agent orchestration.
- Auditable tool calls and data reads.
- Bounded blast radius for buggy prompts, expensive loops, or unsafe tool actions.
- Low-friction onboarding for thousands of users.
- Predictable latency and cost attribution for interactive and long-running workflows.

** High level design and diagram (at block level)

```text
Business User / Builder / App
        |
        v
Agent API / CoWork / CoCo / SDK
        |
        v
Agent Control Plane
 Definitions | Versions | Policies | Budgets | Sharing
        |
        v
Orchestration Runtime
 Planner | Tool Router | Reflection | Run Event Log
        |
        +------------------+-------------------+------------------+
        v                  v                   v                  v
 Structured Tools     Search Tools        Code Sandbox       MCP/Custom Tools
 Analyst / SQL        Cortex Search       Python Runtime     UDF/SP/External
        |                  |                   |                  |
        +------------------+-------------------+------------------+
                           |
                           v
 Catalog / RBAC / Lineage / Warehouse Compute / Audit
```

*** Explain the blocks

- Agent API, CoWork, CoCo, and SDK surfaces accept interactive, embedded, and developer workflow requests.
- Agent Control Plane owns agent objects, versions, tool grants, rollout state, sharing, and resource budgets.
- Orchestration Runtime runs the plan, tool-use, reflection loop and records structured run events.
- Structured Tools generate governed SQL over semantic views and warehouse compute.
- Search Tools retrieve from approved unstructured indexes with metadata filters and citations.
- Code Sandbox executes Python or data-development tasks with constrained filesystem, network, packages, and warehouse access.
- MCP and Custom Tools expose approved external systems, stored procedures, UDFs, and reusable skills.
- Catalog, RBAC, lineage, audit, and billing provide the shared governance substrate.

*** Core components and low-level design

- **Agent object store**
  Stores immutable versions of agent instructions, model policy, tool references, allowed roles, budget bindings, and rollout state. Updates create new versions, and production traffic is pinned to a version or a staged rollout rule.
- **Permission compiler**
  Resolves the caller's active role, agent grants, tool object grants, row access policies, and network policy into a short-lived execution capability. The runtime passes this capability to every tool call so a tool cannot read data the user could not read directly.
- **Tool router**
  Maintains a registry of tool schemas, privilege requirements, side-effect level, expected cost, timeout, and approval policy. It admits read-only tools automatically, routes high-risk or write tools through approval, and rejects ambiguous or over-broad tool arguments before execution.
- **Run event log**
  Appends every plan step, tool call, SQL query id, search result id, sandbox artifact, approval, token count, warehouse usage, and final answer reference. This powers replay, evaluation, debugging, cost attribution, and compliance review.
- **Sandbox manager**
  Starts isolated execution with scoped credentials, package allowlists, CPU/memory/time limits, result-size limits, and egress controls. Artifacts are written to governed stages and linked back to the run event log.
- **Evaluation service**
  Samples runs, accepts user feedback, replays benchmark prompts against candidate agent versions, checks answer quality, citation coverage, tool correctness, policy violations, and cost regressions before rollout.

*** Explain the control flow

An admin creates an agent object, attaches tools, grants roles, sets budgets, and stages a version. The policy compiler validates that every tool has an owner, privilege boundary, timeout, budget, and audit sink. Evaluation jobs compare the new version against baseline prompts. A rollout controller promotes it to selected users or teams, watches quality and cost metrics, and can roll back to the prior version without deleting run history.

*** Explain the data flow

A user asks a business question. The runtime loads the agent version and thread context, compiles the user's execution capability, chooses tools, and emits run events. Structured subqueries run against semantic views and warehouse compute. Unstructured retrieval hits Cortex Search-style indexes. Code steps execute in the sandbox and write governed artifacts. External actions pass through MCP/custom-tool policy and optional approval. The final response returns with citations, generated artifacts, cost metadata, and feedback hooks.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Explain each of the option/topic with pros and cons

Topic: governance boundary for agent actions.

- Agent has its own service role
  - Pros: simple to provision and cache.
  - Cons: risks over-broad access, confusing accountability, and privilege escalation through prompts.
- Agent always executes as the querying user
  - Pros: easiest mental model and preserves existing RBAC semantics.
  - Cons: harder for shared service workflows and background automation.
- Capability-based hybrid
  - Pros: user role remains the default, while explicitly approved service capabilities cover narrow background actions.
  - Cons: requires a robust permission compiler, token issuance, revocation, and audit model.

L7 answer: default to user-role execution for reads and analysis. Require explicit scoped capabilities, owner approval, argument validation, and replayable audit for write actions or background automation.

Topic: centralized orchestration versus tool-local autonomy.

- Centralized orchestration runtime
  - Pros: uniform tracing, budgets, model routing, and policy enforcement.
  - Cons: can become a bottleneck and may not understand domain-specific tool behavior.
- Tool-local autonomous agents
  - Pros: domain teams can optimize independently and hide implementation detail.
  - Cons: inconsistent safety, harder cross-tool reasoning, and fragmented observability.
- Federated agents behind a governed tool contract
  - Pros: central policy and tracing with local domain specialization.
  - Cons: needs strict schemas, version compatibility, and failure contracts.

L7 answer: use a central runtime for policy, planning, tracing, and budgets, but allow domain agents or skills behind typed tool contracts when they publish clear side-effect, cost, and data-access metadata.

Topic: evaluation and rollout.

- Manual prompt testing
  - Pros: fast for early prototypes.
  - Cons: misses regressions and does not scale to thousands of users.
- Offline benchmark replay
  - Pros: catches quality, citation, and policy regressions before rollout.
  - Cons: benchmark drift and synthetic prompts can miss real-world behavior.
- Online canary with feedback and rollback
  - Pros: validates real workloads and cost impact.
  - Cons: requires careful blast-radius limits and user-visible fallback.

L7 answer: combine offline replay with online canaries. Gate rollout on answer quality, policy violations, cost per successful task, tool error rate, and user feedback, then retain every run for audit and debugging.

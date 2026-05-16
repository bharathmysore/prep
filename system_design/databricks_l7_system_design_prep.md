# Databricks L7 System Design Prep

This guide uses Databricks-style cloud, data, and distributed systems prompts. These are representative preparation questions, not a claim about Databricks' private interview bank.

Maintainer note: treat this as a living catalog. Add, remove, merge, or reorder prompts as public sources and target role expectations change. Do not encode the current question count in the file name or title; compute it from the numbered `## N.` headings when needed.

Reference context:
- Databricks engineering interview preparation: https://www.databricks.com/sites/default/files/2025-04/engineering-careers-site-interview-prep-april-2025-002.pdf
- Delta Lake documentation: https://docs.delta.io/index.html
- Apache Spark Structured Streaming guide: https://spark.apache.org/docs/latest/structured-streaming-programming-guide.html
- Databricks Lakehouse architecture overview: https://www.databricks.com/blog/what-is-data-lakehouse
- Databricks Lakeflow Spark Declarative Pipelines documentation: https://docs.databricks.com/aws/en/ldp
- Databricks Lakeflow Jobs documentation: https://docs.databricks.com/aws/en/jobs/
- Databricks Lakeflow Jobs configuration and queueing documentation: https://docs.databricks.com/aws/en/jobs/configure-job
- Databricks Lakeflow Jobs monitoring and observability documentation: https://docs.databricks.com/aws/en/jobs/monitor
- Databricks system design guide: https://www.systemdesignhandbook.com/guides/databricks-system-design-interview/

Use each answer as a 45-60 minute interview outline. At L7, explain control plane, data plane, correctness model, failure handling, migration, operability, ownership, and cost.

## 1. Design Delta Lake On Cloud Object Storage

* **Question**
  Design an ACID table layer over cloud object storage for large-scale analytical workloads.

* **Answer**
  * **Scope**
    Support analytical tables stored as columnar files in S3, ADLS, or GCS. Include batch writes, streaming writes, reads, updates, deletes, schema evolution, time travel, concurrent writers, and table optimization. Exclude OLTP point-write workloads and sub-millisecond serving.
  * **Functional Requirements**
    Create and manage tables, read consistent snapshots, append data, update/delete/merge records, enforce schemas, support rollback/time travel, compact files, vacuum old data, and allow multiple engines to interoperate through table metadata.
  * **Non Functional Requirements**
    Petabyte-scale storage, high durability, high read throughput, low metadata overhead, safe concurrent writes, high availability inside a region, cost-efficient storage, and recoverability after writer or cluster failure.
  * **High level design and diagram (at block level)**
    ```text
    Spark / SQL / Jobs
            |
            v
    Table API / Transaction Coordinator
            |
            +--> Catalog / Permissions
            |
            +--> Delta Transaction Log (_delta_log)
            |
            v
    Parquet Writers / Readers
            |
            v
    Cloud Object Storage
    Data Files | Log Files | Checkpoints
    ```
    * **Explain the blocks**
      Spark, SQL warehouses, and jobs issue table operations. The table API interprets reads and writes against a table snapshot. The transaction coordinator validates conflicts and publishes commits. The catalog stores table identity, schema, owner, and permissions. The transaction log records ordered metadata changes and file additions/removals. Object storage stores immutable Parquet data files, JSON log entries, and checkpoint files.
    * **Explain the control flow**
      Table creation, schema changes, permissions, retention, compaction policy, and feature flags flow through the control plane. A writer reads the latest table version, computes candidate data files, validates schema and constraints, checks whether concurrent commits conflict, then atomically publishes the next log version. Checkpointing periodically compacts log state so future readers do not replay every JSON commit.
    * **Explain the data flow**
      For writes, executors produce Parquet files in object storage and the coordinator commits metadata that makes those files visible. For reads, the engine loads the latest snapshot from the log/checkpoint, prunes files using partition and file statistics, scans only needed columns, and returns results. Deletes and updates usually create new files plus remove-file tombstones rather than mutating existing files in place.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Concurrent writers: pessimistic locking vs optimistic concurrency vs single writer**
      The hard problem is making object storage behave like an ACID table store even though object storage is not a database. Pessimistic locking is intuitive and prevents conflicts early, but introduces lock-service availability risk and limits throughput. A single-writer model is simple, but underuses distributed compute and blocks independent pipelines. Optimistic concurrency scales better: writers proceed independently and conflicts are detected at commit time, but conflicted writers must retry. Prefer optimistic concurrency with table-version validation, conflict detection, idempotent retries, and narrow transaction scopes.
    * **Metadata scaling: replay logs vs checkpoints vs manifest service**
      Replaying every transaction log entry is simple, but slow for long-lived tables. Periodic checkpoints reduce snapshot load time, but add checkpoint-write overhead and recovery logic. A dedicated manifest or metadata service can speed planning further, but adds another consistency boundary. Prefer log plus checkpoint as the default, and add cached manifests for very large/hot tables.
    * **Updates and deletes: copy-on-write vs merge-on-read**
      Copy-on-write rewrites files and keeps reads simple and fast, but expensive for small updates. Merge-on-read stores delta files and resolves them at read time, improving write latency but increasing read complexity. Prefer copy-on-write for analytics-heavy tables and evaluate merge-on-read when update volume is high and read latency can tolerate extra work.

## 2. Design A Real-Time Ingestion Pipeline Into A Lakehouse

* **Question**
  Design a pipeline that ingests high-volume events into Delta tables with low latency and reliable recovery.

* **Answer**
  * **Scope**
    Ingest events from Kafka, Kinesis, Event Hubs, files, or application logs into Bronze, Silver, and Gold Delta tables. Include schema validation, deduplication, checkpointing, backpressure, and late-event handling. Exclude deep BI semantic modeling.
  * **Functional Requirements**
    Read from streaming sources, preserve source offsets, validate schemas, deduplicate events, quarantine bad records, transform raw events into cleaned tables, aggregate into serving tables, and expose processing lag and quality metrics.
  * **Non Functional Requirements**
    High throughput, near-real-time latency, recoverability, backpressure handling, horizontal scale, replayability, bounded state growth, and predictable operating cost.
  * **High level design and diagram (at block level)**
    ```text
    Event Producers
          |
          v
    Kafka / Kinesis / Event Hubs
          |
          v
    Streaming Ingestion Job
      Schema | Dedupe | Quality Checks
          |
          +--> Checkpoint Store
          |
          v
    Bronze Delta -> Silver Delta -> Gold Delta
          |
          v
    Monitoring / Alerts / Data Quality UI
    ```
    * **Explain the blocks**
      Producers create durable event streams. The streaming job reads offsets, validates data, deduplicates, and writes to Delta. Checkpoints store offsets, state, and progress. Bronze preserves raw data, Silver stores cleaned/enriched data, and Gold serves business aggregates. Monitoring tracks lag, throughput, errors, and quality failures.
    * **Explain the control flow**
      Pipeline owners define source subscriptions, schema versions, trigger intervals, checkpoint locations, quality rules, retention, autoscaling limits, and alert thresholds. The scheduler starts or restarts streaming jobs, and the streaming engine uses checkpoints to resume from the last committed state after failures.
    * **Explain the data flow**
      Events enter the stream, are consumed in micro-batches or continuous mode, validated, written to Bronze, transformed to Silver, and aggregated into Gold. Invalid records go to quarantine tables. Metrics and lineage events are emitted along the path.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Processing model: micro-batch vs continuous processing**
      Micro-batch processing is easier to checkpoint, optimize, and recover, but adds trigger-interval latency. Continuous processing can reduce latency, but has more restrictions and operational complexity. Prefer micro-batch for most lakehouse pipelines because second-level latency is often acceptable and correctness is easier to explain.
    * **Delivery semantics: at-most-once vs at-least-once vs effectively-once**
      At-most-once is low overhead but can lose data. At-least-once preserves data under retry but can duplicate records. Effectively-once uses source offsets, deterministic batch IDs, idempotent writes, and transactional sink commits to make retries safe. Prefer effectively-once within the lakehouse, while being clear that external systems may only provide at-least-once guarantees.
    * **Backpressure: drop, buffer, throttle, or scale**
      Dropping events protects the platform but violates correctness for analytics. Buffering in Kafka absorbs spikes but increases lag. Throttling producers protects downstream systems but requires producer cooperation. Autoscaling improves throughput but raises cost and has startup delay. Prefer buffering plus autoscaling and explicit lag SLOs; use throttling for abusive or non-critical producers.

## 3. Design CDC From OLTP Databases Into A Lakehouse

* **Question**
  Design a change data capture system that replicates OLTP database changes into queryable lakehouse tables.

* **Answer**
  * **Scope**
    Capture inserts, updates, deletes, and schema changes from relational databases into Delta tables. Include backfill, replay, ordering, deduplication, and current-state materialization. Exclude bidirectional replication.
  * **Functional Requirements**
    Read database logs, publish changes to a durable stream, preserve per-table and per-key ordering, support initial snapshots, apply deletes, handle schema evolution, expose replication lag, and rebuild target tables from raw CDC history.
  * **Non Functional Requirements**
    Low source impact, low replication lag, high correctness, replayability, fault tolerance, scalable merge performance, and clear operational visibility.
  * **High level design and diagram (at block level)**
    ```text
    OLTP Database
      WAL / Binlog
          |
          v
    CDC Connector -> Durable Stream -> CDC Processor
          |                               |
          v                               v
    Offset Store                    Raw CDC Delta
                                          |
                                          v
                                   Current-State Delta
    ```
    * **Explain the blocks**
      The CDC connector tails the source log and records offsets. The durable stream decouples source capture from lakehouse processing. The processor normalizes events, handles schema changes, and writes raw CDC. Current-state tables are built with merges or incremental apply logic.
    * **Explain the control flow**
      Operators register source databases, tables, primary keys, snapshot settings, schema compatibility rules, and lag alerts. The connector controls snapshot and log-tail phases. The processor tracks offsets and target table versions so replays and restarts are deterministic.
    * **Explain the data flow**
      Initial snapshot records land as baseline events. Subsequent WAL/binlog changes flow into the stream, then into raw CDC tables. Merge jobs apply the latest change per primary key to current-state Delta tables, including tombstones for deletes.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Initial load plus live changes**
      Running a full snapshot and then starting CDC later can miss or duplicate updates. Locking the source table gives correctness but can hurt production. A consistent snapshot plus log position is safer: capture a snapshot at a known log offset, then replay changes after that offset. Prefer source-native snapshot mechanisms that minimize locks and produce a resumable boundary.
    * **Ordering: global order vs per-key order**
      Global ordering is simple to reason about but expensive and hard across partitions. Arrival-order processing is fast but can corrupt current state. Per-key ordering using source commit sequence, transaction ID, or LSN scales well and is sufficient for materializing rows. Prefer per-key ordering with version checks during merge.
    * **Schema evolution: fail fast vs permissive evolution**
      Failing on schema change protects downstream correctness but increases operational toil. Permissive evolution keeps pipelines running but can silently break consumers. Prefer explicit compatibility rules: additive changes auto-evolve, destructive changes require approval and versioned migration.

## 4. Design A Medallion Data Architecture

* **Question**
  Design a lakehouse data architecture with Bronze, Silver, and Gold layers for enterprise analytics and ML.

* **Answer**
  * **Scope**
    Organize raw, cleaned, and business-ready datasets for multiple domains. Include quality gates, lineage, replay, ownership, and serving paths. Exclude detailed dashboard design.
  * **Functional Requirements**
    Ingest raw data, preserve source fidelity, validate and deduplicate data, join and enrich records, produce domain-level tables, support backfills, expose lineage, and allow BI/ML consumers to discover trusted datasets.
  * **Non Functional Requirements**
    Traceability, reproducibility, governance, cost control, modular ownership, predictable SLAs, and safe evolution over time.
  * **High level design and diagram (at block level)**
    ```text
    Sources
      |
      v
    Bronze Tables -> Silver Tables -> Gold Tables
      |                |              |
      v                v              v
    Raw QA          Clean QA       Business QA
      |
      v
    Lineage / Catalog / Access Control
    ```
    * **Explain the blocks**
      Bronze stores immutable raw records. Silver standardizes schemas, filters bad data, deduplicates, and enriches. Gold stores curated aggregates, dimensional models, or ML-ready feature tables. Quality checks and lineage attach trust and observability to every layer.
    * **Explain the control flow**
      Data owners define contracts, quality expectations, retention, and access policies for each layer. Orchestration controls dependencies, backfills, release gates, and rollback. Catalog metadata helps consumers understand freshness, ownership, and lineage.
    * **Explain the data flow**
      Raw source data lands in Bronze. Transformations produce Silver tables with normalized and deduped records. Business logic produces Gold tables optimized for downstream query, reporting, or ML workloads.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Layering: direct-to-Gold vs Bronze/Silver/Gold**
      Direct-to-Gold minimizes latency and storage but makes debugging, replay, and governance difficult. Bronze/Silver/Gold improves traceability and recovery, but adds storage, compute, and pipeline latency. Prefer layered architecture for enterprise data platforms, with exceptions for truly ephemeral or low-value datasets.
    * **Data quality enforcement: warn, quarantine, or block**
      Warning keeps data flowing but can propagate bad data. Quarantine protects trusted tables but requires remediation workflows. Blocking preserves correctness but can break critical downstream jobs. Prefer severity-based handling: block contract violations for Gold, quarantine suspicious records in Silver, and preserve raw data in Bronze.
    * **Ownership: centralized data team vs domain ownership**
      A central team gives consistency but becomes a bottleneck. Domain ownership scales knowledge but can fragment standards. Prefer federated ownership with central platform standards for naming, contracts, quality, lineage, and security.

## 5. Design A Distributed SQL Query Engine

* **Question**
  Design a distributed SQL engine that executes analytical queries over lakehouse tables at large scale.

* **Answer**
  * **Scope**
    Support SQL parsing, planning, optimization, distributed execution, shuffle, caching, and result delivery over Delta/Parquet data. Exclude transactional OLTP workloads.
  * **Functional Requirements**
    Parse SQL, authenticate users, resolve metadata, optimize plans, scan files, filter, join, aggregate, sort, spill, retry failed tasks, and return results through JDBC/ODBC or APIs.
  * **Non Functional Requirements**
    Low interactive latency, high throughput, high concurrency, fault tolerance, efficient object-storage access, predictable costs, and workload isolation.
  * **High level design and diagram (at block level)**
    ```text
    SQL Client
        |
        v
    Coordinator
      Parser | Analyzer | Optimizer
        |
        +--> Catalog / Stats / Permissions
        |
        v
    Distributed Workers
      Scan | Filter | Join | Aggregate | Spill
        |
        +--> Shuffle Service
        |
        v
    Object Storage / Cache
    ```
    * **Explain the blocks**
      The coordinator owns query lifecycle and planning. The catalog provides table metadata, permissions, and statistics. Workers execute fragments. The shuffle service exchanges intermediate data. Cache and object storage provide data access.
    * **Explain the control flow**
      A query enters the coordinator, is authenticated and authorized, analyzed against catalog metadata, optimized using statistics, split into stages, assigned to workers, monitored for progress, and either completed, retried, or cancelled.
    * **Explain the data flow**
      Workers scan selected files and columns, apply filters, exchange rows for joins and aggregations through shuffle, spill large intermediates when necessary, and stream final results back to the coordinator and client.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Query planning: static optimization vs adaptive execution**
      Static optimization is simpler and predictable, but can choose bad plans when statistics are stale. Adaptive execution adjusts joins, partitions, and skew handling at runtime, but increases engine complexity. Prefer static cost-based planning plus adaptive execution for large distributed joins.
    * **Join strategy: broadcast vs shuffle vs sort-merge**
      Broadcast joins are fast for small dimension tables but can overload worker memory. Shuffle hash joins scale to larger tables but are network heavy. Sort-merge joins handle large inputs but require sorting and can spill. Prefer choosing based on table size, stats, memory, and skew.
    * **Object storage access: direct reads vs caching**
      Direct reads are simple and elastic but have higher latency and request cost. Local or remote cache accelerates repeated scans but can be stale and expensive. Prefer cache for hot data and rely on transaction-log versions to validate cache correctness.

## 6. Design A Metadata And Catalog Service

* **Question**
  Design a metadata/catalog service for lakehouse tables, schemas, permissions, and lineage.

* **Answer**
  * **Scope**
    Manage catalogs, schemas, tables, views, functions, storage locations, ownership, permissions, lineage, and audit metadata. Exclude full query execution.
  * **Functional Requirements**
    Create/drop/alter objects, resolve table names, store schemas and table properties, enforce grants, expose lineage, record audits, support search/discovery, and serve metadata to many query engines.
  * **Non Functional Requirements**
    High availability, strong consistency for grants and object definitions, low lookup latency, scalable metadata size, secure multi-tenancy, and clear auditability.
  * **High level design and diagram (at block level)**
    ```text
    Clients / Engines / Admin UI
            |
            v
    Catalog API
      AuthN | AuthZ | Validation
            |
            +--> Metadata DB
            +--> Policy Engine
            +--> Audit Log
            +--> Metadata Cache
    ```
    * **Explain the blocks**
      Catalog API is the front door for object metadata. Metadata DB is the authoritative store. Policy engine evaluates grants and data policies. Audit log records administrative and data-access events. Cache reduces read latency for hot metadata.
    * **Explain the control flow**
      Admin changes flow through validation, authorization, transactionally update metadata, emit audit events, and invalidate or version caches. Engines fetch object metadata and policy decisions before query planning.
    * **Explain the data flow**
      Query engines request table metadata, schema, storage location, and permissions. The catalog returns a versioned response. Lineage and audit events flow back from engines to the catalog backend or event pipeline.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Consistency: strong metadata vs eventual metadata**
      Strong consistency prevents stale schemas and permission leaks, but can limit availability and increase write latency. Eventual consistency scales reads but risks incorrect authorization or stale table definitions. Prefer strong consistency for permissions and object definitions, with versioned read caches for performance.
    * **Cache invalidation**
      No cache is correct but slow at large query volume. Long TTL cache is fast but dangerous for grants. Versioned cache with invalidation is more complex but safer. Prefer versioned metadata, short TTLs, and immediate invalidation for security-sensitive changes.
    * **Catalog topology: centralized vs regional**
      Centralized catalog simplifies governance but adds cross-region latency and blast radius. Regional catalogs improve locality but complicate global consistency. Prefer a logically centralized catalog with regional read replicas and carefully controlled write paths.

## 7. Design Unity Catalog-Style Access Control

* **Question**
  Design fine-grained access control and governance for lakehouse data.

* **Answer**
  * **Scope**
    Support identity, groups, roles, grants, external locations, table/column/row policies, audit logs, and policy enforcement across SQL, notebooks, jobs, and ML workloads.
  * **Functional Requirements**
    Authenticate users and service principals, authorize catalog/schema/table/view/file access, enforce column masks and row filters, isolate workspaces, govern external storage, record audit trails, and support policy review.
  * **Non Functional Requirements**
    Security correctness, low query overhead, explainable policy decisions, high availability, tenant isolation, and compliance-grade auditability.
  * **High level design and diagram (at block level)**
    ```text
    User / Service Principal
            |
            v
    Query Engine / Job Runtime
            |
            v
    Policy Decision Point
      Grants | Row Filters | Column Masks
            |
            +--> Identity Provider
            +--> Catalog
            +--> Audit Pipeline
            |
            v
    Storage Credential Broker
            |
            v
    Cloud Storage
    ```
    * **Explain the blocks**
      Query engines and job runtimes request authorization decisions. The policy decision point evaluates identity, grants, and fine-grained rules. Catalog stores protected objects. Credential broker issues scoped storage access. Audit pipeline records who accessed what.
    * **Explain the control flow**
      Administrators define identities, groups, grants, policies, and storage locations. Policy updates are validated, versioned, audited, and propagated to enforcement points. Engines must check policy before planning and before accessing data.
    * **Explain the data flow**
      User queries are rewritten or filtered based on policy decisions. Authorized runtimes receive scoped credentials or access tokens, read permitted files, and emit audit events for data access and administrative changes.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Enforcement location: client, query engine, or storage layer**
      Client-side enforcement is flexible but insecure because clients can bypass it. Query-engine enforcement is practical and supports SQL rewrites, but all engines must integrate correctly. Storage-layer enforcement is strong but hard to express row/column policies across file formats. Prefer query-engine enforcement plus scoped storage credentials and denial-by-default paths.
    * **Fine-grained policies: views vs native row/column policies**
      Views are portable and easy to reason about, but can sprawl and be bypassed if base tables are exposed. Native policies are centralized and reusable, but require engine support. Prefer native policies for governed platforms, with secure views for compatibility.
    * **Performance vs security**
      Evaluating policies on every request can add latency. Caching decisions improves performance but can leak access after revocation if stale. Prefer short-lived, versioned policy decisions and immediate invalidation for revocation.

## 8. Design A Data Pipeline Job Scheduler

* **Question**
  Design a scheduler for batch and streaming data pipelines with DAGs, retries, dependencies, and backfills.

* **Answer**
  * **Scope**
    Manage workflow definitions, schedules, DAG dependencies, task execution, retries, backfills, alerts, and job history. Exclude low-level compute provisioning internals unless asked.
  * **Functional Requirements**
    Create DAGs, trigger by time/event/manual request, enforce dependencies, submit tasks to compute, retry failures, support backfills, pause/resume jobs, alert on SLA misses, and expose run history.
  * **Non Functional Requirements**
    High availability, durable state, scalable scheduling, idempotent task execution, predictable retries, multi-tenant fairness, and operational debuggability.
  * **High level design and diagram (at block level)**
    ```text
    User / API / Git
          |
          v
    Workflow Definition Store
          |
          v
    Scheduler / DAG Planner
          |
          v
    Task Queue / Lease Store
          |
          v
    Workers / Cluster Launcher
          |
          v
    Logs / Metrics / Run State DB
    ```
    * **Explain the blocks**
      Definition store holds DAGs and job configs. Scheduler computes runnable tasks. Queue and lease store coordinate execution. Workers launch jobs or clusters. Run state DB records attempts, outputs, and failures. Logs and metrics power debugging and alerts.
    * **Core components and low-level design**
      * **Workflow Definition Service**
        Owns job definitions, task graphs, schedules, event triggers, parameters, Git references, access-control metadata, duration thresholds, streaming backlog thresholds, and max-concurrency settings. APIs include `CreateJob`, `ResetJob`, `RunNow`, `RepairRun`, `PauseSchedule`, and `CancelRun`. Store every definition with an immutable version so a run can be explained later even after the job is edited.
      * **DAG Planner and Trigger Engine**
        Converts schedules, file-arrival events, API calls, and manual runs into run intents. It validates acyclic task graphs, expands `for each` style fanout, evaluates conditional branches, and assigns each run an idempotency key. The invariant is that a given trigger identity creates at most one authoritative run record.
      * **Run Coordinator**
        Owns the state machine for `Queued -> Pending -> Running -> Succeeded | Failed | TimedOut | Canceled | Skipped`. It enforces workspace, job, and task concurrency limits before dispatching work. When limits are hit, it can queue runs for a bounded window instead of skipping immediately. Each task attempt has a lease, heartbeat, retry policy, timeout, and output commit token.
      * **Task Dispatcher and Compute Broker**
        Maps runnable tasks to serverless compute, job clusters, existing clusters, SQL warehouses, pipeline tasks, Python scripts, notebooks, or external orchestration adapters. It is responsible for compute policy checks, cluster reuse, cold-start accounting, secrets injection, and cancellation propagation. Dispatch is at-least-once, so tasks must write outputs idempotently or commit through a transactional sink.
      * **Run State, Audit, and Observability Store**
        Persists run and task attempts, parameters, queue wait, source code version, compute target, terminal status, error code, lineage, and cost attribution tags. A compact hot store serves the UI and APIs; colder system tables support account-wide analytics over job history, cost, and failure patterns. Metrics include task duration, retries, queue wait, rows read/written, streaming backlog, and per-task logs.
    * **Explain the control flow**
      Users register workflows and schedules through UI, API, CLI, or declarative bundles. The trigger engine converts time, event, and manual triggers into versioned run records. The coordinator evaluates dependency readiness, checks concurrency limits, queues or skips runs according to policy, creates task attempts, and hands runnable tasks to the dispatcher. Workers renew leases while running, report progress, and transition attempts to terminal states. Repair runs reuse the original graph and parameters unless the operator explicitly selects a different repair scope.
    * **Explain the data flow**
      Task execution reads and writes Delta tables, streaming sources, notebooks, models, or external systems. Output commits use task attempt IDs or table transactions so retries do not publish duplicate data. Status events, logs, metrics, lineage, and output metadata flow back to run state and observability stores. Streaming tasks also emit backlog metrics so operators can distinguish slow compute from upstream source growth.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Scheduler HA: single leader vs active-active**
      A single scheduler is simple but a single point of failure. Leader election gives HA with simpler consistency, but failover can delay schedules. Active-active improves availability but risks duplicate scheduling without careful coordination. Prefer leader election for scheduling decisions and lease-based task execution for recovery.
    * **Duplicate execution**
      Preventing duplicates completely is hard under worker crashes and network partitions. Global locks reduce duplicates but hurt availability. Idempotent tasks plus attempt IDs, leases, and output commit protocols are more robust. Prefer idempotent pipeline tasks and transactional output commits.
    * **Queueing, concurrency, and fairness**
      Skipping runs when concurrency is exhausted is simple and protects the platform, but users can lose scheduled work. Unbounded queueing preserves work but creates stale backlogs and noisy-neighbor pressure. Per-job queues give local control, while workspace-level admission control protects shared compute. Prefer bounded queueing with max wait time, per-job max concurrent runs, workspace task limits, tenant quotas, and explicit skipped or timed-out outcomes when work is no longer useful.
    * **Repair and rerun semantics**
      Rerunning the full DAG is easy to explain but can be expensive and can duplicate side effects. Repairing only failed tasks saves time, but downstream tasks may depend on partial outputs. Prefer repair runs that reuse the original job version, parameters, and successful outputs by default, while forcing downstream reruns when failed tasks produced externally visible or non-transactional side effects.
    * **Observability: UI-first vs system-table-first**
      UI timelines and DAG views help the owner debug one run quickly. System tables and APIs are better for fleet-level cost, reliability, and SLA analysis. Prefer both: hot per-run views for debugging, plus durable account-wide run/task tables for dashboards, anomaly detection, and weekly reliability reviews.
    * **Backfills: integrated vs separate system**
      Treating backfills like normal runs reuses scheduler logic but can starve current workloads. A separate backfill system gives isolation but duplicates functionality. Prefer integrated backfills with priority, quotas, and catch-up controls.

## 9. Design Autoscaling Spark Clusters

* **Question**
  Design an autoscaling compute system for Spark batch, streaming, and interactive workloads.

* **Answer**
  * **Scope**
    Add and remove executors or nodes based on workload demand, while preserving correctness and controlling cost. Include job metrics, cluster manager integration, idle termination, and workload policies.
  * **Functional Requirements**
    Monitor pending tasks and resource usage, scale out when demand increases, scale in when idle, respect min/max limits, support instance pools, handle worker loss, and report cost/utilization.
  * **Non Functional Requirements**
    Fast scale-out, safe scale-in, cost efficiency, workload isolation, low disruption, predictable performance, and cloud-provider failure tolerance.
  * **High level design and diagram (at block level)**
    ```text
    Jobs / Queries
         |
         v
    Spark Driver
         |
         +--> Metrics Collector
         |
         v
    Autoscaler Policy Engine
         |
         v
    Cluster Manager / Cloud Provider
         |
         v
    Executors / Worker Nodes
    ```
    * **Explain the blocks**
      Spark driver exposes task backlog and stage state. Metrics collector gathers executor utilization, pending tasks, shuffle pressure, and streaming lag. Policy engine decides desired capacity. Cluster manager provisions or removes nodes. Executors run tasks and store cache/shuffle data.
    * **Explain the control flow**
      Admins define min/max size, node types, pools, idle timeouts, priorities, and scale policies. The autoscaler periodically computes desired capacity, requests nodes, drains or removes idle executors, and records decisions for debugging.
    * **Explain the data flow**
      Jobs send tasks to executors. Metrics about task backlog, CPU, memory, I/O, shuffle, and lag flow to the autoscaler. Scaling changes affect future task placement and resource availability.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Scale-out signal: CPU vs task backlog vs streaming lag**
      CPU utilization is easy to measure but misses blocked or queued work. Task backlog reflects Spark demand but can overreact to short stages. Streaming lag tracks freshness but not batch workloads. Prefer workload-specific policies combining backlog, utilization, and lag.
    * **Scale-in: fast vs conservative**
      Fast scale-in saves money but can lose cache, shuffle files, and task locality. Conservative scale-in stabilizes performance but wastes idle spend. Prefer graceful decommissioning, idle thresholds, and protection for executors with active shuffle or cached data.
    * **Cold starts: provision on demand vs warm pools**
      On-demand provisioning saves idle cost but increases startup latency. Warm pools reduce latency but cost more. Prefer pools for interactive/latency-sensitive workloads and on-demand nodes for batch elasticity.

## 10. Design Storage Compaction And File Optimization

* **Question**
  Design a background system that fixes small files and improves table layout for faster queries.

* **Answer**
  * **Scope**
    Optimize Delta/Parquet tables by compacting small files, clustering data, collecting stats, and scheduling maintenance safely with concurrent readers/writers. Exclude query engine internals except planning impact.
  * **Functional Requirements**
    Detect inefficient file layouts, select candidate partitions, rewrite files into target sizes, preserve table snapshots, support clustering/Z-ordering, collect metrics, and avoid disrupting active workloads.
  * **Non Functional Requirements**
    Improved query latency, bounded maintenance cost, safe concurrency, minimal ingestion impact, fairness across tables, and easy rollback through transaction history.
  * **High level design and diagram (at block level)**
    ```text
    Table Metrics Collector
          |
          v
    Optimization Planner
          |
          v
    Compaction / Clustering Workers
          |
          v
    Delta Commit Protocol
          |
          v
    Optimized Data Files + Updated Stats
    ```
    * **Explain the blocks**
      Metrics collector identifies small files, poor clustering, and scan inefficiency. Planner prioritizes tables and partitions. Workers rewrite data files. Delta commit protocol atomically removes old files and adds optimized files. Stats support future pruning.
    * **Explain the control flow**
      Owners configure target file size, clustering columns, maintenance windows, budgets, and priorities. The planner schedules maintenance, checks active workload signals, and commits rewritten files only if no conflicting table changes invalidate the optimization.
    * **Explain the data flow**
      Workers read many small files, repartition/sort/cluster records, write larger files, and publish a transaction-log commit that makes optimized files visible while old files remain available for existing snapshots until vacuum.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **When to compact: inline, scheduled, or adaptive**
      Inline compaction keeps files healthy immediately but slows ingestion. Scheduled compaction is efficient and predictable but allows temporary query degradation. Adaptive background compaction reacts to actual table health but is more complex. Prefer adaptive or scheduled background compaction for large platforms.
    * **Clustering strategy: partitioning vs Z-ordering vs liquid/adaptive clustering**
      Static partitions are simple and prune well when filters match, but high-cardinality partitions create small files. Z-ordering improves multi-column skipping but requires rewrite cost and chosen columns. Adaptive clustering can follow workload changes but needs telemetry and policy complexity. Prefer workload-driven clustering and avoid over-partitioning.
    * **Concurrency with writers**
      Rewriting files while ingestion runs can conflict with concurrent commits. Blocking writers preserves simplicity but hurts freshness. Optimistic conflict detection allows concurrency but may retry compaction. Prefer optimistic commits and partition-scoped optimization.

## 11. Design Exactly-Once Streaming Writes

* **Question**
  Design streaming writes that avoid losing or duplicating data when jobs fail and restart.

* **Answer**
  * **Scope**
    Provide effectively-once writes from a replayable source into Delta tables using checkpoints, state stores, and transactional commits. Include failure recovery. Exclude non-replayable sources unless discussed as a limitation.
  * **Functional Requirements**
    Track source offsets, process batches deterministically, maintain state, commit output atomically, resume after failure, deduplicate records when necessary, and expose progress.
  * **Non Functional Requirements**
    Correctness under crashes, bounded recovery time, manageable checkpoint size, low latency, and operational transparency.
  * **High level design and diagram (at block level)**
    ```text
    Replayable Source
          |
          v
    Streaming Engine
      Batch ID | Offsets | State
          |
          +--> Checkpoint Store
          |
          v
    Transactional Delta Sink
          |
          v
    Progress / Metrics
    ```
    * **Explain the blocks**
      Source provides offsets or sequence numbers. Streaming engine processes deterministic batches. Checkpoint store records offsets, state, and committed progress. Delta sink commits outputs using transaction IDs or batch IDs. Metrics expose lag and failure state.
    * **Explain the control flow**
      Job config defines checkpoint path, trigger interval, state TTL, and sink transaction settings. On restart, the engine loads checkpoint state, determines the next uncommitted batch, and replays source data as needed.
    * **Explain the data flow**
      Each batch reads a source offset range, transforms records, writes output files, commits them to the sink, and records progress. If the job crashes, replayed batches either produce the same committed output or are skipped by idempotent sink logic.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Failure between output write and offset checkpoint**
      Saving offsets first can lose data because the system thinks records are processed before output is durable. Writing output first can duplicate records after retry. Atomic sink transactions with batch IDs make replay safe. Prefer output commit protocols that detect duplicate batch commits.
    * **State store correctness**
      Keeping all state forever supports late events but grows unbounded. TTL and watermarks bound state but may drop very late corrections. Externalizing state can improve durability but adds latency. Prefer checkpointed state with business-defined watermark and recovery tests.
    * **External sinks**
      Delta can support transactional commits; many external APIs cannot. Synchronous calls to external sinks are simple but often only at-least-once. Idempotency keys and dedupe tables reduce duplicates but cannot force third-party exactly-once. State this limit clearly in the interview.

## 12. Design Late Event Handling For Streaming Aggregations

* **Question**
  Design a streaming aggregation system that correctly handles events arriving out of order or late.

* **Answer**
  * **Scope**
    Aggregate events by event time, handle out-of-order records, produce windowed outputs, and bound state size. Include watermarks and late-event policy. Exclude full fraud or sessionization models unless asked.
  * **Functional Requirements**
    Parse event timestamps, group by windows, update aggregates, accept late events within a configured delay, drop or quarantine too-late events, and expose lateness metrics.
  * **Non Functional Requirements**
    Bounded memory/state, predictable latency, acceptable correctness, recoverable state, and transparent data-quality reporting.
  * **High level design and diagram (at block level)**
    ```text
    Event Stream
         |
         v
    Timestamp Extractor
         |
         v
    Window Aggregator <--> State Store
         |
         +--> Watermark Manager
         |
         v
    Output Table
         |
         v
    Late Event Quarantine / Metrics
    ```
    * **Explain the blocks**
      Timestamp extractor identifies event time. Aggregator groups records by time windows and keys. State store holds open windows. Watermark manager decides when windows are finalized. Quarantine captures records too late to apply.
    * **Explain the control flow**
      Pipeline owners configure window size, allowed lateness, output mode, state TTL, and late-event handling. The system uses these policies to evict state and finalize outputs.
    * **Explain the data flow**
      Events update window state until the watermark passes the window. Outputs are emitted as complete or updated aggregates. Events beyond the allowed lateness threshold are dropped, counted, or written to a side table.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Watermark length**
      Short watermarks reduce memory and latency but drop legitimate late data. Long watermarks improve correctness but increase state, checkpoint size, and recovery time. Prefer a watermark based on observed lateness distribution and business tolerance.
    * **Output mode: append vs update vs complete**
      Append mode is efficient and final, but only works after windows close. Update mode supports changing aggregates but requires consumers to handle revisions. Complete mode is simple for small states but expensive. Prefer append for finalized windows and update mode when consumers need fresher partial aggregates.
    * **Too-late events**
      Dropping late events is cheap but hides data loss. Reopening windows improves correctness but creates downstream corrections. Quarantine preserves auditability and allows backfill. Prefer quarantine plus periodic correction workflows for high-value metrics.

## 13. Design A Multi-Tenant Lakehouse Platform

* **Question**
  Design a lakehouse platform that serves many customers or business units with shared infrastructure and strong isolation.

* **Answer**
  * **Scope**
    Support many tenants, workspaces, catalogs, compute clusters, storage locations, quotas, billing, and governance policies. Include noisy-neighbor controls. Exclude sales/account management workflows.
  * **Functional Requirements**
    Create tenant workspaces, isolate identities and data, enforce quotas, allocate compute, meter usage, manage secrets, support per-tenant policies, and provide admin visibility.
  * **Non Functional Requirements**
    Tenant isolation, security, fairness, high availability, cost attribution, blast-radius control, scalable onboarding, and compliance readiness.
  * **High level design and diagram (at block level)**
    ```text
    Tenant Admin / User
          |
          v
    Workspace API / Control Plane
          |
          +--> Identity / Policy / Catalog
          +--> Quota / Billing / Metering
          +--> Compute Manager
          |
          v
    Tenant Compute Plane
          |
          v
    Governed Storage / Network / Secrets
    ```
    * **Explain the blocks**
      Workspace API manages tenant operations. Identity/policy/catalog govern access. Quota and metering enforce fairness and cost attribution. Compute manager provisions clusters or warehouses. Storage, network, and secret boundaries protect data.
    * **Explain the control flow**
      Tenant onboarding creates workspace objects, identity bindings, storage credentials, quotas, and default policies. Control-plane changes are audited and propagated to compute runtimes. Quota enforcement and billing consume usage events from the data plane.
    * **Explain the data flow**
      User jobs run in tenant-scoped compute, access authorized catalog objects and storage paths, emit usage and audit events, and return results to the tenant workspace.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Isolation: shared compute vs dedicated compute vs hybrid**
      Shared compute improves utilization and lowers cost but risks noisy neighbors and weaker isolation. Dedicated compute improves security and predictability but costs more. Hybrid gives default efficiency and allows dedicated pools for sensitive workloads. Prefer hybrid with policy-driven placement.
    * **Quota enforcement**
      Hard quotas protect the platform but can block important work. Soft quotas with alerts preserve flexibility but may allow runaway spend. Priority-based quotas are powerful but complex. Prefer soft budgets plus hard guardrails for extreme limits and abuse.
    * **Blast radius**
      One global control plane is efficient but risky during outages. Per-region cells reduce blast radius but duplicate operations. Prefer cell-based architecture for large-scale platforms, with tenant placement and regional failover plans.

## 14. Design Query Admission Control

* **Question**
  Design admission control for SQL warehouses so expensive or excessive queries do not overload the platform.

* **Answer**
  * **Scope**
    Classify, queue, admit, throttle, reject, or cancel queries based on priority, estimated cost, user limits, and system health. Include fairness and operational controls.
  * **Functional Requirements**
    Enforce concurrency limits, prioritize workloads, estimate query cost, queue requests, cancel runaway queries, protect critical tenants, expose queue status, and integrate with billing/quota.
  * **Non Functional Requirements**
    Predictable latency, high cluster utilization, fairness, overload protection, low decision latency, and explainable rejections.
  * **High level design and diagram (at block level)**
    ```text
    SQL Request
        |
        v
    Admission Controller
      Auth | Priority | Cost Estimate | Quota
        |
        +--> Queue Manager
        +--> Warehouse Metrics
        +--> Policy Store
        |
        v
    Query Coordinator / Execution Engine
    ```
    * **Explain the blocks**
      Admission controller decides whether a query can run. Cost estimator uses plan, stats, and historical data. Queue manager orders waiting queries. Warehouse metrics report current load. Policy store contains tenant/user limits and priorities.
    * **Explain the control flow**
      Admins define warehouse concurrency, user priorities, timeouts, memory limits, and budget policies. Each submitted query is classified, costed, admitted, queued, or rejected. Runtime monitors can cancel queries that exceed limits.
    * **Explain the data flow**
      Query text and metadata enter the admission layer. Accepted queries flow to the coordinator. Execution metrics flow back to update load, improve estimates, and feed billing/quota systems.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Cost estimation**
      Rule-based estimates are fast and explainable but crude. Optimizer statistics improve accuracy but can be stale. ML/historical estimates can learn patterns but are harder to explain. Prefer a hybrid: static guardrails, optimizer estimates, and feedback from runtime history.
    * **Fairness policy**
      FIFO is simple but allows large queries to block small ones. Shortest-job-first improves latency but can starve large jobs. Weighted fair queuing balances tenants and priorities but adds complexity. Prefer weighted fair queuing with starvation prevention.
    * **Overload behavior**
      Queueing preserves work but increases user-visible latency. Rejection gives fast feedback but hurts user experience. Auto-scaling can absorb load but increases cost and has delay. Prefer queueing within SLO, auto-scale when justified, and reject with clear reason beyond limits.

## 15. Design Observability For Spark Jobs

* **Question**
  Design an observability platform that helps users debug and optimize Spark jobs and SQL queries.

* **Answer**
  * **Scope**
    Collect and surface runtime metrics, logs, traces, query plans, stage/task timelines, data quality signals, and automated diagnosis for Spark and SQL workloads.
  * **Functional Requirements**
    Capture driver/executor logs, Spark events, task metrics, shuffle metrics, spill, skew, GC, input/output rows, query plans, failures, alerts, and historical comparisons.
  * **Non Functional Requirements**
    Low overhead, scalable telemetry ingestion, searchable retention, high-cardinality control, privacy/security, and actionable UX.
  * **High level design and diagram (at block level)**
    ```text
    Drivers / Executors / SQL Warehouses
          |
          v
    Telemetry Agents / Event Listeners
          |
          v
    Metrics + Logs + Traces Pipeline
          |
          v
    Observability Store / Indexes
          |
          v
    UI / Alerts / Diagnosis Engine
    ```
    * **Explain the blocks**
      Agents and listeners collect runtime events. Telemetry pipeline normalizes and samples data. Store indexes metrics, logs, traces, and plans. UI and alerting help users diagnose latency, failures, skew, spills, and bad data.
    * **Explain the control flow**
      Operators configure retention, sampling, access control, alert thresholds, and diagnosis rules. Jobs attach run IDs and lineage IDs so telemetry can be correlated across systems.
    * **Explain the data flow**
      Runtime events stream from drivers and executors into telemetry pipelines. Aggregated metrics and indexed logs are stored for dashboards, alerts, root-cause analysis, and historical comparison.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Telemetry volume**
      Capturing every task event gives deep debugging but can be expensive at scale. Sampling lowers cost but may miss rare failures. Aggregation reduces volume but loses detail. Prefer tiered telemetry: always collect summaries, sample details, and retain full detail for failed or high-value jobs.
    * **Diagnosis depth**
      Infrastructure metrics are easy but often insufficient. Spark-level metrics identify skew/spill/shuffle issues. Plan-level analysis gives the most actionable recommendations but requires engine integration. Prefer combining all three and presenting concise root-cause hints.
    * **Privacy and access**
      Logs may contain sensitive data. Open access helps debugging but violates least privilege. Redaction and scoped access improve safety but add complexity. Prefer default redaction, tenant-scoped access, and audit logs for telemetry reads.

## 16. Design A Feature Store

* **Question**
  Design a feature store that supports reusable ML features for offline training and online inference.

* **Answer**
  * **Scope**
    Manage feature definitions, offline feature tables, online serving, freshness metadata, lineage, point-in-time joins, and feature discovery. Include batch and streaming features.
  * **Functional Requirements**
    Register features, compute features, store offline history, publish online values, perform point-in-time training joins, validate freshness, discover owners, and track lineage from source data to models.
  * **Non Functional Requirements**
    Training-serving consistency, low online latency, reproducibility, governance, freshness SLOs, high availability, and cost efficiency.
  * **High level design and diagram (at block level)**
    ```text
    Source Data
        |
        v
    Feature Pipelines
        |
        +--> Offline Feature Store / Delta
        |
        +--> Online Feature Store / KV
        |
        v
    Feature Registry / Lineage / Quality
        |
        +--> Training Jobs
        +--> Model Serving
    ```
    * **Explain the blocks**
      Feature pipelines compute reusable features. Offline store keeps historical feature values for training. Online store serves latest values to models. Registry stores definitions, owners, entities, freshness, and lineage.
    * **Explain the control flow**
      ML teams register feature definitions, entity keys, freshness requirements, validation rules, and publication policies. The platform schedules computation, validates quality, updates registry metadata, and manages access control.
    * **Explain the data flow**
      Source data is transformed into feature values. Historical values land in Delta for training. Fresh values are published to a low-latency online store for inference. Training jobs join labels to point-in-time-correct feature values.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Training-serving skew**
      Separate code paths for training and serving are flexible but often diverge. Shared feature definitions reduce skew but require platform discipline. Online-only features are fresh but hard to reproduce. Prefer shared definitions, offline history, online publication, and validation comparing offline/online values.
    * **Point-in-time correctness**
      Joining training labels to latest features leaks future information. Snapshot joins improve correctness but are expensive. Precomputed feature snapshots speed training but add storage. Prefer point-in-time joins using event timestamps and entity keys, optimized with partitioning and indexes.
    * **Online store choice**
      A managed KV store provides low latency but may lack analytical history. Serving from Delta is simpler but too slow for many real-time models. Hybrid offline Delta plus online KV is common and practical.

## 17. Design ML Training And Model Registry Platform

* **Question**
  Design a platform for reproducible ML training, experiment tracking, model registry, and deployment.

* **Answer**
  * **Scope**
    Support notebooks/jobs, distributed training, experiment tracking, artifact storage, model registry, approval workflows, and batch or online deployment. Exclude model architecture research.
  * **Functional Requirements**
    Launch training jobs, track parameters/metrics/code/data versions, store artifacts, register model versions, manage stages, approve deployments, rollback models, and audit access.
  * **Non Functional Requirements**
    Reproducibility, scalable compute, governance, cost control, artifact durability, lineage, and reliable deployment.
  * **High level design and diagram (at block level)**
    ```text
    Notebook / Job / CI
          |
          v
    Training Orchestrator -> Compute Cluster / GPU Pool
          |
          v
    Experiment Tracker -> Artifact Store
          |
          v
    Model Registry -> Approval / Deployment
          |
          +--> Batch Scoring
          +--> Online Serving
    ```
    * **Explain the blocks**
      Orchestrator starts training on CPU/GPU clusters. Experiment tracker records metadata. Artifact store keeps model files and environment snapshots. Registry manages model versions and lifecycle stages. Deployment sends models to batch jobs or serving endpoints.
    * **Explain the control flow**
      Users define training jobs, environments, resource limits, and experiment metadata. Promotion workflows require validation and approval before deployment. Registry state changes are audited and can trigger deployment or rollback.
    * **Explain the data flow**
      Training reads datasets/features, produces metrics and model artifacts, stores them durably, registers a version, and deploys approved versions to scoring paths. Predictions and model metrics flow back into monitoring.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Reproducibility**
      Tracking only code and parameters is insufficient if data changes. Snapshotting data versions improves reproducibility but increases storage and metadata needs. Containerizing environments helps dependency consistency but adds build complexity. Prefer tracking code commit, environment, parameters, feature/table versions, and artifacts.
    * **Batch vs online serving**
      Batch scoring is cheaper and simpler but stale. Online serving gives fresh predictions and interactive UX but needs low-latency infrastructure, autoscaling, and monitoring. Prefer batch for periodic decisions and online serving for request-time decisions.
    * **Model promotion**
      Manual approval is safe but slow. Fully automated promotion is fast but risky. Policy-based promotion with metrics gates balances speed and safety. Prefer automated validation plus human approval for high-impact models.

## 18. Design Vector Search And RAG On A Lakehouse

* **Question**
  Design a vector search and retrieval-augmented generation platform over enterprise data.

* **Answer**
  * **Scope**
    Ingest documents and structured records, chunk content, generate embeddings, build vector indexes, enforce permissions, retrieve context, and serve RAG applications. Exclude LLM training.
  * **Functional Requirements**
    Connect to data sources, chunk and normalize documents, embed text, build ANN indexes, filter by metadata and permissions, refresh indexes, serve low-latency retrieval, and track source lineage.
  * **Non Functional Requirements**
    Retrieval relevance, low query latency, scalable indexing, freshness, access-control correctness, cost-efficient embedding, and observability.
  * **High level design and diagram (at block level)**
    ```text
    Documents / Tables
          |
          v
    Ingestion + Chunking
          |
          v
    Embedding Jobs
          |
          +--> Metadata / Permissions / Lineage
          |
          v
    Vector Index Service
          |
          v
    RAG Retrieval API -> LLM Application
    ```
    * **Explain the blocks**
      Ingestion reads documents and tables. Chunking creates retrievable units. Embedding jobs convert chunks into vectors. Metadata stores source IDs, versions, and permissions. Vector index supports ANN search. Retrieval API combines semantic search, filters, and ranking.
    * **Explain the control flow**
      Admins configure sources, embedding model, chunking strategy, index refresh cadence, permissions, retention, and quality evaluation. Index builds are versioned and rolled out safely.
    * **Explain the data flow**
      Source content is chunked, embedded, indexed, and made available for retrieval. At query time, the API embeds the query, searches the index with permission filters, ranks chunks, and returns context to the LLM application.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Index freshness: full rebuild vs incremental update**
      Full rebuilds are simple and produce clean indexes but are expensive and stale between runs. Incremental updates are fresh and efficient but complicate deletion, versioning, and index quality. Prefer incremental updates with periodic full rebuild or compaction.
    * **ANN accuracy vs latency**
      Exact search maximizes recall but is too slow at large scale. Approximate indexes improve latency and cost but may miss relevant chunks. Prefer ANN with evaluation-driven recall targets and reranking for top candidates.
    * **Security filtering**
      Filtering after retrieval is easy but can leak timing or reduce recall because inaccessible chunks occupy top slots. Filtering inside the index/query is safer but more complex. Prefer permission-aware retrieval or pre-partitioned indexes for sensitive corporate data.

## 19. Design Cross-Region Replication And Disaster Recovery

* **Question**
  Design replication and disaster recovery for lakehouse data, metadata, and workloads across regions.

* **Answer**
  * **Scope**
    Replicate table data, transaction logs, catalog metadata, secrets references, job definitions, and selected serving endpoints across regions. Include RPO/RTO and failover. Exclude active-active write conflict resolution unless asked.
  * **Functional Requirements**
    Copy committed table versions, validate replicated files, replicate catalog objects and grants, support failover, test recovery, expose lag, and restore workloads in a secondary region.
  * **Non Functional Requirements**
    Clear RPO/RTO, durability, correctness, cost control, security, operational simplicity, and low blast radius.
  * **High level design and diagram (at block level)**
    ```text
    Primary Region
      Delta Tables | Catalog | Jobs
          |
          v
    Replication Controller
      Version Tracking | Validation | Lag Metrics
          |
          v
    Secondary Region
      Delta Tables | Catalog Replica | Standby Jobs
    ```
    * **Explain the blocks**
      Primary region receives writes. Replication controller tracks committed table versions and metadata changes. Validation checks file existence and checksums. Secondary region stores replicas and standby configs. Lag metrics expose recovery readiness.
    * **Explain the control flow**
      Operators define replication scope, schedule, priority, encryption, failover criteria, and recovery runbooks. The controller copies changes in version order, marks safe recovery points, and can promote secondary systems during disaster.
    * **Explain the data flow**
      Table log entries and referenced data files are copied from primary to secondary. Catalog metadata and job configs are synchronized. During failover, reads and selected writes move to the secondary after a validated recovery point.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Synchronous vs asynchronous replication**
      Synchronous replication minimizes data loss but increases write latency and reduces availability under regional issues. Asynchronous replication is cheaper and faster for writes but can lose recent commits. Prefer async for most analytical data with explicit RPO; use stronger replication only for critical metadata or regulatory data.
    * **Replicating data and log consistency**
      Copying log entries before data files can create snapshots that point to missing files. Copying files first may copy uncommitted data. Prefer version-aware replication: copy data files, validate them, then publish the corresponding log version in secondary.
    * **Failover mode: read-only vs read-write**
      Read-only failover is simpler and avoids split-brain but limits business continuity. Read-write failover restores service but creates reconciliation challenges when primary returns. Prefer read-only or controlled write failover unless the business requires active writes.

## 20. Design Secure Data Sharing Across Organizations

* **Question**
  Design a secure system that lets one organization share lakehouse datasets with another organization.

* **Answer**
  * **Scope**
    Allow providers to share tables, views, or snapshots with recipients using governed access. Include identity, authorization, auditing, revocation, and freshness. Exclude public open-data portals unless asked.
  * **Functional Requirements**
    Create shares, grant recipients access, enforce row/column filters, support expiration and revocation, provide audit logs, allow versioned snapshots, and minimize unnecessary copying.
  * **Non Functional Requirements**
    Security, governance, interoperability, low copy cost, availability, privacy, and compliance evidence.
  * **High level design and diagram (at block level)**
    ```text
    Provider Admin
          |
          v
    Sharing Control Plane
      Share Definitions | Grants | Policies
          |
          +--> Catalog / Policy Engine
          +--> Audit Pipeline
          |
          v
    Sharing Service / Protocol Endpoint
          |
          v
    Recipient Client / Workspace
          |
          v
    Provider Storage Or Snapshot Export
    ```
    * **Explain the blocks**
      Sharing control plane manages shares and recipients. Catalog and policy engine enforce what is exposed. Sharing service serves metadata and data access. Audit pipeline records access. Recipient clients query shared data or consume exported snapshots.
    * **Explain the control flow**
      Provider creates a share, selects objects, applies filters and expiration, grants a recipient, and monitors access. Revocation updates policies, invalidates credentials, and emits audit events.
    * **Explain the data flow**
      Recipient requests shared metadata and data. The service verifies identity and policy, then grants controlled access to provider data or a snapshot copy. Read events flow into provider audit logs.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Zero-copy vs copied sharing**
      Zero-copy sharing is fresh, cheap, and governed, but recipients depend on provider availability and protocol compatibility. Copied sharing gives recipient independence and stable snapshots, but creates stale data and extra storage. Prefer zero-copy for governed analytics and snapshot export for contractual deliveries or disconnected environments.
    * **Revocation**
      Immediate revocation is easier with zero-copy access but still must handle cached credentials and downloaded data. Copied data cannot be truly revoked after delivery. Prefer short-lived credentials, audit logs, and contractual controls for exported data.
    * **Policy granularity**
      Sharing whole tables is simple but may overexpose data. Views and row/column filters reduce exposure but add planning and performance overhead. Prefer least-privilege shares with filtered views or native policies for sensitive data.

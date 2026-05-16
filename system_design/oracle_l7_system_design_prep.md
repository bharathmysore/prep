# Oracle L7 System Design Interview Prep

This guide uses Oracle/OCI-style prompts that are representative of cloud, database, infrastructure, and distributed-systems interviews. It is not an official Oracle question bank.

The file is intentionally count-neutral: add, remove, merge, or reorder questions as public signal and user focus change.

Reference context:
- https://www.designgurus.io/answers/detail/what-are-the-top-system-design-interview-questions-for-oracle-interview
- https://www.tryexponent.com/questions?company=oracle&role=swe&type=system-design
- https://docs.oracle.com/en-us/iaas/Content/GSG/Concepts/concepts-physical.htm
- https://docs.oracle.com/en-us/iaas/Content/Object/Concepts/objectstorageoverview.htm
- https://docs.oracle.com/en-us/iaas/Content/Monitoring/Concepts/monitoringoverview.htm
- https://docs.oracle.com/en-us/iaas/Content/Block/Concepts/overview.htm
- https://docs.oracle.com/en-us/iaas/Content/Streaming/Concepts/streamingoverview.htm

Use each answer as a 45-minute interview outline. At L7, spend less time naming products and more time explaining boundaries, failure modes, tenant isolation, rollout strategy, cost, and operational ownership.

## 1. Design OCI Object Storage

* **Question**
  Design a highly durable cloud object storage service like OCI Object Storage or S3.

* **Answer**
  * **Scope**
    Store and retrieve immutable objects inside buckets. Support large files, metadata, lifecycle policies, replication, IAM, and high durability.
  * **Functional Requirements**
    Create and delete buckets, upload and download objects, list by prefix, support multipart upload, store metadata and checksums, support versioning, retention, lifecycle rules, and cross-region replication.
  * **Non Functional Requirements**
    Very high durability, high availability, strong tenant isolation, secure authorization, low cost per GB, horizontal scalability, and predictable behavior during node, rack, or availability-domain failures.
  * **High level design and diagram (at block level)**
    ```text
    Client -> API Gateway/Auth -> Object API Service
                                  |        |        |
                                  v        v        v
                           Metadata   Placement   Events
                             Store      Service      |
                                        |            v
                                        v      Lifecycle/Replication
                                  Storage Nodes
    ```
    * **Explain the blocks**
      API Gateway validates identity, tenancy, throttles, and request shape. Object API Service handles bucket/object operations. Metadata Store tracks bucket, object, version, checksum, retention, and location metadata. Placement Service chooses storage nodes across failure domains. Storage Nodes persist chunks. Lifecycle and Replication workers expire, archive, or copy objects.
    * **Explain the control flow**
      Bucket creation, IAM policy, retention, lifecycle, replication, encryption, and quota settings go through the control plane. These settings are validated, versioned, audited, and cached by the API fleet.
    * **Explain the data flow**
      Upload authenticates, writes an intent to metadata, streams object chunks to selected storage nodes, verifies checksums, replicates or erasure-codes data, commits metadata, and emits object-created events. Download resolves metadata, selects healthy chunks or replicas, streams data back, and records access metrics.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Durability: replication vs erasure coding**
      The hard problem is storing data cheaply while surviving many failures.

      | Option | Pros | Cons |
      |---|---|---|
      | Full replication | Simple reads, fast repair, easy reasoning | High storage cost |
      | Erasure coding | Lower storage overhead at scale | More complex reads, repair, and degraded-mode behavior |
      | Hybrid | Balances hot-object performance and cold-object cost | More lifecycle and operational complexity |

      Prefer replication for new or hot objects and erasure coding for colder large objects once access patterns stabilize.

## 2. Design A Distributed Database

* **Question**
  Design a distributed relational or key-value database for enterprise workloads.

* **Answer**
  * **Scope**
    Support partitioned data, replication, transactions, failover, backup, and query routing. Focus on the storage and serving architecture, not SQL parser internals.
  * **Functional Requirements**
    Create tables or collections, read and write by key or query, support transactions, maintain indexes, rebalance shards, backup and restore data, and handle node failure.
  * **Non Functional Requirements**
    High availability, strong consistency for critical data, horizontal scalability, predictable latency, operational observability, and safe online maintenance.
  * **High level design and diagram (at block level)**
    ```text
    Client -> SQL/API Router -> Query Planner/Coordinator -> Shard Map
                                                           |
                                     +---------------------+---------------------+
                                     v                     v                     v
                                  Shard A               Shard B               Shard C
                                Leader/Replicas       Leader/Replicas       Leader/Replicas
    ```
    * **Explain the blocks**
      Router accepts requests and finds target shards. Planner and Coordinator execute distributed queries and transactions. Shard Map tracks key ranges or hash partitions. Leaders handle writes. Replicas serve reads and provide failover. WAL and consensus preserve ordering and durability.
    * **Explain the control flow**
      Schema creation, index creation, partitioning, replica placement, backup policies, failover, and rebalancing are managed by the control plane. The control plane updates shard maps and rolls them out safely to routers.
    * **Explain the data flow**
      A query enters the router. Point reads route to one shard. Writes go to the shard leader, append to WAL, replicate to quorum, apply to storage, and acknowledge. Distributed transactions coordinate prepare/commit across involved shards.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Consistency during failures**
      The hard problem is balancing correctness, latency, and availability under partitions.

      | Option | Pros | Cons |
      |---|---|---|
      | Strong quorum consistency | Clear correctness, simpler app logic | Higher latency and reduced availability during partitions |
      | Eventual consistency | Low latency and high availability | Stale reads and conflict resolution burden |
      | Tunable consistency | Lets workloads choose | More complex API, testing, and operations |

      Prefer strong consistency for metadata, financial, and enterprise control-plane data. Offer weaker or tunable consistency for read-heavy, non-critical workloads.

## 3. Design Cloud Block Storage

* **Question**
  Design a cloud block volume service used by compute instances.

* **Answer**
  * **Scope**
    Provide attachable virtual disks with low latency, snapshots, encryption, replication, resizing, and durable writes.
  * **Functional Requirements**
    Create, delete, resize, attach, detach, read, write, snapshot, clone, restore, encrypt, and expose volume health metrics.
  * **Non Functional Requirements**
    Low latency, high IOPS, durable writes, fault-domain resilience, secure tenant isolation, fast recovery, and predictable performance tiers.
  * **High level design and diagram (at block level)**
    ```text
    Compute VM -> Block Device Driver -> Volume Frontend
                                             |
                                             v
                                    Volume Metadata
                                             |
                                             v
                                    Extent Mapping
                                             |
                                             v
                                    Storage Extent Nodes
                                             |
                                             v
                                    Snapshot/Replication
    ```
    * **Explain the blocks**
      The device driver exposes the virtual disk to the VM. Volume Frontend handles reads and writes. Metadata tracks ownership, attachment, encryption, and lifecycle. Extent Mapping maps logical blocks to physical extents. Extent Nodes store replicated block data. Snapshot service uses copy-on-write or redirect-on-write metadata.
    * **Explain the control flow**
      User creates a volume with size, performance, and encryption settings. The control plane allocates extents, records metadata, validates attach permissions, updates compute-device mapping, and manages snapshot schedules.
    * **Explain the data flow**
      VM writes a block through the driver. The frontend resolves extent mapping, sends the write to storage replicas, waits for quorum, updates metadata if needed, and acknowledges. Reads fetch from a healthy nearby replica.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Replication strategy**
      The hard problem is preserving data without adding too much write latency.

      | Option | Pros | Cons |
      |---|---|---|
      | Synchronous replication | Strong durability and clean failover | Higher write latency |
      | Asynchronous replication | Lower write latency | Recent writes can be lost during failure |
      | Quorum replication | Good balance of durability and availability | More complex repair and split-brain handling |

      Prefer synchronous quorum inside a region or availability domain and asynchronous cross-region replication for disaster recovery.

## 4. Design A Multi-Tenant SaaS Platform

* **Question**
  Design a multi-tenant enterprise SaaS application.

* **Answer**
  * **Scope**
    Support many enterprise tenants with isolation, custom configuration, quotas, audit trails, secure access, and billing.
  * **Functional Requirements**
    Onboard tenants, manage users and roles, store tenant-specific config, execute business APIs, emit audit logs, and meter usage.
  * **Non Functional Requirements**
    Strong tenant isolation, compliance, scalability, availability, predictable performance, cost efficiency, and safe data deletion/export.
  * **High level design and diagram (at block level)**
    ```text
    Tenant Users -> API Gateway -> AuthN/AuthZ -> Tenant Router
                                                   |
                                                   v
                                           Application Services
                                                   |
                                                   v
                                         Tenant-Aware Data Layer
                                                   |
                                       Shared/Dedicated Tenant Stores
    ```
    * **Explain the blocks**
      Gateway handles edge routing and throttling. Auth services validate identity and roles. Tenant Router injects tenant context. Application Services run business logic. Tenant-Aware Data Layer enforces tenant predicates and isolation. Audit and Billing pipelines record activity.
    * **Explain the control flow**
      Tenant admins configure users, roles, features, quotas, data residency, and integrations. Control-plane config is validated, versioned, and pushed to runtime services.
    * **Explain the data flow**
      Request carries tenant identity. Gateway authenticates, services execute with tenant context, data layer enforces tenant scope on every read/write, and audit/usage events are emitted asynchronously.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Shared vs dedicated tenancy**
      The hard problem is balancing isolation and cost.

      | Option | Pros | Cons |
      |---|---|---|
      | Shared database and tables | Lowest cost, easiest fleet management | Higher blast radius, noisy-neighbor risk |
      | Database per tenant | Strong isolation, easier export and restore | Expensive and operationally heavy |
      | Hybrid | Supports both small and large tenants | More routing, migration, and support complexity |

      Prefer hybrid: shared infrastructure for small tenants, dedicated or isolated cells for large or regulated tenants.

## 5. Design OCI IAM / Authorization

* **Question**
  Design a cloud identity and access management system.

* **Answer**
  * **Scope**
    Authorize users, services, groups, dynamic principals, and resources across tenancies, compartments, and regions.
  * **Functional Requirements**
    Manage users, groups, policies, roles, service principals, temporary credentials, resource-level permissions, and audit logs.
  * **Non Functional Requirements**
    Very high availability, low authorization latency, strong security, auditability, global consistency expectations, and safe revocation.
  * **High level design and diagram (at block level)**
    ```text
    Client/Service -> Identity Provider -> Token Service
                                              |
                                              v
                                    Policy Evaluation Service
                                      |        |        |
                                      v        v        v
                                  Policy   Resource   Audit
                                  Store      Store      Log
    ```
    * **Explain the blocks**
      Identity Provider authenticates users and services. Token Service issues signed credentials. Policy Evaluator answers authorization questions. Policy Store stores rules. Resource Store stores tenancy, compartment, and resource hierarchy. Audit Log records decisions and admin changes.
    * **Explain the control flow**
      Admins create users, groups, compartments, dynamic groups, and policies. Policy updates are validated, versioned, audited, and propagated to regional evaluator caches.
    * **Explain the data flow**
      A request carries a token. Target service validates token and asks the evaluator whether subject, action, resource, and context are allowed. Decision is returned and logged.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Centralized vs cached authorization**
      The hard problem is enforcing changes quickly without making every service depend on a remote call.

      | Option | Pros | Cons |
      |---|---|---|
      | Central evaluator | Consistent and auditable | Latency and availability dependency |
      | Local cached policies | Fast and resilient | Stale permission risk |
      | Hybrid | Good latency with bounded staleness | Invalidation and emergency revoke complexity |

      Prefer regional evaluators with short-lived caches and a high-priority revocation channel for sensitive permissions.

## 6. Design Cloud Monitoring And Alarming

* **Question**
  Design a cloud monitoring service for metrics, dashboards, and alarms.

* **Answer**
  * **Scope**
    Collect, store, query, visualize, and alert on metrics from cloud services and customer workloads.
  * **Functional Requirements**
    Ingest metrics, query time-series data, create dashboards, define alarms, evaluate alarm windows, and send notifications.
  * **Non Functional Requirements**
    High ingestion throughput, low query latency, reliable alarm evaluation, multi-tenant isolation, high availability, and cost-efficient retention.
  * **High level design and diagram (at block level)**
    ```text
    Agents/Services -> Metrics Ingestion API -> Stream/Buffer -> Time-Series Storage
                                                                    |
                                                    +---------------+---------------+
                                                    v                               v
                                             Query Engine                    Alarm Engine
                                                    |                               |
                                                    v                               v
                                               Dashboards                  Notification Service
    ```
    * **Explain the blocks**
      Agents emit metrics. Ingestion API authenticates and validates dimensions. Stream buffers bursts. Time-Series Store compresses and partitions data. Query Engine serves dashboards. Alarm Engine evaluates rules and sends notifications.
    * **Explain the control flow**
      Users define namespaces, dimensions, dashboards, alarm expressions, thresholds, evaluation windows, and destinations. Config is stored, versioned, and distributed to alarm evaluators.
    * **Explain the data flow**
      Metrics arrive, are validated, partitioned by tenant/namespace/metric/time, stored, queried, and evaluated against alarm rules. State changes emit notification events.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **High-cardinality metrics**
      The hard problem is giving rich debugging without exploding storage and query cost.

      | Option | Pros | Cons |
      |---|---|---|
      | Allow all dimensions | Maximum flexibility | Unbounded cost and slow queries |
      | Strict dimension limits | Predictable cost | Less diagnostic power |
      | Tiered cardinality | Balances cost and depth | More product and billing complexity |

      Prefer per-tenant cardinality quotas, reserved dimensions, sampling for very high-cardinality streams, and premium high-cardinality tiers.

## 7. Design Centralized Logging

* **Question**
  Design a large-scale cloud logging platform.

* **Answer**
  * **Scope**
    Collect logs from services and customer workloads, index them, search them, retain them, and archive them.
  * **Functional Requirements**
    Ingest logs, parse structured fields, search by time/service/tenant/severity, configure retention, export/archive logs, and enforce access control.
  * **Non Functional Requirements**
    High write throughput, searchable within seconds, durable ingestion, cost-efficient storage, tenant isolation, and privacy-aware redaction.
  * **High level design and diagram (at block level)**
    ```text
    Log Agents -> Log Ingestion API -> Durable Stream -> Parser/Enricher
                                                            |
                                             +--------------+--------------+
                                             v                             v
                                        Search Index                Object Archive
                                             |
                                             v
                                       Query API / UI
    ```
    * **Explain the blocks**
      Agents collect logs. Ingestion API authenticates and validates. Durable Stream absorbs bursts. Parser extracts fields and redacts sensitive data. Search Index powers fast queries. Object Archive stores raw logs cheaply.
    * **Explain the control flow**
      Admins configure log sources, parsing rules, retention, redaction, indexes, and access policies. Config is pushed to agents and ingestion services.
    * **Explain the data flow**
      Logs flow from agents to ingestion, then through durable streams, parsing, enrichment, indexing, and archival. Queries hit the index for hot data and archive for cold data.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Index everything vs selective indexing**
      The hard problem is balancing search power and cost.

      | Option | Pros | Cons |
      |---|---|---|
      | Index everything | Best search experience | Very expensive at scale |
      | Selective indexing | Lower cost | Limited ad hoc search |
      | Hot/cold tiering | Strong balance | More complex query path |

      Prefer indexing recent logs and key fields, archiving raw logs, and offering rehydration or async search for cold data.

## 8. Design Notification / Alert Delivery

* **Question**
  Design a reliable notification system for alarms, operational events, and enterprise alerts.

* **Answer**
  * **Scope**
    Deliver notifications over email, SMS, webhook, Slack, PagerDuty, and in-app channels. Focus on reliability, preferences, dedupe, and retries.
  * **Functional Requirements**
    Create topics and subscriptions, send notifications, retry failures, deduplicate, respect preferences, track delivery status, and support templates.
  * **Non Functional Requirements**
    High availability, at-least-once delivery, low latency for critical alerts, provider fault tolerance, rate limiting, auditability, and tenant isolation.
  * **High level design and diagram (at block level)**
    ```text
    Producer Services -> Notification API -> Dedup/Policy/Preference -> Durable Queue
                                                                           |
                                                                           v
                                                                    Channel Workers
                                                                  /    |     |     \
                                                              Email   SMS Webhook PagerDuty
                                                                           |
                                                                           v
                                                                 Delivery Status Store
    ```
    * **Explain the blocks**
      Notification API accepts messages. Dedup prevents repeated sends. Policy and Preference services select channels and suppressions. Durable Queue persists work. Channel Workers send to providers. Status Store records attempts and outcomes.
    * **Explain the control flow**
      Users configure topics, subscriptions, endpoints, severity routing, quiet hours, retry policies, templates, and provider credentials.
    * **Explain the data flow**
      Producer emits event. System validates, dedupes, applies preferences, enqueues per channel, workers send to providers, and provider responses update delivery status.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Delivery semantics**
      The hard problem is making notifications reliable without promising impossible external guarantees.

      | Option | Pros | Cons |
      |---|---|---|
      | At-most-once | Avoids duplicates | Can lose alerts |
      | At-least-once | Reliable under retry | Possible duplicates |
      | Exactly-once | Appealing product contract | Usually impossible end to end with third-party providers |

      Prefer at-least-once internally with idempotency keys, dedupe windows, retry budgets, and honest external contracts.

## 9. Design Distributed Rate Limiter

* **Question**
  Design a rate limiter for cloud APIs.

* **Answer**
  * **Scope**
    Limit requests per user, tenant, API, resource, region, and global account.
  * **Functional Requirements**
    Define limits, enforce limits at gateway/service level, support burst behavior, return retry-after, expose usage, and allow admin overrides.
  * **Non Functional Requirements**
    Very low latency, high availability, eventual global accuracy, horizontal scale, abuse resistance, and safe degradation.
  * **High level design and diagram (at block level)**
    ```text
    Client -> API Gateway -> Local Rate Limiter -> Regional Counter Store
                                                        |
                                                        v
                                                Global Quota Reconciler
                                                        |
                                                        v
                                                Config / Policy Store
    ```
    * **Explain the blocks**
      Gateway intercepts requests. Local Limiter makes fast token decisions. Regional Counter Store tracks usage. Global Reconciler syncs usage across regions. Policy Store contains quota rules.
    * **Explain the control flow**
      Admins configure tenant, API, and service quotas. Config is versioned and propagated to gateways and limiter instances.
    * **Explain the data flow**
      Each request checks local tokens. If allowed, usage counters increment and the request proceeds. Regional usage is periodically reconciled into global quota state.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Local vs global enforcement**
      The hard problem is enforcing global quotas without adding a global dependency to every request.

      | Option | Pros | Cons |
      |---|---|---|
      | Central global limiter | Accurate enforcement | Latency, availability, and bottleneck risk |
      | Local regional limiter | Fast and resilient | Temporary quota overshoot |
      | Hybrid | Practical balance | Reconciliation and edge cases |

      Prefer local fast-path enforcement with global reconciliation. Use synchronous global checks only for expensive or sensitive operations.

## 10. Design Cloud API Gateway

* **Question**
  Design an API gateway for cloud services.

* **Answer**
  * **Scope**
    Expose APIs securely with routing, authentication, authorization hooks, throttling, observability, request validation, and versioning.
  * **Functional Requirements**
    Route requests, authenticate, authorize, rate limit, transform headers, collect metrics/logs, support versions, and roll out route config safely.
  * **Non Functional Requirements**
    Low latency, high availability, horizontal scale, secure edge posture, safe config rollout, and clear blast-radius boundaries.
  * **High level design and diagram (at block level)**
    ```text
    Client -> Edge Load Balancer -> API Gateway Data Plane -> Service Router -> Backends

    Control Plane:
    Admin API -> Route/Policy Store -> Config Distributor -> Gateway Fleet
    ```
    * **Explain the blocks**
      Edge Load Balancer distributes traffic. Gateway Data Plane performs hot-path checks. Service Router maps path, method, version, and tenant to backends. Control Plane manages routes and policies. Config Distributor rolls out validated snapshots.
    * **Explain the control flow**
      Admins create routes, auth policies, throttles, transformations, and backend mappings. Config is validated, versioned, canaried, and pushed to gateways.
    * **Explain the data flow**
      Client request reaches gateway, TLS terminates, auth and rate-limit checks run, request routes to backend, response returns, and metrics/logs/traces are emitted.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Thin gateway vs rich gateway**
      The hard problem is deciding which cross-cutting behavior belongs at the edge.

      | Option | Pros | Cons |
      |---|---|---|
      | Thin gateway | Simple, fast, easier to operate | Duplicated service logic |
      | Rich gateway | Centralized policy and consistency | Complex bottleneck and broad blast radius |
      | Plugin model | Flexible for teams | Security and operational complexity |

      Prefer centralizing routing, auth hooks, throttling, and observability while keeping domain logic in services.

## 11. Design Compute Resource Scheduler

* **Question**
  Design a scheduler for placing VMs or bare metal instances.

* **Answer**
  * **Scope**
    Allocate compute capacity across hosts, racks, fault domains, availability domains, and capacity pools.
  * **Functional Requirements**
    Create instances, select shapes, reserve capacity, enforce quota, support placement constraints, track host inventory, and handle host failures.
  * **Non Functional Requirements**
    High placement success, fast scheduling, efficient utilization, failure-domain awareness, fairness across tenants, and safe maintenance.
  * **High level design and diagram (at block level)**
    ```text
    Create VM Request -> Compute API -> Quota/Policy Check -> Scheduler
                                                                  |
                                               +------------------+------------------+
                                               v                                     v
                                      Inventory Service                     Placement Engine
                                               |                                     |
                                               v                                     v
                                           Host Agents                    Capacity Reservations
    ```
    * **Explain the blocks**
      Compute API receives requests. Quota checks validate tenant entitlement. Inventory Service tracks host resources and health. Scheduler filters and ranks candidates. Placement Engine reserves capacity. Host Agents launch instances.
    * **Explain the control flow**
      Operators define shapes, capacity pools, maintenance windows, placement policies, fault domains, and evacuation rules. Control loops update inventory and host health.
    * **Explain the data flow**
      Create request enters Compute API. Scheduler finds a host, reserves capacity, instructs host agent to provision the VM, and streams status back to the user.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Packing vs spreading workloads**
      The hard problem is optimizing utilization without increasing correlated failure impact.

      | Option | Pros | Cons |
      |---|---|---|
      | Bin packing | High utilization and lower cost | Higher correlated failure risk |
      | Spreading | Better availability and thermal/power distribution | Capacity fragmentation |
      | Policy-based hybrid | Flexible by workload class | More scheduler complexity |

      Prefer spreading for HA and customer-facing workloads, packing for lower-priority or batch workloads.

## 12. Design Managed Container Service

* **Question**
  Design a managed Kubernetes-like container service.

* **Answer**
  * **Scope**
    Provision clusters, manage control planes, nodes, upgrades, networking, autoscaling, identity, and observability.
  * **Functional Requirements**
    Create and delete clusters, add node pools, deploy workloads, autoscale, upgrade clusters, integrate with load balancers, and expose cluster telemetry.
  * **Non Functional Requirements**
    Reliable control plane, secure tenant isolation, fast provisioning, safe upgrades, regional availability, and clear customer/operator responsibility boundaries.
  * **High level design and diagram (at block level)**
    ```text
    User/CLI -> Container Service API -> Cluster Control Plane Manager
                                           |
                                           v
                                    Node Pool Manager
                                      |      |      |
                                      v      v      v
                                  Compute  Network IAM
                                      |
                                      v
                                  Worker Nodes
    ```
    * **Explain the blocks**
      Service API manages clusters. Control Plane Manager provisions and monitors API servers and controllers. Node Pool Manager manages workers. Cloud APIs create compute, networking, IAM bindings, and load balancers. Worker Nodes run customer workloads.
    * **Explain the control flow**
      User configures cluster version, node pools, network, IAM, and upgrade policy. Control plane reconciles desired state, provisions resources, and rolls upgrades.
    * **Explain the data flow**
      Application traffic enters load balancer, reaches worker nodes, and routes to containers. Cluster telemetry flows to monitoring and logging.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Safe upgrades**
      The hard problem is upgrading shared infrastructure without breaking customer workloads.

      | Option | Pros | Cons |
      |---|---|---|
      | In-place upgrade | Simple and cheap | Higher downtime and rollback risk |
      | Rolling upgrade | Safer, incremental | Slower and needs surge capacity |
      | Blue/green cluster | Very safe and rollbackable | Expensive and migration-heavy |

      Prefer rolling control-plane and node-pool upgrades with compatibility checks, surge nodes, health gates, and rollback.

## 13. Design Multi-Region Active-Active Service

* **Question**
  Design a service that runs actively in multiple regions.

* **Answer**
  * **Scope**
    Serve users globally with regional failover, low latency, data replication, and operational controls.
  * **Functional Requirements**
    Route users to nearby healthy regions, support regional failover, replicate data, handle conflicts, expose health, and maintain auditability.
  * **Non Functional Requirements**
    High availability, low latency, disaster recovery, bounded data loss, predictable consistency, and operational simplicity.
  * **High level design and diagram (at block level)**
    ```text
                         Global Traffic Manager
                                   |
                    +--------------+--------------+
                    v                             v
                 Region A                      Region B
              API/Workers/DB                 API/Workers/DB
                    |                             |
                    +-------- Replication --------+
    ```
    * **Explain the blocks**
      Global Traffic Manager routes by health, geography, and policy. Regional APIs serve local traffic. Regional DBs and queues store local state. Replication syncs state across regions. Health systems drive failover.
    * **Explain the control flow**
      Operators configure region ownership, failover policies, replication topology, conflict rules, and recovery runbooks. Control plane can drain, fail over, or disable a region.
    * **Explain the data flow**
      User request goes to nearest healthy region. Writes commit locally, then replicate. Reads prefer local region unless stronger consistency requires routing to owner or quorum.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Write conflict handling**
      The hard problem is allowing regional writes without corrupting shared state.

      | Option | Pros | Cons |
      |---|---|---|
      | Single writer per key | Simple consistency | Requires ownership routing and failover |
      | Multi-writer with conflict resolution | Highly available | Hard correctness and product semantics |
      | Global consensus | Strong correctness | High latency and lower partition tolerance |

      Prefer partitioning ownership by tenant or user. Use global consensus only for small, critical control-plane state.

## 14. Design Backup And Disaster Recovery Platform

* **Question**
  Design a backup and restore platform for cloud databases or volumes.

* **Answer**
  * **Scope**
    Provide scheduled backups, point-in-time restore, cross-region copies, encryption, retention, and compliance workflows.
  * **Functional Requirements**
    Define backup policies, take full and incremental backups, restore to a point in time, copy backups cross-region, verify integrity, and audit restores.
  * **Non Functional Requirements**
    Low RPO/RTO, durable storage, minimal production impact, encryption, retention compliance, and continuous restore testing.
  * **High level design and diagram (at block level)**
    ```text
    Backup Policy API -> Scheduler -> Snapshot/Change Capture Agent
                                             |
                                             v
                                      Backup Stream
                                             |
                                             v
                                   Object Storage Archive
                                             |
                                             v
                                      Restore Service
    ```
    * **Explain the blocks**
      Policy API manages schedules and retention. Scheduler triggers jobs. Agent captures full snapshots or incremental changes. Backup Stream chunks, compresses, encrypts, and transfers data. Archive stores backups. Restore Service rebuilds resources.
    * **Explain the control flow**
      Admin creates backup policy, retention, region targets, encryption settings, and restore permissions. Scheduler enforces policy and records job state.
    * **Explain the data flow**
      Agent captures data changes, chunks and encrypts them, streams to archive, verifies checksums, records restore metadata, and supports later restore jobs.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Full vs incremental backups**
      The hard problem is balancing backup cost with restore speed.

      | Option | Pros | Cons |
      |---|---|---|
      | Full backups | Simple restore | Expensive and slow |
      | Incremental backups | Efficient backup and storage | Longer restore chains and more metadata risk |
      | Synthetic full | Efficient with faster restore | More backend processing complexity |

      Prefer periodic full backups plus frequent incrementals, with synthetic fulls and automated restore validation.

## 15. Design Cloud Data Warehouse

* **Question**
  Design a scalable cloud data warehouse.

* **Answer**
  * **Scope**
    Store large analytical datasets and run distributed SQL queries over batch and streaming data.
  * **Functional Requirements**
    Load data, run SQL, manage schemas and catalog, support batch/streaming ingestion, handle concurrent users, and enforce access control.
  * **Non Functional Requirements**
    Petabyte-scale storage, fast analytical queries, elastic compute, high availability, cost isolation, strong metadata consistency, and workload isolation.
  * **High level design and diagram (at block level)**
    ```text
    Clients/BI -> SQL Gateway -> Query Planner/Optimizer
                                       |
                                       v
                            Distributed Execution Engine
                                       |
                                       v
                         Columnar Storage on Object Store
                                       |
                                       v
                                Metadata Catalog
    ```
    * **Explain the blocks**
      SQL Gateway accepts queries. Planner optimizes execution. Execution Engine runs distributed fragments. Columnar Storage keeps compressed table data in object storage. Catalog stores schemas, partitions, stats, transactions, and permissions.
    * **Explain the control flow**
      Users create warehouses, schemas, tables, roles, policies, and compute clusters. Control plane manages scaling, metadata commits, and permissions.
    * **Explain the data flow**
      Query is parsed, optimized, split into stages, executed across workers, reads relevant columnar data, shuffles intermediate results, and returns final rows.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Separate storage and compute**
      The hard problem is scaling independently without making every query slow.

      | Option | Pros | Cons |
      |---|---|---|
      | Coupled storage/compute | Fast local reads | Harder independent scaling and elasticity |
      | Separated storage/compute | Elastic and cost-efficient | Object-store latency and metadata pressure |
      | Hybrid cache | Good performance and elasticity | Cache invalidation and warmup complexity |

      Prefer separated storage and compute with local SSD cache, statistics, pruning, and metadata optimization.

## 16. Design Streaming / Event Bus Service

* **Question**
  Design a Kafka-like event streaming service.

* **Answer**
  * **Scope**
    Support durable publish/subscribe messaging with replay, ordering, retention, partitions, and consumer groups.
  * **Functional Requirements**
    Create topics, publish events, consume events, retain events, replay from offsets, support consumer groups, and enforce ACLs.
  * **Non Functional Requirements**
    High throughput, durable storage, ordered delivery per partition, horizontal scale, low publish latency, and operational repairability.
  * **High level design and diagram (at block level)**
    ```text
    Producers -> Broker Frontend -> Partition Leaders -> Replicated Log Storage
                                                               |
                                                               v
                                                        Consumer Groups
                                                               |
                                                               v
                                                          Offset Store
    ```
    * **Explain the blocks**
      Broker Frontend accepts publish and consume. Partition Leaders order writes. Replicated Log Storage persists events. Consumer Groups coordinate parallel reads. Offset Store tracks progress.
    * **Explain the control flow**
      Admins create topics, partitions, retention, ACLs, and replication factor. Cluster controller assigns leaders and handles rebalancing.
    * **Explain the data flow**
      Producer sends event to partition leader. Leader appends to log, replicates, acknowledges, and consumers fetch by partition while committing offsets.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Partition count and ordering**
      The hard problem is scaling throughput while preserving meaningful ordering.

      | Option | Pros | Cons |
      |---|---|---|
      | Few partitions | Simpler ordering and operations | Limited throughput |
      | Many partitions | More parallelism | More metadata, rebalancing, and small-file/log overhead |
      | Dynamic partitioning | Adapts to growth | Complex key movement and ordering changes |

      Prefer partitioning by stable key with enough headroom, and document that ordering is per key or partition.

## 17. Design Distributed Cache

* **Question**
  Design a distributed cache for cloud services.

* **Answer**
  * **Scope**
    Provide low-latency key-value caching with TTL, eviction, sharding, replication, resizing, and observability.
  * **Functional Requirements**
    Get, set, delete keys, expire TTLs, evict under memory pressure, resize cluster, replicate data, expose metrics, and isolate tenants.
  * **Non Functional Requirements**
    Low millisecond or sub-millisecond latency, high throughput, graceful node failure, predictable memory use, and controlled stale-data risk.
  * **High level design and diagram (at block level)**
    ```text
    Client -> Cache Client Library -> Consistent Hash Ring
                                             |
                              +--------------+--------------+
                              v              v              v
                           Cache 1        Cache 2        Cache 3
                              |
                              v
                      Optional DB Fallback
    ```
    * **Explain the blocks**
      Client Library routes keys. Hash Ring maps keys to nodes. Cache Nodes store values in memory. Replication copies hot or critical keys. Optional DB Fallback serves cache misses.
    * **Explain the control flow**
      Operators configure cluster size, memory limits, eviction policy, replication factor, tenant quotas, and resize operations.
    * **Explain the data flow**
      Client computes target node and reads key. On miss, service reads database and populates cache. Writes update or invalidate cache depending on policy.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Cache invalidation**
      The hard problem is avoiding stale data without turning cache into a write bottleneck.

      | Option | Pros | Cons |
      |---|---|---|
      | TTL only | Simple and resilient | Stale reads until expiry |
      | Write-through | More consistent | Slower writes and cache dependency |
      | Explicit invalidation | Fresher data | Complex coordination and missed invalidation risk |

      Prefer cache-aside with TTL for most data and explicit invalidation for correctness-sensitive objects.

## 18. Design Billing And Metering

* **Question**
  Design a cloud billing and usage metering system.

* **Answer**
  * **Scope**
    Collect usage from cloud services, aggregate by customer, SKU, resource, and time window, price it, and produce invoices and usage reports.
  * **Functional Requirements**
    Ingest usage events, validate and deduplicate them, aggregate usage, apply pricing, generate invoices, expose reports, and handle late events.
  * **Non Functional Requirements**
    Correctness, auditability, high throughput, idempotency, late-arrival tolerance, security, and explainable billing.
  * **High level design and diagram (at block level)**
    ```text
    Cloud Services -> Usage Event API -> Durable Event Log -> Validation/Dedup
                                                                  |
                                                                  v
                                                           Aggregation Engine
                                                                  |
                                                                  v
                                                            Pricing Engine
                                                                  |
                                                                  v
                                                       Invoice/Reporting Store
    ```
    * **Explain the blocks**
      Usage API receives metering records. Event Log stores immutable raw usage. Validation and Dedup clean records. Aggregation rolls up usage. Pricing applies SKU and rate plans. Invoice Store records billable output.
    * **Explain the control flow**
      Finance and product teams configure SKUs, rate plans, credits, discounts, tax rules, and billing cycles. Pricing config is versioned and auditable.
    * **Explain the data flow**
      Services emit usage events. Pipeline validates, dedupes, aggregates by customer/SKU/time, prices usage, and generates invoice lines and usage reports.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Real-time vs batch billing**
      The hard problem is giving customers fast visibility while preserving invoice correctness.

      | Option | Pros | Cons |
      |---|---|---|
      | Real-time billing | Fast anomaly detection and visibility | Expensive and complex |
      | Batch billing | Simple and authoritative | Delayed feedback |
      | Dual pipeline | Fast estimates plus accurate invoices | Reconciliation complexity |

      Prefer immutable raw usage events, near-real-time estimates, and an authoritative batch invoice pipeline.

## 19. Design Secrets / Key Management Service

* **Question**
  Design a cloud secrets and encryption key management service.

* **Answer**
  * **Scope**
    Manage encryption keys, secrets, rotation, grants, audit, and envelope encryption for services and customers.
  * **Functional Requirements**
    Create keys and secrets, encrypt/decrypt data keys, rotate keys, grant and revoke access, audit usage, support HSM-backed keys, and schedule deletion.
  * **Non Functional Requirements**
    Extremely strong security, high availability, low latency, tenant isolation, tamper-evident audit, and safe emergency controls.
  * **High level design and diagram (at block level)**
    ```text
    Client Service -> KMS API -> AuthZ/Policy Engine -> Key Metadata Store
                                                               |
                                                               v
                                                        HSM / Key Store
                                                               |
                                                               v
                                                           Audit Log
    ```
    * **Explain the blocks**
      KMS API exposes crypto operations. Policy Engine validates access. Metadata Store stores key versions and config. HSM or Key Store protects key material. Audit Log records every operation.
    * **Explain the control flow**
      Admin creates keys, aliases, rotation policies, grants, and deletion schedules. Control plane manages versions and access policy.
    * **Explain the data flow**
      Client requests encrypt or decrypt. KMS authenticates, authorizes, selects key version, performs crypto operation, logs access, and returns result.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Data-key caching**
      The hard problem is reducing KMS latency without weakening key control.

      | Option | Pros | Cons |
      |---|---|---|
      | No caching | Best audit/control | Higher latency and KMS dependency |
      | Long-lived cache | Fast and resilient | Larger exposure window |
      | Short-lived cache | Good latency/security balance | Revocation and TTL tuning complexity |

      Prefer envelope encryption with short-lived data-key caching, strict audit, and fast revocation for high-risk keys.

## 20. Design Service Discovery And Health Checking

* **Question**
  Design service discovery and health checking for cloud microservices.

* **Answer**
  * **Scope**
    Let services find healthy instances dynamically across deployments, availability domains, and regions.
  * **Functional Requirements**
    Register instances, deregister instances, health check instances, query healthy endpoints, support metadata and versions, and integrate with load balancers.
  * **Non Functional Requirements**
    Low lookup latency, fast failure detection, high availability, eventual consistency, safe deployment behavior, and resistance to thundering herds.
  * **High level design and diagram (at block level)**
    ```text
    Service Instance -> Registration Agent -> Service Registry
                                                     |
                                                     v
                                               Health Checker
                                                     |
                                                     v
                                             Discovery API / DNS
                                                     |
                                                     v
                                           Clients / Load Balancers
    ```
    * **Explain the blocks**
      Registration Agent announces instances. Registry stores service metadata. Health Checker verifies liveness and readiness. Discovery API or DNS exposes endpoints. Clients and load balancers route only to healthy instances.
    * **Explain the control flow**
      Service owners define service name, version, health check path, deployment metadata, TTL, routing policy, and rollout rules.
    * **Explain the data flow**
      Instance registers on startup and heartbeats periodically. Health checker updates status. Clients query discovery and route to healthy endpoints.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **DNS vs client-side vs load-balancer discovery**
      The hard problem is balancing simplicity, freshness, and routing control.

      | Option | Pros | Cons |
      |---|---|---|
      | DNS-based discovery | Simple and language agnostic | TTL staleness and limited routing logic |
      | Client-side discovery | Fast and flexible | Requires smart clients and shared libraries |
      | Load-balancer discovery | Centralized and mature | Extra hop and cost |

      Prefer DNS or load balancer for simple services and client-side discovery for high-throughput internal services that need rich routing.

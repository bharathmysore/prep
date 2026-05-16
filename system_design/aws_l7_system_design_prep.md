# AWS L7 System Design Prep

This guide uses AWS/Amazon-style prompts that commonly appear in public prep material and are representative of cloud and distributed-systems interviews. It is not an official AWS question bank.

Maintenance note: treat this as a living company-specific catalog. Do not encode a fixed question count in this file name or title. Future agents should compute the current count from numbered `## N.` headings and keep numbering consistent after additions, removals, merges, or reordering.

Reference context:
- https://systemdesigntrainer.com/blog/amazon-system-design-interview-guide/
- https://www.systemdesignhandbook.com/blog/amazon-system-design-interview-questions/
- https://www.educative.io/blog/amazon-system-design-interview-questions
- https://interviewkickstart.com/blogs/interview-questions/amazon-software-engineer-system-design-interview-questions
- https://aws.amazon.com/bedrock/agentcore/
- https://docs.aws.amazon.com/bedrock-agentcore/latest/devguide/what-is-bedrock-agentcore.html
- https://aws.amazon.com/blogs/aws/introducing-amazon-bedrock-agentcore-securely-deploy-and-operate-ai-agents-at-any-scale/
- https://aws.amazon.com/blogs/machine-learning/securing-ai-agents-with-temporal-policies-in-amazon-bedrock-agentcore/
- https://aws.amazon.com/blogs/machine-learning/configure-rate-limits-for-ai-traffic-on-agentcore-gateway/

Use each answer as a 45-minute interview outline. At L7, spend less time naming services and more time explaining boundaries, failure modes, migration, ownership, and cost.

## 1. Design Amazon S3 / Object Storage

* **Question**
  Design a highly durable, highly available object storage service like Amazon S3.

* **Answer**
  * **Scope**
    Store immutable object versions inside buckets. Support PUT, GET, DELETE, LIST, multipart upload, metadata, ACL/policy, versioning, lifecycle, and cross-region replication.
  * **Functional Requirements**
    Create buckets, upload/download objects, list by prefix, delete objects, version objects, enforce auth, validate checksums, support large objects, and emit audit/events.
  * **Non Functional Requirements**
    Very high durability, high availability, horizontal scale, multi-AZ resilience, low cost per GB, strong read-after-write for common operations, tenant isolation, and repairability.
  * **High level design and diagram (at block level)**
    ```text
    Client -> API Fleet -> Auth/IAM -> Metadata Service -> Placement Service
                    |                              |
                    v                              v
              Data Router -> Storage Nodes <-> Repair/Rebalance
                    |
                    v
              Events/Inventory/Lifecycle/Replication
    ```
    * **Explain the blocks**
      API Fleet handles REST requests. Auth/IAM validates identity and bucket policies. Metadata Service stores bucket/object metadata and versions. Placement Service chooses storage nodes/AZs. Storage Nodes persist chunks with checksums. Repair/Rebalance heals under-replicated data. Lifecycle/Replication moves, expires, or copies objects.
    * **Explain the control flow**
      Bucket creation, ACL updates, lifecycle rules, replication policies, and placement policies go through the control plane. Config is validated, versioned, audited, and pushed to request-time caches. Storage fleet membership and capacity maps are continuously updated by placement/rebalancing control loops.
    * **Explain the data flow**
      PUT authenticates, writes metadata intent, streams object chunks to multiple storage nodes, validates checksums, commits metadata, and emits object-created events. GET authenticates, resolves object metadata, selects healthy replicas/chunks, streams data back, and records access metrics.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Durability: replication vs erasure coding**
      Full replication is simple and fast to repair, but expensive. Erasure coding is cheaper at large scale, but read/repair paths are more complex and require careful degraded-read handling. A common design uses replication for hot/new objects and erasure coding for warm/cold objects.
    * **Consistency model**
      Strong consistency simplifies users and applications, but costs coordination in metadata. Eventual consistency reduces coordination, but creates surprising LIST/GET behavior. Prefer strongly consistent metadata operations within a region, with asynchronous cross-region replication.
    * **Hot prefixes and noisy tenants**
      Lexicographic partitioning can overload a prefix. Hash or dynamically split metadata partitions, add adaptive throttling, and isolate abusive tenants with per-account quotas.

## 2. Design DynamoDB / Distributed Key-Value Store

* **Question**
  Design a managed, low-latency, highly available key-value/document database.

* **Answer**
  * **Scope**
    Support tables, partition keys, sort keys, conditional writes, secondary indexes, TTL, streams, backup/restore, and optional strong reads.
  * **Functional Requirements**
    Put/get/update/delete items, scan/query by key, enforce conditional writes, maintain indexes, emit change streams, scale throughput, and recover from node/AZ failure.
  * **Non Functional Requirements**
    Single-digit millisecond latency, high availability, predictable performance, elastic scale, multi-tenant isolation, durability, and operational automation.
  * **High level design and diagram (at block level)**
    ```text
    Client -> Request Router -> Partition Map -> Storage Partition
                            |              -> Replica Set
                            v
                  Control Plane / Auto Split / Capacity
                            |
                            v
                 Streams / Index Builders / Backup
    ```
    * **Explain the blocks**
      Request Router authenticates and routes by partition key. Partition Map maps key ranges/hash slots to replicas. Storage Partitions store items. Replica Set provides durability. Control Plane manages tables, partitions, splits, capacity, and failover. Streams and Index Builders process changes.
    * **Explain the control flow**
      Table creation defines schema, billing mode, indexes, and limits. Control plane allocates partitions, monitors heat, splits partitions, updates routing maps, and manages backup/restore. Clients see stable APIs while partitions move behind the scenes.
    * **Explain the data flow**
      Write request hashes the partition key, routes to the leader or quorum group, persists the item, updates indexes/streams asynchronously or transactionally depending on guarantee, and returns. Reads route to leader/replica depending on consistency mode.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Consistency options**
      Strong reads give intuitive correctness but add latency and reduce availability during partitions. Eventual reads are faster and more available, but stale. Offer both; default to eventual for scale, strong when callers need read-after-write.
    * **Hot partitions**
      A single popular key can saturate one partition. Options: adaptive capacity is transparent but limited; write sharding spreads load but complicates reads; caching helps read heat but not write heat.
    * **Secondary indexes**
      Synchronous index updates improve query correctness but increase write latency. Async index updates improve write throughput but introduce index lag. Use explicit consistency expectations and expose lag metrics.

## 3. Design SQS / Distributed Message Queue

* **Question**
  Design a distributed message queue for decoupling producers and consumers.

* **Answer**
  * **Scope**
    Support send, receive, ack/delete, visibility timeout, delayed messages, retries, DLQ, standard queues, and FIFO queues.
  * **Functional Requirements**
    Accept messages, durably store them, deliver to consumers, prevent immediate duplicate processing through leases, retry failed messages, preserve FIFO when requested, and expose metrics.
  * **Non Functional Requirements**
    High availability, high throughput, at-least-once delivery, bounded latency, tenant isolation, durability, backpressure, and operational simplicity.
  * **High level design and diagram (at block level)**
    ```text
    Producers -> Queue API -> Partition Router -> Broker Partitions
                                           |       |
    Consumers <- Poll API <- Lease Manager <- Message Store
                                           |
                                           v
                                      DLQ / Metrics
    ```
    * **Explain the blocks**
      Queue API accepts sends and receives. Partition Router spreads messages. Broker Partitions own durable message logs. Lease Manager tracks visibility timeouts. Message Store persists message bodies. DLQ captures exhausted retries.
    * **Explain the control flow**
      Queue creation sets retention, visibility timeout, FIFO mode, DLQ policy, and access policy. Control plane updates routing, partition counts, and quotas. Consumers configure polling and ack behavior.
    * **Explain the data flow**
      Producer sends a message, router assigns a partition, broker persists replicas, and message becomes visible. Consumer polls, receives a leased message, processes it, and deletes it. If the lease expires, the message becomes visible again or moves to DLQ after retry limits.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Standard vs FIFO**
      Standard queues scale very high and are simpler, but can duplicate and reorder messages. FIFO queues provide ordering per message group and dedupe windows, but lower throughput. Choose FIFO only for workflows that truly require ordered side effects.
    * **Visibility timeout**
      Short timeout retries quickly but risks duplicate processing for slow consumers. Long timeout reduces duplicates but delays recovery. Let consumers extend leases for long tasks.
    * **Push vs pull**
      Pull is easier for consumer backpressure and firewall boundaries. Push reduces latency but risks overwhelming consumers. Use pull for a general-purpose queue.

## 4. Design Kinesis / Event Streaming Platform

* **Question**
  Design a high-throughput event streaming service like Kinesis.

* **Answer**
  * **Scope**
    Append events to streams, partition by key, retain ordered logs, allow multiple consumer groups, replay offsets, and scale shards.
  * **Functional Requirements**
    Produce events, consume by offset, checkpoint progress, shard/split streams, enforce retention, support fanout, and expose lag/throughput metrics.
  * **Non Functional Requirements**
    High write throughput, ordered delivery per partition, durability, replayability, low consumer lag, multi-tenant quotas, and cost-efficient retention.
  * **High level design and diagram (at block level)**
    ```text
    Producers -> Ingest Fleet -> Shard Router -> Log Shards -> Replicas
                                             |
    Consumers <- Read API <- Offset/Checkpoint Store
                                             |
                                             v
                                  Retention / Compaction / Metrics
    ```
    * **Explain the blocks**
      Ingest Fleet validates and batches writes. Shard Router maps partition keys to shards. Log Shards store append-only segments. Replicas provide durability. Offset Store tracks consumer positions. Retention removes old segments.
    * **Explain the control flow**
      Stream creation defines shard count, retention, encryption, and quotas. Resharding updates the shard map and creates parent/child shard lineage. Consumer registration configures fanout and checkpointing.
    * **Explain the data flow**
      Producer sends records with partition keys. Router appends records to shard leaders. Consumers read sequentially from shards, process records, and checkpoint offsets. On failure, consumers resume from last checkpoint.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Ordering vs scale**
      Ordering per partition is tractable; global ordering limits throughput. Increase shards for scale, but require callers to choose partition keys carefully.
    * **Hot shards**
      Poor partition keys overload one shard. Solutions: randomize keys, split shards, adaptive routing, or aggregate hot-key state downstream.
    * **Consumer fanout**
      Shared consumers are cheaper but compete for read throughput. Dedicated fanout gives predictable latency but costs more and increases broker load.

## 5. Design Lambda / Serverless Compute

* **Question**
  Design a serverless function execution platform like AWS Lambda.

* **Answer**
  * **Scope**
    Support function upload, versions, triggers, invocation, concurrency limits, logs, metrics, retries, and tenant isolation.
  * **Functional Requirements**
    Deploy code, invoke sync/async, run in isolated sandboxes, scale by demand, connect to event sources, stream logs, and enforce IAM/resource limits.
  * **Non Functional Requirements**
    Low operational burden, elastic scale, strong isolation, predictable billing, acceptable cold starts, high availability, and abuse containment.
  * **High level design and diagram (at block level)**
    ```text
    Developer -> Control Plane -> Function Store / Config Store

    Event Source -> Invoke Router -> Scheduler -> Worker Pool -> Sandbox
                                      |              |
                                      v              v
                               Concurrency Manager  Logs/Metrics
    ```
    * **Explain the blocks**
      Control Plane manages function metadata and versions. Function Store holds code/images. Invoke Router handles requests. Scheduler assigns work to workers. Worker Pool hosts sandboxes. Concurrency Manager enforces limits. Logs/Metrics captures execution output.
    * **Explain the control flow**
      Function deployment validates code, stores package, records version/config, and propagates config to invoke routers. Scaling policies and reserved concurrency are enforced by control loops.
    * **Explain the data flow**
      Event arrives, router authenticates and resolves function config, scheduler finds warm capacity or creates a sandbox, worker runs handler, returns result or records async retry, and streams logs/metrics.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Cold starts**
      On-demand sandboxes reduce cost but add latency. Warm pools reduce latency but consume idle resources. Provisioned concurrency gives predictable latency but shifts cost to the customer.
    * **Isolation model**
      Containers are efficient but require strong kernel/runtime hardening. MicroVMs provide stronger isolation with more overhead. Use microVM-style isolation for untrusted multi-tenant code.
    * **Async retries**
      Automatic retries improve reliability but can amplify downstream incidents. Add DLQs, max retry age, idempotency guidance, and circuit breakers.

## 6. Design API Gateway With Distributed Rate Limiting

* **Question**
  Design a managed API gateway that authenticates, routes, throttles, and observes customer APIs.

* **Answer**
  * **Scope**
    Support API definitions, routes, auth, request validation, rate limits, quotas, transforms, logs, metrics, and multi-region deployment.
  * **Functional Requirements**
    Configure APIs, deploy stages, authenticate requests, enforce limits, route to backends, transform payloads, and expose access logs/metrics.
  * **Non Functional Requirements**
    Low latency, high availability, high throughput, accurate enough throttling, config safety, tenant isolation, and fast rollback.
  * **High level design and diagram (at block level)**
    ```text
    Admin -> Control Plane -> Config Store -> Config Publisher

    Client -> Edge/API Fleet -> Auth -> Rate Limiter -> Router -> Backend
                              |          |
                              v          v
                         Logs/Metrics  Quota Store
    ```
    * **Explain the blocks**
      Control Plane manages API definitions. Config Publisher pushes versions to gateways. Edge/API Fleet terminates requests. Auth validates identity. Rate Limiter enforces local/global quotas. Router forwards to integrations.
    * **Explain the control flow**
      Users create APIs and stages. Config is validated, versioned, deployed gradually, and cached by data-plane nodes. Rollback swaps active config version.
    * **Explain the data flow**
      Request enters gateway, auth runs, rate limit is checked, request is transformed, routed to backend, response is transformed, and logs/metrics are emitted.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Central vs local rate limiting**
      Central limiters are accurate but add latency and can fail closed/open. Local token buckets are fast and resilient but may overshoot. Hybrid local reservations plus periodic reconciliation is a good default.
    * **Config rollout**
      Instant global rollout is fast but risky. Gradual rollout catches bad configs but adds complexity. Use validation, canaries, and versioned rollback.
    * **Backend overload**
      Passing all traffic through can melt backends. Add adaptive throttling, circuit breakers, retry budgets, and per-route concurrency limits.

## 7. Design CloudWatch / Metrics And Logs Platform

* **Question**
  Design a cloud monitoring platform for metrics, logs, alerts, and dashboards.

* **Answer**
  * **Scope**
    Ingest metrics/logs/traces, store them with retention tiers, query them, evaluate alarms, and show dashboards.
  * **Functional Requirements**
    Accept agent/service telemetry, aggregate metrics, index logs, query by time/resource, trigger alerts, manage retention, and enforce tenant permissions.
  * **Non Functional Requirements**
    High ingest scale, bounded query latency, high alarm reliability, cost control, cardinality control, durability, and regional isolation.
  * **High level design and diagram (at block level)**
    ```text
    Agents/Services -> Ingest API -> Stream Buffer -> Processors
                                         |             |
                                         v             v
                                   Raw Log Store   Metric TSDB
                                         |             |
                                         v             v
                                  Query Service   Alarm Evaluator
    ```
    * **Explain the blocks**
      Ingest API validates telemetry. Stream Buffer absorbs bursts. Processors parse, aggregate, and route. Raw Log Store keeps compressed logs. Metric TSDB stores time series. Query Service reads both. Alarm Evaluator checks rules.
    * **Explain the control flow**
      Users configure dashboards, alarms, retention, and log parsing rules. Control plane validates rules, shards alarm evaluation, and deploys query/retention policy.
    * **Explain the data flow**
      Agents batch telemetry to ingestion. Logs are compressed/indexed by time and labels. Metrics are aggregated and written to TSDB. Queries fan out by tenant/time shards. Alarm results emit notifications.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **High-cardinality metrics**
      Unlimited labels make storage and queries explode. Reject abusive dimensions, sample, pre-aggregate, or price by cardinality. Pros: protects platform. Cons: surprises users.
    * **Raw logs vs indexed logs**
      Full indexing gives fast search but high cost. Partial indexing plus cold scans is cheaper but slower. Use tiered storage and explicit query cost controls.
    * **Alarm correctness**
      Missing data can be treated as OK, breaching, or unknown. Each choice has false-positive/false-negative tradeoffs. Let users choose and make evaluation state visible.

## 8. Design CloudFront / CDN

* **Question**
  Design a global content delivery network.

* **Answer**
  * **Scope**
    Cache and serve static/dynamic content from edge locations with TLS, origin fetch, invalidation, signed URLs, and DDoS/WAF integration.
  * **Functional Requirements**
    Route users to nearby edge, cache responses, fetch from origin on miss, invalidate content, support custom domains/certs, and collect access logs.
  * **Non Functional Requirements**
    Very low latency, high availability, massive throughput, origin protection, cache efficiency, global resilience, and config safety.
  * **High level design and diagram (at block level)**
    ```text
    Admin -> Control Plane -> Distribution Config -> Edge Config Push

    Client -> DNS/Anycast -> Edge Cache -> Regional Shield -> Origin
                              |
                              v
                        Logs / WAF / Metrics
    ```
    * **Explain the blocks**
      DNS/Anycast sends users to edge. Edge Cache serves cached objects. Regional Shield collapses origin misses. Origin is customer backend. Control Plane manages distributions. WAF filters abusive traffic.
    * **Explain the control flow**
      Customers configure origins, cache keys, TTLs, certs, and invalidations. Config is validated and propagated globally with staged rollout. Invalidation commands remove or mark objects stale at edge.
    * **Explain the data flow**
      Client request hits edge. Edge computes cache key and serves hit or requests from shield/origin. Response is cached according to policy and returned. Logs flow asynchronously.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **TTL vs freshness**
      Long TTLs increase hit rate and reduce origin load but risk stale content. Short TTLs improve freshness but increase latency/cost. Use cache-control, versioned URLs, and targeted invalidation.
    * **Origin protection**
      Direct misses can stampede origin. Shielding, request coalescing, stale-while-revalidate, and negative caching reduce load.
    * **Dynamic content**
      Caching personalized responses is risky. Require careful cache keys, signed cookies/URLs, and default private/no-store behavior for auth content.

## 9. Design Route 53 / Global DNS And Traffic Routing

* **Question**
  Design a globally distributed authoritative DNS and traffic-management service.

* **Answer**
  * **Scope**
    Manage hosted zones, DNS records, health checks, weighted routing, latency routing, failover, and domain registration integration.
  * **Functional Requirements**
    Create/update records, answer DNS queries, route by policy, monitor endpoint health, propagate zone changes, and audit changes.
  * **Non Functional Requirements**
    Extremely high availability, low query latency, DDoS resistance, safe propagation, correctness, and eventual global convergence.
  * **High level design and diagram (at block level)**
    ```text
    Admin -> DNS Control Plane -> Zone Store -> Global Propagation

    Resolver -> Edge DNS Servers -> Routing Policy Engine
                                    |
                                    v
                             Health Check State
    ```
    * **Explain the blocks**
      Control Plane manages zones. Zone Store holds versioned records. Propagation distributes signed zone data. Edge DNS Servers answer queries. Policy Engine evaluates weighted/latency/failover rules. Health Check State influences answers.
    * **Explain the control flow**
      Record changes are validated, versioned, signed if needed, and propagated to authoritative fleets. Health checkers update endpoint status and policy state.
    * **Explain the data flow**
      Resolver asks for a record. Edge DNS server evaluates current zone and policy state, returns answer with TTL, and emits query metrics.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **DNS TTL and failover**
      Low TTL improves failover but increases query load. High TTL improves cache efficiency but slows failover. Use low TTL for critical failover records and health-based routing.
    * **Split-brain routing**
      Inconsistent health state can route users incorrectly. Use regional health consensus, hysteresis, and conservative failover thresholds.
    * **DDoS resilience**
      Authoritative DNS is attack-prone. Use anycast, overprovisioning, aggressive caching, rate limiting, and isolated control plane.

## 10. Design IAM / Authorization Service

* **Question**
  Design a cloud identity and authorization service for all service APIs.

* **Answer**
  * **Scope**
    Users, roles, policies, temporary credentials, request signing, policy evaluation, audit, and federation.
  * **Functional Requirements**
    Create identities, attach policies, assume roles, issue temporary credentials, authorize API calls, log decisions, and support emergency revocation.
  * **Non Functional Requirements**
    Very high availability, low latency, secure by default, strong auditability, least privilege, global scale, and safe policy propagation.
  * **High level design and diagram (at block level)**
    ```text
    Admin -> IAM Control Plane -> Policy Store -> Policy Compiler

    Client -> Service API -> AuthN -> AuthZ Evaluator -> Decision
                                |             |
                                v             v
                         Credential Store   Policy Cache
    ```
    * **Explain the blocks**
      Control Plane manages identities and policies. Policy Store is source of truth. Policy Compiler normalizes policies. AuthN validates signatures/tokens. AuthZ Evaluator decides allow/deny. Policy Cache serves low-latency reads.
    * **Explain the control flow**
      Policy changes are validated, versioned, audited, compiled, and propagated to regional caches. Credential issuance is governed by trust policies and session limits.
    * **Explain the data flow**
      Service receives signed request, authenticates caller, loads cached identity/policies/context, evaluates deny/allow rules, returns decision, and emits audit logs.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Cached auth vs revocation**
      Caching makes every service fast and resilient, but permission changes can be stale. Short TTLs and revocation channels reduce risk at higher control-plane load.
    * **Centralized evaluator vs embedded evaluator**
      Centralized is easier to update and audit but adds dependency latency. Embedded libraries/caches are faster but harder to keep consistent. Use embedded evaluation with shared policy engine and signed policy bundles.
    * **Policy complexity**
      Rich policies are powerful but hard to reason about. Add policy simulation, explainable decisions, linting, and explicit deny precedence.

## 11. Design EBS / Distributed Block Storage

* **Question**
  Design a durable, low-latency virtual block storage service for compute instances.

* **Answer**
  * **Scope**
    Volumes, attach/detach, reads/writes by block address, snapshots, replication within an AZ, encryption, IOPS/throughput limits, and recovery.
  * **Functional Requirements**
    Create volumes, attach to instances, serve block I/O, persist writes, snapshot changed blocks, restore volumes, and fail over storage nodes.
  * **Non Functional Requirements**
    Low latency, high durability, predictable IOPS, crash consistency, AZ-level resilience, noisy-neighbor isolation, and fast repair.
  * **High level design and diagram (at block level)**
    ```text
    EC2 Host -> Block Frontend -> Volume Router -> Primary Replica
                                            |       |
                                            v       v
                                      Secondary  Snapshot Pipeline -> Object Store
    Control Plane -> Volume Metadata / Placement / Limits
    ```
    * **Explain the blocks**
      Block Frontend exposes the virtual device. Volume Router maps block ranges to replicas. Primary/Secondary replicas synchronously persist writes. Snapshot Pipeline copies changed blocks. Control Plane manages attach, placement, and limits.
    * **Explain the control flow**
      Volume creation selects placement and replication group. Attach updates host routing and access permissions. Limits and encryption keys are configured before data-plane access.
    * **Explain the data flow**
      Write goes from host to primary replica, replicates to secondary, commits after durability threshold, and acks. Reads go to primary or healthy replica. Snapshots asynchronously copy changed blocks.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Synchronous vs asynchronous replication**
      Sync replication protects acknowledged writes but increases latency. Async improves latency but risks data loss. For block storage, sync within AZ is preferred.
    * **Snapshot consistency**
      Crash-consistent snapshots are easy but may require app recovery. App-consistent snapshots need guest coordination and are harder. Offer both through APIs.
    * **Noisy neighbor**
      Shared storage fleets can suffer contention. Enforce per-volume IOPS, isolate hot volumes, and use admission control.

## 12. Design Aurora / Cloud Relational Database

* **Question**
  Design a managed relational database with distributed storage and fast failover.

* **Answer**
  * **Scope**
    SQL compute nodes, distributed storage, replicas, backups, failover, transactions, read scaling, and monitoring.
  * **Functional Requirements**
    Execute SQL, persist transactions, support read replicas, recover after compute failure, backup continuously, restore point-in-time, and scale storage.
  * **Non Functional Requirements**
    ACID semantics, high availability, low write latency, fast failover, durable storage, predictable reads, and operational automation.
  * **High level design and diagram (at block level)**
    ```text
    Client -> DB Proxy -> Writer / Readers
                           |
                           v
                    Distributed Storage Volume
                     /   /   /   |   \   \
                  AZ Replica Segments + Log Store
                           |
                           v
                    Backup / Recovery / Metrics
    ```
    * **Explain the blocks**
      DB Proxy manages connections/failover. Writer handles transactions. Readers serve read-only queries. Distributed Storage persists redo/log records across AZs. Backup/Recovery continuously archives logs.
    * **Explain the control flow**
      Cluster creation provisions compute and storage volume. Failover control loop promotes a replica and updates proxy endpoints. Backup and retention policies are managed separately.
    * **Explain the data flow**
      Write transaction generates log records, sends to distributed storage quorum, commits after enough replicas persist. Readers apply logs and serve queries. Backups stream log/history to durable storage.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Compute-storage separation**
      Separating storage enables fast failover and elastic storage, but makes storage latency central. Local storage is faster but harder to fail over. For managed cloud DB, separation is worth it.
    * **Read replica lag**
      Async replicas scale reads but can be stale. Sync reads are consistent but slower. Expose read-after-write options or route critical reads to writer.
    * **Failover**
      Fast failover reduces downtime but risks split brain. Use fencing tokens, lease-based writer ownership, and proxy endpoint control.

## 13. Design ECS/EKS / Container Orchestration Scheduler

* **Question**
  Design a container orchestration platform that schedules and runs services across a fleet.

* **Answer**
  * **Scope**
    Clusters, tasks/pods, services, deployments, health checks, autoscaling, service discovery, and node agents.
  * **Functional Requirements**
    Submit workloads, place containers, restart failed tasks, roll out versions, expose services, scale based on metrics, and collect logs.
  * **Non Functional Requirements**
    High control-plane availability, efficient utilization, fault tolerance, safe deployments, tenant isolation, and predictable scheduling latency.
  * **High level design and diagram (at block level)**
    ```text
    User -> Control Plane API -> Desired State Store -> Scheduler
                                               |          |
                                               v          v
                                      Controllers     Node Agents
                                                        |
                                                        v
                                                Containers / Network
    ```
    * **Explain the blocks**
      API accepts desired state. Desired State Store persists cluster state. Scheduler places tasks. Controllers reconcile services/deployments. Node Agents start/monitor containers. Networking provides service connectivity.
    * **Explain the control flow**
      User submits desired service. Scheduler chooses nodes based on resources and constraints. Controllers watch state and keep actual state matching desired state. Autoscaler adjusts nodes/tasks.
    * **Explain the data flow**
      Runtime traffic flows through load balancers/service discovery to containers. Logs and metrics flow from node agents to telemetry systems.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Bin packing vs spreading**
      Bin packing lowers cost but increases correlated failure/noisy neighbor risk. Spreading improves availability but costs more. Use configurable placement strategies.
    * **Scheduler scalability**
      Global scheduling is optimal but slow at huge scale. Partitioned scheduling scales better but can be less optimal. Use queues, caching, and incremental scheduling.
    * **Rolling deploy safety**
      Fast rollout reduces time-to-market but can blast all users. Canary, health gates, automatic rollback, and surge/unavailable limits reduce risk.

## 14. Design EventBridge / Distributed Scheduler

* **Question**
  Design a distributed event bus and scheduler for cron-style and event-pattern rules.

* **Answer**
  * **Scope**
    Event ingestion, rules, cron schedules, target delivery, retries, DLQs, schemas, and cross-account routing.
  * **Functional Requirements**
    Put events, match rules, trigger targets, run schedules, retry failures, route to DLQ, and expose delivery metrics.
  * **Non Functional Requirements**
    High availability, near-real-time delivery, at-least-once semantics, scalable rule matching, schedule accuracy, tenant isolation, and replayability.
  * **High level design and diagram (at block level)**
    ```text
    Admin -> Rule/Schedule Control Plane -> Rule Store / Time Wheel

    Producers -> Event Ingest -> Match Engine -> Target Queues -> Delivery Workers
                                           |
                                           v
                                      Retry / DLQ / Archive
    ```
    * **Explain the blocks**
      Control Plane manages rules and schedules. Rule Store stores event patterns. Time Wheel tracks due schedules. Match Engine evaluates events. Target Queues isolate deliveries. Workers invoke targets.
    * **Explain the control flow**
      Users create rules/schedules and targets. Config is validated, partitioned, and published to matchers. Schedules are sharded by next-fire time.
    * **Explain the data flow**
      Event enters ingest, match engine finds rules, creates target delivery records, workers invoke targets, failures retry with backoff, and exhausted deliveries go to DLQ.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Rule matching scale**
      Naive scan of all rules is slow. Index rules by event source/type/account and then evaluate predicates. More indexing improves speed but complicates updates.
    * **Schedule accuracy**
      Exact global second-level execution is costly. Bucketed scheduling is scalable but may have seconds of jitter. State SLA clearly.
    * **At-least-once delivery**
      Retries improve reliability but can duplicate target side effects. Include idempotency tokens and target-level dedupe guidance.

## 15. Design Step Functions / Workflow Orchestrator

* **Question**
  Design a durable workflow orchestration service.

* **Answer**
  * **Scope**
    Workflow definitions, executions, state transitions, waits, retries, branches, callbacks, activity workers, and audit history.
  * **Functional Requirements**
    Start workflows, persist state, execute steps, retry failures, wait for timers/callbacks, branch/parallelize work, and expose execution history.
  * **Non Functional Requirements**
    Durable execution, exactly-once state transition intent, at-least-once task invocation, auditability, long-running workflow support, and scale.
  * **High level design and diagram (at block level)**
    ```text
    Developer -> Definition Store

    Client -> Execution API -> State Engine -> Execution State Store
                                |     |
                                v     v
                          Timer Queue Task Queues -> Workers/Targets
                                |
                                v
                         History / Metrics / DLQ
    ```
    * **Explain the blocks**
      Definition Store holds workflow specs. State Engine interprets definitions. Execution Store persists current state and history. Timer Queue wakes waits. Task Queues invoke workers/targets.
    * **Explain the control flow**
      Definition changes are versioned. Starting an execution binds it to a definition version. State Engine decides next state and persists transitions before invoking external work.
    * **Explain the data flow**
      Execution starts, state is recorded, task is dispatched, result returns or times out, next transition is persisted, and history is appended until success/failure.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Orchestration vs choreography**
      Orchestration centralizes visibility and retries, but can become a bottleneck. Choreography scales independently but is harder to debug. Use orchestration for business-critical workflows.
    * **Exactly-once myths**
      You can make workflow state transitions exactly-once with conditional writes, but external task execution is usually at-least-once. Require idempotent activities.
    * **Long-running workflows**
      Keeping state in memory is simple but fragile. Persist state after every transition and rebuild execution from store/history.

## 16. Design Multi-Region Checkout / Order System

* **Question**
  Design a checkout and order-management system for a large e-commerce platform.

* **Answer**
  * **Scope**
    Cart checkout, inventory reservation, payment authorization, order creation, fulfillment events, cancellations, refunds, and notifications.
  * **Functional Requirements**
    Validate cart, price items, reserve inventory, authorize payment, create order, progress order states, handle duplicate checkout, and recover from partial failures.
  * **Non Functional Requirements**
    High availability, correctness for money/order state, low checkout latency, idempotency, auditability, regional resilience, and graceful degradation.
  * **High level design and diagram (at block level)**
    ```text
    Client -> Checkout API -> Order Orchestrator -> Order DB
                             |       |       |
                             v       v       v
                        Pricing  Inventory Payment
                             |
                             v
                    Event Bus -> Fulfillment / Notifications / Analytics
    ```
    * **Explain the blocks**
      Checkout API receives requests. Order Orchestrator manages saga/state machine. Order DB stores source-of-truth order state. Pricing, Inventory, and Payment are domain services. Event Bus decouples downstream work.
    * **Explain the control flow**
      Product teams configure payment providers, inventory policies, order state transitions, retry limits, and regional routing. Rollouts must preserve backwards-compatible order events.
    * **Explain the data flow**
      Checkout request uses idempotency key, validates price, reserves inventory, authorizes payment, writes order, emits event, and downstream systems fulfill/notify asynchronously.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Distributed transaction vs saga**
      Two-phase commit gives atomicity but hurts availability and couples teams. Saga is more available and practical, but needs compensation. Use saga with explicit states and reconciliation.
    * **Duplicate checkout**
      Network retries can create double orders. Use idempotency keys keyed by customer/cart/payment attempt and return prior result.
    * **Inventory correctness**
      Strong global inventory prevents oversell but adds latency. Reservation with TTL is faster and practical, with reconciliation and customer-friendly fallback.

## 17. Design Distributed Inventory Management

* **Question**
  Design an inventory service for millions of SKUs across many warehouses and regions.

* **Answer**
  * **Scope**
    Track available, reserved, sold, returned, damaged, and inbound inventory by SKU/location. Support reservations and real-time availability reads.
  * **Functional Requirements**
    Update stock, reserve items, expire reservations, commit sales, release stock, ingest warehouse events, and publish availability to catalog/search.
  * **Non Functional Requirements**
    High read scale, correctness under concurrency, low reservation latency, eventual reconciliation, auditability, and regional partition tolerance.
  * **High level design and diagram (at block level)**
    ```text
    Warehouse/Orders -> Inventory API -> SKU-Location Partitions -> Inventory Ledger
                                                |                    |
                                                v                    v
                                       Reservation Store       Availability Views
                                                |
                                                v
                                      Events/Reconciliation/Search
    ```
    * **Explain the blocks**
      Inventory API handles mutations. SKU-Location Partitions serialize updates per item/location. Ledger records every stock movement. Reservation Store tracks holds with TTL. Availability Views serve fast reads.
    * **Explain the control flow**
      Business config defines reservation TTL, oversell policy, warehouse priority, and reconciliation rules. Partition ownership and rebalancing are controlled centrally.
    * **Explain the data flow**
      Order reserves inventory against a SKU/location partition. Reservation decreases available and records hold. Payment/order commit converts reservation to sold. Expiry releases stock. Events update search/catalog.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Ledger vs mutable counters**
      Mutable counters are fast but hard to audit. Ledger is auditable and reconcilable but requires materialized views. Use ledger as source of truth plus cached counters.
    * **Oversell policy**
      Zero oversell requires strict coordination and may reject valid orders. Bounded oversell improves availability but risks customer harm. Use category-specific policy.
    * **Hot SKUs**
      Flash sales overload one key. Use reservation pools, regional allocations, queueing, and admission control.

## 18. Design Product Search For Amazon Catalog

* **Question**
  Design a product search system for a massive e-commerce catalog.

* **Answer**
  * **Scope**
    Search by text, filters, sorting, ranking, autocomplete, personalization, price/inventory freshness, and indexing pipeline.
  * **Functional Requirements**
    Ingest catalog updates, build indexes, answer search queries, rank results, filter by attributes, update price/availability, and run experiments.
  * **Non Functional Requirements**
    Low query latency, high availability, high relevance, freshness for price/stock, scalable indexing, and graceful degradation.
  * **High level design and diagram (at block level)**
    ```text
    Catalog/Inventory/Price -> Event Stream -> Index Pipeline -> Search Index Shards

    User -> Search API -> Query Parser -> Retrieval -> Ranking -> Results
                                 |             |
                                 v             v
                         Autocomplete      Feature Store
    ```
    * **Explain the blocks**
      Index Pipeline normalizes product documents. Search Index Shards store inverted indexes. Query Parser handles tokens/filters. Retrieval finds candidates. Ranking orders them. Feature Store provides personalization/business features.
    * **Explain the control flow**
      Search relevance teams configure analyzers, ranking models, boosts, synonyms, and experiments. Index schema changes are versioned and rolled out with backfills.
    * **Explain the data flow**
      Catalog events update index documents. User query is parsed, routed to shards, candidates are retrieved, ranking model scores results, price/inventory overlays are applied, and response is returned.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Freshness vs relevance**
      Full reindexing gives consistent documents but is slow. Partial updates improve freshness but complicate index correctness. Keep fast-changing price/inventory in separate overlay service.
    * **Caching**
      Caching popular queries lowers latency, but personalization reduces hit rate. Cache unpersonalized candidate sets and rerank per user.
    * **Index partitioning**
      Partition by term/doc can scale differently. Term partitioning complicates ranking; document partitioning requires fanout. Most search systems shard documents and merge top-K.

## 19. Design Recommendation Platform

* **Question**
  Design a recommendation platform for personalized product recommendations.

* **Answer**
  * **Scope**
    Collect user events, train models, compute candidates, serve low-latency recommendations, run experiments, and monitor quality.
  * **Functional Requirements**
    Ingest behavior, build features, train models, generate candidates, rank results, serve APIs, handle cold start, and measure outcomes.
  * **Non Functional Requirements**
    Low serving latency, high availability, model freshness, feature consistency, privacy, explainability enough for operations, and safe experimentation.
  * **High level design and diagram (at block level)**
    ```text
    Clicks/Orders -> Event Stream -> Feature Pipeline -> Offline Store
                                         |              |
                                         v              v
                                   Online Store     Training Jobs -> Model Registry
                                                          |
    User -> Rec API -> Candidate Gen -> Ranker -> Filters/Rules -> Results
    ```
    * **Explain the blocks**
      Event Stream captures behavior. Feature Pipeline computes online/offline features. Training Jobs build models. Model Registry versions models. Candidate Gen retrieves possible items. Ranker scores them. Filters enforce policy.
    * **Explain the control flow**
      ML teams publish feature definitions and models through registry, run offline validation, canary online traffic, and roll back on metric regressions.
    * **Explain the data flow**
      User events update features. Training uses historical data. Serving request fetches user/context features, gets candidates, ranks items, applies business rules, and logs impressions/clicks.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Batch vs real-time features**
      Batch features are cheap and stable but stale. Real-time features improve relevance but are harder to operate. Use hybrid: batch baseline plus streaming deltas.
    * **Candidate generation vs ranking**
      One big model is simple conceptually but too slow. Two-stage candidate plus ranker is scalable but can lose good items early. Monitor recall at candidate stage.
    * **Feedback loops**
      Recommenders can overfit to their own exposure. Use exploration, holdouts, diversity rules, and counterfactual evaluation.

## 20. Design Notification System For Prime Day Scale

* **Question**
  Design a multi-channel notification platform for very high-volume events.

* **Answer**
  * **Scope**
    Email, push, SMS, in-app, templates, user preferences, scheduling, campaigns, retries, dedupe, compliance, and provider failover.
  * **Functional Requirements**
    Accept notification intents, select channels, render templates, enforce preferences/quiet hours, rate-limit, send through providers, track status, and retry safely.
  * **Non Functional Requirements**
    High throughput, high availability, at-least-once processing, best-effort dedupe, low provider blast radius, compliance, observability, and cost control.
  * **High level design and diagram (at block level)**
    ```text
    Product Services -> Ingest API -> Durable Stream -> Orchestrator
                                               |        |       |
                                               v        v       v
                                      Preferences  Policy  Template Render
                                               |
                                               v
                        Channel Queues -> Email/Push/SMS Workers -> Providers
                                               |
                                               v
                                      Status / Analytics / DLQ
    ```
    * **Explain the blocks**
      Ingest API validates requests. Durable Stream buffers spikes. Orchestrator applies preferences/policy/dedupe. Template Render personalizes content. Channel Queues isolate providers. Workers send and record status. DLQ captures failures.
    * **Explain the control flow**
      Product teams configure templates, campaigns, quotas, experiments, and compliance rules in a control plane. Config is versioned, approved, and published to runtime caches.
    * **Explain the data flow**
      Product emits intent with idempotency key. Orchestrator validates policy, selects channel, renders message, enqueues channel task, worker sends to provider, callback/status updates analytics.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Exactly-once delivery**
      True exactly-once is unrealistic with third-party providers. At-least-once with idempotency and dedupe is practical. Communicate product guarantee as no intentional duplicates and best-effort dedupe.
    * **Provider throttling**
      Blind retries can worsen outages. Use per-provider queues, adaptive rate limits, circuit breakers, and fallback providers when allowed.
    * **Preference/compliance correctness**
      Cached preferences are fast but can be stale. Strong lookups are safer but increase latency. Use cache with short TTL for normal messages and stronger checks for regulated/unsubscribe-sensitive cases.

## 21. Design An Enterprise AI Agent Platform

* **Question**
  Design a secure enterprise AI agent platform like Amazon Bedrock AgentCore that lets teams deploy, run, observe, and govern long-running agents with tools, memory, identity, policy, evaluation, and operational isolation.

* **Answer**
  * **Scope**
    Support hosted agent runtimes, short and long-running sessions, tool and API access, browser/code execution, memory, identity, audit, observability, evaluation, and marketplace or internal agent sharing. Exclude model training internals except model selection and runtime compatibility.
  * **Functional Requirements**
    Register agent versions, start sessions, invoke models and tools, persist memory, call enterprise APIs with scoped identity, run sandboxed code/browser actions, stream status, collect traces, evaluate behavior, and roll back unsafe releases.
  * **Non Functional Requirements**
    Strong tenant isolation, least-privilege identity, deterministic audit trails, safe tool execution, resumable long-running workflows, predictable latency for interactive steps, durable execution for background tasks, cost attribution, and fast rollback.
  * **High level design and diagram (at block level)**
    ```text
    User / App / Event -> Agent Gateway -> Auth, Quota, Policy
                                      |
                                      v
                            Agent Runtime Orchestrator
                                      |
            +-------------------------+--------------------------+
            v                         v                          v
      Model Gateway             Tool Gateway               Memory Service
            |                         |                          |
            v                         v                          v
      Foundation Models       APIs / MCP / Code / Browser   Session + Long-Term Memory

    Control plane:
    Agent Registry -> Versioning -> Eval/Guardrails -> Rollout
    Identity Broker -> Secrets Vault -> Audit Log
    Observability -> Traces, Metrics, Cost, Failure Analysis
    ```
    * **Explain the blocks**
      Agent Gateway authenticates callers, validates tenant/project policy, applies quotas, and creates resumable sessions. Agent Runtime Orchestrator owns planning loops, turn budgets, cancellation, checkpointing, and session state. Model Gateway normalizes access to model providers and applies model routing policy. Tool Gateway exposes enterprise APIs, MCP-style tools, code execution, and browser automation through scoped capabilities. Memory Service stores session state, user or workflow memory, retrieval indexes, and retention policy. Agent Registry versions agent prompts, tools, models, policies, and deployment metadata. Eval/Guardrails run pre-release and online checks. Identity Broker maps user or service identity to least-privilege tool credentials.
    * **Core components and low-level design**
      * **Agent registry and version contract**
        Stores agent ID, version, owner, model policy, tool allowlist, memory schema, identity scopes, guardrails, eval suite, rollout state, and artifact hashes. The invariant is that a runtime session pins an immutable agent version so rollback or config edits do not change behavior mid-session.
      * **Runtime state machine**
        Tracks `Created -> Running -> WaitingForTool -> WaitingForHuman -> Checkpointed -> Completed | Failed | Canceled`. Each transition is idempotent and guarded by a session epoch so retries cannot duplicate external side effects.
      * **Tool gateway**
        Validates tool schema, target resource, identity scope, payload size, network destination, and risk class before execution. Read-only and low-risk calls can be auto-approved by policy; external writes, broad data access, secret use, and destructive actions require stronger gates.
      * **Stateful policy engine**
        Evaluates user, tenant, agent version, target resource, historical interaction, and temporal context before a tool call executes. Keep the policy state outside the model, version every policy bundle, and record the decision inputs so a later audit can explain why a call was allowed, denied, or escalated.
      * **AI traffic gateway**
        Applies per-agent, per-tool, per-user, and per-tenant request limits before model or tool execution. Limits should support burst and sustained windows, fail-closed behavior for high-risk tools, graceful degradation for read-only tools, and response metadata that lets callers back off instead of retrying blindly.
      * **Memory service**
        Separates ephemeral scratchpad, durable session transcript, user/workflow memory, and retrieval indexes. Writes carry source, timestamp, tenant, retention class, and redaction state so operators can audit why future agent turns used a fact.
      * **Evaluation and rollout controller**
        Runs offline regression suites, tool-call simulations, policy checks, adversarial prompts, and cost/latency smoke tests before deployment. Online rollout uses shadow traffic, tenant allowlists, canaries, automated halt, and rollback to the previous pinned version.
    * **Explain the control flow**
      Admins define org-level tool policy, model allowlists, identity scopes, retention, approval rules, and audit requirements. Agent owners register versions with prompt, tools, memory, guardrails, and eval suites. The control plane validates the package, runs evaluations, and promotes it through dev, canary, and production stages. Runtime policy snapshots are published to gateways and workers with versioned config.
    * **Explain the data flow**
      A user or event starts a session through the gateway. The runtime loads the pinned agent version, retrieves relevant memory, invokes a model, requests tool calls through the gateway, stores traces and checkpoints, and streams progress. Tool results, memory writes, model responses, and human approvals flow into the audit log and observability pipeline. Completion emits a final result, callback, ticket, workflow state, or artifact.
  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Hosted runtime vs customer-managed runtime**
      Hosted runtime gives consistent isolation, observability, scaling, and policy enforcement, but requires customers to trust the platform with prompts, traces, and tool access. Customer-managed runtime improves data residency and environment control, but weakens central governance and increases operational burden. Prefer hosted runtime for standard enterprise agents, with private networking, customer-managed keys, and strict retention controls for sensitive tenants.
    * **Tool safety and side-effect control**
      Agents become risky when model output can mutate external systems. Static allowlists are predictable but too coarse; per-call human approval is safe but slow; risk scoring reduces friction but must be transparent. Use structured tool schemas, scoped identity, idempotency keys, dry-run modes, target-resource policy, and explicit approval for high-risk writes.
    * **Stateless policy vs temporal policy**
      Stateless checks are fast and simple, but miss risk that only appears after a sequence of steps, such as repeated failed access attempts, progressively broader data reads, or a tool call that is only safe after a prior confirmation. Temporal policy uses session history and recent actions to make better decisions, but adds state, latency, and replay complexity. Prefer a hybrid: fast stateless checks for every call, plus temporal policy for high-risk tools, sensitive resources, elevated scopes, and anomalous behavior.
    * **Gateway-level rate limits vs per-tool limits**
      Per-tool limits protect fragile backends, but they do not see aggregate agent behavior across tools. Gateway-level limits provide one enforcement point for tenant budgets and runaway loops, but can become too blunt if all tools share one quota. Use layered limits: global tenant budget, per-agent concurrency, per-tool burst/sustained windows, and explicit retry-after signals.
    * **Memory correctness and privacy**
      Long-term memory improves personalization and workflow continuity, but stale or overbroad memory can leak data or create wrong decisions. Keep memory typed, source-linked, tenant-scoped, time-bounded, and editable. Separate retrieval memory from authoritative system records, and require tools to re-read critical state before acting.
    * **Evaluation coverage vs release velocity**
      Exhaustive evaluation catches regressions but slows iteration. Thin smoke tests release quickly but miss tool and policy failures. Use layered gates: required policy and schema checks for every change, focused regression suites per agent family, shadow traffic before broad rollout, and online SLO/error/cost guardrails that can halt releases automatically.

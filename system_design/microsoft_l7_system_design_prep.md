# Microsoft L7 System Design Interview Prep

These are representative Microsoft and Azure-style system design prompts, not an official question bank. This is a living, count-neutral prep set that can be expanded or refined over time. The answers are tuned for senior/staff/principal interviews where the signal is architecture judgment, tradeoffs, reliability, security, cost, and operational ownership.

Useful public references:
- https://www.designgurus.io/blog/microsoft-system-design-interview-questions
- https://www.systemdesignhandbook.com/guides/microsoft-system-design-interview/
- https://www.codinginterview.com/guide/microsoft-system-design-interview-questions/
- https://www.geeksforgeeks.org/system-design/microsoft-system-design-interview-questions/
- https://devblogs.microsoft.com/foundry/whats-new-in-microsoft-foundry-build-2026/
- https://azure.microsoft.com/en-us/products/ai-foundry/agent-service/
- https://learn.microsoft.com/en-us/azure/foundry/agents/how-to/tools/model-context-protocol
- https://learn.microsoft.com/en-us/azure/foundry/mcp/build-your-own-mcp-server
- https://devblogs.microsoft.com/foundry/build-2026-from-observability-to-roi-for-ai-agents-on-any-framework/

## 1. Design OneDrive / Cloud File Sync

* Question

Design a cloud file storage and sync service like OneDrive.

* Answer

** Scope

Support file upload, download, folder hierarchy, sharing, version history, device sync, conflict handling, and enterprise policy enforcement. Exclude full Office collaborative editing unless the interviewer asks.

** Functional Requirements

- Upload/download large files.
- Sync files across devices.
- Maintain folders, metadata, versions, and deletes.
- Share files with users or groups.
- Detect and resolve concurrent updates.
- Support search and audit logs.

** Non Functional Requirements

- High durability for file content.
- Low-latency metadata operations.
- Eventual global replication for content.
- Strong consistency for file metadata within a drive.
- Secure tenant isolation, encryption, access control, and auditability.

** High level design and diagram (at block level)

```text
[Clients]
   |
[Edge/API Gateway]
   |
+----------------+       +----------------+       +----------------+
| Upload/Sync API| ----> | Metadata Svc   | ----> | Metadata Store |
+----------------+       +----------------+       +----------------+
        |                         |
        v                         v
+----------------+       +----------------+
| Chunk Service  | ----> | Blob Storage   |
+----------------+       +----------------+
        |
        v
+----------------+       +----------------+       +----------------+
| Change Log     | ----> | Sync Notifier  | ----> | Devices        |
+----------------+       +----------------+       +----------------+
```

*** Explain the blocks

- Clients: desktop, mobile, and web sync agents.
- Edge/API Gateway: auth, throttling, routing, request validation.
- Upload/Sync API: manages upload sessions, delta sync, and download links.
- Metadata Service: owns folders, file records, permissions, versions, tombstones.
- Chunk Service: splits files into blocks, deduplicates optional chunks, verifies checksums.
- Blob Storage: durable object or block storage.
- Change Log: append-only stream of mutations per drive or folder.
- Sync Notifier: pushes changes to devices or lets clients poll deltas.

*** Explain the control flow

Admins configure tenant policies, sharing rules, retention, encryption, and quotas. Product/control APIs validate changes and publish versioned config to the runtime services. The sync path reads cached policy and quota decisions so file operations do not depend on slow admin systems.

*** Explain the data flow

For upload, the client requests an upload session, uploads chunks to the chunk service, the service writes blobs and verifies checksums, then commits metadata atomically. The metadata service appends a change event. Sync clients consume the change log or receive notifications and fetch the changed metadata/content.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Conflict resolution

Problem: two devices can edit the same file offline and later sync conflicting versions.

- Last-writer-wins plus conflict copy: simple and user-safe because no data is deleted; cons are duplicate files and user cleanup.
- Version vectors per device: detects causality and avoids false conflicts; cons are more metadata and harder client logic.
- File locks/check-out: prevents conflicts for some file types; cons are poor offline UX and lock expiry edge cases.

Recommended answer: use version vectors to detect conflicts, preserve both versions, and expose conflict resolution in clients. Use locks only for special enterprise workflows.

## 2. Design Microsoft Teams Chat

* Question

Design a real-time chat system like Microsoft Teams.

* Answer

** Scope

Support one-to-one chats, group chats, channels, message history, read receipts, reactions, search, notifications, and compliance retention. Exclude audio/video unless explicitly requested.

** Functional Requirements

- Send and receive messages in near real time.
- Store conversation history.
- Support channels with many members.
- Track read receipts, edits, deletes, reactions, and mentions.
- Search messages with permission filtering.
- Notify offline users.

** Non Functional Requirements

- Low latency for active conversations.
- High availability across regions.
- Durable message storage.
- Ordered messages per conversation or per channel.
- Enterprise compliance, eDiscovery, retention, and audit.

** High level design and diagram (at block level)

```text
[Clients]
   |
[WebSocket Gateway] ---- [REST API Gateway]
   |                         |
   v                         v
[Presence Svc]          [Message Svc]
                            |
        +-------------------+-------------------+
        v                   v                   v
 [Message Store]      [Event Stream]      [Search Indexer]
                            |
              +-------------+-------------+
              v                           v
        [Fanout Workers]            [Notification Svc]
              |                           |
              v                           v
        [Recipient Sessions]        [Push/Email]
```

*** Explain the blocks

- WebSocket Gateway: maintains live client connections.
- REST API Gateway: handles send, edit, delete, history fetch.
- Message Service: validates membership, assigns sequence numbers, persists messages.
- Message Store: partitioned by conversation or channel.
- Event Stream: durable events for fanout, search, compliance, notifications.
- Fanout Workers: deliver to connected users.
- Search Indexer: builds inverted index with ACL metadata.
- Notification Service: sends push/email for offline users.

*** Explain the control flow

Tenant admins configure retention, external access, compliance policies, and channel settings. These policies are stored in a control plane and cached by message, search, and notification services. Membership changes flow through a directory/group service and update authorization caches.

*** Explain the data flow

A client sends a message to the gateway. The message service checks auth and membership, assigns an ordered sequence, writes to the message store, and publishes an event. Fanout workers deliver to online recipients. Notification workers handle offline users. Indexers asynchronously update search.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Fanout strategy

Problem: small chats need instant delivery, but large channels can have thousands of members.

- Fanout on write: fast reads and simple unread counts; cons are expensive writes for large channels.
- Fanout on read: cheap writes and good for huge channels; cons are slower reads and more client merge work.
- Hybrid: fanout small conversations, use read-time fetch for large channels; cons are more operational complexity.

Recommended answer: hybrid. Use fanout-on-write for direct/group chats and fanout-on-read for large channels.

## 3. Design Teams Video Conferencing

* Question

Design a video conferencing service like Microsoft Teams Meetings.

* Answer

** Scope

Support meeting creation, join, participant state, audio/video streams, screen sharing, recording hooks, and regional media routing. Exclude advanced transcription unless asked.

** Functional Requirements

- Create/join meetings.
- Negotiate media capabilities.
- Send audio, video, and screen share.
- Support participant roster and mute state.
- Adapt to network quality.
- Handle recording and compliance controls.

** Non Functional Requirements

- Very low latency and jitter.
- High availability for signaling and media.
- Regional routing close to users.
- Graceful degradation under packet loss.
- Security through encryption and access control.

** High level design and diagram (at block level)

```text
[Clients]
   |
[Meeting/API Gateway]
   |
[Signaling Service] ---- [Meeting State Store]
   |
   v
[Media Router Selector]
   |
   v
[Regional SFU Cluster] ---- [Recording/Compliance Pipeline]
   |
   v
[Other Participants]
```

*** Explain the blocks

- Meeting/API Gateway: auth, meeting creation, join requests.
- Signaling Service: session negotiation, ICE candidates, roster changes.
- Meeting State Store: meeting metadata and participant state.
- Media Router Selector: chooses the best media region/SFU.
- SFU Cluster: forwards media streams without full decode/reencode.
- Recording Pipeline: optional capture for recording and compliance.

*** Explain the control flow

The control plane creates meeting objects, validates join permissions, selects media regions, and manages participant state. Policy controls recording, external participants, lobby, and compliance. Control messages are small but correctness-sensitive.

*** Explain the data flow

Clients exchange signaling through the signaling service, then send encrypted RTP media to the selected SFU. The SFU forwards selected streams to participants and adapts layers based on bandwidth. Metrics flow to monitoring for quality decisions.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Media architecture

Problem: media traffic is high-bandwidth and latency-sensitive.

- Peer-to-peer: lowest infrastructure cost for two people; cons are poor NAT traversal and bad scaling for groups.
- MCU: server mixes all streams into one; pros are simple clients and stable layout, cons are high server CPU and latency.
- SFU: forwards streams and lets clients decode; pros are scalable and low latency, cons are more client complexity.

Recommended answer: use SFU for most meetings, with P2P optimization for small calls if worth the complexity.

## 4. Design Outlook / Email Delivery

* Question

Design a cloud email delivery and mailbox system like Outlook/Exchange Online.

* Answer

** Scope

Support inbound and outbound email, mailbox storage, spam filtering, search, folders, delivery retries, and enterprise policy. Exclude calendar unless asked.

** Functional Requirements

- Receive and send email.
- Store mailbox messages and attachments.
- Support folders, labels, rules, and search.
- Filter spam, malware, and phishing.
- Retry transient delivery failures.
- Generate audit and compliance records.

** Non Functional Requirements

- High deliverability and durability.
- Per-mailbox consistency.
- Strong security and abuse protection.
- Large-scale indexing.
- Compliance retention and legal hold.

** High level design and diagram (at block level)

```text
[Internet SMTP]
   |
[SMTP Edge Gateways]
   |
[Spam/Malware/Policy Filters]
   |
[Delivery Queue]
   |
[Mailbox Service] ---- [Mailbox Store]
   |                         |
   v                         v
[Search Indexer]       [Attachment Store]
   |
[Clients / Notifications]
```

*** Explain the blocks

- SMTP Edge Gateways: receive/send SMTP traffic.
- Filters: reputation, spam, malware, DLP, tenant policy.
- Delivery Queue: durable buffer for retry and backpressure.
- Mailbox Service: applies mailbox rules and writes messages.
- Mailbox Store: partitioned durable storage by mailbox.
- Search Indexer: asynchronously indexes body and metadata.
- Attachment Store: optimized blob store for large attachments.

*** Explain the control flow

Admins configure anti-spam, retention, transport rules, DLP, and legal hold. Policies are validated and pushed to filters and mailbox services. Outbound reputation and throttling controls are managed separately.

*** Explain the data flow

Inbound mail enters SMTP edge, passes filters, is placed on a durable queue, and is delivered to the mailbox store. The mailbox service applies rules, triggers notifications, and emits events for indexing and compliance.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Filtering latency versus security

Problem: deep scanning improves security but can delay delivery.

- Inline full scanning: best protection before delivery; cons are higher latency and risk of filter outages blocking mail.
- Async post-delivery scanning: faster delivery; cons are unsafe messages may briefly appear.
- Tiered scanning: quick inline checks plus deeper async scans; cons are more complex states.

Recommended answer: tiered scanning with quarantine and recall capability.

## 5. Design Azure Blob Storage / Object Storage

* Question

Design a durable object storage service like Azure Blob Storage.

* Answer

** Scope

Support buckets/containers, objects/blobs, multipart upload, versioning, access control, lifecycle policies, replication, and large object reads/writes.

** Functional Requirements

- Put, get, delete, list objects.
- Upload large objects in parts.
- Maintain metadata, versions, and leases.
- Support access policies and signed URLs.
- Replicate data across zones or regions.
- Provide lifecycle transitions and retention.

** Non Functional Requirements

- Extremely high durability.
- High availability.
- Cost-efficient storage.
- Predictable read/write throughput.
- Strong tenant isolation and encryption.

** High level design and diagram (at block level)

```text
[Clients]
   |
[Front Door/API Gateway]
   |
[Namespace Service] ---- [Metadata Store]
   |
[Placement Service]
   |
[Storage Nodes / Erasure Sets]
   |
[Replication + Repair Manager]
```

*** Explain the blocks

- Front Door/API Gateway: auth, request routing, throttling.
- Namespace Service: containers, object names, metadata, versions.
- Metadata Store: strongly consistent namespace data.
- Placement Service: maps object chunks to storage nodes.
- Storage Nodes: store chunks or erasure-coded fragments.
- Replication/Repair Manager: detects failures and rebuilds copies.

*** Explain the control flow

Control plane manages accounts, containers, access policies, lifecycle rules, replication policy, and capacity placement. Config is versioned and pushed to front doors, namespace, and repair managers.

*** Explain the data flow

For PUT, the client sends object data to front door. The namespace service allocates an object/version, placement selects nodes, storage nodes persist chunks, and metadata is committed after durability quorum. For GET, metadata resolves chunk locations and streams data from storage nodes.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Replication versus erasure coding

Problem: storage needs extreme durability at reasonable cost.

- Full replication: simple, fast reads, easy repair; cons are 3x or higher storage cost.
- Erasure coding: lower cost for same durability; cons are more CPU, more complex repair, slower small reads.
- Hybrid: replicate hot data, erasure-code cold data; cons are lifecycle complexity.

Recommended answer: hybrid based on object size, temperature, and tier.

## 6. Design a Globally Distributed Database Like Cosmos DB

* Question

Design a globally distributed NoSQL database with configurable consistency.

* Answer

** Scope

Support partitioned key-value/document storage, global replication, configurable consistency, change feed, backups, and multi-tenant quotas.

** Functional Requirements

- Read/write documents by key.
- Query within partition and secondary indexes where supported.
- Replicate data across regions.
- Offer multiple consistency levels.
- Handle partition split/merge.
- Support change feed and backup/restore.

** Non Functional Requirements

- Low latency globally.
- High availability during regional failures.
- Predictable throughput per tenant.
- Strong isolation and encryption.
- Clear consistency contracts.

** High level design and diagram (at block level)

```text
[Clients/SDK]
   |
[Global Front Door]
   |
[Partition Router]
   |
+------------------+      +------------------+
| Region A Replicas| <--> | Region B Replicas|
+------------------+      +------------------+
        |                         |
        v                         v
 [Log + Storage]           [Log + Storage]
        |
[Change Feed / Backup / Indexing]
```

*** Explain the blocks

- Client SDK: routing cache, retries, consistency selection.
- Global Front Door: region routing and failover.
- Partition Router: maps partition keys to replica sets.
- Replica Sets: maintain logs and storage for each partition.
- Change Feed: ordered partition changes for downstream systems.
- Backup/Indexing: async durability and query support.

*** Explain the control flow

Control plane manages accounts, regions, throughput, consistency defaults, partition maps, and failover policy. Partition map changes are versioned and propagated to routers and SDKs.

*** Explain the data flow

Writes route to the owning partition and replication protocol. Depending on consistency, the system waits for leader/quorum/local commit before acknowledging. Reads route to local or quorum replicas based on requested consistency.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Consistency levels

Problem: global apps need a balance of correctness, latency, and availability.

- Strong: easiest programming model; cons are high cross-region latency and lower availability.
- Bounded staleness: predictable lag; cons are more complex to reason about.
- Session: read-your-writes for a user; cons require session tokens.
- Eventual: lowest latency and highest availability; cons allow stale/conflicting reads.

Recommended answer: make session consistency default, allow stronger modes per workload.

## 7. Design a Distributed Cache

* Question

Design a distributed cache service similar to Azure Cache for Redis.

* Answer

** Scope

Support get/set/delete, TTL, eviction, sharding, replication, failover, metrics, and tenant quotas. Exclude full Redis command compatibility unless asked.

** Functional Requirements

- Store key-value pairs with TTL.
- Serve low-latency reads/writes.
- Evict under memory pressure.
- Replicate for availability.
- Support cluster scaling and failover.
- Provide metrics and admin APIs.

** Non Functional Requirements

- Sub-millisecond to low-millisecond latency.
- High throughput.
- Graceful degradation under node failure.
- Bounded memory usage.
- Isolation between tenants.

** High level design and diagram (at block level)

```text
[Clients]
   |
[Cache SDK / Proxy]
   |
[Shard Map Service]
   |
+-----------+   +-----------+   +-----------+
| Shard 1   |   | Shard 2   |   | Shard N   |
| Primary   |   | Primary   |   | Primary   |
| Replica   |   | Replica   |   | Replica   |
+-----------+   +-----------+   +-----------+
```

*** Explain the blocks

- Cache SDK/Proxy: routes keys via consistent hashing or shard map.
- Shard Map Service: owns membership and shard assignments.
- Cache Shards: in-memory key-value stores.
- Replicas: serve failover and optionally reads.
- Metrics/Admin: observability, scaling, and tenant quotas.

*** Explain the control flow

Control plane provisions clusters, assigns shards, manages failover, and pushes shard-map versions to proxies or SDKs. Scaling triggers resharding with controlled key movement.

*** Explain the data flow

Client computes or fetches shard mapping, sends get/set to the target shard, and receives response. Writes replicate to a secondary synchronously or asynchronously depending on durability requirement.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Cache consistency and invalidation

Problem: cached data can become stale after source-of-truth updates.

- Cache-aside: app reads DB on miss and writes cache; pros simple, cons stale data and stampedes.
- Write-through: writes update cache and DB together; pros fresher cache, cons higher write latency.
- Event invalidation: DB updates publish invalidation; pros good freshness, cons event loss/order complexity.

Recommended answer: cache-aside plus TTL for general use, event invalidation for critical hot objects.

## 8. Design a CDN / Edge Caching System

* Question

Design a CDN for static and semi-static content.

* Answer

** Scope

Support global edge caching, cache invalidation, origin shielding, TLS, routing, metrics, and tenant-specific cache rules.

** Functional Requirements

- Route users to nearby edge nodes.
- Cache content by URL and headers.
- Fetch from origin on miss.
- Invalidate or purge content.
- Support TLS and custom domains.
- Collect logs and metrics.

** Non Functional Requirements

- Low latency globally.
- High availability during regional failures.
- High cache hit ratio.
- Origin protection under traffic spikes.
- Secure tenant isolation.

** High level design and diagram (at block level)

```text
[Users]
   |
[DNS / Anycast Routing]
   |
[Edge POP]
   |
   +--> [Local Cache]
   |
[Regional Shield Cache]
   |
[Customer Origin]

[CDN Control Plane] --> [Rules/Certs/Invalidations] --> [Edge POPs]
```

*** Explain the blocks

- DNS/Anycast Routing: sends users to nearby healthy edge.
- Edge POP: terminates TLS and serves cached content.
- Local Cache: stores hot content.
- Shield Cache: reduces origin fanout.
- Origin: customer backend or object storage.
- Control Plane: certs, rules, purge, routing policy.

*** Explain the control flow

Customers configure domains, TLS certs, cache rules, origin settings, and purge requests. The control plane validates changes and distributes signed config to edge nodes.

*** Explain the data flow

User request reaches nearest edge. On hit, edge serves content immediately. On miss, edge checks shield, then origin. Response is cached according to rules and returned to the user. Logs stream asynchronously.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Cache freshness

Problem: stale content is bad, but origin fetches are expensive.

- Long TTL: high hit rate and low origin load; cons stale content risk.
- Short TTL: fresher content; cons lower hit rate and more origin traffic.
- Explicit invalidation: fast targeted freshness; cons control-plane scale and propagation delay.
- Revalidation with ETag: balances freshness and bandwidth; cons origin still sees requests.

Recommended answer: combine TTL, purge, and ETag. Use stale-while-revalidate for resilience.

## 9. Design a Distributed Rate Limiter for Azure APIs

* Question

Design a global rate limiting and quota service for cloud APIs.

* Answer

** Scope

Support per-tenant, per-user, per-resource, per-region, and global limits. Include policy updates, bursts, metrics, and enforcement at API gateways.

** Functional Requirements

- Enforce request limits.
- Support token bucket and fixed/sliding windows.
- Allow policy changes without redeploying gateways.
- Emit usage metrics and audit records.
- Support global and regional quotas.

** Non Functional Requirements

- Very low latency in request path.
- Highly available enforcement.
- Reasonable global accuracy.
- Safe fail-open or fail-closed modes by API class.
- Tenant isolation and observability.

** High level design and diagram (at block level)

```text
[API Clients]
   |
[Regional API Gateway] --> [Local Rate Limiter]
   |                              |
   v                              v
[Backend APIs]              [Regional Counter Store]
                                  |
                                  v
                          [Global Aggregator]
                                  |
[Quota Control Plane] ---> [Policy Store] ---> [Gateways]
```

*** Explain the blocks

- API Gateway: enforcement point.
- Local Rate Limiter: fast in-memory or regional check.
- Regional Counter Store: durable counters or token state.
- Global Aggregator: reconciles regional usage.
- Policy Store: versioned limits and tenant rules.
- Control Plane: admin APIs, approvals, rollout.

*** Explain the control flow

Admins or services define quota policies. Control plane validates them, versions them, and pushes to regional gateways. Gateways cache policy to avoid runtime dependency on central config.

*** Explain the data flow

Each API request hits the gateway. Gateway checks local tokens/counters. Allowed requests proceed to backend; denied requests return 429 with retry hints. Usage events flow to regional counters and global aggregation.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Global quota accuracy

Problem: enforcing a global quota across regions can add latency and reduce availability.

- Central counter: accurate; cons high latency and single bottleneck.
- Regional counters: fast and available; cons can overshoot global quota.
- Token pre-allocation by region: bounded overshoot; cons uneven demand can strand capacity.

Recommended answer: pre-allocate quota tokens to regions and rebalance asynchronously. Use central checks only for strict compliance limits.

## 10. Design a Notification Platform

* Question

Design an enterprise notification platform for email, push, SMS, and in-app messages.

* Answer

** Scope

Support many producer teams, templates, preferences, policies, retries, deduplication, delivery status, and multi-region reliability.

** Functional Requirements

- Accept notification requests.
- Render templates.
- Apply user preferences and compliance rules.
- Send through multiple channels.
- Retry transient failures.
- Track delivery status.

** Non Functional Requirements

- High availability.
- At-least-once internal processing.
- Low duplicate rate.
- Scalable fanout.
- Strong audit and abuse controls.

** High level design and diagram (at block level)

```text
[Producer Services]
   |
[Ingestion API]
   |
[Durable Event Stream]
   |
[Notification Orchestrator]
   |
   +--> [Preference/Policy Svc]
   +--> [Template Renderer]
   +--> [Dedupe/Rate Limit]
   |
[Channel Queues]
   |
+----------+   +----------+   +----------+   +-----------+
| Email    |   | Push     |   | SMS      |   | In-App    |
| Worker   |   | Worker   |   | Worker   |   | Worker    |
+----------+   +----------+   +----------+   +-----------+
   |
[Delivery Status / Analytics]
```

*** Explain the blocks

- Ingestion API: auth, validation, idempotency key handling.
- Event Stream: durable buffer and replay source.
- Orchestrator: applies business decisions.
- Preference/Policy Service: user choices, quiet hours, compliance.
- Template Renderer: channel-specific content.
- Channel Queues/Workers: isolate provider failures.
- Status/Analytics: delivery state and reporting.

*** Explain the control flow

Product teams configure templates, campaigns, quotas, and policies in a control plane. Config is approved, versioned, and cached by orchestrators and workers.

*** Explain the data flow

Producer submits a notification. Ingestion stores it in the event stream. Orchestrator checks policy and preferences, renders content, dedupes, then enqueues per-channel work. Workers send to providers and emit status events.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Delivery semantics

Problem: third-party providers make true exactly-once delivery impossible end to end.

- At-most-once: no duplicates; cons lost notifications.
- At-least-once: reliable delivery; cons duplicates possible.
- Exactly-once internal state: good for internal transitions; cons does not cover external providers.

Recommended answer: at-least-once processing with idempotency keys, provider request IDs, dedupe tables, and user-visible duplicate suppression.

## 11. Design Azure Monitor / Observability Platform

* Question

Design a cloud observability platform for logs, metrics, traces, dashboards, and alerts.

* Answer

** Scope

Support telemetry ingestion from many services, real-time alerting, log search, time-series metrics, trace correlation, retention, and tenant isolation.

** Functional Requirements

- Ingest logs, metrics, and traces.
- Query recent and historical telemetry.
- Trigger alerts.
- Support dashboards.
- Correlate traces across services.
- Enforce tenant access control.

** Non Functional Requirements

- High ingestion throughput.
- Low-latency alerts.
- Cost-efficient retention.
- Query isolation between tenants.
- High cardinality controls.

** High level design and diagram (at block level)

```text
[Agents/SDKs]
   |
[Regional Ingestion Gateway]
   |
[Validation/Sampling/Enrichment]
   |
+----------------+   +----------------+   +----------------+
| Metrics Store  |   | Log Store      |   | Trace Store    |
+----------------+   +----------------+   +----------------+
        |                   |                   |
        +-------------------+-------------------+
                            |
                     [Query + Alert Engine]
                            |
                     [Dashboards / APIs]
```

*** Explain the blocks

- Agents/SDKs: emit telemetry.
- Ingestion Gateway: auth, throttling, batching.
- Enrichment: adds tenant, region, resource metadata.
- Metrics Store: time-series optimized.
- Log Store: indexed or columnar log search.
- Trace Store: span graph and correlation.
- Query/Alert Engine: queries and rule evaluation.

*** Explain the control flow

Users define workspaces, retention, alert rules, sampling policy, and access control. Control plane pushes configs to ingestion and alert engines.

*** Explain the data flow

Telemetry arrives in batches, is validated and enriched, then routed to the right store. Metrics are aggregated, logs are indexed, traces are linked by trace ID. Alert engine evaluates streams or windows and sends notifications.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Cost versus fidelity

Problem: full telemetry is expensive, but missing data hurts incident response.

- Keep everything: best debugging; cons very expensive.
- Head sampling: predictable cost; cons misses rare errors.
- Tail sampling: captures interesting traces; cons requires buffering and more complex ingestion.
- Tiered retention: hot recent data and cold archived data; cons query complexity.

Recommended answer: tiered retention plus adaptive sampling, with no sampling for critical errors and security logs.

## 12. Design a Distributed Job Scheduler

* Question

Design a distributed job scheduler for batch and recurring cloud jobs.

* Answer

** Scope

Support job submission, cron jobs, dependencies, retries, priorities, worker pools, status tracking, and cancellation.

** Functional Requirements

- Submit and schedule jobs.
- Assign tasks to workers.
- Track job state.
- Retry failures.
- Support dependencies and recurring schedules.
- Cancel or pause jobs.

** Non Functional Requirements

- Highly available scheduler.
- No lost jobs.
- Idempotent execution.
- Fairness across tenants.
- Scalable worker pools.

** High level design and diagram (at block level)

```text
[Users/Services]
   |
[Job API]
   |
[Job Metadata Store]
   |
[Scheduler]
   |
[Task Queue]
   |
[Worker Pool]
   |
[Status/Event Stream]
```

*** Explain the blocks

- Job API: validates submissions and updates.
- Metadata Store: desired state and job history.
- Scheduler: turns jobs into runnable tasks.
- Task Queue: durable dispatch with leases.
- Worker Pool: executes tasks and heartbeats.
- Status Stream: progress, logs, metrics, notifications.

*** Explain the control flow

Control plane configures queues, priorities, worker pools, tenant quotas, and schedule definitions. Scheduler reads desired state and emits runnable tasks based on policy.

*** Explain the data flow

User submits a job. Metadata is committed. Scheduler detects it, enqueues tasks, workers lease tasks, execute, checkpoint, and report status. Failed or expired leases are retried.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Task ownership and retries

Problem: workers can crash after starting work, so tasks can be duplicated or lost.

- Ack before execution: simple; cons lost tasks on crash.
- Ack after execution: no lost tasks; cons duplicate execution possible.
- Leases with heartbeats: bounded ownership and retry; cons requires idempotent tasks and lease tuning.

Recommended answer: leases plus idempotency keys and checkpointing.

## 13. Design a Feature Flag / Config Platform

* Question

Design a feature flag and dynamic configuration platform for many Microsoft services.

* Answer

** Scope

Support flags, targeting, experiments, staged rollout, emergency kill switches, audit, approvals, SDKs, and local evaluation.

** Functional Requirements

- Create and update flags.
- Target by tenant, user, region, app, or percentage.
- Roll out gradually.
- Evaluate flags in application code.
- Audit all changes.
- Roll back quickly.

** Non Functional Requirements

- Extremely low evaluation latency.
- High availability even if control plane is down.
- Safe propagation.
- Strong auditability.
- Consistent SDK behavior.

** High level design and diagram (at block level)

```text
[Admins/Product Teams]
   |
[Config Control Plane]
   |
[Validation + Approval]
   |
[Versioned Config Store]
   |
[Config Distribution Service]
   |
[SDK Local Cache in Services]
```

*** Explain the blocks

- Control Plane: UI/API for flags.
- Validation/Approval: prevents unsafe rules.
- Versioned Store: immutable config versions and audit history.
- Distribution Service: streams or polls config changes.
- SDK Cache: local in-process evaluation.

*** Explain the control flow

A team creates or edits a flag. The control plane validates targeting, records approval, writes a new version, and distributes it. Services receive the update and atomically switch local config versions.

*** Explain the data flow

At runtime, application code calls the SDK. The SDK evaluates locally using cached config and request attributes. No remote call is needed in the hot path.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Local versus remote evaluation

Problem: flag checks happen in latency-sensitive code paths.

- Remote evaluation: centralized and consistent; cons adds latency and outage dependency.
- Local evaluation: fast and resilient; cons config can be stale.
- Hybrid: local default with remote for complex experiments; cons two modes to operate.

Recommended answer: local evaluation for most flags, with bounded staleness and emergency kill-switch propagation.

## 14. Design Microsoft Entra ID / Authentication Service

* Question

Design an enterprise identity and token service like Microsoft Entra ID.

* Answer

** Scope

Support login, MFA, token issuance, refresh tokens, directory lookup, policy evaluation, federation, key rotation, and audit.

** Functional Requirements

- Authenticate users and applications.
- Issue and validate tokens.
- Enforce conditional access policies.
- Support MFA and federation.
- Rotate signing keys.
- Log security events.

** Non Functional Requirements

- Very high availability.
- Low login latency.
- Strong security.
- Global scale.
- Compliance and auditability.

** High level design and diagram (at block level)

```text
[Clients/Apps]
   |
[Auth Front Door]
   |
[Credential + Federation Service]
   |
[Policy/MFA Engine]
   |
[Token Issuer] ---- [Key Management]
   |
[Directory Store] ---- [Audit/Security Logs]
```

*** Explain the blocks

- Auth Front Door: protocol endpoints and routing.
- Credential/Federation Service: password, cert, SAML/OIDC federation.
- Policy/MFA Engine: conditional access and risk checks.
- Token Issuer: creates signed access and refresh tokens.
- Key Management: signing key generation and rotation.
- Directory Store: users, groups, apps, tenants.
- Audit Logs: immutable security trail.

*** Explain the control flow

Admins configure apps, tenant policies, MFA requirements, federation settings, and key rotation. Control changes are audited and pushed globally to policy and token services.

*** Explain the data flow

Client starts auth flow. Auth service validates credentials or federation assertion, evaluates policies and MFA, then token issuer signs tokens. APIs later validate tokens using cached public keys.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Token revocation

Problem: stateless tokens scale well but are hard to revoke immediately.

- Long-lived JWTs: low validation cost; cons revocation delay.
- Short-lived access tokens plus refresh tokens: better security; cons more refresh traffic.
- Introspection on every request: immediate revocation; cons latency and central dependency.

Recommended answer: short-lived access tokens, refresh token revocation, key rollover, and introspection only for high-risk operations.

## 15. Design a Multi-Tenant SaaS Platform

* Question

Design a secure multi-tenant SaaS platform for enterprise customers.

* Answer

** Scope

Support tenant onboarding, tenant isolation, quotas, regional placement, billing, admin roles, audit logs, and shared service operations.

** Functional Requirements

- Create and manage tenants.
- Isolate tenant data.
- Enforce quotas and plans.
- Support tenant admins and RBAC.
- Route requests to correct region/shard.
- Produce usage and billing records.

** Non Functional Requirements

- Strong tenant isolation.
- High availability.
- Noisy-neighbor protection.
- Compliance and data residency.
- Cost-efficient shared infrastructure.

** High level design and diagram (at block level)

```text
[Tenant Users/Admins]
   |
[Global Gateway]
   |
[Tenant Resolver]
   |
+----------------+      +----------------+
| Shared Services| ---> | Tenant Metadata|
+----------------+      +----------------+
   |
[Regional App Clusters]
   |
[Tenant Data Shards] ---- [Usage/Billing/Audit]
```

*** Explain the blocks

- Global Gateway: auth and routing.
- Tenant Resolver: maps tenant to region, shard, plan, policy.
- Tenant Metadata: tenant config and lifecycle state.
- Shared Services: common APIs.
- Regional App Clusters: run tenant workloads.
- Data Shards: store tenant data.
- Usage/Billing/Audit: metering and compliance.

*** Explain the control flow

Tenant lifecycle operations create metadata, assign region/shards, configure quotas, and provision resources. Admin policy changes are versioned and distributed to gateways and services.

*** Explain the data flow

User request reaches gateway, tenant is resolved, auth and policy are checked, then request routes to the correct regional cluster and tenant shard. Usage and audit events are emitted asynchronously.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Tenant isolation model

Problem: enterprises need isolation, but full isolation is expensive.

- Shared database with tenant ID: cheapest; cons highest blast radius and strict query discipline required.
- Separate schema/database per tenant: better isolation; cons operational overhead.
- Dedicated cluster for large tenants: strongest isolation; cons highest cost.

Recommended answer: tiered model. Small tenants share shards, large or regulated tenants get dedicated databases or clusters.

## 16. Design Search for Office Documents

* Question

Design enterprise search across Office documents, Teams files, and OneDrive content.

* Answer

** Scope

Support document ingestion, text extraction, indexing, ranking, ACL filtering, freshness, snippets, and tenant isolation.

** Functional Requirements

- Crawl or receive document change events.
- Extract text and metadata.
- Build searchable indexes.
- Apply permission filtering.
- Rank results.
- Return snippets and facets.

** Non Functional Requirements

- Low query latency.
- Fresh enough indexing.
- No permission leaks.
- Scalable indexing pipeline.
- High recall and relevant ranking.

** High level design and diagram (at block level)

```text
[Content Sources]
   |
[Change Feed / Crawler]
   |
[Extraction Pipeline]
   |
[Indexer] ---- [ACL Index]
   |
[Search Index]
   |
[Query Service] ---- [Ranking Service]
   |
[Clients]
```

*** Explain the blocks

- Content Sources: OneDrive, SharePoint, Teams, email.
- Change Feed/Crawler: detects new and changed documents.
- Extraction Pipeline: parses text, metadata, language, entities.
- Indexer: writes terms and document metadata.
- ACL Index: stores permissions efficiently.
- Query Service: parses query and retrieves candidates.
- Ranking Service: orders and personalizes results.

*** Explain the control flow

Admins configure searchable sources, retention, eDiscovery, and access policies. Indexing rules and schema changes are rolled out through the control plane.

*** Explain the data flow

Document changes enter the pipeline, content is extracted, terms and ACLs are indexed. At query time, the service validates identity, searches candidate documents, filters by ACL, ranks, and returns results.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Permission filtering

Problem: search must not leak documents the user cannot access.

- Post-filtering: simple indexes; cons wasted query work and risk of sparse results.
- Pre-filtering by ACL tokens: faster and safer; cons larger index and complex group expansion.
- Hybrid: pre-filter common ACLs, post-filter edge cases; cons complexity.

Recommended answer: pre-filter using compact ACL tokens, with final authoritative check for sensitive result opens.

## 17. Design an Event Streaming Platform Like Event Hubs

* Question

Design a managed event streaming service.

* Answer

** Scope

Support topics, partitions, producers, consumers, consumer groups, retention, replay, ordering per partition, quotas, and schema compatibility.

** Functional Requirements

- Producers publish events.
- Consumers read events independently.
- Support replay from offsets.
- Retain events by time or size.
- Scale partitions.
- Track consumer group offsets.

** Non Functional Requirements

- High throughput.
- Low publish latency.
- Durable replicated logs.
- Backpressure handling.
- Tenant isolation and quotas.

** High level design and diagram (at block level)

```text
[Producers]
   |
[Broker Front Door]
   |
[Partition Router]
   |
+-------------+   +-------------+   +-------------+
| Partition 0 |   | Partition 1 |   | Partition N |
| Log Replicas|   | Log Replicas|   | Log Replicas|
+-------------+   +-------------+   +-------------+
   |
[Consumers / Consumer Groups]
```

*** Explain the blocks

- Broker Front Door: auth, batching, throttling.
- Partition Router: maps events to partitions.
- Partition Logs: append-only durable logs.
- Replicas: durability and failover.
- Consumer Groups: independent offset tracking.
- Control Plane: topics, partitions, quotas, retention.

*** Explain the control flow

Users create namespaces, topics, partitions, retention policies, and access keys. Control plane assigns partitions and propagates metadata to brokers.

*** Explain the data flow

Producer publishes a batch. Router picks partition by key or round-robin. Broker appends to replicated log and returns offset. Consumers poll or stream from offsets and commit progress.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Ordering and scale

Problem: total ordering limits throughput.

- Global order: simple consumer semantics; cons single partition bottleneck.
- Per-key partition order: scalable and useful for entities; cons no global order.
- No ordering: maximum throughput; cons harder consumers.

Recommended answer: guarantee ordering only within a partition/key. Let users choose partition keys.

## 18. Design a Distributed Lock / Coordination Service

* Question

Design a distributed lock and coordination service for cloud systems.

* Answer

** Scope

Support locks, leases, leader election, watches, fencing tokens, and highly consistent metadata. Exclude general database features.

** Functional Requirements

- Acquire and release locks.
- Expire locks if clients die.
- Renew leases.
- Elect leaders.
- Notify clients of changes.
- Provide fencing tokens.

** Non Functional Requirements

- Strong consistency.
- Correctness over availability.
- Low but not ultra-low latency.
- Survive node failures.
- Simple operational model.

** High level design and diagram (at block level)

```text
[Clients]
   |
[Coordination API]
   |
[Consensus Cluster]
   |
+----------+  +----------+  +----------+
| Node A   |  | Node B   |  | Node C   |
| Raft Log |  | Raft Log |  | Raft Log |
+----------+  +----------+  +----------+
   |
[Watch/Event Service]
```

*** Explain the blocks

- Coordination API: lock, lease, election endpoints.
- Consensus Cluster: replicated state machine.
- Raft Log: ordered mutations.
- Watch Service: notifies clients of state changes.
- Session Manager: tracks TTL and heartbeats.

*** Explain the control flow

Operators configure cluster membership, quorum size, backup, and access control. Membership changes are done carefully through consensus to avoid split brain.

*** Explain the data flow

Client requests a lock. Leader appends lock acquisition to consensus log. Once committed by quorum, client receives lease and fencing token. Client renews periodically. Expiry or release appends another log entry.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Fencing tokens

Problem: a paused client may think it still owns a lock after lease expiry.

- Lock only: easy API; cons stale owner can still write.
- Lease with TTL: recovers from crashes; cons clock and pause edge cases.
- Fencing token: downstream services reject stale writes; cons requires integration with protected resource.

Recommended answer: leases plus monotonically increasing fencing tokens.

## 19. Design Real-Time Collaborative Editing for Word Online

* Question

Design real-time collaborative document editing.

* Answer

** Scope

Support multiple users editing one document, presence, comments, offline/reconnect, version history, and durable saves. Exclude full Office file format internals.

** Functional Requirements

- Apply concurrent edits.
- Show collaborators and cursors.
- Sync edits in near real time.
- Persist snapshots and operation logs.
- Recover after disconnect.
- Preserve document history.

** Non Functional Requirements

- Low editing latency.
- No lost edits.
- Convergent document state.
- Scalable per-document sessions.
- Secure sharing and permissions.

** High level design and diagram (at block level)

```text
[Editors]
   |
[Collab Gateway / WebSocket]
   |
[Document Session Service]
   |
[Operation Log] ---- [Snapshot Store]
   |
[Transform/Merge Engine]
   |
[Presence Service]
```

*** Explain the blocks

- Collab Gateway: live connections.
- Document Session Service: owns active document state.
- Operation Log: durable ordered edit operations.
- Snapshot Store: periodic compacted document state.
- Transform/Merge Engine: resolves concurrent edits.
- Presence Service: cursors, selections, active users.

*** Explain the control flow

Document permissions, sharing, retention, and lock policies are managed in the control plane. Session placement assigns an active document to a region/server and handles failover.

*** Explain the data flow

Client sends edit operation with base version. Session service orders it, transforms/merges against concurrent operations, appends to log, updates in-memory state, and broadcasts transformed operation to collaborators.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** OT versus CRDT

Problem: concurrent edits must converge without losing user intent.

- Operational Transform: mature for centralized editors; cons complex transform functions.
- CRDT: strong convergence and offline support; cons larger metadata and harder compaction.
- Server serialization only: simple; cons poor offline/concurrent behavior.

Recommended answer: OT or CRDT depending on product needs. For Word-like central sessions, OT with durable op logs is a defensible default.

## 20. Design Azure Resource Manager / Cloud Provisioning Control Plane

* Question

Design a cloud resource provisioning control plane like Azure Resource Manager.

* Answer

** Scope

Support resource create/update/delete, templates, validation, policy, RBAC, async workflows, idempotency, auditing, and regional resource providers.

** Functional Requirements

- Accept resource provisioning requests.
- Validate identity, policy, quota, and schema.
- Orchestrate long-running operations.
- Track operation status.
- Retry or roll back failures.
- Audit all changes.

** Non Functional Requirements

- High availability for control plane APIs.
- Idempotent operations.
- Consistent desired state.
- Safe retries and recovery.
- Strong security, RBAC, and audit.

** High level design and diagram (at block level)

```text
[Users/SDK/CLI/Portal]
   |
[Global ARM API Gateway]
   |
[Auth/RBAC/Policy/Quota]
   |
[Desired State Store]
   |
[Workflow Orchestrator]
   |
[Regional Resource Providers]
   |
[Actual Resources / Data Planes]
```

*** Explain the blocks

- Global API Gateway: request entry, throttling, routing.
- Auth/RBAC/Policy/Quota: validates who can do what.
- Desired State Store: durable resource model and operation records.
- Workflow Orchestrator: async state machine for long operations.
- Resource Providers: service-specific provisioning logic.
- Data Planes: actual compute/storage/network resources.

*** Explain the control flow

User submits a create/update/delete request. Control plane validates identity, policy, quota, and schema, writes desired state, starts an operation workflow, and calls resource providers. Status is exposed through operation APIs.

*** Explain the data flow

Most provisioning is control-plane data: resource specs, operation events, and status updates. Actual user data never flows through ARM; it lives in each resource data plane. Providers report actual state back to reconcile with desired state.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Async provisioning and idempotency

Problem: provisioning can take minutes and failures can happen after partial resource creation.

- Synchronous API: simple for users; cons timeouts and poor failure recovery.
- Async workflow: robust and observable; cons more complex state management.
- Saga with compensation: handles partial failures; cons rollback may be imperfect for external resources.

Recommended answer: async workflow with idempotency keys, operation IDs, desired-state reconciliation, and compensating actions where safe.

## 21. Design A Microsoft Foundry Agent Runtime And Tool Platform

* Question

Design a Microsoft Foundry-style enterprise agent platform that can host production agents, connect them to governed tools and enterprise knowledge, publish them into Microsoft 365 surfaces, and operate them with identity, observability, evaluation, and policy controls.

* Answer

** Scope

Support developer-built agents that may use different frameworks and models, run in session-isolated hosted runtimes, call MCP/OpenAPI/toolbox tools, use managed memory and grounding, publish to Teams or Microsoft 365 Copilot, and satisfy enterprise governance requirements. Exclude model training and generic chat UI unless asked.

Public Microsoft context from 2026: Microsoft Foundry material emphasizes hosted agents with sandboxed sessions, durable state, framework flexibility, Toolboxes as MCP-compatible governed tool bundles, Entra Agent ID, private networking, tracing, evaluations, monitor/optimize loops, and agent distribution into Teams and Microsoft 365 Copilot.

** Functional Requirements

- Register agents with owner, tenant, framework, model route, memory policy, tool set, publication targets, rollout state, and approval policy.
- Run each agent invocation in an isolated session with durable state, filesystem workspace, secrets access, and bounded execution time.
- Connect agents to public and private MCP servers, OpenAPI tools, enterprise knowledge, code execution, web/file search, and reusable toolboxes.
- Enforce Entra-based user, tenant, agent, and tool identities with delegated access, least privilege, secret rotation, and audit.
- Require human approval or policy approval for high-risk tool calls and external side effects.
- Capture traces, model calls, tool calls, handoffs, evaluations, guardrail outcomes, costs, and user feedback.
- Support canary rollout, rollback, version pinning, and publishing into Teams or Microsoft 365 Copilot with tenant-wide visibility controls.

** Non Functional Requirements

- Strong tenant isolation across sessions, tools, memory, logs, and private networks.
- Low startup latency for common agents while preserving cold-start isolation.
- Bounded blast radius for runaway agents, compromised tools, bad prompts, and model regressions.
- Explainable governance decisions for tool allowlists, approvals, data access, and publication.
- High availability for invocation routing and audit capture; graceful degradation when optional tools or knowledge sources fail.
- Cost controls for model calls, tool calls, code execution, memory growth, trace retention, and evaluation sampling.

** High level design and diagram (at block level)

```text
[Developers / Admins / Users]
        |
[Agent Registry + Publish Portal]
        |
        +--> [Policy / RBAC / Entra Agent ID]
        +--> [Model Router / BYOM Gateway]
        +--> [Memory + Knowledge Plane]
        +--> [Toolbox / MCP / OpenAPI Gateway]
        |
[Invocation Router]
        |
[Hosted Session Runtime Pool]
        |
        +--> [Sandbox Workspace + State Store]
        +--> [Tool Execution Connectors]
        +--> [Trace / Eval / Monitor Pipeline]
        |
[Teams / Microsoft 365 Copilot / Apps]
```

*** Explain the blocks

- Agent Registry + Publish Portal: stores versioned agent definitions, owners, prompts, framework adapter, tool permissions, channel bindings, and rollout state.
- Policy / RBAC / Entra Agent ID: resolves user, tenant, and agent identity, checks delegated permissions, issues scoped tokens, and records approvals.
- Model Router / BYOM Gateway: routes model traffic to Foundry models, enterprise gateways, or bring-your-own model endpoints while applying rate limits and audit.
- Memory + Knowledge Plane: owns session, user, procedural, and enterprise knowledge state with retention, data residency, and ACL-aware retrieval.
- Toolbox / MCP / OpenAPI Gateway: exposes governed tool bundles through stable endpoints, injects credentials, enforces allowlists, and normalizes tool call results.
- Invocation Router: accepts user or workflow invocations, selects an agent version, allocates a runtime session, and handles retries/idempotency.
- Hosted Session Runtime Pool: runs agent code in isolated sandboxes with filesystem, network, CPU/memory/time budgets, and durable checkpoints.
- Trace / Eval / Monitor Pipeline: joins prompt, model, tool, sub-agent, policy, feedback, quality score, cost, and latency events into an operational loop.

*** Core components and low-level design

The key component is the Tool And Identity Gateway because it decides whether non-deterministic agent plans can safely affect enterprise systems.

Important state:

- `AgentVersion`: prompt, framework adapter, model route, memory policy, tool grants, approval policy, guardrail set, publication targets, and rollout percentage.
- `ToolGrant`: toolbox or MCP server id, allowed tools, required scopes, private-network requirement, approval mode, timeout, side-effect classification, and data-handling policy.
- `Invocation`: user id, tenant id, agent version, session id, idempotency key, input hash, channel, budget, and trace id.
- `ToolCallDecision`: tool name, arguments hash, resolved identity, policy version, approval result, execution lease, response hash, and audit record id.
- `SessionState`: runtime image, workspace snapshot, memory pointers, checkpoints, pending approvals, and cancellation token.

Tool call flow:

```text
agent proposes tool call
  -> normalize tool name and arguments
  -> resolve user/agent identity and delegated scopes
  -> check tool allowlist, network class, side-effect class, data policy
  -> require approval when policy says write/high-risk/unknown
  -> issue short-lived tool token and execution lease
  -> call toolbox/MCP/OpenAPI endpoint with timeout and trace context
  -> redact, store audit record, return bounded result to session
```

Runtime sessions should be resumable but not immortal. Keep durable state in a session store and make the sandbox disposable. Long-running routines checkpoint plan state, memory references, pending tool calls, and last acknowledged side effect. Retries replay from the checkpoint and use the idempotency key plus tool execution lease to avoid duplicate writes.

*** Explain the control flow

Developers register an agent version, attach tools from an organizational catalog, choose model routes, define memory/knowledge policies, and request publication. Admin policy validates data boundary, tool risk, RBAC, and channel visibility before the agent is canaried. Runtime policy changes are versioned: a tool can be removed or moved to approval-required without editing the agent code, and active sessions pick up stricter policy at the next tool boundary.

Operators review dashboards for trace failure clusters, evaluation regressions, guardrail hits, latency, model spend, tool errors, and user feedback. Rollback pins traffic to the previous agent version and disables newly introduced tool grants first, because tool-side effects usually carry the highest enterprise risk.

*** Explain the data flow

User requests enter through Teams, Copilot, an app, or API. The invocation router resolves tenant and agent version, creates or resumes a session, and passes input plus trace context to the hosted runtime. The agent calls the model router for reasoning, retrieves governed knowledge, and requests tools through the gateway. Tool responses return only bounded, redacted payloads to the agent session. Traces and eval events flow asynchronously to the observability plane, while audit records for identity, approval, and side effects are written synchronously before the tool result is released.

** Deep dive topics and questions -> Explain the problem and suggest solutions

*** Hosted runtime isolation

Problem: agents need state, filesystem access, and long-running routines, but enterprise tenants cannot share execution state or secrets.

- Shared workers: cheap and warm; cons weak isolation and hard cleanup after arbitrary code.
- Per-session containers or sandboxes: strong isolation and clear lifecycle; cons cold starts and capacity overhead.
- Hybrid warm pools with disposable sessions: balances startup latency with isolation; cons more runtime orchestration.

Recommended answer: use disposable per-session sandboxes backed by warm runtime pools. Persist only explicit checkpoints, memory references, artifacts, and audit records outside the sandbox.

*** Tool governance and MCP connectivity

Problem: MCP and OpenAPI tools make agents useful, but tool calls can leak data, mutate production systems, or create duplicated side effects.

- Let agents call any registered tool: fast developer workflow; cons unacceptable enterprise risk.
- Static per-agent allowlists only: safer; cons poor handling of high-risk arguments and changing data boundaries.
- Policy-evaluated tool gateway: checks tool, arguments, identity, data class, and approval mode per call; cons adds latency and policy complexity.

Recommended answer: route all tool calls through a gateway that enforces allowlists, scoped credentials, private-network boundaries, side-effect classification, idempotency leases, and human approval for high-risk writes.

*** Observability and evaluation loop

Problem: model and tool behavior changes over time, so success cannot be measured with ordinary service metrics alone.

- Logs and metrics only: operationally familiar; cons misses reasoning and quality regressions.
- Offline evals only: controlled and reproducible; cons misses production drift.
- Trace-linked online and offline evaluation: connects real failures to replayable tests and optimization suggestions; cons needs retention and sampling discipline.

Recommended answer: capture OpenTelemetry-style traces for prompts, model calls, tool calls, sub-agent hops, approvals, and results. Attach rubric, safety, and task-completion evaluations to trace ids so regressions drive canary rollback and backlog items.

*** Model routing and enterprise control

Problem: teams may need Foundry-hosted models, Azure model endpoints, or private model gateways for compliance and cost.

- Single platform model: simplest operations; cons lock-in and poor fit for specialized workloads.
- Bring-your-own model for every team: flexible; cons fragmented observability and policy.
- Governed model router: centralizes policy while supporting multiple endpoints; cons requires normalization of errors, budgets, and capabilities.

Recommended answer: put a model router behind the agent runtime. It should enforce tenant budgets, data-residency constraints, model allowlists, fallback policy, trace propagation, and per-model capability metadata.

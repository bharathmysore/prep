# OpenAI L7 System Design Prep: Question Catalog

Sources used for prompt selection and OpenAI-style emphasis: IGotAnOffer OpenAI system design/SWE guides, Exponent OpenAI guide, and System Design Handbook OpenAI guide. Treat these as publicly reported and representative prompts, not a guaranteed current question bank.

Use this document as a 45-60 minute whiteboard answer skeleton. The question count is intentionally not encoded in this file name or title so future agents can keep the catalog current as prompts are added, removed, merged, or reordered. For L7, do not only draw boxes. Explain control plane, data plane, bottleneck, failure mode, cost lever, safety/privacy concern, and rollout plan.

## Useful Public References

* OpenAI API platform: https://openai.com/api/
* OpenAI Responses API migration guide: https://platform.openai.com/docs/guides/responses-vs-chat-completions
* OpenAI tools guide: https://platform.openai.com/docs/guides/tools/file-search
* OpenAI file search guide: https://platform.openai.com/docs/guides/tools-file-search/
* OpenAI web search guide: https://platform.openai.com/docs/guides/tools-web-search
* OpenAI computer use guide: https://platform.openai.com/docs/guides/tools-computer-use
* OpenAI Codex harness platform note: https://developers.openai.com/blog/codex-as-a-platform
* OpenAI repetitive workflow automation note: https://developers.openai.com/blog/automating-repetitive-work-at-openai-with-codex
* OpenAI WebMCP Challenge: https://openai.com/webmcp-challenge/
* OpenAI Workspace Agents help: https://help.openai.com/en/articles/20001143/

---

## 1. Design ChatGPT At Massive Scale

* **Question**
  * Design the infrastructure to serve a ChatGPT-like product for hundreds of millions of weekly users.

* **Answer**

  **Scope**
  * Consumer and API traffic for text conversations.
  * Real-time token streaming.
  * Conversation history, auth, quota, safety checks, model routing, observability.
  * Exclude model training internals unless interviewer asks.

  **Functional Requirements**
  * Users can send prompts and receive streamed responses.
  * Maintain conversation state and message history.
  * Route to appropriate model/version.
  * Enforce auth, quota, rate limits, and safety policies.
  * Support retries, cancellation, and degraded operation.

  **Non Functional Requirements**
  * Low time-to-first-token (TTFT).
  * High availability under traffic spikes.
  * Multi-tenant isolation.
  * Cost-efficient GPU utilization.
  * Privacy, auditability, abuse resistance.

  **High Level Design And Diagram**

  ```text
  Client
    |
  Edge / CDN / WAF
    |
  API Gateway -- Auth -- Quota -- Abuse Checks
    |
  Conversation Service ---- Conversation DB
    |
  Model Gateway / Router ---- Config & Policy Store
    |
  Safety Precheck
    |
  Inference Scheduler / Queue
    |
  GPU Inference Workers
    |
  Streaming Gateway
    |
  Client
    |
  Telemetry / Billing / Audit Streams
  ```

  **Explain The Blocks**
  * Edge/WAF handles TLS, coarse abuse filtering, regional routing.
  * API Gateway enforces auth, org/project limits, request validation.
  * Conversation Service manages message persistence and context assembly.
  * Model Gateway chooses model, region, capacity pool, and fallback.
  * Inference Scheduler balances latency, priority, and batching.
  * Streaming Gateway keeps long-lived token streams decoupled from API servers.
  * Telemetry/Billing emits usage, cost, latency, safety, and error events.

  **Explain The Control Flow**
  * Admins configure model availability, policies, quotas, experiment rollout, and regional routing.
  * Config is versioned, validated, and pushed to gateway/router caches.
  * Rollouts use canary percentages, shadow traffic, dashboards, and rollback gates.

  **Explain The Data Flow**
  * Request enters gateway, passes auth/quota, loads conversation context, runs pre-safety checks, routes to model pool, streams tokens back, persists final response, and emits metering/telemetry events.

  **Deep Dive Topics And Questions**
  * **Question: How do you balance latency and GPU utilization?**
    * Small batches: pros are low latency and predictable TTFT; cons are poor GPU utilization and high cost.
    * Dynamic batching: pros are better throughput and cost; cons are queue delay and more complex scheduling.
    * SLA-tiered pools: pros are predictable premium latency; cons are capacity fragmentation and idle reserved GPUs.
    * Recommendation: use dynamic batching with strict queue-delay budgets and separate pools for interactive, batch, and premium workloads.
  * **Question: How do you handle overload?**
    * Reject early: pros are protects core system; cons are visible failures.
    * Degrade model: pros are preserves service; cons are quality inconsistency.
    * Queue requests: pros are avoids drops; cons are bad UX for interactive chat.
    * Recommendation: load shed at gateway, use smaller/faster fallback models, preserve streaming health for accepted requests.

---

## 2. Design Real-Time LLM Response Streaming

* **Question**
  * Design a service that streams model responses token by token to clients reliably.

* **Answer**

  **Scope**
  * One-way model-token streaming for web/mobile/API clients.
  * Reconnect, cancellation, partial persistence, timeout, and backpressure.

  **Functional Requirements**
  * Start response quickly.
  * Stream tokens in order.
  * Support cancellation and retry.
  * Persist final answer and optionally partial answer.
  * Handle client disconnects safely.

  **Non Functional Requirements**
  * Low TTFT and low jitter.
  * High concurrent connections.
  * Backpressure-aware.
  * Graceful degradation during network or worker failures.

  **High Level Design And Diagram**

  ```text
  Client
    |
  Streaming API Gateway
    |
  Session Service ---- Session Store
    |
  Model Router
    |
  Inference Worker
    |
  Token Stream Broker
    |
  Stream Fanout Node
    |
  Client
  ```

  **Explain The Blocks**
  * Streaming API Gateway terminates SSE/WebSocket connections.
  * Session Service creates stream IDs and reconnect tokens.
  * Inference Worker generates ordered token deltas.
  * Token Stream Broker decouples model worker from client connection.
  * Fanout Node handles slow clients, buffering, heartbeat, and finalization.

  **Explain The Control Flow**
  * Operators configure stream timeout, max tokens, buffer limits, model-specific routing, and retry policies.
  * Gateway receives config updates without draining healthy streams.

  **Explain The Data Flow**
  * Client opens stream, request is routed to inference, worker emits token deltas to broker, fanout node writes ordered events to client, final event commits message status.

  **Deep Dive Topics And Questions**
  * **Question: SSE or WebSockets?**
    * SSE: pros are simple, HTTP-friendly, automatic reconnect; cons are one-way and less flexible.
    * WebSocket: pros are bidirectional and good for rich collaboration; cons are harder scaling, load balancing, and observability.
    * Recommendation: use SSE for basic token streaming, WebSockets only when bidirectional interaction is required.
  * **Question: Persist partial tokens?**
    * Persist every token: pros are strong recovery; cons are high write amplification.
    * Persist final only: pros are simple and cheap; cons are poor recovery/debuggability.
    * Periodic checkpoint: pros are balanced; cons are slightly stale on failure.
    * Recommendation: checkpoint periodically and persist final response synchronously.

---

## 3. Design The OpenAI Playground

* **Question**
  * Design a developer-facing playground for prompts, model parameters, conversations, tools, and API examples.

* **Answer**

  **Scope**
  * Web product for interactive testing.
  * Prompt/session history, model settings, files, tool calls, sharing, and generated API snippets.

  **Functional Requirements**
  * Create/edit/run prompts.
  * Select model and parameters.
  * Save, fork, and share sessions.
  * Show streamed responses and traces.
  * Generate equivalent API request examples.

  **Non Functional Requirements**
  * Fast interactive UX.
  * Strong tenant isolation.
  * Accurate billing/quota integration.
  * Privacy controls and retention settings.

  **High Level Design And Diagram**

  ```text
  Browser UI
    |
  Playground Backend
    |---- Session DB
    |---- File/Object Store
    |---- Snippet Generator
    |
  Execution Service
    |
  Model Gateway
    |
  Inference / Tools
    |
  Trace, Usage, Billing Streams
  ```

  **Explain The Blocks**
  * Browser UI manages prompt editor, parameter controls, traces, and diff/fork flows.
  * Playground Backend owns sessions, permissions, saved presets, and sharing.
  * Execution Service converts UI state into model API requests.
  * Snippet Generator emits SDK/cURL examples from the canonical request.
  * Trace/Usage streams power debugging and billing.

  **Explain The Control Flow**
  * Admins configure allowed models, org-level permissions, retention, sharing policy, and beta feature flags.
  * Product experiments safely roll out UI and model defaults by tenant/cohort.

  **Explain The Data Flow**
  * User edits prompt, sends run request, backend persists a session version, execution service calls model gateway, response streams to UI, final output and trace are saved.

  **Deep Dive Topics And Questions**
  * **Question: How do you model session history?**
    * Mutable document: pros are simple; cons are hard audit/diff.
    * Event-sourced versions: pros are great for fork, replay, audit; cons are more complex.
    * Recommendation: store immutable session runs plus current workspace pointer.
  * **Question: How much trace data should users see?**
    * Full trace: pros are transparent and debuggable; cons may expose sensitive internals.
    * Redacted trace: pros are safer; cons less useful.
    * Recommendation: expose user-relevant trace with policy-based redaction.

---

## 4. Design An LLM-Powered Enterprise Search System

* **Question**
  * Design enterprise search over internal documents with natural language queries and role-based access control.

* **Answer**

  **Scope**
  * Search over SaaS/document sources such as Drive, Slack, Jira, Confluence, GitHub.
  * Retrieval-augmented generation (RAG), citations, ACL enforcement, freshness.

  **Functional Requirements**
  * Connect data sources and ingest documents.
  * Extract, chunk, embed, and index content.
  * Enforce user permissions.
  * Answer questions with citations.
  * Update/deindex deleted or permission-changed content.

  **Non Functional Requirements**
  * No data leakage across users/tenants.
  * Fresh enough index.
  * Low query latency.
  * High recall and answer quality.
  * Auditable access decisions.

  **High Level Design And Diagram**

  ```text
  Source Connectors
    |
  Ingestion Queue
    |
  Parser / Chunker / PII Filter
    |
  Embedding Service
    |
  Vector Index + Metadata/ACL Store

  User Query
    |
  Query Service -- AuthZ
    |
  Retriever -- Vector Index / Keyword Index
    |
  Reranker
    |
  LLM Answer Generator
    |
  Answer + Citations
  ```

  **Explain The Blocks**
  * Connectors pull or receive source changes.
  * Parser/Chunker normalizes documents into retrievable chunks.
  * Embedding Service creates vectors.
  * Vector/Keyword indexes support semantic and lexical retrieval.
  * ACL Store maps chunks to users/groups/tenants.
  * Reranker improves top-k quality before generation.

  **Explain The Control Flow**
  * Admin installs connectors, maps identity providers, sets retention, source scopes, and indexing policies.
  * Connector health, freshness SLAs, and permission-sync lag are monitored.

  **Explain The Data Flow**
  * Documents are ingested, transformed, embedded, indexed with ACL metadata. Query is authenticated, rewritten/embedded, filtered by permissions, retrieved, reranked, and passed to the LLM with citations.

  **Deep Dive Topics And Questions**
  * **Question: ACL filtering before or after vector search?**
    * Pre-filter: pros are safer and avoids leakage; cons can reduce recall and complicate indexes.
    * Post-filter: pros are simple and fast; cons risks empty results and must never expose unauthorized snippets.
    * Hybrid: pros balance safety/quality; cons more complex.
    * Recommendation: tenant partitioning plus pre-filter for coarse ACL, post-filter for fine ACL, and never send unauthorized chunks to LLM.
  * **Question: How do you maintain freshness?**
    * Polling: pros simple; cons stale and API-heavy.
    * Webhooks/change streams: pros fresh; cons source-specific complexity.
    * Hybrid: pros robust; cons operational overhead.
    * Recommendation: webhooks where available plus periodic reconciliation scans.

---

## 5. Design A Vector Database For Billions Of Embeddings

* **Question**
  * Design a vector database that stores and searches billions of embeddings with metadata filters.

* **Answer**

  **Scope**
  * Multi-tenant vector storage, ANN search, metadata filters, updates/deletes, replication, and snapshots.

  **Functional Requirements**
  * Insert/update/delete vectors.
  * Search nearest neighbors by vector.
  * Filter by metadata and tenant.
  * Scale index building and serving.
  * Support backups and index compaction.

  **Non Functional Requirements**
  * Low p95/p99 query latency.
  * High recall.
  * High availability.
  * Efficient storage and memory use.
  * Tenant isolation.

  **High Level Design And Diagram**

  ```text
  Client SDK/API
    |
  Query Coordinator
    |---- Metadata Filter Service
    |
  Shard Routers
    |
  Vector Index Shards ---- Object Storage Snapshots
    |
  Result Merger / Reranker

  Write API -> WAL -> Segment Builder -> Index Builder -> Shards
  ```

  **Explain The Blocks**
  * Query Coordinator validates tenant, applies filters, fans out to shards.
  * Index Shards serve ANN queries from memory/disk structures.
  * WAL gives durable writes before async indexing.
  * Segment/Index Builder compacts updates into searchable segments.
  * Snapshot store supports recovery and replica bootstrap.

  **Explain The Control Flow**
  * Operators configure shard count, replication factor, index type, compaction, memory budgets, and tenant placement.
  * Rebalancing and schema changes are controlled through a versioned control plane.

  **Explain The Data Flow**
  * Writes append to WAL, are acknowledged based on durability policy, then indexed into segments. Queries route to relevant shards, execute ANN plus metadata filtering, merge top-k, and return results.

  **Deep Dive Topics And Questions**
  * **Question: Exact search or approximate search?**
    * Exact search: pros maximum accuracy; cons too slow/expensive at billion scale.
    * ANN/HNSW/IVF/PQ: pros scalable and fast; cons recall tuning and index complexity.
    * Recommendation: ANN with recall/latency knobs, optional exact rerank for top candidates.
  * **Question: How do metadata filters interact with ANN?**
    * Filter before ANN: pros correctness and fewer candidates; cons fragmented indexes.
    * Filter after ANN: pros simpler; cons may miss relevant filtered items.
    * Recommendation: pre-filter high-selectivity tenant/security dimensions, post-filter low-selectivity attributes, rerank larger candidate sets.

---

## 6. Design GPU Scheduling For Training And Inference

* **Question**
  * Design a GPU scheduler that allocates scarce compute across training, inference, batch, and research workloads.

* **Answer**

  **Scope**
  * Cluster inventory, job submission, quota, priority, preemption, placement, health, and utilization.

  **Functional Requirements**
  * Submit jobs with resource requirements.
  * Allocate GPUs/hosts/network topology.
  * Enforce team/project quotas.
  * Preempt or checkpoint lower-priority jobs.
  * Track health and job status.

  **Non Functional Requirements**
  * High utilization.
  * Predictable latency for inference.
  * Fairness across teams.
  * Fault tolerance.
  * Support heterogeneous GPU types.

  **High Level Design And Diagram**

  ```text
  Job API / CLI
    |
  Admission Controller -- Quota / Policy Store
    |
  Scheduler
    |---- Cluster Inventory
    |---- Priority Queue
    |---- Placement Optimizer
    |
  Node Agents
    |
  GPU Nodes / Pods
    |
  Metrics / Events / Checkpoint Store
  ```

  **Explain The Blocks**
  * Admission Controller validates quota, priority, and constraints.
  * Scheduler selects nodes using topology, GPU type, memory, network, and fairness.
  * Node Agents report health and launch workloads.
  * Checkpoint Store supports preemption and recovery.

  **Explain The Control Flow**
  * Platform owners configure quotas, priorities, preemption rules, maintenance windows, and reserved pools.
  * Scheduler continuously reconciles desired job state with cluster state.

  **Explain The Data Flow**
  * Jobs enter queue, are admitted, placed on nodes, run with telemetry, checkpoint periodically, and emit completion/failure events.

  **Deep Dive Topics And Questions**
  * **Question: Centralized or distributed scheduling?**
    * Centralized: pros global optimization and simpler policy; cons bottleneck and single control-plane risk.
    * Distributed: pros scalable and fault tolerant; cons conflicts and less optimal placement.
    * Recommendation: logically centralized scheduler with sharded queues and highly available replicas.
  * **Question: Preempt training jobs for inference?**
    * Preemption: pros protects user-facing latency; cons wastes work without checkpointing.
    * Reservations: pros predictable capacity; cons lower utilization.
    * Recommendation: reserve inference pools, allow preemption only for checkpointable/best-effort jobs.

---

## 7. Design An Inference Batching Service

* **Question**
  * Design a service that batches LLM inference requests to maximize GPU utilization while meeting latency SLAs.

* **Answer**

  **Scope**
  * Runtime batching for interactive and batch inference.
  * Queueing, scheduling, cancellation, fairness, metrics.

  **Functional Requirements**
  * Group compatible requests by model/version.
  * Respect latency and priority classes.
  * Handle variable input/output lengths.
  * Support cancellation and streaming.
  * Emit utilization and queue metrics.

  **Non Functional Requirements**
  * Low p95 TTFT.
  * High token throughput.
  * Fairness across tenants.
  * Resilience to hot tenants and long generations.

  **High Level Design And Diagram**

  ```text
  Model Gateway
    |
  Per-Model Priority Queues
    |
  Batch Builder
    |
  GPU Runtime Worker
    |
  Token Dispatcher
    |
  Streaming Gateway
  ```

  **Explain The Blocks**
  * Priority Queues isolate tenants/SLA classes.
  * Batch Builder forms batches by model, context length, deadline, and memory budget.
  * Runtime Worker performs prefill and decode.
  * Token Dispatcher maps generated tokens back to individual streams.

  **Explain The Control Flow**
  * Runtime config defines max batch size, max queue delay, per-tier priority, memory thresholds, and admission limits.
  * Autoscaler adjusts worker pools based on queue depth and GPU utilization.

  **Explain The Data Flow**
  * Requests are queued, batched, sent to GPU, token outputs are demultiplexed to streams, and usage metrics are emitted per request.

  **Deep Dive Topics And Questions**
  * **Question: Static vs dynamic batching?**
    * Static: pros simple and predictable; cons poor under variable load.
    * Dynamic: pros better utilization; cons harder tail-latency control.
    * Recommendation: dynamic batching with deadline-aware admission.
  * **Question: How do long prompts affect batching?**
    * Mix all lengths: pros simple; cons padding waste and latency.
    * Bucket by length: pros efficient; cons more queues and possible starvation.
    * Recommendation: bucket by approximate token length and enforce starvation protection.

---

## 8. Design File Upload And RAG Ingestion

* **Question**
  * Design file upload and ingestion for a ChatGPT/RAG product that supports large files and retrieval.

* **Answer**

  **Scope**
  * Upload, virus scanning, parsing, chunking, embedding, indexing, status, and deletion.

  **Functional Requirements**
  * Upload files reliably.
  * Extract text and metadata.
  * Chunk and embed content.
  * Index chunks for retrieval.
  * Show processing status and errors.
  * Delete files and derived embeddings.

  **Non Functional Requirements**
  * Scalable async processing.
  * Tenant isolation.
  * Secure handling of sensitive files.
  * Idempotent retries.
  * Compliance deletion.

  **High Level Design And Diagram**

  ```text
  Client
    |
  Upload API -> Object Store
    |
  File Metadata DB
    |
  Ingestion Queue
    |
  Scanner -> Parser -> Chunker -> Embedding Workers -> Index Writer
    |
  Vector Store / Metadata Store
  ```

  **Explain The Blocks**
  * Upload API creates signed URLs and records metadata.
  * Object Store holds raw files.
  * Scanner detects malware and unsafe content.
  * Parser extracts text by file type.
  * Chunker prepares retrieval units.
  * Embedding Workers create vectors.
  * Index Writer commits searchable chunks with ACLs.

  **Explain The Control Flow**
  * Admin config controls allowed file types, max size, retention, parser versions, embedding model versions, and deletion policy.
  * Pipeline versions are tracked so old files can be reprocessed.

  **Explain The Data Flow**
  * File uploaded to object storage, metadata event enqueued, pipeline stages process file, chunks and embeddings are stored, index status becomes ready.

  **Deep Dive Topics And Questions**
  * **Question: Synchronous or asynchronous ingestion?**
    * Synchronous: pros immediate UX for small files; cons timeouts and poor scalability.
    * Async: pros scalable and reliable; cons user waits for readiness.
    * Recommendation: async for most files, with fast-path sync only for tiny files.
  * **Question: How do you delete derived data?**
    * Best-effort async delete: pros cheap; cons compliance risk.
    * Tombstone plus verified sweep: pros auditable; cons operational complexity.
    * Recommendation: tombstone immediately, block retrieval, then verify physical deletion from object store and indexes.

---

## 9. Design API Rate Limiting And Quotas For LLM APIs

* **Question**
  * Design rate limiting and quota enforcement for a multi-tenant LLM API.

* **Answer**

  **Scope**
  * Per-user, org, project, model, region, token, and spend limits.
  * Support burst limits and monthly quotas.

  **Functional Requirements**
  * Enforce request and token limits.
  * Support different tiers and models.
  * Return clear retry/error information.
  * Protect system from abuse and accidental spikes.
  * Expose usage dashboards.

  **Non Functional Requirements**
  * Low latency on request path.
  * High availability.
  * Reasonably accurate global accounting.
  * Resistant to race conditions.

  **High Level Design And Diagram**

  ```text
  API Gateway
    |
  Local Rate Limit Cache
    |
  Quota Service ---- Quota Config Store
    |
  Usage Event Stream
    |
  Aggregator ---- Usage DB / Billing
  ```

  **Explain The Blocks**
  * Gateway performs fast-path checks.
  * Local cache stores hot token buckets.
  * Quota Service owns configuration and slower global decisions.
  * Usage stream records actual consumed tokens.
  * Aggregator computes dashboards and billing reconciliation.

  **Explain The Control Flow**
  * Sales/admin systems update tier, spend caps, and model entitlements.
  * Config propagates to gateways with versioning and emergency blocklists.

  **Explain The Data Flow**
  * Request checks local bucket, optionally reserves estimated tokens, inference runs, actual usage event is emitted, aggregator reconciles reserved vs actual usage.

  **Deep Dive Topics And Questions**
  * **Question: Centralized or regional limiters?**
    * Centralized: pros accurate; cons adds latency and availability dependency.
    * Regional/local: pros fast and resilient; cons temporary global overshoot.
    * Recommendation: local fast path with bounded leases from global quota service.
  * **Question: Limit requests or tokens?**
    * Request limits: pros simple; cons unfair across tiny vs huge prompts.
    * Token limits: pros cost-aligned; cons final output tokens unknown upfront.
    * Recommendation: combine request limits with estimated token reservation and post-hoc reconciliation.

---

## 10. Design Billing And Usage Metering For Model APIs

* **Question**
  * Design billing and usage metering for an LLM API with token-based pricing.

* **Answer**

  **Scope**
  * Accurate usage collection, aggregation, pricing, invoices, dashboards, and reconciliation.

  **Functional Requirements**
  * Capture prompt/completion tokens, model, org, project, region, and timestamp.
  * Aggregate usage by billing period.
  * Support pricing changes and discounts.
  * Provide near-real-time dashboard.
  * Reconcile with invoices.

  **Non Functional Requirements**
  * Accuracy and auditability.
  * Idempotency.
  * High durability.
  * Privacy-aware logging.
  * Backfill support.

  **High Level Design And Diagram**

  ```text
  Inference Service
    |
  Usage Event Producer
    |
  Durable Event Log
    |
  Stream Aggregator ---- Pricing Config
    |
  Usage Warehouse ---- Billing System
    |
  Customer Dashboard
  ```

  **Explain The Blocks**
  * Usage Event Producer emits immutable metering records.
  * Event Log protects against data loss.
  * Aggregator dedupes, applies pricing dimensions, and computes usage.
  * Warehouse stores auditable history.
  * Billing System invoices and collects payment.

  **Explain The Control Flow**
  * Pricing, discounts, contract terms, and billing periods are versioned.
  * Backfills are run with explicit replay windows and audit records.

  **Explain The Data Flow**
  * Inference emits usage event with idempotency key, stream aggregator processes and dedupes, usage DB updates dashboard, billing job creates invoice.

  **Deep Dive Topics And Questions**
  * **Question: Real-time billing or batch billing?**
    * Real-time: pros great visibility and spend control; cons harder accuracy under late events.
    * Batch: pros simpler and auditable; cons poor customer feedback.
    * Recommendation: near-real-time estimates plus authoritative batch reconciliation.
  * **Question: What if events are duplicated or delayed?**
    * Dedup by request ID: pros straightforward; cons request retries need careful IDs.
    * Exactly-once stream: pros appealing; cons usually impractical end to end.
    * Recommendation: at-least-once events with deterministic idempotency keys.

---

## 11. Design A Safety And Moderation Pipeline

* **Question**
  * Design a system that applies safety policy to model inputs and outputs at scale.

* **Answer**

  **Scope**
  * Input/output moderation, policy evaluation, logging, human review, and policy rollout.

  **Functional Requirements**
  * Classify user input and model output.
  * Apply policy decisions: allow, block, transform, escalate.
  * Support policy versioning.
  * Log decisions for audit.
  * Route ambiguous cases to review.

  **Non Functional Requirements**
  * Low added latency.
  * High precision for severe harm categories.
  * Scalable under product traffic.
  * Explainable/auditable decisions.

  **High Level Design And Diagram**

  ```text
  Request
    |
  Safety Precheck ---- Policy Store
    |
  Model Inference
    |
  Safety Postcheck
    |
  Response Filter / Refusal / Transform
    |
  Client
    |
  Safety Logs / Review Queue / Metrics
  ```

  **Explain The Blocks**
  * Precheck filters obvious disallowed requests before expensive inference.
  * Postcheck catches unsafe generated output.
  * Policy Store versions category rules and thresholds.
  * Review Queue supports human escalation and labeling.
  * Logs/metrics track regressions and abuse.

  **Explain The Control Flow**
  * Safety team updates policies and classifier thresholds through staged rollout.
  * Experiments compare false positives/negatives before global release.

  **Explain The Data Flow**
  * Request is classified, policy decision determines whether model runs, output is classified again, response is allowed/refused/transformed, decision events are logged.

  **Deep Dive Topics And Questions**
  * **Question: Inline or async moderation?**
    * Inline: pros prevents unsafe responses; cons increases latency and cost.
    * Async: pros cheap and scalable; cons may act too late.
    * Recommendation: inline for high-risk categories, async for analytics and retroactive enforcement.
  * **Question: Rules or ML classifiers?**
    * Rules: pros predictable and auditable; cons brittle.
    * ML: pros broad coverage; cons false positives and drift.
    * Recommendation: hybrid policy engine using rules for hard constraints and classifiers for semantic risk.

---

## 12. Design A Model Gateway And Router

* **Question**
  * Design a gateway that routes requests across models, versions, regions, and capacity pools.

* **Answer**

  **Scope**
  * Routing layer between product/API traffic and model serving backends.
  * Includes canary, fallback, policy, region, and load-based routing.

  **Functional Requirements**
  * Route by model, tenant, region, policy, and capacity.
  * Support model version rollout.
  * Fail over when pools are unhealthy.
  * Enforce entitlements and safety requirements.
  * Emit routing decisions for audit.

  **Non Functional Requirements**
  * Very low routing overhead.
  * High availability.
  * Predictable behavior.
  * Safe rollout and rollback.

  **High Level Design And Diagram**

  ```text
  API Gateway
    |
  Model Router
    |---- Entitlement Store
    |---- Routing Config Store
    |---- Health/Load Signals
    |
  Capacity Pools
    |---- Model A v1
    |---- Model A v2 Canary
    |---- Fallback Model
  ```

  **Explain The Blocks**
  * Router evaluates tenant, model, policy, region, and live capacity.
  * Config Store holds rollout percentages and allowed fallbacks.
  * Health Signals come from serving pools.
  * Capacity Pools isolate versions and workloads.

  **Explain The Control Flow**
  * Platform teams publish routing configs through review, validation, canary, and rollback.
  * Health monitors dynamically remove bad pools.

  **Explain The Data Flow**
  * Request arrives with model intent, router checks entitlement and policy, selects backend, forwards request, records routing decision and outcome.

  **Deep Dive Topics And Questions**
  * **Question: Rule-based routing or adaptive routing?**
    * Rule-based: pros predictable and auditable; cons less optimal under fast-changing load.
    * Adaptive: pros improves latency/cost; cons can be hard to debug and unstable.
    * Recommendation: rule-based safety envelope with adaptive load balancing inside allowed pools.
  * **Question: Fallback to a different model?**
    * Fallback: pros preserves availability; cons quality/behavior changes.
    * No fallback: pros predictable; cons higher error rate during incidents.
    * Recommendation: allow explicit fallback only when product contract permits it.

---

## 13. Design An Evaluation And Experimentation Platform

* **Question**
  * Design a platform for evaluating model, prompt, policy, and product changes before and after launch.

* **Answer**

  **Scope**
  * Offline evals, online experiments, shadow traffic, human review, rollout gates, dashboards.

  **Functional Requirements**
  * Run eval suites against candidate models/prompts.
  * Compare metrics across versions.
  * Support online A/B and shadow testing.
  * Collect human and automated judgments.
  * Gate rollouts and trigger rollback.

  **Non Functional Requirements**
  * Reproducibility.
  * Statistical rigor.
  * Privacy-safe datasets.
  * Low interference with production.
  * Auditability.

  **High Level Design And Diagram**

  ```text
  Eval Config UI/API
    |
  Dataset Registry ---- Version Store
    |
  Eval Orchestrator
    |
  Model/Prompt Runner
    |
  Scorers / Human Review
    |
  Metrics Warehouse / Dashboards
    |
  Rollout Gate
  ```

  **Explain The Blocks**
  * Dataset Registry tracks test sets and provenance.
  * Orchestrator schedules eval runs.
  * Runner executes candidates against prompts/tasks.
  * Scorers compute quality, safety, latency, and cost metrics.
  * Rollout Gate integrates with model router/config systems.

  **Explain The Control Flow**
  * Teams define eval suites and launch criteria.
  * Candidate changes cannot advance past gates without passing required metrics.

  **Explain The Data Flow**
  * Dataset and candidate version are selected, orchestrator runs tasks, results are scored, metrics are compared to baseline, rollout decision is recorded.

  **Deep Dive Topics And Questions**
  * **Question: Offline evals or online experiments?**
    * Offline: pros safe and reproducible; cons may not match real users.
    * Online: pros realistic; cons riskier and slower to interpret.
    * Recommendation: require offline gates before online canaries.
  * **Question: Automated scoring or human review?**
    * Automated: pros fast and scalable; cons can miss subtle quality/safety issues.
    * Human: pros nuanced; cons slow and expensive.
    * Recommendation: automated broad regression checks plus targeted human review for high-risk categories.

---

## 14. Design A Distributed Job Scheduler

* **Question**
  * Design a distributed job scheduler for recurring and one-off background workloads.

* **Answer**

  **Scope**
  * Job submission, scheduling, leases, retries, heartbeats, priorities, recurring schedules.

  **Functional Requirements**
  * Submit jobs and define schedules.
  * Assign jobs to workers.
  * Retry failures.
  * Avoid duplicate execution where possible.
  * Track job status and logs.

  **Non Functional Requirements**
  * Durable state.
  * Horizontal scalability.
  * High availability.
  * Idempotent execution model.
  * Fairness and isolation.

  **High Level Design And Diagram**

  ```text
  Job API
    |
  Job Store ---- Schedule Store
    |
  Scheduler Leader(s)
    |
  Ready Queue
    |
  Worker Fleet
    |
  Result Store / Logs / Metrics
  ```

  **Explain The Blocks**
  * Job Store persists definitions and state transitions.
  * Scheduler promotes due jobs into ready queues.
  * Workers acquire leases, execute jobs, heartbeat, and write results.
  * Result/log systems support debugging and replay.

  **Explain The Control Flow**
  * Operators configure worker pools, retry policies, priority classes, max concurrency, and tenant quotas.
  * Scheduler leadership uses lease/election to avoid conflicting promotions.

  **Explain The Data Flow**
  * Job is submitted, stored, becomes due, placed on ready queue, leased by worker, executed, status committed, retries scheduled if needed.

  **Deep Dive Topics And Questions**
  * **Question: How do you avoid duplicate execution?**
    * Strong locking: pros reduces duplicates; cons bottlenecks and lock failure modes.
    * Lease + idempotency: pros scalable and robust; cons jobs must be idempotent.
    * Recommendation: lease-based execution with idempotency keys and deduped outputs.
  * **Question: Push or pull workers?**
    * Push: pros low latency; cons harder backpressure.
    * Pull: pros workers self-regulate; cons polling overhead.
    * Recommendation: pull with long polling or queue semantics.

---

## 15. Design A Webhook Delivery System

* **Question**
  * Design a webhook system that reliably delivers events to customer endpoints.

* **Answer**

  **Scope**
  * Event ingestion, customer subscriptions, signing, delivery, retries, replay, logs.

  **Functional Requirements**
  * Customers register endpoints and event types.
  * System signs and sends webhook events.
  * Retry failed deliveries.
  * Provide delivery logs and replay.
  * Enforce per-customer rate limits.

  **Non Functional Requirements**
  * At-least-once delivery.
  * Isolation between customers.
  * Durable event storage.
  * Backpressure and abuse protection.

  **High Level Design And Diagram**

  ```text
  Product Services
    |
  Event Bus
    |
  Subscription Matcher
    |
  Delivery Queue per Customer
    |
  Webhook Workers
    |
  Customer Endpoints
    |
  Delivery Log / DLQ / Replay API
  ```

  **Explain The Blocks**
  * Event Bus stores product events durably.
  * Subscription Matcher expands events to endpoint deliveries.
  * Per-customer queues isolate slow or broken endpoints.
  * Workers sign payloads and send HTTP requests.
  * DLQ and Replay support debugging and recovery.

  **Explain The Control Flow**
  * Customers configure endpoints, secrets, event filters, and retry preferences.
  * Platform config controls max attempts, backoff, timeout, and disable thresholds.

  **Explain The Data Flow**
  * Event published, matcher creates delivery tasks, workers send signed requests, response status recorded, failures retried or moved to DLQ.

  **Deep Dive Topics And Questions**
  * **Question: Exactly-once webhooks?**
    * Exactly-once: pros attractive contract; cons impossible across customer endpoints.
    * At-least-once: pros practical and reliable; cons customers must dedupe.
    * Recommendation: at-least-once with event IDs, signatures, retry logs, and idempotency guidance.
  * **Question: One global queue or per-customer queues?**
    * Global: pros simple; cons noisy customers block others.
    * Per-customer: pros isolation and rate control; cons more queue management.
    * Recommendation: partition by customer or endpoint with fairness scheduling.

---

## 16. Design A Fault-Tolerant Polite Web Crawler

* **Question**
  * Design a web crawler that scales to very high request volume while respecting politeness and robots.txt.

* **Answer**

  **Scope**
  * URL discovery, crawl frontier, robots handling, domain throttling, dedupe, parsing, storage.

  **Functional Requirements**
  * Discover and fetch URLs.
  * Respect robots.txt and crawl delay.
  * Avoid duplicate fetches.
  * Parse links/content.
  * Prioritize freshness and importance.

  **Non Functional Requirements**
  * High throughput.
  * Politeness per domain.
  * Fault tolerance.
  * Efficient dedupe.
  * Abuse-safe identity and rate controls.

  **High Level Design And Diagram**

  ```text
  Seed URLs
    |
  URL Frontier
    |
  Domain Scheduler ---- Robots Cache
    |
  Fetch Workers
    |
  Parser / Canonicalizer
    |
  Content Store + URL Dedupe + Link Graph
  ```

  **Explain The Blocks**
  * URL Frontier stores candidate URLs by priority.
  * Domain Scheduler enforces per-host politeness.
  * Robots Cache stores robots.txt rules.
  * Fetch Workers retrieve pages.
  * Parser extracts content and links.
  * Dedupe prevents repeated crawl and storage waste.

  **Explain The Control Flow**
  * Operators set crawl policies, domain allow/block lists, global QPS, user agent, and freshness priorities.
  * Scheduler updates domain budgets and backoff based on errors.

  **Explain The Data Flow**
  * URL is selected, robots checked, fetch executed, content parsed, new links canonicalized and deduped, content stored/indexed.

  **Deep Dive Topics And Questions**
  * **Question: How do you partition the frontier?**
    * By URL hash: pros even distribution; cons hard domain politeness.
    * By domain: pros easy politeness; cons hot-domain skew.
    * Recommendation: partition by domain with hot-domain splitting and global fairness.
  * **Question: Bloom filter or exact dedupe?**
    * Bloom filter: pros memory efficient; cons false positives may skip URLs.
    * Exact store: pros no false positives; cons expensive at scale.
    * Recommendation: Bloom filter for fast precheck plus exact canonical URL store for important URLs.

---

## 17. Design An In-Memory Database

* **Question**
  * Design an in-memory database similar to Redis.

* **Answer**

  **Scope**
  * Key-value operations, data structures, persistence, replication, sharding, eviction.

  **Functional Requirements**
  * Get/set/delete keys.
  * Support TTL and common data structures.
  * Persist snapshots or logs.
  * Replicate for availability.
  * Evict under memory pressure.

  **Non Functional Requirements**
  * Very low latency.
  * High throughput.
  * Predictable memory use.
  * Fast failover.
  * Data durability depending on mode.

  **High Level Design And Diagram**

  ```text
  Client
    |
  Request Router
    |
  Shard Primary ---- Replica(s)
    |                  |
  Memory Store       Replication Log
    |
  Snapshot / AOF Persistence
  ```

  **Explain The Blocks**
  * Router maps keys to shards.
  * Primary handles writes and replication.
  * Replicas serve reads if allowed and support failover.
  * Memory Store holds data structures and TTL metadata.
  * Persistence layer writes snapshots or append-only logs.

  **Explain The Control Flow**
  * Cluster manager tracks shard membership, failover, rebalancing, and memory thresholds.
  * Operators configure durability mode, eviction policy, and replica count.

  **Explain The Data Flow**
  * Client request routes by key, primary reads/writes memory, write updates log/replicas, response returns after configured durability threshold.

  **Deep Dive Topics And Questions**
  * **Question: Snapshot or append-only log?**
    * Snapshot: pros compact and fast recovery baseline; cons can lose recent writes.
    * AOF: pros better durability; cons write amplification and replay time.
    * Recommendation: combine periodic snapshots with AOF for durability-sensitive modes.
  * **Question: Synchronous or asynchronous replication?**
    * Sync: pros stronger consistency; cons higher latency and lower availability.
    * Async: pros fast; cons possible data loss on failover.
    * Recommendation: async by default, optional quorum ack for stronger durability.

---

## 18. Design Slack-Style Chat

* **Question**
  * Design Slack: real-time messaging for workspaces, channels, threads, presence, and search.

* **Answer**

  **Scope**
  * Workspace/channel chat, direct messages, presence, notifications, search, and file links.

  **Functional Requirements**
  * Send/receive messages in real time.
  * Support channels, DMs, threads, reactions.
  * Store searchable history.
  * Track presence and read state.
  * Notify offline users.

  **Non Functional Requirements**
  * Low message latency.
  * Durable ordered conversation history.
  * Scalable fanout.
  * Multi-tenant isolation.
  * Reliable offline sync.

  **High Level Design And Diagram**

  ```text
  Clients
    |
  Realtime Gateway
    |
  Message Service ---- Message DB
    |
  Fanout Service ---- Presence Service
    |
  Online Clients
    |
  Search Indexer / Notification Service
  ```

  **Explain The Blocks**
  * Realtime Gateway manages WebSocket connections.
  * Message Service validates and persists messages.
  * Fanout Service delivers to subscribed online users.
  * Presence Service tracks active connections.
  * Search Indexer and Notification Service process async events.

  **Explain The Control Flow**
  * Workspace admins configure retention, permissions, app integrations, and notification policies.
  * Gateway membership subscriptions change as users join/leave channels.

  **Explain The Data Flow**
  * Client sends message, message is persisted with sequence ID, fanout service delivers to online members, search/notification events fire asynchronously.

  **Deep Dive Topics And Questions**
  * **Question: Fanout-on-write or fanout-on-read?**
    * Fanout-on-write: pros fast reads and push delivery; cons expensive for huge channels.
    * Fanout-on-read: pros cheaper writes; cons slower clients and more query load.
    * Recommendation: hybrid, fanout-on-write for small/medium channels and read-based fetch for very large channels.
  * **Question: How do you guarantee ordering?**
    * Global ordering: pros simple mental model; cons expensive and unnecessary.
    * Per-channel ordering: pros scalable and sufficient; cons cross-channel order not guaranteed.
    * Recommendation: per-channel monotonically increasing sequence numbers.

---

## 19. Design An Observability Platform For LLM Infrastructure

* **Question**
  * Design observability for LLM serving infrastructure across gateways, routers, inference workers, and GPUs.

* **Answer**

  **Scope**
  * Metrics, logs, traces, request IDs, GPU telemetry, model quality signals, cost and safety metrics.

  **Functional Requirements**
  * Collect and query metrics/logs/traces.
  * Correlate one user request across services.
  * Alert on latency, errors, safety, and capacity.
  * Support dashboards and incident debugging.
  * Retain data by sensitivity and value.

  **Non Functional Requirements**
  * Low overhead on hot path.
  * High-cardinality support where needed.
  * Privacy-aware redaction.
  * Reliable ingestion during incidents.
  * Cost control.

  **High Level Design And Diagram**

  ```text
  Services / GPU Nodes
    |
  Telemetry SDK / Agents
    |
  Local Buffer
    |
  Metrics Pipeline ---- TSDB
  Logs Pipeline ------- Log Store
  Traces Pipeline ----- Trace Store
    |
  Query / Dashboard / Alerting
  ```

  **Explain The Blocks**
  * SDK/Agents collect structured telemetry.
  * Local Buffer protects services from backend outages.
  * Separate pipelines optimize metrics, logs, and traces.
  * Stores support queries, dashboards, and alerts.
  * Redaction/classification protects sensitive payloads.

  **Explain The Control Flow**
  * Operators configure sampling, retention, redaction, alert thresholds, and incident dashboards.
  * Schema registry manages metric names and dimensions.

  **Explain The Data Flow**
  * Services emit telemetry with request IDs, agents buffer and ship, pipelines aggregate/index, dashboards and alerts query stores.

  **Deep Dive Topics And Questions**
  * **Question: Full tracing or sampled tracing?**
    * Full: pros best debugging; cons enormous cost and privacy exposure.
    * Sampled: pros cost-effective; cons can miss rare failures.
    * Recommendation: adaptive sampling, always keep errors/slow requests, sample healthy traffic.
  * **Question: High-cardinality labels?**
    * Allow all: pros flexible debugging; cons TSDB blowups.
    * Strict whitelist: pros cost control; cons slower investigations.
    * Recommendation: governed dimensions for metrics, richer attributes in traces/logs with retention controls.

---

## 20. Design A Secure Code Execution Sandbox

* **Question**
  * Design a secure code execution platform for running user-generated code from an AI assistant.

* **Answer**

  **Scope**
  * Execute untrusted code, manage files, timeouts, network policy, resource limits, logs, and results.

  **Functional Requirements**
  * Run code in isolated environment.
  * Upload/download files within session.
  * Enforce CPU, memory, disk, time, and network limits.
  * Return stdout/stderr/artifacts.
  * Tear down or persist session state based on policy.

  **Non Functional Requirements**
  * Strong isolation.
  * Fast startup.
  * Abuse resistance.
  * Cost control.
  * Auditable execution.

  **High Level Design And Diagram**

  ```text
  Assistant / Client
    |
  Execution API
    |
  Policy + Quota Check
    |
  Sandbox Scheduler
    |
  Isolated Runtime Pool
    |---- Container / MicroVM
    |---- Ephemeral FS
    |---- Network Guard
    |
  Artifact Store / Logs / Metrics
  ```

  **Explain The Blocks**
  * Execution API receives code and session context.
  * Policy/Quota checks allowed tools, limits, and tenant tier.
  * Scheduler assigns execution to warm or cold sandbox.
  * Runtime Pool isolates untrusted code.
  * Network Guard restricts egress.
  * Artifact Store captures outputs safely.

  **Explain The Control Flow**
  * Security/platform teams configure allowed packages, network rules, runtime images, resource budgets, retention, and abuse policies.
  * Runtime image rollouts use canaries and vulnerability scans.

  **Explain The Data Flow**
  * Code request is validated, sandbox allocated, files mounted, code executes with limits, output/artifacts are collected, session state is saved or destroyed.

  **Deep Dive Topics And Questions**
  * **Question: Containers or microVMs?**
    * Containers: pros fast and efficient; cons weaker isolation boundary.
    * MicroVMs: pros stronger isolation; cons slower startup and higher overhead.
    * Recommendation: microVMs for untrusted external code, containers may work for trusted/internal workloads.
  * **Question: Warm pool or cold start every time?**
    * Warm pool: pros low latency; cons cost and state leakage risk.
    * Cold start: pros clean isolation and lower idle cost; cons slower UX.
    * Recommendation: warm pool with strict reset/snapshot hygiene and short TTL.

---

## 21. Design An Agent Tool-Use Control Plane

* **Question**
  * Design the platform layer that lets AI agents call hosted tools, remote MCP servers, customer functions, file search, web search, and computer-use environments safely across many enterprise tenants.

* **Answer**

  **Scope**
  * Agent orchestration and tool execution for API and enterprise agent products.
  * Include tool registration, permissioning, runtime policy, secrets, approvals, audit, sandboxing, retries, and observability.
  * Exclude training the base model and implementing every individual third-party connector.

  **Functional Requirements**
  * Register built-in tools, remote MCP servers, and customer-defined functions with schemas, owners, versions, and scopes.
  * Let agents request tool calls while a policy engine decides whether the call is allowed, needs approval, or must be blocked.
  * Execute tool calls with tenant-scoped credentials, network controls, rate limits, idempotency keys, and timeout budgets.
  * Persist tool-call traces, inputs/outputs where policy allows, approvals, errors, and billing events.
  * Support admin controls for disabling tools, rotating secrets, revoking connectors, and inspecting agent runs.

  **Non Functional Requirements**
  * Low added latency for simple tool calls.
  * Strong tenant isolation and least-privilege credential handling.
  * Auditable decisions and replayable incident investigations.
  * Safe degradation if a tool, connector, or policy service fails.
  * Clear abuse controls for web access, computer control, code execution, and customer data retrieval.

  **High Level Design And Diagram**

  ```text
  Agent Runtime / Responses API
    |
  Tool Call Broker
    |
    +--> Tool Registry / Schema Store
    +--> Policy + Approval Engine
    +--> Secrets Broker
    +--> Rate Limit / Quota
    |
  Tool Execution Plane
    |---- Hosted Tools: File Search / Web Search / Code Interpreter
    |---- Remote MCP Gateway
    |---- Customer Function Gateway
    |---- Computer-Use Sandbox
    |
  Audit Log / Trace Store / Billing Events / Admin Console
  ```

  **Explain The Blocks**
  * Agent Runtime emits structured tool-call intents with model, tenant, user, agent, and conversation context.
  * Tool Registry stores tool schemas, versions, risk class, owner, auth mode, and data-handling policy.
  * Policy and Approval Engine evaluates tenant policy, user grants, tool risk, data sensitivity, and step-up approval requirements.
  * Secrets Broker mints short-lived credentials or scoped tokens without exposing long-lived secrets to the model.
  * Tool Execution Plane runs the call behind network, timeout, concurrency, and output-size controls.
  * Audit and Trace Store records what was requested, what policy decided, what executed, and what result returned.

  **Explain The Control Flow**
  * Admins register tools, approve connectors, define risk tiers, configure allowed domains, set data-retention rules, and publish policy versions.
  * Tool schemas and policies are canaried and cached in the runtime so agents do not depend on a slow admin path.
  * Emergency controls can disable a tool globally, for one tenant, or for one agent version.

  **Explain The Data Flow**
  * Agent proposes a tool call.
  * Broker validates the schema and asks the policy engine for a decision.
  * If approved, execution gets scoped credentials, calls the hosted tool or remote endpoint, normalizes the result, redacts sensitive fields, and returns structured output to the agent.
  * Telemetry, audit events, usage counters, and security signals flow asynchronously to storage and admin surfaces.

  **Deep Dive Topics And Questions**
  * **Question: Prompt-level policy or external policy enforcement?**
    * Prompt-only rules are flexible and cheap, but the model can misunderstand or be manipulated.
    * External policy enforcement is auditable and consistent, but adds latency and requires schema discipline.
    * Recommendation: keep high-level intent in the prompt, but enforce permissions, network access, credential scope, and high-risk actions outside the model.
  * **Question: Should agents call tools directly or through a broker?**
    * Direct calls reduce latency and implementation overhead, but scatter auth, logging, retries, and abuse controls.
    * A broker centralizes policy, secrets, idempotency, and observability, but can become a critical dependency.
    * Recommendation: use a highly available broker with local policy caches, per-tool circuit breakers, and tenant-level kill switches.
  * **Question: How do you handle computer-use and code-execution tools?**
    * Treat them as high-risk tools with stronger isolation, short sessions, egress allow-lists, screen/file redaction, and explicit human approval for sensitive actions.
    * Persist enough trace detail for audit, but separate sensitive artifacts from general logs and honor tenant retention policies.

---

## 22. Design An Agent-Ready Workflow Automation Platform

* **Question**
  * Design a Codex-style platform that lets teams turn recurring engineering and operations work into reviewable, reusable, agent-executable workflows across notebooks, internal tools, web apps, and scheduled jobs.

* **Answer**

  **Scope**
  * Support bounded noninteractive runs, resumable long-running tasks, browser-embedded tool surfaces, scheduled automations, approval gates, shared workflow artifacts, and replayable run history.
  * Include Codex-harness-style execution, WebMCP-style browser tools, workflow notebooks, context indexes, task state, permissions, and audit.
  * Exclude training base models and replacing every existing CI/orchestration system.

  **Functional Requirements**
  * Define reusable workflows with instructions, allowed tools, required inputs, expected outputs, and approval points.
  * Start runs from CLI jobs, SDK/API callers, schedules, chat, or an agent-ready web page.
  * Let agents read and update workflow artifacts through structured tools instead of screen-only UI guessing.
  * Persist commands, decisions, artifacts, status, approvals, failures, and final summaries for future runs.
  * Support resume, cancellation, retries, notifications, and human review before consequential actions.

  **Non Functional Requirements**
  * Strong sandboxing and least-privilege tool access.
  * Deterministic audit trails for regulated engineering and operations workflows.
  * Low-friction authoring so operators capture context while doing the work.
  * Clear blast-radius limits for automated background execution.
  * Portability across local apps, static web apps, CI jobs, and internal operational dashboards.

  **High Level Design And Diagram**

  ```text
  User / Schedule / CI / Internal App
    |
  Workflow Entry Point
    |---- codex exec for bounded jobs
    |---- Codex SDK for programmatic task control
    |---- App Server for persistent conversations and approvals
    |---- WebMCP for browser-side app tools
    |
  Agent Harness
    |
    +--> Context Loader / Artifact Index
    +--> Tool Registry + Permission Policy
    +--> Sandbox / Shell / Browser Runtime
    +--> Approval + Notification Service
    +--> Run Log / Notebook / Trace Store
    |
  Shared Artifacts / Google Drive / Repo / Ticket / Dashboard
  ```

  **Explain The Blocks**
  * Workflow Entry Point chooses the integration layer: noninteractive CLI execution for bounded jobs, SDK calls for applications that start and resume tasks, app-server sessions for streamed events and approvals, or WebMCP for live browser-side app tools.
  * Agent Harness owns the loop: gather context, plan, call tools, request approval, checkpoint, and produce structured output.
  * Context Loader reads prior notebooks, Markdown indexes, run history, tickets, runbooks, and repository files to avoid relearning the workflow each time.
  * Tool Registry exposes only the tools a workflow needs, with schemas, risk class, resource scope, and approval mode.
  * Sandbox Runtime isolates shell, filesystem, browser, and network access according to workflow policy.
  * Run Log and Notebook Store keep reviewed commands, outputs, decisions, artifacts, and a compact index that future agents can search.

  **Explain The Control Flow**
  * Platform owners define global permission classes, sandbox profiles, network policies, retention rules, and approval requirements.
  * Workflow authors capture a repeatable procedure as instructions plus tool grants, test it in review mode, then publish a version.
  * Schedules, CI jobs, or API callers start pinned workflow versions. Policy decides which steps run automatically, which require approval, and which are blocked.
  * Each run writes a durable summary and artifact index before completion so the next run starts with current operational context.

  **Explain The Data Flow**
  * A trigger creates a task with workflow version, inputs, identity, and target resources.
  * The harness loads context, executes bounded tool calls, updates the notebook or target app through structured APIs, and streams progress.
  * Approval events, command outputs, generated patches, screenshots, links, and final results flow into the trace store and artifact index.
  * Follow-up notifications point reviewers to the exact run, changed artifacts, and pending decision.

  **Deep Dive Topics And Questions**
  * **Question: CLI job, SDK, app server, or WebMCP?**
    * CLI execution is simple for bounded background jobs and CI, but weak for interactive state and approvals.
    * SDK and app-server integrations fit products that need task lifecycle control, streaming, resume, and approval handling.
    * WebMCP fits agent-ready web apps because the browser can expose structured tools beside the human UI without adding a separate backend tool server.
    * Recommendation: choose the narrowest integration that matches the workflow lifecycle; do not force every workflow through one surface.
  * **Question: How do you keep recurring runs from losing context?**
    * Chat history alone is hard to search, easy to fork, and poor as an operational record.
    * Durable notebooks plus Markdown indexes make commands, outcomes, and decisions discoverable by humans and agents.
    * Recommendation: write compact run summaries, link durable artifacts, and promote stable procedures into versioned workflow definitions.
  * **Question: How much autonomy should scheduled workflows have?**
    * Full automation reduces toil but can mutate the wrong target when input discovery or policy is stale.
    * Review-only automation is safe but may not remove enough operational load.
    * Recommendation: use graduated autonomy: read-only discovery by default, automatic low-risk edits in named scopes, and fresh human approval for external writes, destructive actions, broad target sets, or ambiguous state.

---

## How To Use This Pack In A Mock Interview

For each question, spend:

1. 5 minutes on scope and requirements.
2. 10 minutes on high-level design.
3. 10 minutes on control flow and data flow.
4. 20 minutes on one deep dive.
5. 5 minutes on risks, rollout, and what you would measure.

For L7, explicitly say:

* "The control plane is where we configure policy, rollout, quota, and ownership."
* "The data plane is the hot path serving user traffic."
* "The main bottleneck is..."
* "The failure mode I care most about is..."
* "The cost lever is..."
* "The privacy/safety risk is..."
* "I would roll this out by..."

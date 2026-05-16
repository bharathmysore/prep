# Anthropic L7 System Design Prep

These are Anthropic-style representative prompts for L7/staff-level system design practice, not an official or guaranteed current interview question bank. The focus is cloud infrastructure, distributed systems, LLM serving, safety infrastructure, data platforms, reliability, privacy, and operability.

Treat this as a living question catalog. The current numbered sections are an index for navigation, not a fixed question count; future agents can add, remove, merge, or reorder prompts as public sources and target-role needs change. When updating, preserve the generic filename and compute the current count from `## N.` headings if needed.

## Reference Context

* Public interview-prep reports: [Exponent Anthropic system design guide](https://www.tryexponent.com/blog/anthropic-system-design-interview), [Educative Anthropic system design guide](https://www.educative.io/blog/anthropic-system-design-interview), and [System Design Handbook Anthropic guide](https://www.systemdesignhandbook.com/guides/anthropic-system-design-interview/).
* Public company context: Anthropic engineering and infrastructure roles emphasize safety-critical systems, scalable ML infrastructure, reliability, privacy, monitoring, and pragmatic distributed systems judgment.
* Claude Code public docs: [custom subagents](https://docs.anthropic.com/en/docs/claude-code/sub-agents), [hooks](https://docs.anthropic.com/en/docs/claude-code/hooks), [MCP](https://docs.anthropic.com/en/docs/claude-code/mcp), [agent teams](https://code.claude.com/docs/en/agent-teams), [routines](https://code.claude.com/docs/en/routines), [channels](https://code.claude.com/docs/en/channels), and [worktrees](https://code.claude.com/docs/en/worktrees).
* Anthropic product and engineering context: [Claude Tag](https://www.anthropic.com/news/introducing-claude-tag) emphasizes channel-scoped team agents, asynchronous work, scheduled follow-ups, scoped memories, tool access, spend limits, and audit logs; [long-running agent harness guidance](https://www.anthropic.com/engineering/effective-harnesses-for-long-running-agents) emphasizes progress artifacts, incremental work, self-verification, and clean resumable state.
* L7 framing: for every answer, explain control plane, data plane, high-risk tradeoff, failure mode, rollout path, and operational ownership.

---

## 1. Design Claude's Real-Time Inference Serving Stack

* **Question**
  * Design a real-time LLM inference platform like Claude that supports millions of users, multiple models, streaming responses, enterprise tenants, safety checks, and high availability.

* **Answer**
  * **Scope**
    * Online text/chat inference only.
    * Include API ingress, auth, quota, routing, GPU serving, token streaming, safety checks, usage metering, and observability.
    * Exclude model training and offline eval internals unless the interviewer asks.

  * **Functional Requirements**
    * Accept chat/completion requests from web, mobile, and API clients.
    * Authenticate callers and enforce tenant/project quotas.
    * Select model, version, region, and fallback.
    * Stream generated tokens to clients.
    * Support timeout, cancellation, retry, and idempotency for request submission.
    * Apply pre-generation and post-generation safety checks.
    * Emit usage, billing, audit, and operational telemetry.

  * **Non Functional Requirements**
    * Low time-to-first-token and predictable p95/p99 latency.
    * High GPU utilization without starving interactive traffic.
    * Multi-region availability and graceful failover.
    * Strong tenant isolation, privacy, and auditability.
    * Backpressure, admission control, and clear overload behavior.
    * Fast rollback for bad model, policy, or routing changes.

  * **High level design and diagram (at block level)**

    ```text
    Client
      |
      v
    Edge / WAF / Load Balancer
      |
      v
    API Gateway
      |
      v
    Auth + Quota + Abuse Checks
      |
      v
    Model Gateway / Request Router
      |
      +--> Safety Pre-Check
      |
      v
    Inference Scheduler / Dynamic Batcher
      |
      v
    GPU Model Server Pool
      |
      v
    Stream Coordinator
      |
      +--> Safety Post-Check
      |
      v
    Client Stream

    Side systems:
    Model Registry, Config Store, Policy Store, Usage/Billing,
    Audit Logs, Metrics, Traces, Alerting
    ```

    * **Explain the blocks**
      * Edge/WAF handles TLS, regional routing, coarse abuse protection, and request size limits.
      * API Gateway validates requests, assigns request IDs, and manages protocol concerns.
      * Auth/Quota/Abuse checks enforce API keys, tenant entitlements, rate limits, and emergency blocks.
      * Model Gateway chooses model version, region, serving pool, priority class, and fallback behavior.
      * Inference Scheduler batches compatible requests and places them on GPU workers.
      * GPU Model Server Pool runs prefill and decode.
      * Stream Coordinator demultiplexes generated tokens back to the correct client stream.
      * Safety services classify prompts and outputs before final delivery.

    * **Explain the control flow**
      * Operators publish model versions, routing weights, tenant limits, safety policies, and rollout plans.
      * Config changes are validated, versioned, canaried, and pushed into low-latency runtime caches.
      * Deployment controllers roll model artifacts onto serving pools, warm caches, verify health, and shift traffic gradually.
      * Incident controls can activate kill switches, lower max output length, disable a model, or route to fallback pools.

    * **Explain the data flow**
      * Request enters the gateway, passes auth/quota, is routed to a model pool, and enters a scheduler queue.
      * Scheduler batches compatible requests, dispatches them to GPU workers, and streams tokens through the coordinator.
      * Safety decisions apply before generation and again before or during response delivery.
      * Usage, latency, token counts, policy decisions, and errors are emitted asynchronously.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Request routing: how should requests be placed across regions and GPU pools?**
      * Round-robin is simple and robust, but ignores model size, context length, GPU load, and tenant priority.
      * Least-loaded routing improves utilization, but depends on fresh load signals and can oscillate during bursts.
      * Tenant-aware routing protects enterprise SLAs and data residency, but fragments capacity.
      * Cost-aware routing reduces spend, but may hurt latency or complicate debugging.
      * Recommendation: use model-aware and load-aware routing with tenant priority, regional affinity, and explicit fallback.

    * **Streaming safety: when do you show tokens to the user?**
      * Pre-check only is fast, but cannot catch unsafe model outputs.
      * Full post-check before streaming is safer, but removes the streaming UX.
      * Chunk-level moderation balances safety and interactivity, but adds buffering and classifier cost.
      * Recommendation: use pre-check plus buffered/chunk-level post-check for sensitive surfaces, stricter policies for high-risk categories.

    * **Overload handling: what happens when GPU capacity is exhausted?**
      * Queueing maximizes eventual completions, but hurts interactive latency.
      * Fail-fast protects the system, but creates visible errors.
      * Fallback to smaller models preserves availability, but changes quality.
      * Priority admission protects paid/SLA traffic, but must be transparent and abuse-resistant.
      * Recommendation: admission control at gateway, priority queues, retry-after responses, and product-approved fallbacks.

---

## 2. Design Dynamic Batching For Synchronous LLM Requests

* **Question**
  * Design a batching system where users submit synchronous API requests, but the platform internally batches them to improve GPU efficiency.

* **Answer**
  * **Scope**
    * Scheduling layer between API requests and GPU workers.
    * Include request queues, batch construction, deadlines, priorities, cancellation, and streaming demultiplexing.

  * **Functional Requirements**
    * Accept concurrent requests for multiple models.
    * Group compatible requests into batches.
    * Respect latency deadlines and tenant priorities.
    * Stream each user's tokens independently.
    * Handle cancellation and timeout.
    * Track queue wait, latency, batch size, and token throughput.

  * **Non Functional Requirements**
    * High GPU utilization.
    * Low p95/p99 latency for interactive requests.
    * Fairness across tenants and priority classes.
    * Avoid head-of-line blocking.
    * Stable behavior during bursts.

  * **High level design and diagram (at block level)**

    ```text
    API Gateway
      |
      v
    Model-Specific Request Queues
      |
      v
    Batch Builder
      |
      v
    GPU Scheduler
      |
      v
    Model Worker
      |
      v
    Result Demux
      |
      v
    Client Streams
    ```

    * **Explain the blocks**
      * Request queues separate work by model, priority, and approximate context length.
      * Batch Builder groups compatible requests within a small delay budget.
      * GPU Scheduler chooses workers based on memory, load, locality, and placement policy.
      * Model Worker performs batched prefill and decode.
      * Result Demux splits outputs into per-client ordered streams.

    * **Explain the control flow**
      * Operators configure max batch size, max queue delay, priority weights, model limits, and overload policy.
      * Scheduler policy can be updated through canary rollout and compared against latency/utilization SLOs.
      * Runtime services consume cached policy to avoid blocking on a central control plane.

    * **Explain the data flow**
      * A request enters a queue, waits briefly for compatible peers, joins a batch, runs on a GPU worker, and has tokens demuxed to the original stream.
      * Cancellation signals flow from the client to the queue or worker so queued work can be dropped and decode can stop cooperatively.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Batch window size: how long should the system wait to form a batch?**
      * No batching gives best individual latency, but wastes GPU.
      * Fixed small windows are predictable, but underperform across traffic patterns.
      * Adaptive windows improve utilization during bursts and reduce waiting during low traffic, but are harder to tune.
      * Recommendation: adaptive batching with strict per-priority maximum queue delay.

    * **Different context lengths: how do you avoid long prompts slowing short prompts?**
      * Mixing all requests maximizes fill rate, but creates head-of-line blocking.
      * Bucketing by context length improves latency predictability, but creates more queues.
      * Separate long-context pools protect normal traffic, but fragment capacity.
      * Recommendation: bucket by model and approximate context length, with a dedicated long-context pool.

    * **Cancellation: how do you stop wasting GPU after a disconnect?**
      * Ignoring cancellation is simple, but wastes scarce GPU capacity.
      * Queue-level cancellation is cheap and effective before scheduling.
      * Cooperative decode cancellation saves more compute, but requires worker support and cleanup.
      * Recommendation: cancel queued work immediately and support cooperative decode cancellation.

---

## 3. Design A Multi-Tenant GPU Scheduler

* **Question**
  * Design a scheduler that allocates GPU capacity across many tenants, models, batch jobs, real-time inference workloads, and priority classes.

* **Answer**
  * **Scope**
    * GPU resource management for serving and batch inference clusters.
    * Include admission control, quotas, placement, preemption, autoscaling, health, and cost visibility.

  * **Functional Requirements**
    * Track GPU inventory, health, memory, topology, and model placement.
    * Admit, queue, reject, or downgrade workloads.
    * Enforce tenant quotas and priority classes.
    * Support preemption for low-priority or checkpointable work.
    * Recover from node, GPU, and model-server failures.
    * Expose utilization, cost, and fairness metrics.

  * **Non Functional Requirements**
    * High GPU utilization.
    * Predictable latency for interactive workloads.
    * Fairness across tenants.
    * Strong isolation for enterprise/SLA workloads.
    * Fast recovery from failures and deploys.
    * Cost-aware capacity planning.

  * **High level design and diagram (at block level)**

    ```text
    Workload API
      |
      v
    Admission Controller
      |
      v
    Global Scheduler
      |
      +--> Tenant Quota Store
      +--> GPU Inventory / Health Store
      +--> Model Registry
      +--> Priority Policy Store
      |
      v
    Cluster Agents
      |
      v
    GPU Nodes / Model Runtimes
    ```

    * **Explain the blocks**
      * Workload API receives serving pool, batch inference, eval, and warmup requests.
      * Admission Controller protects the cluster from overload and enforces tenant entitlements.
      * Global Scheduler makes placement decisions using quota, inventory, topology, and model requirements.
      * Cluster Agents execute placements, report health, and manage local model runtimes.
      * Autoscaler adds/removes capacity based on queue depth, utilization, and demand forecasts.

    * **Explain the control flow**
      * Platform operators define hardware pools, tenant commitments, burst rules, preemption policy, and maintenance windows.
      * Model owners publish hardware requirements and placement constraints into the registry.
      * Scheduler changes are validated with simulation and canaried before broad rollout.

    * **Explain the data flow**
      * Workloads are submitted, admitted, placed on nodes, and executed by model runtimes.
      * Utilization, queue wait, preemption events, and completion data flow back to metrics, billing, and planning.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Fairness policy: how do you allocate scarce GPUs?**
      * Static quotas are predictable, but waste unused capacity.
      * Weighted fair sharing improves utilization and fairness, but is more complex to debug.
      * Pure business-value routing maximizes revenue, but can starve important internal or low-tier workloads.
      * Recommendation: committed quota plus weighted burst sharing with explicit priority classes and auditability.

    * **Preemption: should real-time traffic evict batch jobs?**
      * No preemption keeps execution simple, but risks interactive latency during spikes.
      * Killing batch jobs frees capacity quickly, but wastes work and creates retry storms.
      * Checkpoint-and-resume reduces wasted work, but requires workload support.
      * Recommendation: reserve interactive headroom and preempt only checkpointable or low-priority workloads.

    * **Placement: where should large models run?**
      * First-fit placement is simple, but ignores topology and can degrade performance.
      * Hardware-specific pools are predictable, but fragment capacity.
      * Topology-aware placement improves throughput for model-parallel workloads, but complicates scheduling.
      * Recommendation: topology-aware placement for large models and replicated hot-model pools for latency-sensitive serving.

---

## 4. Design Token Streaming To Millions Of Users

* **Question**
  * Design the infrastructure for streaming LLM-generated tokens to a very large number of concurrent users with low latency and reliable connection handling.

* **Answer**
  * **Scope**
    * Streaming layer after a request is accepted.
    * Include protocol choice, connection management, buffering, reconnect, cancellation, ordering, and backpressure.

  * **Functional Requirements**
    * Stream tokens in order as generated.
    * Support browser and API clients.
    * Handle disconnects, cancellation, and timeouts.
    * Provide best-effort resume for short interruptions.
    * Emit stream-level metrics and errors.

  * **Non Functional Requirements**
    * Low time-to-first-visible-token.
    * High concurrent connection count.
    * Bounded memory per stream.
    * Regional availability.
    * Secure tenant isolation.
    * Graceful behavior under slow clients.

  * **High level design and diagram (at block level)**

    ```text
    Client
      |
      v
    Edge Load Balancer
      |
      v
    Streaming Gateway
      |
      v
    Stream Coordinator
      |
      +<-- Tokens -- Inference Worker
      |
      v
    Bounded Token Buffer
      |
      v
    Streaming Gateway
      |
      v
    Client
    ```

    * **Explain the blocks**
      * Edge Load Balancer routes clients to nearby healthy streaming gateways.
      * Streaming Gateway maintains SSE, HTTP chunked, or WebSocket connections.
      * Stream Coordinator maps inference job IDs to client streams and sequence numbers.
      * Inference Worker emits tokens and completion/error events.
      * Bounded Token Buffer absorbs short stalls and supports limited reconnect.

    * **Explain the control flow**
      * Operators configure connection limits, idle timeouts, max stream duration, per-tenant concurrency, and slow-client policies.
      * Gateways advertise health and capacity to load balancers.
      * Incident controls can reduce max output tokens, disable resume, or shed low-priority streams.

    * **Explain the data flow**
      * Client opens a stream and receives a stream ID.
      * Inference worker emits ordered token chunks to the coordinator.
      * Coordinator forwards chunks to the gateway, which writes them to the client connection.
      * Disconnect/cancel signals propagate back to stop generation and release buffers.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **SSE vs WebSocket: which protocol should be used?**
      * SSE is simple, HTTP-friendly, and good for one-way token streams, but weak for bidirectional control.
      * WebSocket is flexible and bidirectional, but requires more connection-state operations and load-balancer care.
      * HTTP chunked transfer is simple for APIs, but has less consistent client behavior.
      * Recommendation: SSE for common one-way streaming, WebSocket for richer agent/tool sessions.

    * **Reconnect/resume: what happens after interruption?**
      * Restarting is simple, but duplicates cost and may produce different output.
      * Short token buffers improve UX, but consume memory and only help brief disconnects.
      * Persisting full streams enables stronger resume, but is costly and raises privacy concerns.
      * Recommendation: sequence tokens and keep a short-lived bounded buffer; persist final responses separately when policy allows.

    * **Backpressure: how do you handle slow clients?**
      * Unlimited buffers protect UX briefly, but can exhaust memory.
      * Dropping slow connections protects the platform, but hurts UX.
      * Slowing generation couples client speed to GPU utilization.
      * Recommendation: bounded buffers, per-stream memory caps, slow-client timeout, and cancellation propagation.

---

## 5. Design A Safety / Safeguards Layer For LLM Requests

* **Question**
  * Design a safety system that screens prompts and model outputs in real time while supporting policy updates, audits, and high reliability.

* **Answer**
  * **Scope**
    * Online safety layer around LLM serving.
    * Include prompt checks, output checks, policy control plane, audit logs, human review, and emergency updates.

  * **Functional Requirements**
    * Classify prompts and outputs by safety category.
    * Enforce policy by product, tenant, region, and model.
    * Support actions: allow, block, transform, redact, regenerate, or escalate.
    * Version policies and classifier models.
    * Record auditable safety decisions.
    * Support manual review of ambiguous/high-severity cases.

  * **Non Functional Requirements**
    * Low added latency.
    * High availability independent of main inference path.
    * High recall for severe policy violations.
    * Low false-positive rate for normal workflows.
    * Strong privacy and restricted log access.
    * Fast rollback for bad policy changes.

  * **High level design and diagram (at block level)**

    ```text
    Client Request
      |
      v
    Prompt Safety Classifier
      |
      v
    Policy Engine
      |
      +--> Block / Transform / Allow
      |
      v
    LLM Inference
      |
      v
    Output Safety Classifier
      |
      v
    Policy Engine
      |
      +--> Block / Redact / Regenerate / Allow
      |
      v
    Client

    Side systems:
    Policy Console, Policy Store, Eval Sets, Audit Log,
    Human Review Queue, Safety Metrics
    ```

    * **Explain the blocks**
      * Prompt Safety Classifier detects unsafe user intent before model execution.
      * Output Safety Classifier evaluates generated content.
      * Policy Engine maps classifier scores and context to an enforcement action.
      * Policy Console lets authorized safety teams manage policy versions.
      * Eval Sets test policy/classifier changes before rollout.
      * Audit Log records decisions, model versions, policy versions, and access-controlled metadata.

    * **Explain the control flow**
      * Safety teams publish policy updates through a versioned console.
      * Changes are tested on eval sets, canaried, and pushed into runtime caches.
      * Emergency overrides require strict access control, audit, and post-incident review.

    * **Explain the data flow**
      * Prompt enters classifier, policy engine decides whether inference proceeds, output is classified, final action is applied, and decision metadata is logged.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Inline vs async safety: should checks block the response path?**
      * Inline checks enforce policy before content reaches users, but add latency and can reduce availability.
      * Async checks preserve latency, but unsafe content may already be delivered.
      * Hybrid uses inline checks for severe categories and async analysis for monitoring.
      * Recommendation: inline severe safety checks; use cached/lightweight classifiers where possible.

    * **Precision vs recall: how do you tune classifiers?**
      * High precision reduces false positives, but misses more violations.
      * High recall catches more unsafe content, but can overblock legitimate usage.
      * Tiered thresholds allow different treatment by severity, tenant, and product surface.
      * Recommendation: high recall for severe harms, higher precision for ambiguous categories, plus review/appeal workflows.

    * **Policy rollout: how do you avoid breaking product behavior?**
      * Immediate global rollout is fast for emergencies, but high blast radius.
      * Canary rollout reduces risk, but slows response.
      * Shadow evaluation catches behavior changes before enforcement, but does not protect users by itself.
      * Recommendation: normal path uses shadow plus canary plus rollback; emergency path uses audited narrow overrides.

---

## 6. Design An Evaluation Platform For New Claude Models

* **Question**
  * Design a platform that evaluates new model versions before launch, tracks regressions, supports human review, and gates production rollout.

* **Answer**
  * **Scope**
    * Offline and nearline evaluation infrastructure for model releases.
    * Include dataset registry, eval execution, scoring, human review, dashboards, and rollout gates.

  * **Functional Requirements**
    * Register eval datasets with versioning and access controls.
    * Run eval jobs across candidate and baseline models.
    * Score outputs using automatic metrics, model-based graders, and human review.
    * Compare candidate models against baselines.
    * Track safety, helpfulness, latency, cost, and regression metrics.
    * Produce release reports and enforce launch gates.

  * **Non Functional Requirements**
    * Reproducibility and auditability.
    * Scalable batch execution.
    * Secure handling of sensitive eval data.
    * Low operational friction for researchers and product teams.
    * Clear lineage between dataset, model, prompt template, scorer, and score.

  * **High level design and diagram (at block level)**

    ```text
    Eval Author / Researcher
      |
      v
    Eval Control Plane
      |
      +--> Dataset Registry
      +--> Model Registry
      +--> Eval Spec Store
      |
      v
    Eval Job Scheduler
      |
      v
    Batch Inference Workers
      |
      v
    Scoring Service
      |
      +--> Automated Metrics
      +--> Model Graders
      +--> Human Review Queue
      |
      v
    Results Store / Dashboard / Release Gate
    ```

    * **Explain the blocks**
      * Eval Control Plane manages eval definitions, permissions, and job submission.
      * Dataset Registry stores immutable dataset versions and metadata.
      * Model Registry identifies candidate and baseline models.
      * Eval Scheduler shards work and assigns it to batch inference.
      * Scoring Service computes metrics and routes ambiguous outputs to human review.
      * Results Store powers dashboards, regression alerts, and launch gates.

    * **Explain the control flow**
      * Model owners define release criteria and submit candidate versions.
      * Eval specs are reviewed, versioned, and run against candidate and baseline models.
      * Release gates consume score thresholds and block, approve, or require manual review.

    * **Explain the data flow**
      * Eval prompts are read from immutable datasets, sent to model workers, scored, and written with full lineage.
      * Aggregated results flow to dashboards, release reports, and rollout systems.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Automated vs human evaluation: where should judgment come from?**
      * Automated metrics are cheap and scalable, but miss nuanced quality and safety issues.
      * Human review is high quality for subjective tasks, but expensive and slow.
      * Model-based grading scales better than human review, but can inherit grader bias.
      * Recommendation: automated metrics for broad regression detection, model graders for scalable qualitative checks, targeted human review for high-risk gates.

    * **Reproducibility: how do you make evals comparable over time?**
      * Mutable datasets are easy to improve, but break comparisons.
      * Immutable versions preserve comparability, but require explicit migration.
      * Fixed decoding improves determinism, but may not represent production sampling.
      * Recommendation: version datasets, prompts, model artifacts, scorer versions, and decoding config.

    * **Launch gating: should eval failures automatically block deploys?**
      * Automatic gates prevent obvious regressions, but can block good launches due to noisy metrics.
      * Manual review adds context, but slows releases.
      * Severity-tiered gates let critical safety regressions block automatically while softer shifts trigger review.
      * Recommendation: hard gates for severe safety/security regressions, review gates for ambiguous quality tradeoffs.

---

## 7. Design Enterprise RAG Over Private Documents

* **Question**
  * Design retrieval augmented generation for enterprise customers where users ask questions over private documents, with strict ACL enforcement and source citations.

* **Answer**
  * **Scope**
    * Ingestion, indexing, retrieval, permission enforcement, and answer generation over customer documents.
    * Include connectors, ACL sync, vector/keyword search, reranking, citations, and deletion.

  * **Functional Requirements**
    * Connect to enterprise sources such as Drive, Slack, Notion, SharePoint, or internal wikis.
    * Ingest documents and metadata.
    * Preserve per-user and per-group ACLs.
    * Chunk, embed, index, and search content.
    * Retrieve relevant passages for a user query.
    * Generate grounded answers with citations.
    * Support updates, deletes, and tenant isolation.

  * **Non Functional Requirements**
    * Strong privacy and access control.
    * Low query latency for interactive use.
    * Fresh enough indexes for changed documents.
    * High recall without leaking unauthorized content.
    * Scalable ingestion for large tenants.
    * Compliance-friendly deletion.

  * **High level design and diagram (at block level)**

    ```text
    Enterprise Sources
      |
      v
    Connector Framework
      |
      v
    Ingestion Queue
      |
      v
    Parser / Chunker / Metadata Extractor
      |
      +--> ACL Sync Service
      |
      v
    Embedding Workers
      |
      v
    Vector Index + Keyword Index + Metadata Store

    Query path:
    User Query -> Query API -> Auth/ACL Context -> Retriever
      -> Reranker -> Context Builder -> LLM -> Answer + Citations
    ```

    * **Explain the blocks**
      * Connector Framework pulls documents and change events from customer systems.
      * Ingestion Queue decouples source systems from processing workers.
      * Parser/Chunker converts files into normalized chunks with metadata.
      * ACL Sync Service maintains user/group permissions.
      * Embedding Workers generate vectors.
      * Vector and Keyword Indexes support semantic and lexical retrieval.
      * Context Builder creates prompt context with citations.

    * **Explain the control flow**
      * Tenant admins authorize connectors, choose sources, configure sync frequency, and set retention policies.
      * Indexing configs, chunking strategy, embedding model version, and ACL policy are versioned per tenant.
      * Admin controls can pause ingestion, force reindexing, or delete tenant data.

    * **Explain the data flow**
      * Documents are ingested, chunked, embedded, indexed, and linked to ACL metadata.
      * At query time, user identity and groups are resolved, authorized chunks are retrieved, reranked, assembled into context, and passed to the LLM.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **ACL enforcement: filter before or after retrieval?**
      * Pre-filtering prevents unauthorized candidates from being considered, but can reduce retrieval performance.
      * Post-filtering is simpler for search engines, but risks leakage through ranking/logging and may return too few results.
      * Hybrid filtering pre-filters by tenant/coarse ACL and post-validates exact permissions.
      * Recommendation: pre-filter by tenant and coarse ACL, then exact ACL validation before context assembly.

    * **Freshness vs cost: how quickly should updates appear?**
      * Full reindexing is simple, but expensive and slow.
      * Incremental ingestion is efficient and fresh, but connector-specific and failure-prone.
      * Scheduled sync is simpler, but stale.
      * Recommendation: event-driven incremental sync where supported, scheduled reconciliation for correctness.

    * **Retrieval quality: vector, keyword, or hybrid?**
      * Vector search captures semantics, but can miss exact identifiers.
      * Keyword search handles exact terms, but misses paraphrase.
      * Hybrid retrieval with reranking improves quality, but adds latency and tuning burden.
      * Recommendation: hybrid retrieval with metadata filters and reranking.

---

## 8. Design An MCP / Tool-Use Platform

* **Question**
  * Design a platform that lets an AI assistant safely call external tools, APIs, and customer systems on behalf of users.

* **Answer**
  * **Scope**
    * Tool registry, permissioning, execution, auditing, confirmation, and sandboxing.
    * Include read-only and mutating tools.

  * **Functional Requirements**
    * Register tools with schemas, auth requirements, risk level, and owner.
    * Let users/tenants grant scoped permissions.
    * Validate model-proposed tool calls against schema and policy.
    * Execute tool calls reliably.
    * Require confirmation for high-risk actions.
    * Audit inputs, outputs, decisions, and external effects.
    * Support retries and idempotency where safe.

  * **Non Functional Requirements**
    * Strong security and least privilege.
    * Low latency for common read-only tools.
    * Tenant and execution isolation.
    * Clear blast-radius controls.
    * High auditability for enterprise customers.
    * Robust failure handling.

  * **High level design and diagram (at block level)**

    ```text
    Assistant Runtime
      |
      v
    Tool Call Broker
      |
      +--> Tool Registry
      +--> Permission / Policy Engine
      +--> Secret Manager
      +--> Confirmation Service
      |
      v
    Sandboxed Tool Executor
      |
      v
    External Tool / Customer API
      |
      v
    Result Normalizer
      |
      v
    Assistant Runtime
    ```

    * **Explain the blocks**
      * Assistant Runtime proposes tool calls.
      * Tool Call Broker validates schema, policy, permissions, and risk.
      * Tool Registry stores definitions, owners, schemas, and versions.
      * Permission/Policy Engine checks user and tenant grants.
      * Secret Manager provides scoped credentials at execution time.
      * Confirmation Service pauses risky actions for approval.
      * Sandboxed Executor isolates tools and enforces limits.

    * **Explain the control flow**
      * Tool owners publish schemas and risk metadata.
      * Tenant admins approve tool availability and permission scopes.
      * Policy changes and tool rollouts are validated, canaried, and audited.
      * Emergency controls can disable a tool, revoke secrets, or require confirmation globally.

    * **Explain the data flow**
      * Model proposes a tool call, broker validates it, executor calls the external system, normalized result returns to the assistant, and audit events are recorded.
      * Mutating calls may pause for user confirmation before execution.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Permission model: how do you prevent overpowered tools?**
      * User-level OAuth reflects user authority, but can be hard to manage across tools.
      * Service-account credentials are easier operationally, but risk excessive privilege.
      * Scoped grants balance usability and safety, but require a strong permission model.
      * Recommendation: user-scoped or tenant-scoped least-privilege grants with risk-tiered confirmation.

    * **Mutating actions: when should confirmation be required?**
      * Always confirming is safest, but makes the product tedious.
      * Never confirming is fast, but dangerous.
      * Risk-based confirmation balances UX and safety, but requires accurate classification.
      * Recommendation: require confirmation for external writes, destructive actions, money movement, permission changes, and ambiguous targets.

    * **Sandboxing: how much isolation is enough?**
      * In-process execution is fast, but weakly isolated.
      * Containerized execution improves isolation, but adds overhead.
      * Remote worker pools with egress controls are safer, but more complex.
      * Recommendation: sandbox untrusted or third-party tools with scoped secrets, egress policy, timeouts, and output limits.

---

## 9. Design A Vector Embedding Platform

* **Question**
  * Design a shared platform for generating, storing, indexing, searching, updating, and deleting embeddings for multiple products and enterprise tenants.

* **Answer**
  * **Scope**
    * Embedding generation and vector search infrastructure.
    * Include ingestion, model versioning, index builds, search, metadata filters, re-embedding, and deletion.

  * **Functional Requirements**
    * Accept documents/chunks/events from multiple sources.
    * Generate embeddings using versioned embedding models.
    * Store vectors with metadata and tenant isolation.
    * Build and update vector indexes.
    * Support ANN search with metadata filters.
    * Re-embed data when models change.
    * Delete tenant/user/document data reliably.

  * **Non Functional Requirements**
    * Low-latency search.
    * High recall for relevant results.
    * Scalable bulk ingestion.
    * Cost-efficient storage and compute.
    * Strong tenant isolation and privacy.
    * Measurable quality through retrieval evals.

  * **High level design and diagram (at block level)**

    ```text
    Producers / Connectors
      |
      v
    Ingestion API
      |
      v
    Ingestion Log / Queue
      |
      v
    Embedding Workers
      |
      v
    Vector Store + Metadata Store
      |
      v
    Index Builder
      |
      v
    Search Serving Layer
      |
      v
    Product Retrieval APIs
    ```

    * **Explain the blocks**
      * Ingestion API validates tenant, schema, and data limits.
      * Ingestion Log decouples producers from embedding workers.
      * Embedding Workers call embedding models and batch work efficiently.
      * Vector Store keeps vectors; Metadata Store keeps ACLs, document IDs, timestamps, and versions.
      * Index Builder creates ANN indexes and manages online swaps.
      * Search Serving Layer handles query embeddings, filters, ANN search, and result normalization.

    * **Explain the control flow**
      * Platform owners publish embedding model versions, index parameters, retention policies, and tenant limits.
      * Index builds run as versioned jobs and are promoted after recall/latency validation.
      * Re-embedding campaigns are scheduled with rate limits and rollback plans.

    * **Explain the data flow**
      * Content enters ingestion, is embedded, stored with metadata, indexed, and served through retrieval APIs.
      * Query text is embedded, searched against the correct tenant/index, filtered by metadata, and returned.
      * Delete events flow through vector store, metadata store, index tombstones, and compaction.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Indexing strategy: exact vs approximate search?**
      * Exact search has perfect recall, but is too slow/expensive at large scale.
      * Approximate nearest neighbor is fast and scalable, but can miss relevant results.
      * Hybrid exact reranking on top candidates improves quality, but adds latency.
      * Recommendation: ANN for candidate generation, then exact/reranker scoring on a bounded candidate set.

    * **Model versioning: what happens when embedding models change?**
      * Re-embedding everything immediately gives consistency, but creates huge compute spikes.
      * Lazy re-embedding spreads cost, but mixes vector spaces and complicates search.
      * Dual-index rollout is safer, but doubles storage temporarily.
      * Recommendation: build new indexes in parallel, evaluate quality, then gradually switch tenants/products.

    * **Deletion: how do you guarantee removed data is not retrieved?**
      * Immediate tombstones remove data from search quickly, but require compaction later.
      * Full index rebuild guarantees clean indexes, but is slow and expensive.
      * Metadata-level delete filters are fast, but leave stale vectors physically present.
      * Recommendation: immediate metadata/tombstone filtering plus async compaction and deletion audit.

---

## 10. Design A Batch Inference API

* **Question**
  * Design a batch inference platform where customers submit large offline jobs and receive results later at lower cost.

* **Answer**
  * **Scope**
    * Asynchronous batch inference for text or multimodal model requests.
    * Include job submission, validation, scheduling, GPU execution, result storage, status, cancellation, and billing.

  * **Functional Requirements**
    * Accept batch jobs with many requests.
    * Validate schema, model, quota, and input size.
    * Store input and output artifacts durably.
    * Schedule jobs across GPU capacity.
    * Support job status, cancellation, retry, and partial results.
    * Emit billing and usage events.

  * **Non Functional Requirements**
    * High throughput and low cost per token.
    * Fairness across tenants.
    * Durable execution under worker failures.
    * Clear completion SLOs rather than interactive latency.
    * Secure tenant isolation.
    * Backpressure during large submissions.

  * **High level design and diagram (at block level)**

    ```text
    Customer
      |
      v
    Batch Job API
      |
      +--> Auth / Quota / Validation
      |
      v
    Input Object Store
      |
      v
    Job Metadata Store
      |
      v
    Batch Scheduler
      |
      v
    Shard Queue
      |
      v
    GPU Batch Workers
      |
      v
    Output Object Store
      |
      v
    Status API / Webhook / Billing Events
    ```

    * **Explain the blocks**
      * Batch Job API creates jobs and validates manifests.
      * Input Object Store holds large request files.
      * Job Metadata Store tracks job state, shards, retries, and progress.
      * Batch Scheduler splits jobs into shards and assigns capacity.
      * Shard Queue provides durable work distribution.
      * GPU Batch Workers process shards and write outputs.
      * Status API and webhooks notify customers.

    * **Explain the control flow**
      * Operators configure batch pools, tenant quotas, max job sizes, retry budgets, and pricing classes.
      * Scheduler decides how much spare GPU capacity batch jobs can consume without harming online inference.
      * Model availability and deprecation are driven by model registry config.

    * **Explain the data flow**
      * Customer uploads job data, submits a manifest, scheduler shards it, workers process shards, outputs are written to object storage, and status/billing events are emitted.
      * Failed shards are retried independently with idempotent output paths.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Scheduling: dedicated or spare capacity?**
      * Dedicated capacity gives predictable completion, but may sit idle.
      * Spare capacity lowers cost, but completion time varies and jobs may be preempted.
      * Hybrid capacity balances predictable paid tiers and opportunistic low-cost jobs.
      * Recommendation: offer priority tiers; dedicated pools for guaranteed SLOs and spare/preemptible capacity for cheaper jobs.

    * **Partial failure: how do you avoid restarting huge jobs?**
      * Whole-job retry is simple, but wasteful.
      * Shard-level retry is efficient, but requires idempotency and output assembly.
      * Checkpointed workers can save long-running shards, but add complexity.
      * Recommendation: shard jobs into idempotent units, retry with bounded budgets, and expose partial completion/error reports.

    * **Result delivery: polling or webhooks?**
      * Polling is simple and reliable, but creates load.
      * Webhooks are convenient, but can fail or be misconfigured.
      * Object-store output with status API is durable, but less immediate.
      * Recommendation: durable output in object storage, status API as source of truth, optional webhooks.

---

## 11. Design Distributed Rate Limiting For The Claude API

* **Question**
  * Design rate limiting and quota enforcement for a global API serving many tenants, models, and priority tiers.

* **Answer**
  * **Scope**
    * Request/token/concurrency limits across regions and model pools.
    * Include tenant quotas, local enforcement, global reconciliation, abuse controls, and emergency throttles.

  * **Functional Requirements**
    * Enforce per-user, per-project, per-tenant, and per-model limits.
    * Support request rate, token rate, concurrent request, and spend limits.
    * Provide clear error responses and retry-after hints.
    * Support temporary quota increases and emergency blocks.
    * Emit usage data for billing and quota dashboards.

  * **Non Functional Requirements**
    * Low-latency hot-path checks.
    * High availability even if global quota store is degraded.
    * Bounded overshoot for expensive resources.
    * Predictable multi-region fairness.
    * Auditability for enterprise limits.

  * **High level design and diagram (at block level)**

    ```text
    API Gateway
      |
      v
    Local Rate Limiter
      |
      +--> Local Token Buckets / Counters
      |
      v
    Request Accepted / Rejected

    Async path:
    Usage Events -> Regional Aggregator -> Global Quota Service
      -> Quota Reconciliation -> Config Push -> Local Limiters

    Control:
    Admin Console -> Quota Config Store -> Runtime Caches
    ```

    * **Explain the blocks**
      * Local Rate Limiter makes fast allow/deny decisions.
      * Token buckets track short-window usage.
      * Regional Aggregator summarizes usage and reduces write load.
      * Global Quota Service maintains tenant entitlements and spend state.
      * Config Push distributes updated limits to gateways.

    * **Explain the control flow**
      * Account teams or tenant admins update quotas through controlled workflows.
      * Quota config is versioned and pushed to regional caches.
      * Emergency controls can block abusive keys or reduce global limits quickly.

    * **Explain the data flow**
      * Each request checks local limiters before entering inference.
      * Accepted usage emits events that aggregate regionally and globally.
      * Reconciled state updates local limiters to keep global overshoot bounded.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Centralized vs local enforcement: where should the decision happen?**
      * Centralized checks enforce exact global limits, but add latency and become an availability bottleneck.
      * Local checks are fast and resilient, but can overshoot global quotas during bursts or partitions.
      * Hybrid local checks with global reconciliation balance latency and accuracy.
      * Recommendation: local hot-path enforcement with bounded regional budgets and async global reconciliation.

    * **What should be limited: requests, tokens, concurrency, or spend?**
      * Request limits are simple, but one request can vary dramatically in cost.
      * Token limits track GPU cost better, but output tokens are known only after generation.
      * Concurrency limits protect serving pools from overload.
      * Spend limits help governance, but need near-real-time accounting.
      * Recommendation: enforce concurrency and estimated token limits upfront, reconcile actual token/spend usage after completion.

    * **Abuse handling: how do you respond to attacks?**
      * Static quotas are predictable, but slow to respond.
      * Dynamic anomaly detection catches spikes, but can false-positive important customers.
      * Manual blocks are precise, but slow.
      * Recommendation: combine static quotas, anomaly alerts, temporary throttles, and audited emergency blocks.

---

## 12. Design Conversation History Storage

* **Question**
  * Design storage for user and enterprise conversation history, including retrieval, privacy controls, retention, deletion, and context assembly for future turns.

* **Answer**
  * **Scope**
    * Durable storage for conversations and messages.
    * Include metadata, encryption, retention, deletion, search, summarization, and hot-path context reads.

  * **Functional Requirements**
    * Create, append, read, rename, archive, and delete conversations.
    * Store ordered messages, attachment metadata, model outputs, and safety metadata.
    * Support recent-conversation listing and pagination.
    * Retrieve context for the next model turn.
    * Enforce tenant/user access controls.
    * Apply retention and deletion policies.

  * **Non Functional Requirements**
    * High availability and durability.
    * Low latency for recent history.
    * Strong privacy, encryption, and access audit.
    * Scalable storage growth.
    * Compliance-friendly deletion semantics.
    * Cost-aware cold storage.

  * **High level design and diagram (at block level)**

    ```text
    Client
      |
      v
    Conversation API
      |
      +--> Auth / ACL
      |
      +--> Conversation Metadata Store
      +--> Message Store
      +--> Attachment Metadata Store
      +--> Recent Conversation Cache
      |
      v
    Context Assembly Service
      |
      +--> Summary Store
      +--> Retrieval Index
      |
      v
    Model Serving
    ```

    * **Explain the blocks**
      * Conversation API handles user-visible history operations.
      * Metadata Store tracks ownership, title, timestamps, and retention policy.
      * Message Store stores ordered message records.
      * Recent Conversation Cache accelerates list and active-thread reads.
      * Context Assembly Service selects messages, summaries, or retrieved snippets.
      * Retention/Delete Pipeline applies data policies across stores and indexes.

    * **Explain the control flow**
      * Tenant admins configure retention, export, data-use, and deletion policies.
      * Storage services consume policy snapshots and enforce them at write/read/delete time.
      * Key rotation, retention sweeps, and legal hold controls run as audited workflows.

    * **Explain the data flow**
      * User messages append to the Message Store, metadata updates, cache refreshes, and model context is assembled.
      * Model responses append with usage and safety metadata.
      * Delete requests create durable jobs that remove or tombstone data across stores and indexes.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Storage model: relational, document, or log-structured?**
      * Relational storage gives strong query semantics, but may be harder to scale for huge message volumes.
      * Document storage maps naturally to conversations, but large growing documents are problematic.
      * Log-structured message storage scales appends and ordering, but needs secondary indexes.
      * Recommendation: metadata in a strongly consistent store, messages in partitioned append-friendly storage, search/retrieval in secondary indexes.

    * **Deletion: hard delete or tombstone?**
      * Hard delete reduces retained data, but is hard to coordinate immediately across replicas and indexes.
      * Tombstones stop serving data quickly and help async cleanup, but retain metadata temporarily.
      * Cryptographic deletion by key destruction can be powerful for encrypted blobs, but needs careful design.
      * Recommendation: immediate tombstone for read-path exclusion, async physical deletion with audit.

    * **Context assembly: full history, summary, or retrieval?**
      * Full history is simple and faithful, but expensive and context-limited.
      * Summaries reduce tokens, but can omit details.
      * Retrieval finds relevant prior turns, but adds relevance risk.
      * Recommendation: combine recent turns, durable summaries, and retrieval for long conversations.

---

## 13. Design Observability For LLM Inference

* **Question**
  * Design observability for LLM inference so teams can debug latency, errors, safety behavior, cost, and model-quality regressions without exposing sensitive user data.

* **Answer**
  * **Scope**
    * Metrics, logs, traces, redaction, sampling, dashboards, alerting, and incident workflows for model serving.
    * Include prompt/response privacy constraints.

  * **Functional Requirements**
    * Collect latency, queue wait, TTFT, tokens/sec, error codes, model version, and region.
    * Track GPU utilization, memory, batch size, and worker health.
    * Emit safety decision metrics and policy versions.
    * Support distributed tracing across gateway, router, scheduler, worker, and stream.
    * Provide dashboards and alerting for SLOs.
    * Support privacy-preserving debugging.

  * **Non Functional Requirements**
    * Low overhead on the inference path.
    * High-cardinality support for tenant/model/region dimensions.
    * Sensitive-data minimization.
    * Reliable telemetry during incidents.
    * Useful correlation across systems.
    * Retention controls and access audit.

  * **High level design and diagram (at block level)**

    ```text
    Serving Services
      |
      +--> Metrics Agent
      +--> Trace Collector
      +--> Redacted Log Pipeline
      |
      v
    Regional Telemetry Buffer
      |
      v
    Metrics TSDB / Trace Store / Log Store
      |
      v
    Dashboards / Alerts / Incident Tools
    ```

    * **Explain the blocks**
      * Metrics Agent captures counters, histograms, and gauges.
      * Trace Collector links request spans across services.
      * Redacted Log Pipeline masks sensitive fields before storage.
      * Regional Telemetry Buffer protects serving services from telemetry backend outages.
      * Dashboards and Alerts expose SLO health, capacity, safety, and cost.
      * Access Control restricts sensitive debugging data.

    * **Explain the control flow**
      * Observability owners define SLOs, sampling rates, redaction policy, retention, and access roles.
      * Runtime services consume sampling config and emit telemetry accordingly.
      * Incident workflows may temporarily increase sampling while preserving privacy controls.

    * **Explain the data flow**
      * Each request emits metrics and trace spans across gateway, router, scheduler, worker, and stream.
      * Logs are redacted and sampled before storage.
      * Alerts fire from SLO burn, GPU saturation, queue growth, error spikes, and safety anomalies.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Prompt logging: should prompts/responses be stored?**
      * Full logging gives excellent debugging, but creates major privacy and compliance risk.
      * No content logging protects privacy, but makes quality and safety debugging harder.
      * Redacted/sampled logging balances utility and privacy, but redaction can fail.
      * Recommendation: metadata-only by default, with tightly controlled consented/redacted/time-limited content sampling where policy allows.

    * **Metrics cardinality: how do you avoid excessive cost?**
      * Detailed dimensions improve diagnosis, but high cardinality can overwhelm stores.
      * Aggregation controls cost, but hides tenant-specific issues.
      * Tiered metrics keep high-cardinality detail for short windows and aggregate long term.
      * Recommendation: budget cardinality, preserve tenant-level detail where contractually needed, and use traces for drilldown.

    * **Alerting: what should page humans?**
      * Raw threshold alerts are simple, but noisy.
      * SLO burn alerts align with user impact, but may miss narrow failures.
      * Composite alerts reduce noise, but can be harder to understand.
      * Recommendation: page on user-impacting SLO burn, severe safety failures, capacity exhaustion, and data-loss risks.

---

## 14. Design Incident Detection And Auto-Rollback For Model Serving

* **Question**
  * Design an incident detection and rollback system for model-serving changes, including model versions, routing config, safety policies, and serving binaries.

* **Answer**
  * **Scope**
    * Automated detection and rollback around production model serving.
    * Include canaries, SLO monitors, anomaly detection, rollback orchestration, and human override.

  * **Functional Requirements**
    * Detect latency, error, safety, quality, and cost regressions.
    * Compare canary traffic to baseline.
    * Automatically pause or rollback risky rollouts.
    * Notify owners and create incident context.
    * Support manual override and postmortem analysis.
    * Maintain audit trail of changes and decisions.

  * **Non Functional Requirements**
    * Low false-negative rate for severe regressions.
    * Low false-positive rate for noisy but healthy rollouts.
    * Fast rollback execution.
    * Small rollout blast radius.
    * Clear ownership and debuggability.
    * Highly available control plane.

  * **High level design and diagram (at block level)**

    ```text
    Change Publisher
      |
      v
    Rollout Controller
      |
      +--> Canary Traffic Splitter
      +--> Health Gate Evaluator
      +--> Change Audit Store
      |
      v
    Production Serving Pools
      |
      v
    Telemetry Stream
      |
      v
    Regression Detector
      |
      v
    Rollout Controller -> Pause / Rollback / Continue
      |
      v
    Alerting / Incident Channel
    ```

    * **Explain the blocks**
      * Change Publisher submits model/config/binary changes.
      * Rollout Controller manages staged rollout percentages and rollback state.
      * Canary Traffic Splitter sends controlled traffic slices to the new version.
      * Health Gate Evaluator checks SLOs and thresholds.
      * Regression Detector compares canary and baseline telemetry.
      * Audit Store records who changed what and why.

    * **Explain the control flow**
      * A change starts at 0 percent, moves through canary stages, and advances only after gates pass.
      * Regression detection can pause rollout or trigger rollback for severe issues.
      * Humans can override gates with justification and audit.

    * **Explain the data flow**
      * Traffic splits between baseline and candidate.
      * Telemetry from both paths streams to detectors.
      * Detector decisions feed back to rollout controller and alerting systems.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **What regressions should trigger automatic rollback?**
      * Error/latency regressions are measurable and good auto-rollback candidates.
      * Safety regressions may be severe but require nuanced classification.
      * Quality regressions can be noisy and hard to attribute quickly.
      * Recommendation: auto-rollback on severe latency/error/safety failures; pause and review ambiguous quality regressions.

    * **Canary size: how much traffic is enough?**
      * Tiny canaries limit blast radius, but may not collect enough signal.
      * Large canaries detect issues faster, but expose more users.
      * Tenant-specific canaries catch customer-specific issues, but add complexity.
      * Recommendation: staged rollout with statistically meaningful gates and extra caution for high-risk tenants/surfaces.

    * **Rollback safety: can rollback make things worse?**
      * Config rollback is fast, but may conflict with state changes.
      * Binary rollback is reliable if backward-compatible, but risky after schema changes.
      * Dual-read/write compatibility reduces rollback risk, but adds implementation complexity.
      * Recommendation: require backward compatibility and test rollback as part of release qualification.

---

## 15. Design Model Rollout / Canary Infrastructure

* **Question**
  * Design infrastructure to roll out new model versions safely across products, tenants, and regions.

* **Answer**
  * **Scope**
    * Model artifact release, traffic shifting, policy compatibility, canary, shadow traffic, and rollback.
    * Focus on online serving rollout rather than model training.

  * **Functional Requirements**
    * Register model artifacts and metadata.
    * Validate model compatibility with serving runtime.
    * Deploy model to selected GPU pools.
    * Warm model and caches before traffic.
    * Route traffic by percentage, tenant, product, or region.
    * Support shadow traffic and A/B comparison.
    * Roll back quickly.

  * **Non Functional Requirements**
    * Small blast radius.
    * Reproducibility and auditability.
    * No downtime during rollout.
    * Secure artifact handling.
    * Capacity-aware deployment.
    * Clear ownership across model and infra teams.

  * **High level design and diagram (at block level)**

    ```text
    Model Artifact Build
      |
      v
    Model Registry
      |
      v
    Release Approval / Eval Gate
      |
      v
    Deployment Controller
      |
      v
    Artifact Distributor / Cache Warmer
      |
      v
    Serving Pools
      |
      v
    Traffic Router
      |
      v
    Canary / Shadow / Production Traffic
    ```

    * **Explain the blocks**
      * Model Registry stores artifact versions, checksums, signatures, compatibility, and release state.
      * Eval Gate checks safety, quality, latency, and cost thresholds.
      * Deployment Controller orchestrates artifact distribution and pool readiness.
      * Artifact Distributor warms regional and node-local caches.
      * Traffic Router shifts traffic gradually by rule.
      * Rollback Controller restores previous routing and serving config.

    * **Explain the control flow**
      * Model teams submit artifacts; release workflow validates and promotes through stages.
      * Deployment configs are versioned and canaried region by region.
      * Rollback config remains available and tested before broad rollout.

    * **Explain the data flow**
      * Model artifacts flow from registry to regional stores and GPU nodes.
      * User requests flow through router to baseline, canary, or shadow targets.
      * Telemetry flows back to gates and dashboards.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Shadow traffic: why run requests through a model without showing outputs?**
      * Shadowing measures latency, cost, and some quality signals without user impact.
      * It doubles inference cost for sampled traffic and cannot measure satisfaction directly.
      * It may create privacy constraints because requests are processed by another model.
      * Recommendation: use controlled, policy-compliant shadow traffic for high-risk releases before visible canaries.

    * **Artifact distribution: how do you avoid slow deploys for huge files?**
      * Direct object-store downloads are simple, but can overload storage and delay rollout.
      * Regional caches reduce latency and load, but need integrity checks and invalidation.
      * Peer-to-peer distribution is efficient at scale, but operationally complex.
      * Recommendation: signed content-addressed artifacts, regional caches, node prewarming, P2P only if fanout becomes a bottleneck.

    * **Traffic shifting: by percentage or tenant?**
      * Percentage rollout is simple and statistically useful, but can mix tenants unpredictably.
      * Tenant rollout isolates enterprise impact and simplifies support, but may be less representative.
      * Region rollout contains regional failures, but may miss tenant-specific issues.
      * Recommendation: combine region, tenant tier, and percentage controls with exclusion lists.

---

## 16. Design A Training-Data Ingestion And Filtering Pipeline

* **Question**
  * Design a pipeline that ingests large volumes of training or evaluation data, filters unsafe or low-quality content, deduplicates records, and produces versioned datasets.

* **Answer**
  * **Scope**
    * Data ingestion and preparation infrastructure.
    * Include source ingestion, parsing, quality filters, PII detection, policy filtering, dedupe, provenance, and dataset snapshots.

  * **Functional Requirements**
    * Ingest data from approved sources.
    * Parse and normalize documents.
    * Deduplicate exact and near-duplicate content.
    * Detect/filter PII, malware, policy-violating, or low-quality content.
    * Track provenance and source metadata.
    * Create immutable dataset versions.
    * Support deletion/exclusion requests.

  * **Non Functional Requirements**
    * Very high throughput and cost efficiency.
    * Reproducible dataset builds.
    * Strong access controls.
    * Auditability of inclusion/exclusion decisions.
    * Data lineage from source to dataset version.
    * Policy agility as data rules evolve.

  * **High level design and diagram (at block level)**

    ```text
    Approved Data Sources
      |
      v
    Source Ingestion Jobs
      |
      v
    Raw Data Lake
      |
      v
    Parser / Normalizer
      |
      v
    Filter Pipeline
      +--> PII Detector
      +--> Safety/Policy Classifier
      +--> Quality Scorer
      +--> Deduper
      |
      v
    Curated Data Lake
      |
      v
    Dataset Builder
      |
      v
    Versioned Dataset Registry
    ```

    * **Explain the blocks**
      * Source Ingestion Jobs import data with source metadata.
      * Raw Data Lake preserves immutable source snapshots under access controls.
      * Parser/Normalizer extracts text and structured fields.
      * Filter Pipeline applies PII, safety, quality, and dedupe checks.
      * Curated Data Lake stores approved records with lineage.
      * Dataset Registry records versions, policy versions, and manifests.

    * **Explain the control flow**
      * Data governance teams approve sources, filtering policy, retention, and access.
      * Filter models/rules are versioned and evaluated before use.
      * Dataset builds launch from manifests and are signed off before downstream use.

    * **Explain the data flow**
      * Data flows from sources to raw storage, through parsing and filters, into curated storage, and finally into versioned datasets.
      * Exclusion/delete signals propagate through manifests, curated data, and future dataset builds.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Deduplication: exact vs near-duplicate?**
      * Exact hashing is cheap and deterministic, but misses lightly edited duplicates.
      * Near-duplicate detection improves quality, but can false-positive and costs more.
      * Source-level dedupe is efficient, but misses cross-source duplication.
      * Recommendation: exact hashing first, near-duplicate detection for high-volume corpora, sampled audit for quality.

    * **Filtering: rule-based or model-based?**
      * Rule-based filters are explainable and fast, but brittle.
      * Model-based classifiers catch nuance, but can be biased, expensive, or hard to audit.
      * Hybrid filters combine speed and coverage, but need strong versioning.
      * Recommendation: deterministic rules for clear exclusions and classifiers for nuanced categories.

    * **Lineage: why track provenance carefully?**
      * Minimal metadata lowers storage cost, but makes audits/removals hard.
      * Full lineage supports compliance and debugging, but increases complexity.
      * Recommendation: preserve source, category/license metadata, processing versions, filter decisions, and dataset inclusion lineage.

---

## 17. Design A Distributed Model Artifact Store

* **Question**
  * Design a distributed artifact store for very large model files that supports secure upload, validation, regional replication, rollout, and fast serving-node downloads.

* **Answer**
  * **Scope**
    * Storage and distribution for model weights and related artifacts.
    * Include content addressing, signatures, replication, cache warming, node-local downloads, and integrity checks.

  * **Functional Requirements**
    * Upload large artifacts from build/training pipelines.
    * Validate checksums, signatures, metadata, and compatibility.
    * Store artifacts durably and immutably.
    * Replicate artifacts across regions.
    * Serve artifacts to deployment systems and GPU nodes.
    * Support rollback to previous versions.
    * Audit artifact access and promotion.

  * **Non Functional Requirements**
    * High durability and integrity.
    * Fast regional availability for deploys.
    * Efficient bandwidth usage.
    * Strong access control and signing.
    * Clear versioning and immutability.
    * Resilience to regional storage outages.

  * **High level design and diagram (at block level)**

    ```text
    Build / Training Pipeline
      |
      v
    Artifact Upload API
      |
      v
    Validation Service
      |
      v
    Content-Addressed Object Store
      |
      v
    Regional Replication Service
      |
      v
    Regional Artifact Caches
      |
      v
    Deployment Controller
      |
      v
    Node-Local Cache / GPU Runtime
    ```

    * **Explain the blocks**
      * Upload API accepts large multipart uploads and metadata.
      * Validation Service checks checksum, signature, manifest, and runtime compatibility.
      * Content-Addressed Object Store stores immutable blobs by digest.
      * Replication Service moves artifacts to required regions.
      * Regional Caches reduce download latency and object-store load.
      * Deployment Controller coordinates artifact availability before rollout.

    * **Explain the control flow**
      * Artifact promotion moves through uploaded, validated, replicated, staged, canary, and production states.
      * Access policies control upload, approval, deploy, and deletion.
      * Deployment waits for replication and cache warmup before shifting traffic.

    * **Explain the data flow**
      * Artifacts are uploaded, validated, replicated, pulled into regional/node-local caches, and loaded by model servers.
      * Checksums are verified at each transfer boundary.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Content-addressed vs named-version storage: how should artifacts be identified?**
      * Named versions are user-friendly, but mutable names create ambiguity.
      * Content-addressed blobs guarantee integrity and dedupe, but are less human-friendly.
      * Recommendation: store by content digest and expose release names as immutable metadata pointers.

    * **Distribution: object storage, regional cache, or peer-to-peer?**
      * Direct object-store downloads are simple, but slow/expensive during large fanout.
      * Regional caches reduce latency and load, but need cache management.
      * Peer-to-peer transfer reduces central bandwidth, but adds failure/security complexity.
      * Recommendation: regional plus node-local caches first; P2P only for very large fleet-wide rollouts.

    * **Security: how do you prevent serving a tampered model?**
      * Checksums catch corruption, but not unauthorized substitution if metadata is compromised.
      * Signing verifies publisher identity, but requires key management.
      * Runtime verification catches last-mile tampering, but adds startup overhead.
      * Recommendation: signed manifests, digest storage, least-privilege promotion, runtime checksum verification, and audit.

---

## 18. Design Prompt / KV Caching For LLM Serving

* **Question**
  * Design caching that reduces repeated LLM computation for common prompts, system instructions, retrieval chunks, or prefix states while preserving correctness and privacy.

* **Answer**
  * **Scope**
    * Prompt-response or prefix/KV caching in the inference serving layer.
    * Include cache keys, invalidation, tenant isolation, privacy, correctness, and cost metrics.

  * **Functional Requirements**
    * Cache reusable prompt prefixes or computed KV states.
    * Support tenant/model/version isolation.
    * Respect safety policy and context changes.
    * Evict stale or low-value entries.
    * Measure hit rate, latency savings, and cost savings.
    * Prevent cross-tenant data leakage.

  * **Non Functional Requirements**
    * Low-latency cache lookup.
    * High correctness and deterministic keying.
    * Secure isolation of cached state.
    * Memory-efficient eviction.
    * Compatibility with model/version changes.
    * Graceful fallback on miss.

  * **High level design and diagram (at block level)**

    ```text
    Request Router
      |
      v
    Cache Key Builder
      |
      v
    Prefix / KV Cache Lookup
      |
      +--> Hit -> Inference Worker Decode From Cached State
      |
      +--> Miss -> Full Prefill -> Cache Writer
      |
      v
    Inference Worker
      |
      v
    Token Stream
    ```

    * **Explain the blocks**
      * Cache Key Builder hashes normalized prefix content plus model, tokenizer, tenant scope, policy version, and decoding-relevant config.
      * Prefix/KV Cache stores computed reusable state.
      * Cache Writer stores new states only when safe and valuable.
      * Eviction Controller removes old or low-value entries.
      * Policy Store controls which surfaces and tenants may use shared or private caching.

    * **Explain the control flow**
      * Operators configure cacheable models, tenants, and prompt regions.
      * Model rollout invalidates or partitions caches by model/tokenizer version.
      * Cache pressure and privacy policy determine eviction and sharing scope.

    * **Explain the data flow**
      * Request enters router, cache key is built, cached prefix state is reused on hit, or full prefill runs on miss and may populate cache.
      * Decode continues normally and streams tokens to the client.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **What should be cached: full responses or KV prefixes?**
      * Full response caching is simple for deterministic outputs, but unsafe for personalized or stochastic responses.
      * KV prefix caching saves expensive prefill while still generating fresh output, but is runtime-specific and memory-heavy.
      * Retrieval-chunk caching accelerates repeated enterprise contexts, but has ACL and freshness risks.
      * Recommendation: prefer prefix/KV caching for stable prefixes and tenant-private contexts; full-response cache only deterministic, non-sensitive calls.

    * **Cache isolation: can tenants share cached states?**
      * Shared caches maximize hit rate, but create severe leakage risk.
      * Tenant-private caches are safer, but reduce hit rate.
      * Public-prefix shared caches may be safe for global immutable prompts, but require strict keying.
      * Recommendation: tenant-private by default; share only verified public immutable prefixes.

    * **Invalidation: when is a cache entry stale?**
      * TTL invalidation is simple, but may keep bad entries after policy/model changes.
      * Versioned keys avoid cross-version reuse, but reduce hit rate.
      * Explicit invalidation is precise, but operationally complex.
      * Recommendation: include model, tokenizer, policy, and prompt-template versions in keys, plus TTL and emergency flush.

---

## 19. Design A Web Crawler For Model Or Evaluation Data

* **Question**
  * Design a compliant, scalable web crawler that discovers, fetches, parses, classifies, and stores web content for approved model or evaluation data workflows.

* **Answer**
  * **Scope**
    * Crawler infrastructure, not legal/policy approval itself.
    * Include URL frontier, dedupe, politeness, robots handling, fetch, parsing, filtering, storage, and recrawl.

  * **Functional Requirements**
    * Discover and enqueue URLs from approved seeds.
    * Respect crawl policy, robots directives, and domain rate limits.
    * Fetch pages reliably with retries.
    * Parse content and metadata.
    * Deduplicate URLs and content.
    * Classify content for quality, safety, language, and source metadata.
    * Store raw and processed content with provenance.
    * Support recrawl schedules and removal requests.

  * **Non Functional Requirements**
    * High throughput with bounded domain impact.
    * Policy compliance and auditability.
    * Fault-tolerant queueing and workers.
    * Cost-efficient storage and processing.
    * Security isolation for untrusted content.
    * Clear data lineage.

  * **High level design and diagram (at block level)**

    ```text
    Approved Seed Sources
      |
      v
    URL Frontier
      |
      +--> URL Deduper
      +--> Robots / Policy Checker
      +--> Domain Rate Limiter
      |
      v
    Fetch Workers
      |
      v
    Raw Content Store
      |
      v
    Parser / Extractor
      |
      v
    Classifier / Filter Pipeline
      |
      v
    Processed Content Store
      |
      v
    Recrawl Scheduler / Removal Pipeline
    ```

    * **Explain the blocks**
      * URL Frontier stores crawl candidates and priority.
      * URL Deduper prevents repeated fetches of equivalent URLs.
      * Robots/Policy Checker enforces crawl rules before fetching.
      * Domain Rate Limiter protects external sites and platform reputation.
      * Fetch Workers download content in sandboxes.
      * Parser extracts text, links, metadata, and canonical URLs.
      * Classifier/Filter Pipeline labels and excludes disallowed data.

    * **Explain the control flow**
      * Governance teams approve seed sources, crawl scope, rate limits, retention, and exclusion policy.
      * Crawler operators tune frontier priority, recrawl frequency, and failure thresholds.
      * Emergency controls can stop domains, remove content, or pause the crawler.

    * **Explain the data flow**
      * Seed URLs enter the frontier, are deduped and policy-checked, fetched, parsed, classified, stored, and used to generate additional candidates.
      * Removal/exclusion events flow to processed stores, raw stores, and future crawl suppression lists.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Frontier ordering: breadth-first, priority, or freshness?**
      * Breadth-first is simple and broad, but ignores value and freshness.
      * Priority-based crawling focuses high-value sources, but can bias coverage.
      * Freshness-based recrawl keeps data current, but can overfocus on frequently changing sites.
      * Recommendation: multi-priority frontier using source quality, freshness need, and politeness constraints.

    * **Robots and policy enforcement: where should it happen?**
      * Worker-level checks are simple, but bugs can leak fetches.
      * Frontier-level checks prevent invalid URLs from dispatching, but policy changes require re-evaluating queues.
      * Defense-in-depth checks at both layers are safer.
      * Recommendation: enforce policy before enqueue, before fetch, and during storage, with audit logs.

    * **Handling untrusted content: how do you fetch safely?**
      * Direct fetch workers are fast, but exposed to malicious pages.
      * Sandboxed fetchers reduce risk, but add overhead.
      * Static-only fetching avoids browser exploit risk, but misses dynamic content.
      * Recommendation: sandboxed fetchers with restricted network/filesystem, content-type limits, malware scanning, and careful JavaScript policy.

---

## 20. Design A Distributed File Cache For Model-Serving Clusters

* **Question**
  * Design a distributed file cache that lets model-serving clusters quickly access large model artifacts, tokenizer files, safety models, and configuration bundles.

* **Answer**
  * **Scope**
    * Regional and node-local caching for large immutable artifacts.
    * Include lookup, warming, eviction, integrity validation, fallback, and failure handling.

  * **Functional Requirements**
    * Cache model artifacts and supporting files near GPU nodes.
    * Validate cached files by checksum/signature.
    * Prewarm files before deploys.
    * Evict unused files under disk pressure.
    * Fall back to regional/object storage on miss.
    * Report cache health, hit rate, and corruption.

  * **Non Functional Requirements**
    * Fast startup for model servers.
    * Low central storage bandwidth.
    * High integrity and consistency for immutable artifacts.
    * Resilience to cache node failures.
    * Cost-efficient storage footprint.
    * Simple operational recovery.

  * **High level design and diagram (at block level)**

    ```text
    Deployment Controller
      |
      v
    Cache Prewarm Service
      |
      v
    Regional Cache Tier
      |
      v
    Node-Local Cache Agent
      |
      v
    Model Runtime

    Miss path:
    Node-Local Cache -> Regional Cache -> Object Store
    ```

    * **Explain the blocks**
      * Deployment Controller requests artifacts for upcoming rollouts.
      * Cache Prewarm Service ensures required files exist in target regions and nodes.
      * Regional Cache Tier reduces object-store load.
      * Node-Local Cache Agent downloads, verifies, and exposes files to model runtimes.
      * Artifact Registry provides digests, sizes, and dependency manifests.
      * Eviction Manager removes low-value cached files under pressure.

    * **Explain the control flow**
      * Rollout config tells the prewarm service which artifact versions are needed in which clusters.
      * Cache policy defines disk budgets, eviction priority, replication, and fallback behavior.
      * Health checks and repair jobs replace corrupted or missing cache entries.

    * **Explain the data flow**
      * On prewarm or runtime miss, a node-local agent requests an artifact from regional cache; regional cache fetches from object store if needed.
      * Artifacts are verified by digest before being exposed to the runtime.
      * Cache hit/miss and validation metrics flow to dashboards.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Cache hierarchy: node-local only or regional plus local?**
      * Node-local only gives fastest runtime access, but many nodes may stampede object storage.
      * Regional cache reduces central load, but adds another tier to operate.
      * P2P cache sharing can be efficient, but creates complex failure and security modes.
      * Recommendation: regional cache plus node-local cache; add P2P only if rollout bandwidth becomes limiting.

    * **Eviction: what should be removed first?**
      * LRU is simple, but may evict artifacts needed for imminent rollouts.
      * Size-aware eviction frees space quickly, but may remove expensive-to-download files.
      * Deployment-aware eviction preserves active and next-version artifacts, but needs control-plane signals.
      * Recommendation: protect active and rollback artifacts, prioritize upcoming rollout artifacts, then evict by recency, size, and download cost.

    * **Consistency and integrity: how do you know a cached file is correct?**
      * Trusting filenames is fast, but unsafe.
      * Checksum verification catches corruption, but costs CPU and time.
      * Signature verification adds publisher authenticity, but requires key management.
      * Recommendation: immutable content-addressed paths, checksum verification at download, signed manifests, and periodic scrub for hot artifacts.

---

## 21. Design An Agentic Coding Platform For Long-Running Cloud Workflows

* **Question**
  * Design a platform like Claude Code that lets developers run local, cloud, scheduled, and event-driven coding agents across repositories with subagents, hooks, MCP tools, worktree isolation, approvals, and resumable sessions.

* **Answer**
  * **Scope**
    * Developer-facing agent runtime and control plane for coding workflows.
    * Include session lifecycle, repository checkout, prompt/routine triggers, subagent and teammate orchestration, tool and MCP access, hooks, permissions, audit, and artifact handoff.
    * Exclude model training and IDE-specific UI details unless asked.

  * **Functional Requirements**
    * Start interactive, background, scheduled, API-triggered, and repository-event-triggered agent sessions.
    * Support channel-scoped team agents that can be mentioned by multiple users, remember approved channel context, and continue asynchronous work.
    * Attach one or more repositories and isolate work using branches, worktrees, or ephemeral sandboxes.
    * Support subagents, parallel agent teams, shared task state, and direct user steering.
    * Connect to tools through MCP, plugins, skills, shell, git, issue trackers, CI, and code review systems.
    * Enforce per-tenant policies for tools, permissions, approvals, secrets, network egress, and mutating actions.
    * Persist transcripts, memory, diffs, artifacts, run status, audit events, and resumable checkpoints.
    * Notify users through web, CLI, API callbacks, chat channels, and pull-request comments.

  * **Non Functional Requirements**
    * Strong safety boundaries for file edits, shell commands, credentials, and external writes.
    * Low latency for interactive sessions and durable execution for unattended routines.
    * Clear ownership, auditability, and rollback for every code or workflow mutation.
    * Good cost controls across long contexts, subagents, and parallel sessions.
    * Tenant isolation for enterprise repositories, connectors, secrets, and logs.
    * Reliable recovery from runner loss, model timeouts, tool failures, and partially completed edits.

  * **High level design and diagram (at block level)**

    ```text
    User / API / Schedule / Git Event / Channel
      |
      v
    Session Control Plane
      |
      +--> Auth, Tenant Policy, Approval Service
      +--> Repository And Worktree Manager
      +--> Routine / Trigger Scheduler
      +--> Agent Definition Registry
      |
      v
    Agent Runtime Orchestrator
      |
      +--> Main Agent Session
      +--> Subagent / Team Coordinator
      +--> Hook Engine
      +--> Tool Broker / MCP Gateway
      |
      v
    Isolated Execution Environment
      |
      +--> Git Workspace / Worktree
      +--> Shell And Build Tools
      +--> Connectors And External APIs
      |
      v
    Artifacts, Diffs, PRs, Run Logs, Notifications

    Side systems:
    Session Store, Memory Store, Audit Log, Secrets Vault,
    Metrics/Tracing, Cost Metering, Artifact Store
    ```

    * **Explain the blocks**
      * Session Control Plane owns session creation, identity, billing, tenancy, routine triggers, and lifecycle state.
      * Repository And Worktree Manager checks out repositories, chooses base refs, creates isolated workspaces, and records produced diffs.
      * Agent Definition Registry stores reusable subagent, team role, skill, plugin, MCP, model, and permission profiles with precedence rules.
      * Agent Runtime Orchestrator runs the main agent loop, delegates work, resumes sessions, and applies turn limits, budgets, and cancellation.
      * Subagent / Team Coordinator creates focused workers, tracks shared tasks, routes messages, prevents duplicate task claims, and merges summaries.
      * Hook Engine runs lifecycle gates before prompts, tool calls, file changes, task completion, compaction, worktree creation, and session end.
      * Tool Broker / MCP Gateway normalizes tool schemas, scopes credentials, rate-limits calls, enforces egress rules, and records tool traces.
      * Isolated Execution Environment runs shell, git, tests, and builds with resource, network, and filesystem controls.
      * Channel Agent Service maps chat channels or project rooms to scoped agent identities, approved tools, allowed memory namespaces, spend caps, and audit streams.

    * **Core components and low-level design**
      * **Session state machine**
        * States: `Created -> PreparingWorkspace -> Running -> WaitingForApproval -> WaitingForTool -> Paused -> Completed | Failed | Canceled`.
        * Durable state includes session ID, tenant, user, repository refs, worktree path, model/profile, trigger source, budgets, active subagents, last checkpoint, and output artifact IDs.
        * Transitions are idempotent and guarded by session epochs so duplicate scheduler events or retries cannot start two writers for the same workspace.
      * **Permission and approval engine**
        * Inputs: action type, target resource, command/tool schema, repository path, tenant policy, user grant, risk class, and current approval state.
        * Outputs: allow, deny, ask user, require plan, run read-only, or defer.
        * Cache low-risk decisions per session, but re-evaluate writes, external calls, secret access, permission changes, and broad file operations.
      * **Workspace isolation manager**
        * Creates clean checkouts or worktrees, copies only approved ignored files, fences concurrent writers by branch/worktree, and records ownership of generated diffs.
        * Merge is explicit: subagents return patches or PRs; the main session decides whether to apply or present conflicts.
      * **Hook execution model**
        * Hooks receive structured event payloads and return decisions or annotations.
        * Synchronous hooks gate high-risk operations; async hooks handle lint, background tests, telemetry enrichment, and notifications.
        * Hook failures use fail-closed for security gates and fail-open with alerting for non-critical observers.
      * **Memory and context manager**
        * Separates transcript, project guidance, subagent context, durable memory, tool results, and summarized checkpoints.
        * Uses compaction with citations back to files, commands, and artifacts so resumed sessions can reconstruct why a decision was made.
        * For channel-scoped agents, separates personal direct-message memory from channel memory and enforces admin-approved cross-channel learning rules.

    * **Explain the control flow**
      * Admins define org policy, approved connectors, permission modes, retention, secret scopes, routine availability, and review requirements.
      * Developers define agent profiles, subagents, hooks, skills, MCP servers, routine triggers, and repository scopes.
      * Channel admins bind an agent identity to a channel, approve accessible repositories and tools, set monthly spend limits, and choose whether ambient follow-ups are enabled.
      * A trigger starts a session. The control plane authenticates it, prepares workspace isolation, loads configuration in precedence order, and starts the runtime.
      * The runtime requests tool calls through the broker. The permission engine either allows, blocks, or pauses for approval.
      * Completion publishes artifacts, summaries, diffs, PRs, or notifications, then writes final run state and audit events.

    * **Explain the data flow**
      * Prompt, repository metadata, policy snapshot, and trigger payload enter the runtime.
      * The agent reads files, asks subagents to explore or implement in isolated contexts, and calls tools through the broker.
      * File changes, command outputs, hook results, and tool traces are written to session storage with redaction policy applied.
      * Final diffs and artifacts flow to code review, CI, chat/API callbacks, and the user's resumable session view.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * **Local interactive session vs cloud routine**
      * Local sessions are fast to start and naturally inherit the developer's environment, but they stop when the laptop or terminal goes away and can be hard to govern centrally.
      * Cloud routines run on durable managed infrastructure and can react to schedules, API calls, or repository events, but need explicit repo access, secret scoping, network policy, and cost controls.
      * Recommendation: share the same agent runtime and policy layer, but use separate execution backends. Keep local sessions optimized for latency and cloud routines optimized for durable, auditable completion.

    * **Subagents vs independent agent teams**
      * Subagents are cheap and useful for focused work whose output can be summarized back to the main session.
      * Independent teammates are better when workers must communicate, challenge each other, or own separate work items, but they multiply token cost and coordination complexity.
      * Recommendation: default to subagents for bounded research, review, and implementation slices. Use teams only when parallel ownership or cross-agent debate materially improves outcome quality.

    * **Permissioning model: static allowlists, per-action approval, or risk scoring**
      * Static allowlists are predictable, but too coarse for ambiguous shell commands and external tools.
      * Per-action approval is safer, but interrupts long-running agents and creates user fatigue.
      * Risk scoring reduces prompts, but must be observable and conservative around broad mutations.
      * Recommendation: combine tenant-level allowlists, structured risk classes, per-target policy, and explicit approval for destructive, external-write, secret, broad-scope, and permission-changing actions.

    * **Workspace isolation and merging**
      * Editing the user's active checkout is simple and preserves local state, but parallel work can conflict and unintended files can leak into results.
      * One worktree per agent isolates edits and makes rollback easy, but requires base-ref policy, ignored-file handling, cleanup, and merge UX.
      * Recommendation: run unattended and parallel workers in worktrees or ephemeral sandboxes. Let the main session own final merge, conflict resolution, and PR creation.

    * **Channel-scoped team agent vs personal agent**
      * A personal agent has a simpler privacy model and can use personal connectors, but teammates cannot easily inspect or resume its context.
      * A channel-scoped agent creates shared continuity for a team, but needs strict memory namespaces, admin-managed tool grants, channel-level spend caps, and logs that show who requested each action.
      * Ambient follow-ups are useful for unresolved work and stale threads, but can create notification fatigue or surprising behavior if the trigger policy is vague.
      * Recommendation: model every channel agent as a separate identity with scoped memory, explicit tool grants, spend budgets, audit logs, and conservative ambient triggers.

    * **Hooks as safety gates vs automation hooks**
      * Synchronous hooks can enforce quality, security, and approval rules before risky actions proceed, but add latency and can block progress if unreliable.
      * Async hooks are good for tests, notifications, and enrichment, but cannot protect the current operation.
      * Recommendation: keep security and mutation gates synchronous with strict timeouts; run expensive tests, analysis, and reporting asynchronously with visible status.

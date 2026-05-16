# NVIDIA L7 System Design Prep: Cloud and Distributed Systems Question Catalog

These are NVIDIA-style and publicly reported/representative prompts, not an official or private question bank. The focus is cloud GPU systems, distributed training, inference serving, data infrastructure, reliability, and multi-tenant platforms.

The question count is intentionally not encoded in this file name or title. Treat this as a living company-specific catalog: future agents should add, remove, merge, reorder, and refresh prompts as public sources and the target role evolve.

Useful public references:
- NVIDIA Triton dynamic batching: https://docs.nvidia.com/deeplearning/triton-inference-server/user-guide/docs/user_guide/batcher.html
- NVIDIA TensorRT-LLM docs: https://docs.nvidia.com/tensorrt-llm/
- NVIDIA Dynamo introduction: https://docs.dynamo.nvidia.com/dynamo/getting-started/introduction
- NVIDIA Dynamo disaggregated serving: https://docs.dynamo.nvidia.com/dynamo/design-docs/disaggregated-serving
- NVIDIA Dynamo platform overview: https://developer.nvidia.com/dynamo
- NVIDIA Dynamo architecture flow: https://docs.nvidia.com/dynamo/latest/design-docs/architecture-flow
- NVIDIA GPU Operator: https://docs.nvidia.com/datacenter/cloud-native/gpu-operator/latest/index.html
- NVIDIA KAI Scheduler open-source overview: https://developer.nvidia.com/blog/nvidia-open-sources-runai-scheduler-to-foster-community-collaboration/
- NVIDIA shared GPU tenant-cluster pattern with KAI Scheduler and vCluster: https://developer.nvidia.com/blog/how-to-run-isolated-tenant-kubernetes-clusters-on-shared-gpu-infrastructure/
- NVIDIA DCGM health monitoring: https://docs.nvidia.com/datacenter/dcgm/latest/learn/modules/health-monitoring.html

For L7, the interviewer is testing whether you can turn an ambiguous infrastructure problem into a durable architecture, identify the highest-risk bottleneck, explain tradeoffs, and propose an execution path that multiple teams could operate.

---

## 1. Design a Cloud-Based GPU Resource Management System

* Question
  * Design a cloud-based system that manages GPU resources across many clusters and tenants.

* Answer
  * **Scope**
    * Manage GPU inventory, job admission, tenant quotas, placement, health, utilization, billing signals, and policy.
    * Assume thousands to hundreds of thousands of GPUs across regions, with mixed GPU SKUs and both training and inference workloads.

  * **Functional Requirements**
    * Register GPU nodes and expose available GPU capacity.
    * Submit jobs with GPU count, GPU type, memory, topology, priority, and deadline.
    * Enforce tenant quotas, reservations, and fair sharing.
    * Schedule jobs onto healthy nodes.
    * Support preemption, retries, job cancellation, and status tracking.
    * Integrate with Kubernetes, device plugins, GPU Operator, or a similar node-management stack.

  * **Non Functional Requirements**
    * High scheduler availability.
    * Low scheduling latency for small jobs.
    * High utilization for expensive GPU capacity.
    * Strong tenant isolation.
    * Auditable quota and policy decisions.
    * Graceful degradation during regional failures.

  * **High level design and diagram (at block level)**

    ```text
    User/API/CLI
        |
        v
    Job API + Auth
        |
        +--> Quota/Policy Service
        +--> Job Metadata Store
        +--> Billing/Usage Events
        |
        v
    Scheduler Control Plane
        |
        +--> Cluster Inventory Service <--- Node Agents / GPU Operator
        +--> Placement Engine
        +--> Health Service
        |
        v
    Cluster Executor / Kubernetes API
        |
        v
    GPU Nodes + Device Plugin + Runtime
    ```

    * **Explain the blocks**
      * Job API validates requests and records the desired job state.
      * Quota/Policy Service checks tenant limits, reservations, priority, and preemption rights.
      * Inventory Service tracks GPUs, SKU, memory, topology, health, and current allocation.
      * Placement Engine chooses nodes based on constraints and objectives.
      * Cluster Executor materializes placements through Kubernetes or another cluster manager.
      * Node Agents publish health, GPU metrics, and allocation state.

    * **Explain the control flow**
      * Admins configure tenants, quotas, GPU pools, reservations, preemption policy, and allowed images.
      * Node agents continuously register capacity and health into the inventory service.
      * Scheduler reads policy and inventory snapshots, writes placement decisions, and updates job state.

    * **Explain the data flow**
      * User submits a job.
      * API validates auth and writes job metadata.
      * Quota service approves or rejects admission.
      * Scheduler picks a placement.
      * Executor starts pods/containers on GPU nodes.
      * Node agents stream job and GPU metrics back to monitoring and billing.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: GPU capacity is expensive, heterogeneous, and topology-sensitive. Poor placement can waste capacity or destroy performance.

    * **Explain each of the option/topic with pros and cons**

    * **Option: strict bin packing**
      * Pros: high utilization, fewer partially used nodes, lower cost.
      * Cons: increases fragmentation for large jobs, can create noisy-neighbor problems, and may hurt fault isolation.

    * **Option: topology-aware placement**
      * Pros: improves distributed training performance by preserving NVLink, rack, and network locality.
      * Cons: more complex scheduling and higher queue time when perfect placement is unavailable.

    * **Option: reservation plus fair-share scheduling**
      * Pros: supports important tenants and avoids starvation.
      * Cons: reserved idle capacity can waste GPUs unless unused reservations are borrowable.

    * **Recommended L7 stance**
      * Use hierarchical scheduling: admission control first, then topology-aware placement, then fair-share/preemption. Keep reserved capacity borrowable, but reclaimable.

---

## 2. Design a Distributed Training System for a Trillion-Parameter LLM

* Question
  * Design a platform for distributed training of a trillion-parameter language model across thousands of GPUs.

* Answer
  * **Scope**
    * Cover job orchestration, data loading, distributed workers, communication, checkpointing, failure recovery, and observability.
    * Exclude model architecture research unless it affects infrastructure.

  * **Functional Requirements**
    * Launch multi-node training jobs.
    * Support data, tensor, pipeline, and sequence parallelism.
    * Stream training data fast enough to keep GPUs busy.
    * Store and restore sharded checkpoints.
    * Track experiments, configs, datasets, metrics, and artifacts.
    * Recover from worker, node, network, and storage failures.

  * **Non Functional Requirements**
    * High GPU utilization.
    * Fault tolerance at large scale.
    * Reproducibility.
    * Efficient network usage.
    * Checkpoint durability.
    * Operational visibility into stragglers and bottlenecks.

  * **High level design and diagram (at block level)**

    ```text
    Researcher / Training API
        |
        v
    Experiment + Job Control Plane
        |
        +--> Dataset Catalog
        +--> Model/Config Registry
        +--> Scheduler
        +--> Checkpoint Metadata
        |
        v
    Distributed Training Coordinator
        |
        v
    Worker Groups across GPU Nodes
        |
        +--> Object Storage / Dataset Cache
        +--> Checkpoint Store
        +--> Metrics/Logs/Traces
        +--> High-Speed Fabric for Collectives
    ```

    * **Explain the blocks**
      * Experiment Control Plane stores run configs, versions, ownership, and metadata.
      * Scheduler allocates GPU gangs with the required topology.
      * Training Coordinator bootstraps ranks, world size, rendezvous, and recovery.
      * Worker Groups run the training framework.
      * Dataset Cache prevents object storage from becoming the bottleneck.
      * Checkpoint Store persists sharded state.

    * **Explain the control flow**
      * Researcher submits a training spec with model, dataset, resources, parallelism, and checkpoint policy.
      * Scheduler reserves a gang of GPU nodes.
      * Coordinator assigns ranks and starts workers.
      * Control plane tracks lifecycle, failures, retries, and checkpoint lineage.

    * **Explain the data flow**
      * Workers stream sharded training data.
      * Forward/backward passes run across GPUs.
      * Gradients/activations move through collective communication.
      * Metrics stream to monitoring.
      * Periodic checkpoint shards are written to durable storage.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: at trillion-parameter scale, neither model state nor optimizer state fits on one GPU, and communication can dominate compute.

    * **Explain each of the option/topic with pros and cons**

    * **Option: data parallelism**
      * Pros: simple mental model, scales well for smaller models.
      * Cons: full model replica per worker; gradient all-reduce cost grows.

    * **Option: tensor parallelism**
      * Pros: splits large layers across GPUs and reduces per-GPU memory pressure.
      * Cons: heavy intra-layer communication and strong topology requirements.

    * **Option: pipeline parallelism**
      * Pros: splits layers across stages and lowers memory per GPU.
      * Cons: pipeline bubbles, complex scheduling, and harder debugging.

    * **Option: ZeRO/FSDP-style sharding**
      * Pros: shards optimizer, gradients, and parameters; strong memory savings.
      * Cons: more communication and more complex failure recovery.

    * **Recommended L7 stance**
      * Use hybrid parallelism: tensor parallelism within high-bandwidth GPU groups, pipeline parallelism across groups, and data parallel replicas across racks. Design checkpointing and observability from day one.

---

## 3. Design an LLM Inference Serving Platform

* Question
  * Design a production platform to serve LLM inference requests on GPUs with streaming responses.

* Answer
  * **Scope**
    * Serve multiple models and tenants with low latency, high throughput, streaming tokens, rate limits, and observability.

  * **Functional Requirements**
    * Accept chat/completion requests.
    * Authenticate tenants and enforce quotas.
    * Route to the right model/version.
    * Stream tokens to clients.
    * Support batching, retries, overload handling, and autoscaling.
    * Collect usage, latency, token, and error metrics.

  * **Non Functional Requirements**
    * Low time-to-first-token.
    * High tokens per second per GPU.
    * Predictable tail latency.
    * Tenant isolation.
    * Safe degradation under overload.
    * Fast model rollout and rollback.

  * **High level design and diagram (at block level)**

    ```text
    Client
      |
      v
    API Gateway
      |
      +--> Auth/Quota/Rate Limit
      +--> Request Validator
      |
      v
    Model Router
      |
      +--> Model Registry
      +--> Capacity/Health Service
      |
      v
    Batching + Inference Workers
      |
      +--> GPU Runtime / TensorRT-LLM / Triton
      +--> KV Cache Manager
      |
      v
    Streaming Response + Usage Events
    ```

    * **Explain the blocks**
      * API Gateway terminates client connections and enforces basic request policy.
      * Model Router selects model version and worker pool.
      * Batching layer groups compatible requests.
      * Inference Workers run optimized GPU execution.
      * KV Cache Manager tracks per-request attention state.
      * Usage pipeline records billing and capacity metrics.

    * **Explain the control flow**
      * Operators register model versions, capacity pools, rollout policy, and tenant limits.
      * Autoscaler adjusts worker replicas based on queue depth, token throughput, and latency.
      * Router consumes model health and capacity signals.

    * **Explain the data flow**
      * Client sends request.
      * Gateway validates auth and quota.
      * Router sends it to a compatible worker pool.
      * Batching layer combines requests.
      * GPU engine performs prefill and decode.
      * Tokens stream back while usage and traces are emitted.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: LLM inference has two different phases: prefill is compute-heavy, while decode is memory-bandwidth-heavy and iterative.

    * **Explain each of the option/topic with pros and cons**

    * **Option: no batching**
      * Pros: simplest and lowest per-request queueing delay.
      * Cons: terrible GPU utilization and high cost.

    * **Option: static batching**
      * Pros: efficient for uniform workloads.
      * Cons: bad for variable prompt and output lengths.

    * **Option: dynamic/continuous batching**
      * Pros: much better throughput and GPU utilization; used by modern serving systems.
      * Cons: more complex scheduling and harder tail-latency control.

    * **Option: quantization**
      * Pros: reduces memory and improves throughput.
      * Cons: possible quality loss and validation burden.

    * **Recommended L7 stance**
      * Use continuous batching, streaming, per-tenant isolation, overload admission control, and model-specific SLO pools. Tune for time-to-first-token and tokens/sec separately.

---

## 4. Design Disaggregated LLM Serving with Separate Prefill and Decode Workers

* Question
  * Design an LLM inference architecture where prompt prefill and token decode are served by separate GPU pools.

* Answer
  * **Scope**
    * Optimize long-context and high-throughput serving by splitting prefill and decode.

  * **Functional Requirements**
    * Route requests to prefill workers.
    * Transfer KV cache to decode workers.
    * Stream output tokens from decode workers.
    * Autoscale prefill and decode independently.
    * Handle cache transfer failures and retries.
    * Track per-phase latency and utilization.

  * **Non Functional Requirements**
    * Low time-to-first-token.
    * Efficient GPU specialization.
    * High network throughput.
    * Bounded KV transfer latency.
    * Fault tolerance across phase boundaries.

  * **High level design and diagram (at block level)**

    ```text
    Client
      |
      v
    Gateway
      |
      v
    Disaggregated Router
      |
      +--> Prefill Pool
      |       |
      |       v
      |    KV Cache Transfer
      |       |
      v       v
    Decode Pool ---> Token Stream
      |
      +--> Cache Metadata Service
      +--> Metrics/Tracing
    ```

    * **Explain the blocks**
      * Router chooses a prefill worker based on load and cache hints.
      * Prefill Pool processes prompts and produces KV cache.
      * KV Cache Transfer moves attention state to decode workers.
      * Decode Pool generates tokens.
      * Cache Metadata Service tracks KV location, size, ownership, and TTL.

    * **Core components and low-level design**
      * **Disaggregated Router API**
        * `Submit(request_id, tenant_id, model_id, tokens, routing_hints)` validates model/version, tenant quota, prompt length, and streaming deadline.
        * `SelectPrefill()` scores workers by queue depth, admitted prefill tokens, prefix-cache overlap, GPU health, and topology domain.
        * `SelectDecode(prefill_result)` scores decode workers by active sequences, KV memory headroom, transfer-domain compatibility, tenant isolation, and deadline slack.
        * Durable state is small: request status, selected workers, transfer token, retry count, and idempotency key. Hot routing state comes from worker heartbeats and cache events.
        * Invariant: a request has at most one active prefill attempt and at most one active decode owner per attempt version.
      * **Prefill Worker**
        * Owns prompt tokenization handoff, prefill execution, KV block allocation, and transfer metadata generation.
        * Publishes `PrefillComplete(request_id, attempt, kv_block_ids, bytes, topology_domain, ttl, checksum_or_epoch)`.
        * Uses bounded admission by prefill tokens, not just request count, because long prompts dominate compute.
        * Failure handling: if prefill fails before ownership is published, the router retries on another prefill worker; if it fails after publishing transfer metadata, the decode worker treats the transfer token as invalid and asks the router for a new attempt.
      * **Decode Worker**
        * Owns token generation, streaming cursor, active sequence memory, and final cleanup.
        * Pulls or receives KV blocks using backend-specific metadata, then starts decode only after transfer validation.
        * Uses per-tenant active-sequence limits and cancellation checks so one long-running stream cannot pin decode capacity indefinitely.
        * Invariant: decode never consumes KV blocks from a different model version, tokenizer version, tenant isolation domain, or request attempt.
      * **KV Transfer Layer**
        * Abstracts direct GPU-to-GPU transfer, RDMA/UCX, PCIe, or fallback CPU staging behind a `Transfer(request_id, source, destination, block_ids, deadline)` API.
        * Maintains transfer leases and deadlines; expired transfers are abandoned and cache blocks are reclaimed by the source worker.
        * Exposes bytes transferred, transfer p95/p99, failed transfers, topology-domain misses, and fallback path rate.
      * **Planner / Autoscaler**
        * Separately forecasts prefill and decode needs from incoming prompt tokens, generated tokens/sec, queueing delay, KV transfer pressure, and SLO burn.
        * Scales related components as a group when ratios matter, for example three prefill nodes and six decode nodes in the same high-speed fabric domain.
        * Uses hysteresis and warm pools because cold GPU worker startup can be slower than the request latency budget.

    * **Explain the control flow**
      * Operators configure pool sizes, GPU types, transfer limits, and routing policy.
      * Autoscaler independently scales prefill and decode capacity.
      * Cache metadata controls which workers can receive or reuse KV state.
      * Deployment policy pins compatible model, tokenizer, runtime, and transfer-library versions across the prefill and decode pools before traffic is shifted.
      * The planner reconciles desired prefill/decode ratios, topology constraints, and warm-pool size; it does not sit on the hot request path.
      * Rollout uses shadow traffic and canary routing by tenant/model because routing mistakes usually appear as tail-latency or transfer-failure regressions before total outage.

    * **Explain the data flow**
      * Request enters router.
      * Prefill worker processes prompt and materializes KV cache.
      * KV cache is transferred to selected decode worker.
      * Decode worker generates tokens and streams them to the client.
      * Metrics are emitted for both phases.
      * If transfer fails within the deadline, the router either retries prefill/decode assignment or falls back to monolithic decode-only serving for eligible short prompts.
      * Completion, cancellation, or timeout emits cleanup events so both source and destination workers can release KV blocks.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: prefill and decode stress GPUs differently. Running both on identical workers can underutilize hardware.

    * **Explain each of the option/topic with pros and cons**

    * **Option: monolithic serving**
      * Pros: simpler, no cross-worker KV transfer.
      * Cons: less efficient for long-context workloads and harder to scale phases independently.

    * **Option: disaggregated prefill/decode**
      * Pros: independent scaling, specialized GPU selection, better utilization.
      * Cons: KV transfer overhead, complex routing, new failure modes.

    * **Option: hybrid mode**
      * Pros: use monolithic for short prompts and disaggregated for long-context requests.
      * Cons: more routing logic and capacity planning complexity.

    * **Topic: cache-aware routing versus pure load balancing**
      * Pure load balancing is simple and robust, but it can route a request away from useful prefix or session KV state and force expensive recomputation.
      * Cache-aware routing reduces prefill cost and TTFT for repeated prompts, long conversations, and system-prompt-heavy workloads.
      * The tradeoff is stale or misleading cache metadata: a worker may advertise reusable blocks that are evicted before the request arrives.
      * Prefer load plus cache-aware scoring, with cache hits treated as a bounded bonus rather than an absolute rule. Include model version, tokenizer version, tenant boundary, and cache epoch in the cache key.

    * **Topic: topology-aware KV transfer**
      * Cross-rack or cross-zone KV transfer can erase the gains from disaggregation, especially for long context windows.
      * Topology-aware routing keeps prefill and decode in the same high-speed transfer domain when possible.
      * The cost is lower scheduling flexibility and possible hot spots in popular topology domains.
      * Prefer topology constraints for large KV transfers and latency-critical tenants; allow wider placement for short prompts, batch jobs, or overload degradation.

    * **Topic: fallback and failure semantics**
      * Treat prefill/decode as at-least-once internal execution with externally idempotent request handling.
      * The user-visible stream should start only after decode ownership is established, because replaying a partially streamed response is hard to hide.
      * If prefill succeeds but transfer or decode fails before streaming, retry with a new attempt and invalidate old transfer tokens.
      * If decode fails after streaming starts, return a clear truncated-response error or continue from a checkpoint only if the runtime can prove deterministic token and KV state recovery.

    * **Recommended L7 stance**
      * Start with monolithic plus continuous batching. Add disaggregation for long-context or highly bursty workloads when profiling shows phase imbalance, and make topology-aware KV transfer, bounded cache-aware routing, explicit fallback, and per-phase SLOs first-class parts of the design.

---

## 5. Design KV Cache Management for LLM Inference

* Question
  * Design a KV cache manager for high-throughput LLM serving.

* Answer
  * **Scope**
    * Manage GPU memory used by attention KV cache across requests, sessions, prefixes, and workers.

  * **Functional Requirements**
    * Allocate and free KV cache blocks.
    * Reuse common prefixes where safe.
    * Track cache ownership, TTL, and worker location.
    * Evict low-value cache under pressure.
    * Enforce tenant memory quotas.
    * Expose cache metrics to router and autoscaler.

  * **Non Functional Requirements**
    * Very low allocation overhead.
    * High cache hit rate for repeated prefixes.
    * Strict memory safety.
    * Predictable behavior under memory pressure.
    * Isolation between tenants and sessions.

  * **High level design and diagram (at block level)**

    ```text
    Request Router
       |
       v
    Cache Metadata Service
       |
       +--> Prefix Index
       +--> Tenant Quota State
       +--> Worker Cache Map
       |
       v
    Worker KV Allocator
       |
       +--> GPU KV Blocks
       +--> CPU/NVMe Spill Tier
       +--> Eviction Policy
    ```

    * **Explain the blocks**
      * Prefix Index maps reusable prompt prefixes to cache entries.
      * Worker Cache Map tells the router which worker has useful cache.
      * Worker KV Allocator manages block-level GPU memory.
      * Spill Tier stores lower-priority KV state outside GPU memory when worthwhile.
      * Eviction Policy chooses what to remove under pressure.

    * **Explain the control flow**
      * Operators define tenant memory quotas and cache reuse policy.
      * Router queries metadata to prefer workers with useful cache.
      * Cache manager reports pressure and hit/miss statistics.

    * **Explain the data flow**
      * During prefill, workers allocate KV blocks.
      * Cache metadata is published.
      * Follow-up requests with matching prefixes route to cache-owning workers.
      * Decode consumes KV blocks until request completion or eviction.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: KV cache improves latency but consumes scarce GPU memory, reducing batch size.

    * **Explain each of the option/topic with pros and cons**

    * **Option: no reuse**
      * Pros: simple and predictable.
      * Cons: repeated prompts waste compute.

    * **Option: prefix caching**
      * Pros: improves time-to-first-token for repeated system prompts and conversations.
      * Cons: requires correctness around tokenization, model version, and tenant isolation.

    * **Option: GPU-only cache**
      * Pros: fastest access.
      * Cons: limited capacity and can crowd out active requests.

    * **Option: tiered GPU/CPU/NVMe cache**
      * Pros: larger effective cache.
      * Cons: transfer latency may outweigh benefit for short requests.

    * **Recommended L7 stance**
      * Use block-based GPU cache with prefix reuse and conservative tenant isolation. Add spill tiers only for long-context workloads where measured reuse justifies transfer cost.

---

## 6. Design a Distributed Cache for GPU Workloads

* Question
  * Design a distributed cache for model weights, datasets, embeddings, and intermediate artifacts used by GPU jobs.

* Answer
  * **Scope**
    * Provide low-latency, high-throughput access to large immutable or versioned artifacts near GPU workers.

  * **Functional Requirements**
    * Cache model artifacts and dataset shards.
    * Support content-addressed keys and versioned metadata.
    * Validate checksums.
    * Replicate hot artifacts across regions/clusters.
    * Evict old or cold data.
    * Expose cache hit rate and bandwidth metrics.

  * **Non Functional Requirements**
    * High read throughput.
    * Low startup latency for GPU jobs.
    * Data integrity.
    * Cost-efficient storage.
    * Resilience to cache node failure.

  * **High level design and diagram (at block level)**

    ```text
    GPU Job / Inference Worker
        |
        v
    Local Node Cache
        |
        v
    Cluster Cache Service
        |
        +--> Artifact Metadata Service
        +--> Peer Cache Nodes
        +--> Regional Object Storage
    ```

    * **Explain the blocks**
      * Local Node Cache serves repeated reads on the same GPU host.
      * Cluster Cache stores hot artifacts shared by many nodes.
      * Artifact Metadata Service maps logical versions to content hashes.
      * Object Storage is the durable source of truth.
      * Peer Cache Nodes can exchange chunks for faster fanout.

    * **Explain the control flow**
      * Operators define cache size, eviction policy, prewarm rules, and artifact trust policy.
      * Metadata service publishes artifact manifests and versions.
      * Cache nodes report health and capacity.

    * **Explain the data flow**
      * Worker asks for artifact.
      * Local cache checks for chunks.
      * Misses go to cluster cache.
      * Remaining misses go to object storage.
      * Chunks are verified and stored locally.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: large model/dataset downloads can delay expensive GPU jobs and overload object storage.

    * **Explain each of the option/topic with pros and cons**

    * **Option: local-only cache**
      * Pros: very fast after warmup and simple.
      * Cons: poor sharing and expensive cold starts.

    * **Option: centralized cluster cache**
      * Pros: better sharing and easier management.
      * Cons: can become a bottleneck or single failure domain.

    * **Option: peer-to-peer distribution**
      * Pros: excellent fanout for massive rollouts.
      * Cons: harder security, debugging, and bandwidth control.

    * **Recommended L7 stance**
      * Use content-addressed artifacts with local plus cluster caches. Add P2P for large fleet rollouts after building strong integrity checks and throttling.

---

## 7. Design a High-Throughput Training Data Pipeline

* Question
  * Design a pipeline that feeds training data fast enough to keep thousands of GPUs saturated.

* Answer
  * **Scope**
    * Cover ingestion, preprocessing, sharding, cataloging, streaming, caching, and lineage.

  * **Functional Requirements**
    * Ingest raw data from multiple sources.
    * Validate, deduplicate, filter, and transform data.
    * Create versioned datasets.
    * Shard data for distributed workers.
    * Stream batches to GPU jobs.
    * Track lineage from raw source to training run.

  * **Non Functional Requirements**
    * Extremely high throughput.
    * Reproducibility.
    * Backpressure handling.
    * Cost-efficient storage.
    * Data quality and privacy controls.

  * **High level design and diagram (at block level)**

    ```text
    Raw Sources
       |
       v
    Ingestion Service
       |
       v
    Validation/Dedup/Transform
       |
       +--> Dataset Catalog
       v
    Sharded Dataset Store
       |
       v
    Data Loader + Cache
       |
       v
    GPU Training Workers
    ```

    * **Explain the blocks**
      * Ingestion Service accepts data from logs, files, streams, and partner feeds.
      * Validation/Dedup/Transform creates clean training-ready records.
      * Dataset Catalog tracks schema, version, lineage, and access rules.
      * Sharded Dataset Store optimizes large sequential and parallel reads.
      * Data Loader prefetches and batches data near GPU workers.

    * **Explain the control flow**
      * Data owners define ingestion rules, schemas, privacy policy, and dataset versions.
      * Training jobs request a dataset version.
      * Catalog resolves manifests and shard assignments.

    * **Explain the data flow**
      * Raw data enters ingestion.
      * Pipeline validates and transforms it.
      * Shards are written to storage and cataloged.
      * Training workers read assigned shards through cache and prefetch layers.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: if data cannot be delivered fast enough, GPUs idle and training cost explodes.

    * **Explain each of the option/topic with pros and cons**

    * **Option: online transforms**
      * Pros: flexible, easy to experiment.
      * Cons: CPU-bound transforms can starve GPUs.

    * **Option: precomputed datasets**
      * Pros: fastest training path and reproducible.
      * Cons: storage-heavy and slower iteration when transforms change.

    * **Option: hybrid**
      * Pros: precompute expensive stable transforms, do cheap augmentation online.
      * Cons: more pipeline complexity.

    * **Recommended L7 stance**
      * Version datasets and precompute heavy transforms. Keep lightweight stochastic augmentation near workers and monitor GPU input starvation as a first-class SLO.

---

## 8. Design a Model Artifact Distribution System

* Question
  * Design a system that distributes large model artifacts to GPU clusters globally.

* Answer
  * **Scope**
    * Move signed, versioned model artifacts from build/registry systems to many serving and training clusters.

  * **Functional Requirements**
    * Register model versions and manifests.
    * Sign and verify artifacts.
    * Replicate artifacts across regions.
    * Support staged rollout and rollback.
    * Prewarm clusters before deployment.
    * Track which clusters run which versions.

  * **Non Functional Requirements**
    * Integrity and provenance.
    * Fast fanout.
    * Low impact on serving workloads.
    * Regional resilience.
    * Auditability.

  * **High level design and diagram (at block level)**

    ```text
    Model Build Pipeline
        |
        v
    Model Registry + Signed Manifest
        |
        v
    Global Replication Service
        |
        +--> Regional Artifact Stores
        +--> Cluster Prewarm Agents
        +--> Deployment Control Plane
        |
        v
    GPU Serving/Training Clusters
    ```

    * **Explain the blocks**
      * Model Registry stores version metadata, compatibility, and manifests.
      * Signed Manifest lists chunks, checksums, model config, and trust metadata.
      * Replication Service copies artifacts to regional stores.
      * Prewarm Agents download artifacts before traffic cutover.
      * Deployment Control Plane maps tenants and endpoints to versions.

    * **Explain the control flow**
      * Release owner publishes model version.
      * Registry validates signature and compatibility.
      * Deployment policy decides target regions and rollout stages.
      * Clusters report artifact readiness.

    * **Explain the data flow**
      * Artifacts are chunked and uploaded.
      * Replication copies chunks to regions.
      * Cluster caches download and verify chunks.
      * Serving workers load the verified local artifact.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: model files can be hundreds of GBs or larger, and synchronized rollouts can overload storage and networks.

    * **Explain each of the option/topic with pros and cons**

    * **Option: direct object-store download**
      * Pros: simple and durable.
      * Cons: poor fanout and slow cold starts.

    * **Option: regional cache**
      * Pros: reduces cross-region bandwidth and improves reliability.
      * Cons: extra consistency and cache invalidation logic.

    * **Option: P2P chunk distribution**
      * Pros: efficient massive fanout.
      * Cons: complex trust, throttling, and failure handling.

    * **Recommended L7 stance**
      * Use signed content-addressed chunks, regional caches, and rollout-aware prewarming. Use P2P only for very large synchronized rollouts.

---

## 9. Design Monitoring for a Large GPU Cluster

* Question
  * Design an observability platform for a fleet of GPU clusters running training and inference.

* Answer
  * **Scope**
    * Metrics, logs, traces, alerts, dashboards, anomaly detection, and debugging workflows for GPUs, nodes, jobs, and tenants.

  * **Functional Requirements**
    * Collect GPU utilization, memory, temperature, power, ECC errors, NVLink, PCIe, and network metrics.
    * Collect job metrics, logs, traces, and scheduler events.
    * Correlate workload performance with placement and hardware health.
    * Alert on failures, saturation, and SLO burn.
    * Support tenant, cluster, job, and node views.

  * **Non Functional Requirements**
    * Scalable high-cardinality ingestion.
    * Low-latency alerting.
    * Long-term retention for capacity planning.
    * Cost controls.
    * Reliable collection during partial failures.

  * **High level design and diagram (at block level)**

    ```text
    GPU Nodes / Jobs
       |
       v
    Node Collectors
       |
       +--> Metrics Pipeline
       +--> Logs Pipeline
       +--> Traces Pipeline
       |
       v
    Observability Store
       |
       +--> Alerting Engine
       +--> Dashboards
       +--> Anomaly/Diagnosis Service
    ```

    * **Explain the blocks**
      * Node Collectors gather device, OS, runtime, and job-level signals.
      * Metrics Pipeline handles time series.
      * Logs Pipeline indexes structured logs.
      * Traces Pipeline captures request/job spans.
      * Diagnosis Service correlates symptoms with likely root causes.

    * **Explain the control flow**
      * Operators define collection policy, sampling, retention, and alert thresholds.
      * Alerting engine evaluates SLOs and health rules.
      * Diagnosis workflows guide remediation.

    * **Explain the data flow**
      * Agents emit metrics/logs/traces.
      * Pipelines enrich with job, tenant, node, GPU, and placement metadata.
      * Stores serve dashboards, alerts, and debugging queries.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: GPU fleet telemetry is high-volume and high-cardinality, but debugging requires detailed labels.

    * **Explain each of the option/topic with pros and cons**

    * **Option: collect everything at high resolution**
      * Pros: excellent debugging.
      * Cons: expensive and can overload stores.

    * **Option: sample aggressively**
      * Pros: cheaper and scalable.
      * Cons: can miss short incidents and rare bottlenecks.

    * **Option: adaptive collection**
      * Pros: cheap baseline with high-resolution capture during anomalies.
      * Cons: more complex collectors and triggers.

    * **Recommended L7 stance**
      * Use tiered telemetry: low-cost baseline for all nodes, high-resolution windows on anomaly, and per-job opt-in profiling.

---

## 10. Design Autoscaling for GPU Inference Clusters

* Question
  * Design an autoscaling system for GPU-backed inference services.

* Answer
  * **Scope**
    * Scale model-serving pools based on demand, latency, queue depth, token rate, model mix, and GPU memory.

  * **Functional Requirements**
    * Track demand per model and tenant.
    * Scale worker replicas up/down.
    * Maintain warm capacity for critical models.
    * Handle cold starts and model loading.
    * Prevent overload and request collapse.
    * Support priority traffic.

  * **Non Functional Requirements**
    * SLO protection.
    * Cost efficiency.
    * Stable behavior without oscillation.
    * Fast scale-up despite slow GPU provisioning.
    * Regional failover support.

  * **High level design and diagram (at block level)**

    ```text
    Inference Metrics
       |
       v
    Demand Forecaster + SLO Evaluator
       |
       v
    Autoscaling Controller
       |
       +--> Capacity Manager
       +--> Model Prewarmer
       +--> Scheduler
       |
       v
    GPU Worker Pools
    ```

    * **Explain the blocks**
      * Demand Forecaster predicts near-term traffic.
      * SLO Evaluator checks latency, queue, and error budgets.
      * Autoscaling Controller decides desired capacity.
      * Capacity Manager allocates nodes or shifts reservations.
      * Model Prewarmer loads weights before traffic arrives.

    * **Explain the control flow**
      * Operators set min/max capacity, priority, cost budgets, and SLOs.
      * Controller periodically computes desired replicas.
      * Scheduler places workers and prewarmer ensures readiness.

    * **Explain the data flow**
      * Requests generate metrics.
      * Metrics feed autoscaler.
      * Autoscaler changes worker pool sizes.
      * Router shifts traffic to ready workers.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: GPU servers and model weights have slow cold starts, so reactive CPU-style autoscaling is often too late.

    * **Explain each of the option/topic with pros and cons**

    * **Option: reactive autoscaling**
      * Pros: simple and cost-aware.
      * Cons: can violate SLOs during sudden spikes.

    * **Option: scheduled/forecast scaling**
      * Pros: handles predictable peaks.
      * Cons: fails on unexpected bursts and bad forecasts.

    * **Option: warm pools**
      * Pros: fast failover and low cold-start latency.
      * Cons: expensive idle GPUs.

    * **Recommended L7 stance**
      * Combine forecast scaling, reactive SLO scaling, and small warm pools for critical models. Use admission control when capacity is exhausted.

---

## 11. Design a Multi-Tenant GPU Cloud

* Question
  * Design a cloud platform where many enterprise tenants run GPU workloads securely.

* Answer
  * **Scope**
    * Tenant onboarding, isolation, quota, networking, billing, scheduling, image control, secrets, and audit.

  * **Functional Requirements**
    * Create tenants/projects.
    * Enforce identity, roles, quotas, and budgets.
    * Run training and inference workloads.
    * Provide network and storage isolation.
    * Support shared physical GPU pools with per-team queues, fractional GPU policies, and optional isolated tenant Kubernetes control planes.
    * Collect usage and billing data.
    * Support tenant-specific policies.

  * **Non Functional Requirements**
    * Strong security.
    * Noisy-neighbor control.
    * High availability.
    * Compliance and auditability.
    * Efficient utilization.

  * **High level design and diagram (at block level)**

    ```text
    Tenant Portal / API
       |
       +--> Identity/IAM
       +--> Quota/Billing
       +--> Policy Engine
       |
       v
    Workload Control Plane
       |
       +--> Scheduler
       +--> Network Isolation
       +--> Secrets/Image Policy
       |
       v
    Isolated GPU Runtime Pools
       |
       +--> Telemetry/Audit
       +--> Tenant Storage
    ```

    * **Explain the blocks**
      * IAM authenticates users and service accounts.
      * Policy Engine evaluates allowed actions and resources.
      * Quota/Billing tracks GPU-hours and limits.
      * Scheduler places workloads into isolated pools, shared fair-share queues, or fractional-GPU pools depending on tenant trust and workload class.
      * Tenant Cluster Broker can provision a virtual Kubernetes control plane per trusted internal team while syncing safe workload objects into the shared host cluster.
      * Runtime Pools enforce container, network, and storage isolation.

    * **Explain the control flow**
      * Tenant admins configure projects, users, budgets, and policies.
      * Control plane validates workload specs against tenant policy, allowed CRDs, image rules, and queue bindings.
      * Scheduler admits only policy-compliant workloads and enforces queue quota, limit, over-quota weight, and priority.
      * Isolation policy chooses between shared-node virtual clusters for trusted teams and private nodes or dedicated pools for untrusted tenants.

    * **Explain the data flow**
      * Tenant workload reads/writes tenant storage.
      * Runtime sends telemetry and usage events.
      * Billing and audit systems consume those events.

    * **Core components and low-level design**
      * **Tenant Cluster Broker**
        * API: `CreateTenantCluster(tenant_id, mode, queue_id, gpu_fraction_policy, allowed_crds)` and `BindWorkload(tenant_cluster, workload_uid, queue_id)`.
        * Stores desired tenant control-plane mode, queue bindings, RBAC boundaries, sync rules, and private-node requirements.
        * In shared-node mode, only workload specs, labels, annotations, and status needed by the host scheduler cross the boundary; tenant API servers, CRDs, and RBAC stay logically isolated.
        * Invariant: a tenant cannot bind work to a queue, namespace, node pool, secret, image policy, or storage class that its broker record does not explicitly allow.
      * **GPU Queue Policy Service**
        * Maintains hierarchical queues with guaranteed quota, max limit, over-quota weight, preemption policy, and billing tags.
        * Converts business policy into scheduler-visible CRDs or internal queue objects and emits every allocation, reclaim, and preemption decision to audit.
        * Handles fractional GPU policy separately from full-GPU gang policy because inference sandboxes and training jobs have different isolation and latency risks.
      * **Isolation Decision Engine**
        * Chooses shared virtual control plane, shared namespace, private nodes, or dedicated pool based on tenant trust, data sensitivity, compliance, noisy-neighbor tolerance, and GPU scarcity.
        * Uses runtime telemetry and incident history to move a tenant from shared pools to private pools when contention or policy violations exceed thresholds.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: maximizing utilization conflicts with strict isolation.

    * **Explain each of the option/topic with pros and cons**

    * **Option: shared GPU nodes**
      * Pros: high utilization and lower cost.
      * Cons: noisy-neighbor and stronger security concerns.

    * **Option: dedicated tenant pools**
      * Pros: strong isolation and predictable performance.
      * Cons: lower utilization and higher cost.

    * **Option: mixed model**
      * Pros: critical workloads get dedicated pools, batch workloads share pools.
      * Cons: more policy and scheduling complexity.

    * **Recommended L7 stance**
      * Use a mixed model: dedicated pools for regulated/high-priority tenants, shared preemptible pools for elastic batch, with strict IAM, network policy, image scanning, and audit.

    * **Virtual tenant clusters over shared GPU nodes**
      * A virtual control plane per trusted team gives autonomy for RBAC, CRDs, schedulers, and debugging without physically splitting scarce GPU nodes.
      * The tradeoff is a more complex sync boundary: queue labels, scheduler names, device-plugin resources, and status must flow to the host cluster without leaking other tenants' objects or granting host-cluster authority.
      * Fractional GPUs improve utilization for notebooks, small inference, and development jobs, but they should be explicitly disabled for workloads requiring hard memory isolation, predictable tail latency, or regulated data boundaries.
      * Recommendation: use virtual tenant clusters for trusted internal platform teams, backed by per-team queue quotas and audit. Use private nodes or dedicated pools for untrusted tenants, regulated workloads, and performance-sensitive multi-GPU training.

---

## 12. Design a Topology-Aware GPU Job Scheduler

* Question
  * Design a scheduler that places GPU jobs based on NVLink, PCIe, rack, and network topology.

* Answer
  * **Scope**
    * Focus on placement quality for distributed training and tightly coupled inference workloads.

  * **Functional Requirements**
    * Discover GPU topology.
    * Accept placement constraints.
    * Allocate gangs of GPUs atomically.
    * Minimize communication cost.
    * Support priority, preemption, and backfill.
    * Enforce queue guarantees, over-quota borrowing, reclaim, and consolidation to reduce fragmentation.
    * Explain scheduling decisions.

  * **Non Functional Requirements**
    * Scheduling latency should be bounded.
    * Avoid starvation.
    * Preserve high utilization.
    * Be resilient to stale topology data.
    * Support heterogeneous hardware.

  * **High level design and diagram (at block level)**

    ```text
    Job Queue
      |
      v
    Constraint Normalizer
      |
      v
    Topology Graph Store
      |
      v
    Placement Solver
      |
      +--> Fairness/Preemption Policy
      +--> Fragmentation Scorer
      |
      v
    Cluster Executor
    ```

    * **Explain the blocks**
      * Constraint Normalizer converts job needs into scheduler predicates.
      * Topology Graph Store models GPU, host, NVLink, PCIe, rack, fabric relationships, and DCGM-derived health state.
      * Placement Solver finds feasible allocations from a stable cluster snapshot.
      * Fairness/Preemption Policy computes per-queue fair share from quota, limit, over-quota weight, and priority.
      * Fragmentation Scorer avoids wasting rare shapes.
      * Executor binds jobs to selected nodes.

    * **Explain the control flow**
      * Node discovery updates topology.
      * Operators define placement, queue, reclaim, consolidation, and preemption policies.
      * Scheduler snapshots nodes, jobs, queues, podgroups, topology, and health before each scheduling cycle.
      * Solver scores candidate placements, records decisions, and emits explainability events for allocation, consolidation, reclaim, and preemption.

    * **Explain the data flow**
      * Job enters queue.
      * Scheduler reads topology and resources.
      * Placement is written to cluster manager.
      * Job runs and reports performance metrics back to improve scoring.

    * **Core components and low-level design**
      * **Scheduler snapshot builder**
        * Creates an immutable scheduling-cycle view of nodes, GPUs, links, queues, podgroups, running allocations, pending jobs, taints, and DCGM health.
        * Rejects or defers decisions if inventory generation, queue policy generation, or topology generation is internally inconsistent.
        * Invariant: a single scheduling cycle never mixes partial node state with newer queue policy state.
      * **Podgroup and gang admission**
        * Treats distributed training workers, placement groups, and tightly coupled inference shards as atomic podgroups with minimum member counts.
        * A podgroup remains pending unless the solver can reserve every required member, topology constraint, and device shape.
        * Supports pipelined allocation only when the resources being freed are tied to committed consolidation, reclaim, or preemption actions.
      * **Consolidation and reclaim planner**
        * Tries low-risk moves first: fill free contiguous GPU blocks, relocate movable jobs, then reclaim over-fair-share queues, then preempt lower-priority jobs.
        * Adds cooldowns and disruption budgets so the scheduler does not thrash interactive users or repeatedly move the same workload.
        * Invariant: reclaim restores an under-served queue toward fair share; preemption only violates a lower-priority job after policy and audit checks pass.
      * **Health-aware placement filter**
        * Consumes passive DCGM health watches for PCIe, memory, thermal/power, NVLink, NVSwitch, ConnectX, and driver signals.
        * Filters hard-failed GPUs from new allocations, de-prioritizes warning GPUs for high-priority gangs, and requests active diagnostics only during safe prologue, epilogue, or maintenance windows.
        * Avoids treating a passive healthy result as proof that every subsystem passed an active stress test.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: optimal placement can be NP-hard at fleet scale, especially with gang scheduling and topology constraints.

    * **Explain each of the option/topic with pros and cons**

    * **Option: greedy scheduler**
      * Pros: fast and simple.
      * Cons: can produce poor placements and fragmentation.

    * **Option: exact optimization solver**
      * Pros: high-quality placement.
      * Cons: too slow or brittle at large scale.

    * **Option: heuristic scoring**
      * Pros: practical balance of quality and speed.
      * Cons: requires tuning and may miss optimal placements.

    * **Recommended L7 stance**
      * Use heuristic scheduling with topology-aware scoring, backfill for small jobs, and periodic defragmentation/preemption for high-priority large jobs.

    * **Stable snapshots vs live reads during scheduling**
      * Live reads reduce staleness but create races where a placement decision combines old node capacity with new queue state.
      * Stable snapshots make decisions explainable and reproducible, but every cycle has bounded staleness and needs conflict handling when applying bindings.
      * Recommendation: compute on stable snapshots, then use optimistic binding with generation checks. If the cluster changed materially, abandon the affected placement and retry in the next cycle.

    * **Allocation, consolidation, reclaim, and preemption order**
      * Direct allocation is least disruptive and should handle normal pending work.
      * Consolidation improves fragmentation by moving jobs, but it can burn locality and user time if applied too aggressively.
      * Reclaim enforces inter-queue fairness when queues exceed fair share; preemption handles same-queue or priority inversions but has the highest user-visible cost.
      * Recommendation: use this order by default: allocate, consolidate within disruption budgets, reclaim from over-fair-share queues, then preempt lower-priority work with explicit audit and retry semantics.

---

## 13. Design Checkpointing and Recovery for Distributed Training

* Question
  * Design a checkpointing and recovery system for large distributed training jobs.

* Answer
  * **Scope**
    * Persist and restore model state, optimizer state, scheduler state, RNG state, data-loader position, and metadata.

  * **Functional Requirements**
    * Write periodic sharded checkpoints.
    * Restore jobs after worker or node failure.
    * Validate checkpoint integrity.
    * Support checkpoint retention policy.
    * Track checkpoint lineage.
    * Allow restart on different hardware shape when possible.

  * **Non Functional Requirements**
    * Low training overhead.
    * Durable storage.
    * Fast restore.
    * Consistent multi-rank state.
    * Cost-efficient retention.

  * **High level design and diagram (at block level)**

    ```text
    Training Workers
       |
       v
    Checkpoint Coordinator
       |
       +--> Local Staging
       +--> Shard Writer
       +--> Metadata Committer
       |
       v
    Durable Checkpoint Store
    ```

    * **Explain the blocks**
      * Checkpoint Coordinator triggers and coordinates checkpoint epochs.
      * Local Staging writes temporary shard files near workers.
      * Shard Writer uploads state shards.
      * Metadata Committer atomically publishes a complete checkpoint manifest.
      * Durable Store persists shards and manifests.

    * **Explain the control flow**
      * Training config defines checkpoint interval and retention.
      * Coordinator announces checkpoint barrier or async capture.
      * Metadata commit marks a checkpoint usable only after all shards pass validation.

    * **Explain the data flow**
      * Workers serialize their shard state.
      * Shards are written to local staging and uploaded.
      * Checksums and sizes are recorded.
      * On restart, workers read manifest and restore assigned shards.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: checkpointing too often burns I/O; checkpointing too rarely loses expensive training progress.

    * **Explain each of the option/topic with pros and cons**

    * **Option: synchronous checkpointing**
      * Pros: simple consistency.
      * Cons: pauses training and can be expensive at scale.

    * **Option: asynchronous checkpointing**
      * Pros: reduces training pause.
      * Cons: extra memory/storage pressure and consistency complexity.

    * **Option: incremental checkpointing**
      * Pros: reduces write volume.
      * Cons: restore becomes more complex and may depend on chains of deltas.

    * **Recommended L7 stance**
      * Use sharded checkpoints with atomic manifests, async upload where possible, and adaptive checkpoint intervals based on job cost, failure rate, and recovery objective.

---

## 14. Design a System to Detect and Diagnose Poor Multi-GPU Performance

* Question
  * Design a system that detects and diagnoses unexpectedly low multi-GPU or multi-node training performance.

* Answer
  * **Scope**
    * Correlate workload metrics, GPU counters, topology, network, logs, and job placement to explain performance problems.

  * **Functional Requirements**
    * Detect slow jobs and stragglers.
    * Compare observed throughput against expected baselines.
    * Collect GPU, CPU, memory, network, and storage metrics.
    * Correlate with topology and placement.
    * Produce likely root-cause recommendations.

  * **Non Functional Requirements**
    * Low overhead by default.
    * Fast triage.
    * High signal-to-noise alerts.
    * Safe opt-in deep profiling.
    * Historical baseline support.

  * **High level design and diagram (at block level)**

    ```text
    Jobs / Nodes / Network
       |
       v
    Lightweight Collectors
       |
       +--> Metrics Store
       +--> Log Store
       +--> Trace/Profile Store
       |
       v
    Diagnosis Engine
       |
       +--> Baseline Comparator
       +--> Topology Correlator
       +--> Recommendation UI/API
    ```

    * **Explain the blocks**
      * Collectors gather GPU utilization, bandwidth, kernel timing, network counters, and logs.
      * Baseline Comparator checks performance against similar workloads.
      * Topology Correlator maps performance to placement and fabric.
      * Recommendation UI reports likely causes and next steps.

    * **Explain the control flow**
      * Operators configure baselines and alert rules.
      * Diagnosis Engine triggers deep profiling only when anomalies appear.
      * Remediation suggestions can feed scheduler rules.

    * **Explain the data flow**
      * Job metrics and node metrics stream to stores.
      * Diagnosis engine joins metrics with job metadata and topology.
      * Output is a ranked list of likely bottlenecks.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: low throughput can be caused by compute, memory, PCIe, NVLink, network, data loading, stragglers, or bad placement.

    * **Explain each of the option/topic with pros and cons**

    * **Option: always-on deep profiling**
      * Pros: best debugging data.
      * Cons: overhead and massive data volume.

    * **Option: lightweight metrics only**
      * Pros: cheap and safe.
      * Cons: may not identify root cause.

    * **Option: adaptive profiling**
      * Pros: low overhead with detailed data during anomalies.
      * Cons: more complex triggers and profiling orchestration.

    * **Recommended L7 stance**
      * Use lightweight always-on telemetry, anomaly detection, then temporary deep profiling with clear guardrails.

---

## 15. Design a Real-Time Video Analytics Platform Using GPUs

* Question
  * Design a GPU-backed platform that analyzes many live video streams in real time.

* Answer
  * **Scope**
    * Ingest video, decode, batch frames, run inference, emit events, and expose APIs for downstream systems.

  * **Functional Requirements**
    * Register video streams.
    * Decode and sample frames.
    * Run object detection or classification models.
    * Track objects across frames.
    * Emit alerts and analytics events.
    * Store selected clips and metadata.

  * **Non Functional Requirements**
    * Low end-to-end latency.
    * High throughput.
    * Fault tolerance.
    * GPU efficiency.
    * Privacy and access control.

  * **High level design and diagram (at block level)**

    ```text
    Cameras / Streams
       |
       v
    Stream Ingest
       |
       v
    Decode + Frame Sampler
       |
       v
    GPU Inference Workers
       |
       +--> Object Tracker
       +--> Event Stream
       +--> Clip/Metadata Store
       |
       v
    Alert/API Consumers
    ```

    * **Explain the blocks**
      * Stream Ingest terminates protocols and buffers input.
      * Decode/Sampler converts streams into frames or clips.
      * GPU Workers run inference in batches.
      * Object Tracker correlates detections over time.
      * Event Stream serves alerts and analytics.

    * **Explain the control flow**
      * Operators configure streams, models, sampling rates, retention, and alert rules.
      * Scheduler places decode and inference workloads.
      * Model registry controls model versions per stream or tenant.

    * **Explain the data flow**
      * Video arrives from cameras.
      * Frames are decoded and sampled.
      * Inference produces detections.
      * Events and metadata flow to consumers and storage.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: video workloads require careful latency, bandwidth, and GPU utilization tradeoffs.

    * **Explain each of the option/topic with pros and cons**

    * **Option: process every frame**
      * Pros: highest accuracy.
      * Cons: expensive and often unnecessary.

    * **Option: frame sampling**
      * Pros: much cheaper.
      * Cons: may miss short events.

    * **Option: edge inference**
      * Pros: lower latency and bandwidth.
      * Cons: harder fleet management and model rollout.

    * **Option: cloud inference**
      * Pros: centralized management and stronger GPUs.
      * Cons: higher bandwidth and latency.

    * **Recommended L7 stance**
      * Use adaptive sampling, batching, and tiered processing: cheap models first, expensive models on interesting segments.

---

## 16. Design an Autonomous-Driving Data Ingestion and Simulation Platform

* Question
  * Design a platform for ingesting vehicle sensor data and running simulation/training workflows.

* Answer
  * **Scope**
    * Handle camera, LiDAR, radar, vehicle logs, maps, labels, scenarios, simulation outputs, and training datasets.

  * **Functional Requirements**
    * Ingest high-volume sensor logs.
    * Index metadata and scenarios.
    * Store raw and processed data.
    * Build curated datasets.
    * Run simulation/replay jobs.
    * Track lineage into training and evaluation.

  * **Non Functional Requirements**
    * Massive storage scale.
    * High-throughput reads for simulation.
    * Data integrity.
    * Privacy and access control.
    * Reproducibility.

  * **High level design and diagram (at block level)**

    ```text
    Vehicle Fleet / Upload
       |
       v
    Ingestion Gateway
       |
       +--> Metadata Extractor
       +--> Privacy/Validation
       |
       v
    Raw Sensor Store
       |
       +--> Scenario Index
       +--> Label Store
       +--> Dataset Builder
       |
       v
    Simulation/Training Clusters
    ```

    * **Explain the blocks**
      * Ingestion Gateway handles uploads and transfer reliability.
      * Metadata Extractor finds time, location, weather, scenario, and sensor attributes.
      * Raw Sensor Store keeps immutable original data.
      * Scenario Index supports search and dataset curation.
      * Dataset Builder creates versioned train/eval sets.
      * Simulation Clusters replay scenarios at scale.

    * **Explain the control flow**
      * Data policy defines retention, access, and privacy transforms.
      * Dataset owners define selection criteria.
      * Simulation jobs request scenario sets and resources.

    * **Explain the data flow**
      * Vehicles upload logs.
      * Data is validated, indexed, and stored.
      * Curated datasets are built from raw logs and labels.
      * Simulation/training jobs consume versioned datasets.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: retaining all raw sensor data enables future use but creates huge cost and governance burdens.

    * **Explain each of the option/topic with pros and cons**

    * **Option: keep all raw data forever**
      * Pros: maximum future flexibility.
      * Cons: very expensive and risky for privacy.

    * **Option: aggressive filtering**
      * Pros: cheaper and simpler.
      * Cons: may discard rare scenarios needed later.

    * **Option: tiered retention**
      * Pros: preserve high-value data, age out low-value data.
      * Cons: requires quality scoring and governance.

    * **Recommended L7 stance**
      * Use immutable raw storage with tiered retention, rich scenario indexing, and explicit data value scoring.

---

## 17. Design a Distributed Rate Limiter for GPU Inference APIs

* Question
  * Design a distributed rate limiter for model inference APIs where request cost varies by prompt and output length.

* Answer
  * **Scope**
    * Limit usage by tenant, model, region, tokens, requests, GPU-seconds, and priority.

  * **Functional Requirements**
    * Authenticate tenant.
    * Estimate request cost.
    * Enforce per-tenant and global limits.
    * Support burst credits and reservations.
    * Degrade or reject overload traffic.
    * Emit usage events for billing.

  * **Non Functional Requirements**
    * Low latency.
    * High availability.
    * Fairness.
    * Abuse resistance.
    * Bounded overshoot.

  * **High level design and diagram (at block level)**

    ```text
    API Gateway
       |
       v
    Local Rate Limiter
       |
       +--> Tenant Policy Cache
       +--> Cost Estimator
       |
       v
    Global Quota Service
       |
       +--> Usage Stream
       +--> Reconciliation Store
       +--> Billing
    ```

    * **Explain the blocks**
      * Local Rate Limiter makes fast decisions near request entry.
      * Tenant Policy Cache avoids synchronous dependency on policy storage.
      * Cost Estimator predicts tokens/GPU cost.
      * Global Quota Service reconciles distributed usage.
      * Billing consumes finalized usage events.

    * **Explain the control flow**
      * Admins configure limits, reservations, burst credits, and priority.
      * Policy propagates to gateways.
      * Reconciliation adjusts available credits and detects overshoot.

    * **Explain the data flow**
      * Request hits gateway.
      * Local limiter estimates cost and admits/rejects.
      * Usage events are emitted after actual token count is known.
      * Global service reconciles estimates with actuals.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: centralized global rate limiting is accurate but can hurt availability and latency.

    * **Explain each of the option/topic with pros and cons**

    * **Option: centralized limiter**
      * Pros: strong consistency and precise quota.
      * Cons: latency bottleneck and failure dependency.

    * **Option: local limiters**
      * Pros: fast and highly available.
      * Cons: temporary global overshoot.

    * **Option: leased tokens**
      * Pros: bounded overshoot with local decisions.
      * Cons: lease management complexity.

    * **Recommended L7 stance**
      * Use local limiters with leased quota buckets and async reconciliation. Use stricter synchronous checks for abuse, compliance, or unpaid tenants.

---

## 18. Design an AI Workflow Orchestration Platform

* Question
  * Design a platform that orchestrates AI workflows for data prep, training, evaluation, deployment, and monitoring.

* Answer
  * **Scope**
    * Support DAG-based workflows with GPU jobs, artifact tracking, retries, schedules, lineage, and approvals.

  * **Functional Requirements**
    * Define workflows as DAGs.
    * Run steps on CPU/GPU resources.
    * Retry failed steps safely.
    * Track datasets, models, metrics, and artifacts.
    * Support manual approval gates.
    * Expose workflow status and logs.

  * **Non Functional Requirements**
    * Reliable orchestration.
    * Idempotent execution.
    * Reproducibility.
    * Access control.
    * Scalability across many teams.

  * **High level design and diagram (at block level)**

    ```text
    Workflow API/UI
       |
       v
    Workflow Definition Store
       |
       v
    Orchestrator
       |
       +--> Scheduler
       +--> State Store
       +--> Artifact/Lineage Store
       +--> Approval/Policy Engine
       |
       v
    CPU/GPU Execution Backends
    ```

    * **Explain the blocks**
      * Workflow API accepts DAG definitions and run requests.
      * Orchestrator evaluates dependencies and state transitions.
      * Scheduler places tasks on execution backends.
      * State Store records step status.
      * Artifact/Lineage Store connects inputs, outputs, and versions.

    * **Explain the control flow**
      * Users define workflow templates.
      * Orchestrator creates runs and advances steps when dependencies complete.
      * Policy Engine enforces approvals before risky deployment steps.

    * **Explain the data flow**
      * Each step reads artifacts and writes new artifacts.
      * Logs and metrics stream to observability.
      * Lineage store records the graph of produced outputs.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: retries and partial failures can corrupt outputs if steps are not idempotent.

    * **Explain each of the option/topic with pros and cons**

    * **Option: retry blindly**
      * Pros: simple.
      * Cons: duplicate outputs, inconsistent state, and hidden corruption.

    * **Option: exactly-once orchestration**
      * Pros: clean semantics in theory.
      * Cons: hard across external systems and expensive.

    * **Option: at-least-once with idempotent steps**
      * Pros: practical and resilient.
      * Cons: each task must use stable run IDs and atomic output commits.

    * **Recommended L7 stance**
      * Use durable workflow state, idempotent task contracts, content-addressed artifacts, and atomic metadata commits.

---

## 19. Design a Secure Model-Serving Platform for Enterprise Customers

* Question
  * Design a secure enterprise platform for serving models to customers with strict privacy and compliance needs.

* Answer
  * **Scope**
    * Secure APIs, tenant isolation, private networking, audit, data retention, artifact security, and policy enforcement.

  * **Functional Requirements**
    * Authenticate and authorize all requests.
    * Isolate tenants at network, compute, and data layers.
    * Encrypt data in transit and at rest.
    * Support private endpoints.
    * Audit admin and inference actions.
    * Enforce retention and redaction policies.

  * **Non Functional Requirements**
    * Strong security posture.
    * Compliance readiness.
    * Low operational risk.
    * High availability.
    * Minimal performance overhead.

  * **High level design and diagram (at block level)**

    ```text
    Enterprise Client
       |
       v
    Private Endpoint / API Gateway
       |
       +--> IAM/AuthZ
       +--> Policy Engine
       +--> Audit Logger
       |
       v
    Tenant-Isolated Model Router
       |
       v
    Isolated Serving Pool
       |
       +--> Encrypted Storage
       +--> Secrets/KMS
       +--> Security Monitoring
    ```

    * **Explain the blocks**
      * Private Endpoint limits public exposure.
      * IAM/AuthZ validates identity and allowed actions.
      * Policy Engine enforces retention, redaction, and model access.
      * Audit Logger records control and data-plane actions.
      * Isolated Serving Pool runs tenant-specific or strongly isolated workloads.

    * **Explain the control flow**
      * Tenant admins configure policies, endpoints, keys, and allowed models.
      * Deployment control plane validates signed artifacts and policy.
      * Audit system records changes and access.

    * **Explain the data flow**
      * Request enters private endpoint.
      * Auth and policy checks run.
      * Router sends request to isolated serving pool.
      * Response streams back; sensitive logs are redacted or suppressed.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: enterprise security often conflicts with shared GPU efficiency.

    * **Explain each of the option/topic with pros and cons**

    * **Option: shared serving pools**
      * Pros: high utilization and lower cost.
      * Cons: more isolation risk and compliance friction.

    * **Option: dedicated serving pools**
      * Pros: strong isolation and easier compliance story.
      * Cons: expensive and lower utilization.

    * **Option: policy-based mixed isolation**
      * Pros: balances cost and security.
      * Cons: needs strong controls and clear customer contracts.

    * **Recommended L7 stance**
      * Use dedicated pools for regulated tenants and shared pools only where policy allows. Make audit, encryption, signed artifacts, and private networking baseline features.

---

## 20. Design a GPU Utilization Optimization System

* Question
  * Design a system that improves GPU utilization and cost efficiency across training and inference workloads.

* Answer
  * **Scope**
    * Collect fleet-wide utilization data, identify waste, recommend or automate tuning, and close the loop with scheduling/autoscaling.

  * **Functional Requirements**
    * Collect GPU utilization, memory, power, queue time, model metrics, and job metadata.
    * Detect underutilized workloads.
    * Recommend batching, placement, GPU SKU, quantization, or parallelism changes.
    * Simulate cost/performance impact.
    * Feed safe recommendations to schedulers/autoscalers.
    * Track improvement over time.

  * **Non Functional Requirements**
    * Low overhead.
    * Explainable recommendations.
    * Safe rollout.
    * High-quality baselines.
    * Cost-aware decisioning.

  * **High level design and diagram (at block level)**

    ```text
    Fleet Telemetry
       |
       v
    Utilization Analytics Store
       |
       v
    Optimization Engine
       |
       +--> Workload Classifier
       +--> What-If Simulator
       +--> Recommendation Service
       |
       v
    Scheduler / Autoscaler / Human Review
    ```

    * **Explain the blocks**
      * Analytics Store joins metrics with workload metadata.
      * Workload Classifier groups similar workloads.
      * What-If Simulator estimates impact of changes.
      * Recommendation Service proposes actions.
      * Scheduler/Autoscaler applies safe automated changes.

    * **Explain the control flow**
      * Operators define optimization policies and safety limits.
      * Engine periodically analyzes workloads.
      * Recommendations are reviewed or automatically applied based on risk.

    * **Explain the data flow**
      * Telemetry flows from jobs and nodes.
      * Analytics system computes utilization and bottlenecks.
      * Recommendations flow to humans or control planes.
      * Results feed back into the baseline.

  * **Deep dive topics and questions -> Explain the problem and suggest solutions**
    * Problem: automated tuning can save millions, but unsafe changes can break latency, quality, or training convergence.

    * **Explain each of the option/topic with pros and cons**

    * **Option: human-only recommendations**
      * Pros: safer and easier to trust.
      * Cons: slower and may not scale.

    * **Option: fully automated optimization**
      * Pros: fast and high leverage.
      * Cons: risky if recommendations are wrong.

    * **Option: staged automation**
      * Pros: safe path from observe to recommend to automate.
      * Cons: slower initial impact.

    * **Recommended L7 stance**
      * Start with explainable recommendations. Automate low-risk actions like idle cleanup and warm-pool sizing; require approval for model quality, quantization, or training changes.

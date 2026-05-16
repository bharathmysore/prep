# CoreWeave-Style L7 System Design Prep

This is a public-prep artifact, not CoreWeave's private question bank. The questions are inferred from CoreWeave's public product surface: GPU cloud, Kubernetes-native infrastructure, AI storage, observability, networking, inference, training, and fleet reliability.

Maintenance note:
- This is a company-specific living guide, not a fixed-count list.
- Keep the filename generic and stable so future agents can update the question catalog without renaming the file.
- When adding or changing questions, verify current public CoreWeave product/interview context and keep the scope cloud, infrastructure, GPU, Kubernetes, and distributed-systems focused.

Useful public anchors:
- CoreWeave platform docs: https://docs.coreweave.com/get-started
- CoreWeave Kubernetes Service: https://www.coreweave.com/products/coreweave-kubernetes-service
- CoreWeave storage: https://www.coreweave.com/products/storage
- CoreWeave observability: https://coreweave.com/observability
- CKS cluster components: https://docs.coreweave.com/products/cks/reference/cluster-components
- CoreWeave SUNK capabilities for AI research clusters: https://coreweave.com/blog/new-coreweave-sunk-capabilities-help-teams-build-modern-ai-research-clusters
- CoreWeave cross-cloud AI with SUNK Anywhere and LOTA Cross-Cloud: https://www.coreweave.com/blog/coreweave-announces-new-capabilities-to-simplify-cross-cloud-ai
- CoreWeave Inference release notes: https://docs.coreweave.com/changelog/release-notes/inference
- CoreWeave rack-scale Vera Rubin infrastructure deep dive: https://coreweave.com/blog/a-deep-dive-on-coreweave-innovations-for-nvidia-vera-rubin-nvl72
- Kubernetes scheduling framework: https://kubernetes.io/docs/concepts/scheduling-eviction/scheduling-framework/
- Kubernetes gang scheduling: https://kubernetes.io/docs/concepts/scheduling-eviction/gang-scheduling/
- Kueue all-or-nothing with ready Pods: https://kueue.sigs.k8s.io/docs/tasks/manage/setup_wait_for_pods_ready/

How to use this:
- In a 45 minute interview, do not recite every detail.
- Spend 5 minutes clarifying, 10 minutes on high-level design, 15 minutes on one or two deep dives, 5 minutes on tradeoffs, and 5 minutes on rollout and operations.
- For L7, always separate control plane from data plane where it makes sense.

---

## 1. Question: Design a GPU Cluster Scheduler

## Answer

### Scope

Design a scheduler for a GPU cloud that places training, inference, batch, and interactive jobs onto a large heterogeneous GPU fleet. The system should support scarce resource allocation, tenant fairness, topology-aware placement, and failure recovery.

### Functional Requirements

- Submit jobs with GPU type, GPU count, CPU, memory, storage, image, region, priority, deadline, and topology constraints.
- Support gang scheduling for distributed training jobs.
- Support bin packing for small jobs and inference workloads.
- Enforce tenant quotas, reservations, and priority classes.
- Track GPU health, node health, network topology, and capacity availability.
- Support preemption, backfill, retries, checkpoint-aware restarts, and job status APIs.

### Non Functional Requirements

- High scheduler availability.
- Low scheduling latency for small jobs.
- Predictable queueing for large distributed jobs.
- High fleet utilization.
- Isolation between tenants.
- Auditable placement decisions.
- Graceful behavior during partial fleet failures.

### High Level Design And Diagram

```text
Client/API/CLI
    |
    v
Job Submission API ---> Auth/Quota/Reservation Service
    |
    v
Job Queue + Priority Queues
    |
    v
Scheduler Core <---- Fleet State Store <---- Node/GPU Agents
    |                    ^
    |                    |
    v                    |
Placement Planner ---> Topology/Health/Inventory Service
    |
    v
Kubernetes/Slurm Adapter
    |
    v
GPU Nodes
```

### Explain The Blocks

- Job Submission API: accepts job specs and validates them.
- Auth/Quota/Reservation Service: enforces tenant rights, committed capacity, and on-demand limits.
- Job Queue: stores pending jobs by tenant, priority, deadline, and resource class.
- Scheduler Core: makes placement decisions.
- Fleet State Store: source of current allocatable capacity and assigned workloads.
- Node/GPU Agents: report GPU health, utilization, ECC errors, driver status, network state, and local capacity.
- Placement Planner: chooses candidate nodes using topology and policy constraints.
- Kubernetes/Slurm Adapter: turns scheduler decisions into runtime actions.

### Scheduler Core Deep Dive

The Scheduler Core owns the scheduling loop, queue policy, atomic reservations, and job state transitions. It should be deterministic enough to debug from logs: given a job, a fleet snapshot version, and policy version, it can explain why a placement was accepted or rejected.

Important internal objects:

- `JobSpec`: requested GPU type/count, workers, GPUs per worker, CPU, memory, image, storage, network, priority, tenant, deadline, checkpoint support, and whether the job is gang-scheduled.
- `JobAttempt`: monotonically increasing attempt id used for idempotent placement, reservation, binding, rollback, and audit.
- `PlacementSet`: the atomic result of scheduling one job attempt. It maps each worker/rank to a node shape and resource reservation. For Kubernetes, the actual GPU device ids may still be assigned by the device plugin, but the scheduler reserves GPU counts and topology; for a tighter custom stack, it can reserve explicit GPU ids.
- `Reservation`: temporary hold on GPUs, CPU, memory, local storage, quota, and network topology slots. It has a TTL and is released if binding or readiness fails.
- `JobState`: `Queued -> Candidate -> Reserved -> Binding -> Starting -> Running`, with failure exits to `Requeued`, `Failed`, `Preempted`, or `Cancelled`.

Queue pickup should balance fairness, priority, and utilization:

- Maintain per-tenant queues, usually sorted by priority, deadline, submit time, and aging.
- Use fair-share or dominant-resource-fairness style accounting so one tenant with large GPU gangs cannot permanently starve smaller tenants.
- Move jobs that recently failed placement into an unschedulable/backoff queue with a reason, such as `insufficient-h100`, `network-topology-fragmented`, or `quota-exceeded`.
- Wake the scheduling loop on job submission, job completion, reservation expiry, node health change, quota change, and preemption completion.
- Avoid strict head-of-line blocking. The scheduler can inspect a bounded window of jobs and backfill smaller jobs that do not consume resources needed by a near-term reservation or higher-priority gang.

Concrete scheduling loop:

```text
while leader:
  snapshot = fleet_state.read_consistent_snapshot()
  candidates = queue.pick_window(policy, fair_share, quotas)
  for job in candidates:
    if !quota.can_admit(job): mark_waiting(job, quota_reason); continue
    plan = placement_planner.plan(job, snapshot)
    if !plan.fits:
      queue.defer(job, plan.failure_reason, backoff)
      continue
    reservation = reserve_all(plan, attempt_id, snapshot.version)
    if !reservation.ok:
      queue.defer(job, reservation.reason, short_backoff)
      continue
    bind_async(job, reservation)
```

The important L7 point is that scheduling and starting are not the same. Scheduling means a placement has been selected and reserved. Starting means the runtime has created the workers and the required workers are ready as a group.

### Placement Planner Deep Dive

The Placement Planner is a constraint solver with a fast common path. It should not just ask "which nodes have free GPUs?" because GPU jobs care about GPU model, interconnect, locality, host resources, storage, and fragmentation.

Filter phase:

- Remove unhealthy, draining, tainted, or tenant-incompatible nodes.
- Match GPU generation, GPU memory, MIG profile if applicable, CPU, host memory, local NVMe, driver/runtime version, image cache, network fabric, storage mount, and region/zone.
- For multi-node jobs, require topology constraints such as same rack, same leaf-spine domain, minimum bandwidth, or homogeneous GPU generation.
- Exclude resources already held by unexpired reservations, not only resources used by running jobs.

Scoring phase:

- For single-node jobs, prefer best-fit packing: use the node that leaves the least unusable GPU/CPU/memory slack.
- For small inference or batch jobs, fill fragmented nodes first so empty 8-GPU or 16-GPU nodes remain available for future gang jobs.
- For full-node training jobs, prefer whole-node allocation to avoid noisy neighbors and simplify GPU/NVLink assumptions.
- For multi-node training, score candidate node sets by topology cost first, then fragmentation. A 64-GPU job usually wants eight 8-GPU nodes in the same network island more than it wants a globally optimal bin-pack.
- Penalize placements that mix GPU SKUs, cross failure domains unnecessarily, exceed thermal or power policy, or put too many replicas of the same tenant in one blast radius.

Bin packing is multi-dimensional:

```text
score(node, job) =
  gpu_fit_weight      * leftover_gpu_penalty
+ cpu_mem_weight      * leftover_host_resource_penalty
+ fragmentation_weight* future_large_job_penalty
+ topology_weight     * network_distance_penalty
+ reliability_weight  * unhealthy_or_hotspot_penalty
+ locality_weight     * data_or_image_cache_bonus
```

For example, a 1-GPU inference pod should usually land on a node with one or two idle GPUs, not an empty 8-GPU node. An 8-GPU training worker should usually get an entire 8-GPU node. A 128-GPU training job should be planned as a set of worker groups, such as 16 workers with 8 GPUs each, or 128 ranks if the framework uses one rank per GPU; either way, the planner should map them to nodes that minimize cross-rack traffic.

### Explain The Control Flow

Admins define GPU pools, node labels, quotas, reservation policies, preemption rules, tenant priorities, and scheduling plugins. Node agents register capacity and health with the control plane. The scheduler continuously reconciles desired job state against real fleet state. Operators can drain nodes, mark GPU pools unhealthy, or change policy through versioned configuration.

### Explain The Data Flow

A tenant submits a job. The API validates it and writes it to the durable queue. The scheduler reads queued jobs, checks quota and reservation state, examines fleet inventory, computes a placement, and asks Kubernetes or Slurm to start the job. Runtime status and metrics flow back from node agents to the scheduler and customer-facing job status APIs.

### Multi-GPU Job Start Semantics

For a multi-GPU or multi-node job, do not report `Started` when the first worker is bound. That creates misleading status and can hide partial-start deadlocks.

Recommended state semantics:

- `Queued`: accepted by the API, waiting for quota, priority, or capacity.
- `Admitted`: queue policy chose the job and quota allows an attempt.
- `Reserved`: all required resources for the attempt are held under one reservation token.
- `Binding`: the scheduler adapter is creating Pods, Slurm allocations, or runtime tasks.
- `Starting`: workers are bound and containers are starting, images are pulling, storage is mounting, and rendezvous configuration is being distributed.
- `Running`: all required workers, or at least `minAvailable` for elastic jobs, have passed readiness and joined the distributed rendezvous.
- `Started`: emit this user-facing event only when the job reaches `Running` for the current attempt generation.

For strict gang jobs, `minAvailable == totalWorkers`, so every rank must be ready before the job is considered started. For elastic training, `minAvailable` can be lower, but the API must say that explicitly. If only 60 of 64 ranks start for a strict gang, the job is still `Starting` until timeout; then the scheduler kills or evicts partial workers, releases the reservation, and requeues or fails the attempt.

### Deep Dive Topics And Questions

#### Topic: Gang Scheduling

Problem: Distributed training jobs often need all workers at once. Starting only half the workers wastes GPUs and can deadlock the job.

Options:

- All-or-nothing gang scheduling
  - Pros: simple job semantics, avoids partial starts, good for training correctness.
  - Cons: can leave capacity idle while waiting for all GPUs.
- Incremental allocation with timeout
  - Pros: can make progress when the fleet is fragmented.
  - Cons: harder job startup logic; can still waste capacity if the gang never completes.
- Reservation-based gang scheduling
  - Pros: predictable start times for important customers.
  - Cons: lower utilization if reservations are not consumed.

Implementation details:

- Model the job as a gang with `totalWorkers`, `gpusPerWorker`, `minAvailable`, topology constraints, and a startup timeout.
- Hold the job out of the active scheduling path until enough worker objects exist and the queue admits the gang.
- Plan the whole gang against one fleet snapshot. The planner returns a complete `PlacementSet`, not one independent placement per worker.
- Reserve all resources atomically. If any node reservation fails because the snapshot is stale, release all reservations and retry later.
- Bind all workers for the attempt. Workers carry the same `gangId`, `attemptId`, and `reservationToken`.
- Wait at a permit/readiness barrier. If all required workers become ready before timeout, release the barrier and mark the job `Running`.
- If readiness times out, kill partial workers, unreserve the whole gang, increment the attempt id, and requeue with backoff.

This avoids the classic deadlock where job A owns 32 GPUs and waits for 32 more while job B owns the other 32 and waits for 32 more. The scheduler either gives a gang enough resources to start or gives it none. To preserve utilization, it can backfill smaller jobs only when they do not violate a reservation window or make an admitted higher-priority gang impossible.

Recommended answer: use gang scheduling for tightly coupled training, plus reservation TTLs, readiness timeout, idempotent rollback, and conservative backfill into gaps while preserving reservation windows.

#### Topic: Utilization Versus Performance

Problem: The scheduler can pack jobs tightly or place them for network locality. These goals conflict.

Options:

- Bin packing
  - Pros: high utilization, fewer fragmented nodes.
  - Cons: may degrade distributed training due to poor topology.
- Spread placement
  - Pros: better fault isolation and thermal/network distribution.
  - Cons: more fragmentation.
- Topology-aware packing
  - Pros: balances locality and utilization.
  - Cons: complex scheduler logic and harder debugging.

Recommended answer: default to topology-aware placement for multi-node GPU jobs, bin packing for single-node inference or batch jobs.

---

## 2. Question: Design a Managed Kubernetes Service For GPU Workloads

## Answer

### Scope

Design a managed Kubernetes service optimized for AI training, inference, and HPC workloads on bare-metal GPU clusters.

### Functional Requirements

- Create, update, scale, and delete customer clusters.
- Manage Kubernetes control planes, GPU node pools, CNI, CSI, ingress, certs, registries, and GPU drivers.
- Support GPU-aware scheduling, node labels, taints, quotas, and RBAC.
- Integrate object storage, file storage, observability, and identity.
- Provide upgrades, maintenance windows, and rollback.
- Expose cluster lifecycle APIs and customer dashboards.

### Non Functional Requirements

- Strong tenant isolation.
- Highly available control plane.
- Minimal workload disruption during upgrades.
- Secure-by-default networking and identity.
- Clear audit trail.
- SLOs for API availability and cluster operations.

### High Level Design And Diagram

```text
Customer Portal/API/CLI
    |
    v
Cluster Management API
    |
    +--> IAM/RBAC
    +--> Quota/Reservation Service
    +--> Billing/Metering
    |
    v
Cluster Reconciler
    |
    +--> Control Plane Provisioner
    +--> Node Pool Manager
    +--> Add-on Manager
    +--> Upgrade Orchestrator
    |
    v
Customer Kubernetes Cluster
    |
    +--> GPU Operator / Device Plugin
    +--> CNI / Network Policies
    +--> CSI / Storage Drivers
    +--> Observability Agents
```

### Explain The Blocks

- Cluster Management API: front door for cluster operations.
- Cluster Reconciler: declarative controller that converges actual state to desired state.
- Control Plane Provisioner: manages API server, etcd, controller manager, and scheduler.
- Node Pool Manager: manages GPU/CPU node lifecycle.
- Add-on Manager: installs and upgrades managed add-ons.
- Upgrade Orchestrator: handles Kubernetes, driver, CNI, and CSI upgrades.

### Explain The Control Flow

The customer requests a cluster. The API validates quota, writes desired state, and the reconciler provisions control plane, node pools, network, storage, and managed add-ons. Changes such as scaling or upgrade requests update desired state. Controllers reconcile actual resources until the cluster is healthy.

### Explain The Data Flow

Customer workloads run directly in the data plane on GPU nodes. Application traffic enters through ingress or private networking. Workloads read training data from object/file storage and emit metrics/logs to observability pipelines. The management plane observes health but should not sit in the hot path of customer inference or training traffic.

### Deep Dive Topics And Questions

#### Topic: Managed Add-ons Versus Customer Control

Problem: GPU workloads depend on drivers, device plugins, CNI, CSI, ingress, cert management, and telemetry. Customers also want flexibility.

Options:

- Fully managed add-ons
  - Pros: consistent support, fewer broken clusters, easier upgrades.
  - Cons: less customer customization.
- Bring-your-own add-ons
  - Pros: maximum flexibility.
  - Cons: support burden and version conflicts.
- Managed defaults with escape hatches
  - Pros: good balance for enterprise customers.
  - Cons: more compatibility matrix work.

Recommended answer: manage critical platform add-ons like GPU operator, CNI, CSI, and observability; allow controlled customization at documented extension points.

#### Topic: Control Plane Isolation

Problem: A noisy or compromised tenant should not affect other tenants.

Options:

- Shared control plane
  - Pros: efficient, easy fleet management.
  - Cons: larger blast radius.
- Per-customer control plane
  - Pros: strong isolation and clearer SLOs.
  - Cons: higher cost and operational overhead.
- Tiered isolation
  - Pros: map isolation to customer tier and workload sensitivity.
  - Cons: product complexity.

Recommended answer: use strong logical isolation by default, dedicated control planes for large or regulated customers.

---

## 3. Question: Design An LLM Inference Serving Platform

## Answer

### Scope

Design a platform for serving LLMs on GPU clusters with streaming responses, dynamic batching, autoscaling, model rollout, multi-tenancy, and latency SLOs.

### Functional Requirements

- Serve chat/completion/embedding requests.
- Route by model, tenant, region, capacity, and SLO.
- Support streaming token responses.
- Support dynamic batching and admission control.
- Support model loading, versioning, canaries, rollback, and A/B tests.
- Track usage by tokens, model, tenant, and GPU cost.

### Non Functional Requirements

- Low tail latency.
- High GPU utilization.
- High availability under bursty demand.
- Isolation between tenants and models.
- Safe overload behavior.
- Accurate metering and audit logs.

### High Level Design And Diagram

```text
Client
  |
  v
API Gateway ---> Auth/Rate Limit/Metering
  |
  v
Model Router <---- Model Registry / Policy Config
  |
  v
Request Queues By Model/Region/SLO
  |
  v
Batcher + Admission Controller
  |
  v
Inference Workers on GPU Nodes
  |
  +--> KV Cache / Model Cache
  +--> Token Streamer
  |
  v
Client Stream + Usage Events
```

### Explain The Blocks

- API Gateway: handles auth, request validation, request size limits, and streaming connection setup.
- Model Router: chooses model version, region, and serving pool.
- Request Queues: isolate traffic classes and models.
- Batcher: combines compatible requests for GPU efficiency.
- Admission Controller: rejects, queues, or sheds load when SLOs cannot be met.
- Inference Workers: run model servers on GPU nodes.
- Model Registry: tracks model artifacts, versions, rollout state, and compatibility.

### Explain The Control Flow

Model owners register model versions and rollout policies. Platform operators define GPU pools, tenant quotas, safety limits, and SLO classes. The autoscaler monitors queue depth, tokens per second, GPU utilization, and latency, then adjusts serving replicas or warm pools. Canary and rollback decisions are controlled by versioned deployment policy.

### Explain The Data Flow

A request enters the gateway, is authenticated, rate limited, and routed to a model queue. The batcher groups it with compatible requests. The inference worker executes prefill and decode on GPU, streams tokens back, and emits usage, latency, and error events for billing and observability.

### Deep Dive Topics And Questions

#### Topic: Dynamic Batching

Problem: GPUs are expensive and benefit from batching, but users care about latency.

Options:

- Fixed batch size
  - Pros: simple and predictable.
  - Cons: poor under variable traffic.
- Dynamic batching with max wait time
  - Pros: improves utilization while bounding latency.
  - Cons: tuning is workload-specific.
- Continuous batching
  - Pros: high throughput for token generation.
  - Cons: complex scheduler and memory management.

Recommended answer: use continuous batching for high-volume LLM serving, with per-SLO max queue wait and admission control.

#### Topic: Overload Handling

Problem: During spikes, uncontrolled queues increase tail latency and burn GPU cycles on doomed requests.

Options:

- Queue everything
  - Pros: simple, fewer immediate errors.
  - Cons: destroys tail latency.
- Fail fast
  - Pros: protects system health.
  - Cons: poor customer experience if too aggressive.
- Tiered admission control
  - Pros: protects premium/latency-sensitive traffic.
  - Cons: requires product and quota policy clarity.

Recommended answer: reject early when estimated queue time exceeds SLO, with per-tenant fairness and clear retry-after signals.

---

## 4. Question: Design A Distributed Training Job Orchestrator

## Answer

### Scope

Design an orchestrator for large training jobs running across many GPU nodes, supporting distributed frameworks, checkpointing, retries, logs, metrics, and topology-aware placement.

### Functional Requirements

- Submit training jobs with framework, image, command, resource shape, dataset, checkpoint path, and priority.
- Start all workers together.
- Provide rendezvous, environment injection, secrets, and service discovery.
- Collect logs, metrics, and artifacts.
- Detect worker, node, GPU, network, and storage failures.
- Restart from checkpoint.

### Non Functional Requirements

- High job success rate.
- Low wasted GPU time.
- Scalable to thousands of GPUs per job.
- Minimal coordinator bottlenecks.
- Clear failure diagnostics.
- Reproducible job configuration.

### High Level Design And Diagram

```text
Training CLI/API
    |
    v
Training Job Controller
    |
    +--> Scheduler/Gang Allocator
    +--> Rendezvous Service
    +--> Checkpoint Manager
    +--> Logs/Metrics Collector
    |
    v
Kubernetes Jobs / Custom Resources
    |
    v
Worker Pods on GPU Nodes
    |
    +--> Dataset Storage
    +--> Checkpoint Storage
    +--> Observability Pipeline
```

### Explain The Blocks

- Training Job Controller: owns lifecycle of a training job.
- Scheduler/Gang Allocator: obtains all required resources.
- Rendezvous Service: helps workers discover rank/world size/master address.
- Checkpoint Manager: tracks valid checkpoint versions.
- Logs/Metrics Collector: aggregates stdout, framework metrics, GPU metrics, and failure events.
- Worker Pods: run actual training code.

### Explain The Control Flow

The user submits desired job state. The controller validates config, asks the scheduler for a gang allocation, creates worker pods, and registers rendezvous metadata. During execution, the controller watches health and either restarts, resumes from checkpoint, or marks the job failed according to policy.

### Explain The Data Flow

Workers read training data from storage, exchange gradients over high-performance networking, periodically write checkpoints, and emit logs/metrics. On failure, the orchestrator reads checkpoint metadata and restarts the job from the latest complete checkpoint.

### Deep Dive Topics And Questions

#### Topic: Failure Recovery

Problem: At large scale, some node or GPU will fail during long training runs.

Options:

- Restart whole job
  - Pros: simple and reliable.
  - Cons: expensive for large jobs.
- Elastic worker replacement
  - Pros: less wasted work.
  - Cons: framework-dependent and complex.
- Checkpoint and restart from last valid state
  - Pros: practical and common.
  - Cons: checkpoint I/O overhead.

Recommended answer: checkpoint-based restart as the baseline; support elastic training only for frameworks and jobs designed for it.

#### Topic: Kubernetes Versus Slurm Semantics

Problem: AI researchers may expect Slurm-like queues, while platform teams want Kubernetes-native control.

Options:

- Pure Kubernetes jobs
  - Pros: cloud-native ecosystem.
  - Cons: weaker HPC queue semantics.
- Slurm on Kubernetes
  - Pros: familiar HPC interface.
  - Cons: additional control layer.
- Unified abstraction
  - Pros: best UX for multiple user types.
  - Cons: harder product surface.

Recommended answer: expose familiar job semantics while implementing on Kubernetes-native controllers where possible.

---

## 5. Question: Design GPU Capacity Reservations And Quotas

## Answer

### Scope

Design a system that manages committed GPU capacity, on-demand capacity, quotas, priorities, and admission control for multiple tenants.

### Functional Requirements

- Create capacity reservations by tenant, GPU type, region, time window, and quantity.
- Enforce hard and soft quotas.
- Support on-demand, committed, and preemptible capacity classes.
- Provide capacity availability APIs.
- Support priority and preemption policies.
- Emit usage and reservation utilization events.

### Non Functional Requirements

- Strong consistency for scarce capacity allocation.
- High availability for read paths.
- Auditable allocation history.
- Low latency admission checks.
- Clear customer-visible errors.

### High Level Design And Diagram

```text
Admin/Customer API
    |
    v
Reservation API ---> Policy Engine
    |
    v
Capacity Ledger <---- Inventory Service
    |
    v
Admission Control
    |
    v
Scheduler
    |
    v
GPU Fleet
```

### Explain The Blocks

- Reservation API: creates and updates reservation objects.
- Policy Engine: validates contracts, quota, and priority rules.
- Capacity Ledger: records allocated, reserved, consumed, and available capacity.
- Inventory Service: provides physical capacity and health.
- Admission Control: decides whether a job can enter scheduling.
- Scheduler: places admitted jobs.

### Explain The Control Flow

Operators or customers create reservations. The policy engine checks capacity and writes a durable reservation entry. Admission control consults the ledger before accepting jobs. Scheduler consumption updates the ledger, and unused reservations can expire, roll over, or become reclaimable based on policy.

### Explain The Data Flow

Job submissions carry tenant and capacity class. Admission control checks reservation/quota state. If accepted, the scheduler places the job and emits usage events. Billing and dashboards consume reservation and usage streams.

### Deep Dive Topics And Questions

#### Topic: Strong Versus Eventual Capacity Accounting

Problem: GPUs are scarce. Double-booking is expensive, but global strong consistency may hurt availability.

Options:

- Strong central ledger
  - Pros: prevents over-allocation.
  - Cons: bottleneck and regional dependency.
- Regional ledgers
  - Pros: low latency and resilient regional operation.
  - Cons: harder global quota enforcement.
- Hybrid
  - Pros: strong per-region allocation, async global reconciliation.
  - Cons: temporary global drift.

Recommended answer: use strong consistency within a region/resource pool and async global aggregation for reporting.

#### Topic: Overbooking

Problem: Not all reservations are fully used, but overbooking can break trust.

Options:

- No overbooking
  - Pros: predictable customer guarantees.
  - Cons: lower utilization.
- Controlled overbooking
  - Pros: higher utilization.
  - Cons: risk of capacity shortfall.
- Overbook only reclaimable/preemptible pools
  - Pros: protects committed customers.
  - Cons: needs clear product semantics.

Recommended answer: never overbook hard committed reservations unless contractually allowed; use preemptible capacity for utilization recovery.

---

## 6. Question: Design AI Dataset Storage For Training

## Answer

### Scope

Design a storage architecture for large training datasets that supports object storage durability, high-throughput reads, file-system semantics where needed, and GPU-local caching.

### Functional Requirements

- Store petabyte-scale datasets and model artifacts.
- Support high-throughput sequential and random reads.
- Support object and file interfaces.
- Support dataset versioning, checksums, and metadata.
- Support regional replication and lifecycle policies.
- Support node-local or GPU-local caching.

### Non Functional Requirements

- High read throughput.
- Low tail latency for hot data.
- Durability.
- Cost efficiency.
- Tenant isolation.
- Observability into storage bottlenecks.

### High Level Design And Diagram

```text
Dataset Upload/Ingestion
    |
    v
Object Storage / Durable Store
    |
    +--> Metadata Catalog
    +--> Version/Checksum Index
    |
    v
High Performance File Layer
    |
    v
Regional / Rack / Node Cache
    |
    v
Training Workers on GPU Nodes
```

### Explain The Blocks

- Object Storage: durable source of truth for datasets.
- Metadata Catalog: tracks dataset versions, shard layout, ownership, and checksums.
- High Performance File Layer: provides POSIX-like or parallel read semantics.
- Cache Layers: keep hot shards close to GPUs.
- Training Workers: consume data during training.

### Explain The Control Flow

Users register datasets, permissions, lifecycle policies, and cache hints. The storage control plane validates metadata, computes shard maps, triggers replication or pre-warming, and manages retention. Operators configure cache policies and storage tiers.

### Explain The Data Flow

Data is ingested into durable storage, validated, and indexed. Training jobs request shards. Reads hit node cache, regional cache, file layer, or object store in that order. Metrics report cache hit rate, throughput, latency, and errors.

### Deep Dive Topics And Questions

#### Topic: Object Storage Versus Parallel File Storage

Problem: Object storage is durable and cheap, but training often needs very high throughput and file-like access.

Options:

- Object storage only
  - Pros: durable, scalable, cost-effective.
  - Cons: higher latency, weaker file semantics.
- Parallel file system
  - Pros: very high throughput and familiar APIs.
  - Cons: expensive and operationally complex.
- Hybrid object plus cache/file acceleration
  - Pros: balances cost, durability, and performance.
  - Cons: cache invalidation and consistency complexity.

Recommended answer: use object storage as source of truth, with file/cache acceleration for hot training paths.

#### Topic: Cache Consistency

Problem: Cached dataset shards can become stale if datasets are mutable.

Options:

- Mutable datasets with invalidation
  - Pros: flexible.
  - Cons: hard to reason about correctness.
- Immutable versioned datasets
  - Pros: reproducible and cache-friendly.
  - Cons: more storage consumption.
- Mutable aliases pointing to immutable versions
  - Pros: usability plus correctness.
  - Cons: requires metadata discipline.

Recommended answer: make dataset versions immutable; allow aliases like latest to move through explicit control-plane updates.

---

## 7. Question: Design A Model Artifact Distribution System

## Answer

### Scope

Design a system that distributes large model weights, containers, tokenizers, and runtime artifacts to many GPU nodes quickly and safely.

### Functional Requirements

- Register model artifacts with version, hash, size, metadata, and access policy.
- Replicate artifacts across regions.
- Download artifacts to serving or training nodes.
- Support canary, rollback, and garbage collection.
- Verify integrity before use.
- Support resumable downloads and throttling.

### Non Functional Requirements

- Fast model startup.
- High availability.
- No corrupted artifact execution.
- Efficient bandwidth use.
- Tenant isolation and access control.
- Clear rollout observability.

### High Level Design And Diagram

```text
Model Build/Registry
    |
    v
Artifact Store + Metadata DB
    |
    v
Regional Replicators
    |
    v
Cache Hierarchy / P2P Distribution
    |
    v
GPU Node Artifact Agent
    |
    v
Model Server
```

### Explain The Blocks

- Model Registry: tracks versions and rollout state.
- Artifact Store: durable storage for weights and related files.
- Regional Replicators: move artifacts close to compute.
- Cache Hierarchy: avoids every node pulling from origin.
- Node Artifact Agent: downloads, verifies, unpacks, and exposes artifacts.
- Model Server: loads artifacts into CPU/GPU memory.

### Explain The Control Flow

A model owner publishes a version. The registry validates metadata and triggers regional replication. Rollout policies tell serving pools which model version to load. Node agents reconcile local artifact state with desired versions and report readiness.

### Explain The Data Flow

Artifact bytes flow from build output into the store, then to regional caches, then to node caches. Model servers load from local disk or memory cache. Hash and signature validation occur before activation.

### Deep Dive Topics And Questions

#### Topic: Centralized Versus P2P Distribution

Problem: Loading a new large model on thousands of nodes can overload origin storage and networks.

Options:

- Centralized download
  - Pros: simple and secure.
  - Cons: origin bottleneck and slow rollouts.
- Hierarchical caches
  - Pros: scalable and easier to control than P2P.
  - Cons: cache capacity and invalidation complexity.
- Peer-to-peer distribution
  - Pros: very efficient for mass rollout.
  - Cons: complex security, integrity, and network control.

Recommended answer: start with hierarchical regional/rack caches; add P2P for very large, frequent rollouts with strict hash validation.

#### Topic: Rollback

Problem: Bad model artifacts can break serving pools.

Options:

- In-place overwrite
  - Pros: simple.
  - Cons: unsafe rollback.
- Immutable versions
  - Pros: instant rollback and reproducibility.
  - Cons: storage cost.
- Immutable plus lifecycle GC
  - Pros: safe and cost-aware.
  - Cons: requires retention policy.

Recommended answer: use immutable artifacts and pointer-based activation.

---

## 8. Question: Design Bare-Metal GPU Node Health Monitoring

## Answer

### Scope

Design health monitoring and remediation for GPU servers running customer workloads on bare metal.

### Functional Requirements

- Monitor GPU temperature, ECC errors, XID errors, NVLink, PCIe, driver state, power, fans, CPU, memory, disk, and network.
- Detect degraded nodes and bad GPUs.
- Cordoning, draining, rebooting, driver reset, and ticket creation.
- Expose health state to scheduler.
- Correlate node health with customer job failures.

### Non Functional Requirements

- Low false positives.
- Fast detection of severe failures.
- Minimal customer disruption.
- Auditable remediation.
- Scales to large fleet.
- Works during partial control-plane outages.

### High Level Design And Diagram

```text
Node Health Agent
    |
    v
Telemetry Pipeline ---> Metrics/Logs/Events Store
    |
    v
Health Classifier
    |
    +--> Scheduler Health API
    +--> Remediation Controller
    +--> Incident System
    |
    v
Node Drain/Repair/Return
```

### Explain The Blocks

- Node Health Agent: collects hardware and OS signals.
- Telemetry Pipeline: transports metrics, logs, and events.
- Health Classifier: turns raw signals into health states.
- Scheduler Health API: prevents placement on bad nodes.
- Remediation Controller: safely drains, resets, reboots, or repairs nodes.
- Incident System: alerts humans for high-risk or repeated issues.

### Explain The Control Flow

Operators define health rules and remediation policies. Agents report raw state. The classifier updates node health. Severe states trigger scheduler cordon and possible automated remediation. Risky actions require human approval or staged automation.

### Explain The Data Flow

Node metrics flow from agents to telemetry. Health decisions flow to scheduler and remediation systems. Customer-facing job events include correlated node and GPU failure signals.

### Deep Dive Topics And Questions

#### Topic: False Positives Versus Fast Remediation

Problem: Draining a node protects jobs from bad hardware but can also kill healthy workloads.

Options:

- Aggressive remediation
  - Pros: faster MTTR and fewer repeated failures.
  - Cons: interrupts workloads unnecessarily if classifier is wrong.
- Conservative remediation
  - Pros: fewer unnecessary disruptions.
  - Cons: degraded nodes keep hurting jobs.
- Severity-tiered remediation
  - Pros: fast for clear failures, cautious for weak signals.
  - Cons: requires careful signal taxonomy.

Recommended answer: auto-cordon on severe hardware faults, require repeated evidence or canary diagnostics for ambiguous degradation.

#### Topic: Local Versus Central Health Decisions

Problem: Central control may be unavailable during outages.

Options:

- Central-only classifier
  - Pros: global context and easier policy updates.
  - Cons: dependency on telemetry/control plane.
- Local node decisions
  - Pros: continues working during control-plane issues.
  - Cons: less global context.
- Hybrid
  - Pros: local emergency actions plus central policy.
  - Cons: more implementation complexity.

Recommended answer: local agents can mark severe self-health failures; central classifier handles correlation and fleet-wide actions.

---

## 9. Question: Design Observability For GPU Clusters

## Answer

### Scope

Design an observability platform that helps customers and operators debug AI workload performance and reliability from application layer down to bare metal.

### Functional Requirements

- Collect metrics, logs, traces, events, and profiles.
- Include Kubernetes, GPU, node, network, storage, and application signals.
- Correlate job failures with infra events.
- Support tenant dashboards and operator dashboards.
- Support alerting, anomaly detection, and retention policies.
- Support high-cardinality labels carefully.

### Non Functional Requirements

- High ingestion throughput.
- Queryable during incidents.
- Tenant isolation.
- Cost control.
- Low overhead on workloads.
- Reliable enough to debug platform outages.

### High Level Design And Diagram

```text
App/Node/GPU/Network/Storage Agents
    |
    v
Local Buffer
    |
    v
Telemetry Ingestion Gateway
    |
    +--> Metrics Store
    +--> Logs Store
    +--> Traces Store
    +--> Events Store
    |
    v
Correlation/Alerting Layer
    |
    v
Dashboards / APIs / Incident Tools
```

### Explain The Blocks

- Agents: collect signals close to workloads.
- Local Buffer: absorbs short network/control-plane failures.
- Ingestion Gateway: validates, samples, authenticates, and routes telemetry.
- Stores: optimized backends for metrics, logs, traces, and events.
- Correlation Layer: joins workload events with infrastructure events.
- Dashboards/APIs: expose insights to users and operators.

### Explain The Control Flow

Admins define scrape configs, sampling rules, retention policies, alert rules, and tenant access. Agents fetch config, collect telemetry, and report health. Alerting rules create incidents or customer-visible advisories.

### Explain The Data Flow

Telemetry flows from jobs and infrastructure through local buffers to ingestion gateways. It is partitioned by tenant, cluster, node, job, and signal type. Queries retrieve correlated views across metrics, logs, traces, and events.

### Deep Dive Topics And Questions

#### Topic: Cardinality Explosion

Problem: AI workloads produce high-dimensional labels like job ID, model ID, GPU ID, tenant ID, and pod ID.

Options:

- Allow all labels
  - Pros: flexible debugging.
  - Cons: cost explosion and degraded queries.
- Strict label allowlist
  - Pros: predictable cost.
  - Cons: less flexible debugging.
- Tiered cardinality
  - Pros: keep important dimensions, sample or downsample others.
  - Cons: more policy complexity.

Recommended answer: enforce label budgets, downsample long-retention data, and preserve raw high-cardinality data for short windows.

#### Topic: Customer Versus Operator Visibility

Problem: Customers need useful visibility, but platform internals may expose sensitive multi-tenant details.

Options:

- Same dashboards for everyone
  - Pros: simple.
  - Cons: security and noise risks.
- Strictly separate views
  - Pros: safer.
  - Cons: may hide useful root cause context.
- Redacted shared model
  - Pros: customer sees relevant infra impact without cross-tenant leaks.
  - Cons: requires careful data shaping.

Recommended answer: expose tenant-scoped workload and infrastructure health, with redacted platform-level causal hints.

---

## 10. Question: Design Topology-Aware Placement For Distributed Training

## Answer

### Scope

Design placement logic that understands GPUs, NVLink, hosts, racks, network switches, regions, and storage locality to optimize distributed training performance.

### Functional Requirements

- Represent physical topology as a graph.
- Accept placement constraints such as same node, same rack, same fabric, or storage-local.
- Score candidate placements.
- Support gang allocation.
- React to topology changes and failures.
- Explain placement decisions.

### Non Functional Requirements

- Better training throughput.
- Low scheduling overhead.
- Avoid pathological fragmentation.
- Resilient to stale topology data.
- Auditable and debuggable.

### High Level Design And Diagram

```text
Topology Discovery
    |
    v
Topology Graph Store <---- Health/Inventory Service
    |
    v
Placement Scorer
    |
    v
Scheduler
    |
    v
GPU Nodes / Racks / Fabrics / Storage
```

### Explain The Blocks

- Topology Discovery: imports hardware, rack, switch, GPU, and storage relationships.
- Topology Graph Store: stores weighted edges and failure domains.
- Health/Inventory Service: marks unavailable resources.
- Placement Scorer: scores candidate sets based on locality, bandwidth, failure domain, and fragmentation.
- Scheduler: reserves and binds resources.

### Explain The Control Flow

Operators load topology metadata. Discovery updates it as fleet changes. Scheduler plugins query the graph while placing jobs. Policy controls whether to optimize for locality, fairness, or utilization.

### Explain The Data Flow

Training job specs flow into the scheduler. The scheduler reads topology and health data, scores candidate GPU sets, and binds jobs. Training traffic then flows across the selected network topology.

### Deep Dive Topics And Questions

#### Topic: Optimal Placement Versus Scheduling Speed

Problem: Finding the perfect placement can be computationally expensive.

Options:

- Exact optimization
  - Pros: best theoretical placement.
  - Cons: slow and hard at fleet scale.
- Greedy heuristics
  - Pros: fast and practical.
  - Cons: can miss better placements.
- Multi-stage scoring
  - Pros: filter quickly, then deeply score few candidates.
  - Cons: more tuning complexity.

Recommended answer: use fast filtering plus heuristic scoring; reserve exact optimization for special large jobs or offline planning.

#### Topic: Fragmentation

Problem: Optimizing each job locally can fragment scarce GPU islands.

Options:

- First-fit placement
  - Pros: simple.
  - Cons: creates fragmentation.
- Best-fit placement
  - Pros: better utilization.
  - Cons: may hurt locality.
- Reservation-aware placement
  - Pros: preserves large islands for future big jobs.
  - Cons: requires prediction and policy.

Recommended answer: account for future large-job demand and preserve contiguous topology islands when queue signals justify it.

---

## 11. Question: Design Autoscaling For GPU Inference

## Answer

### Scope

Design autoscaling for model serving fleets where GPUs are expensive, model load times are long, and traffic is bursty.

### Functional Requirements

- Scale serving replicas by model, region, tenant class, and SLO.
- Use signals such as queue depth, tokens/sec, GPU utilization, latency, and memory.
- Support warm pools and preloaded model caches.
- Support scale-to-zero for low-traffic models where acceptable.
- Avoid thrashing.
- Integrate with reservation and quota systems.

### Non Functional Requirements

- Low tail latency.
- High GPU utilization.
- Cost efficiency.
- Predictable customer SLOs.
- Safe overload behavior.
- Stable scaling decisions.

### High Level Design And Diagram

```text
Metrics/Queue Signals
    |
    v
Autoscaling Controller <---- Policy/Quota/Reservation Config
    |
    v
Capacity Planner
    |
    +--> Warm Pool Manager
    +--> Kubernetes HPA/Custom Scaler
    +--> Model Preloader
    |
    v
Inference Serving Pools
```

### Explain The Blocks

- Autoscaling Controller: computes desired serving capacity.
- Capacity Planner: checks available GPUs and reservations.
- Warm Pool Manager: maintains pre-initialized capacity.
- Custom Scaler: creates or removes replicas.
- Model Preloader: loads artifacts before routing traffic.
- Serving Pools: handle inference traffic.

### Explain The Control Flow

Operators define scaling policies and SLO classes. The controller watches traffic and capacity signals. It asks the capacity planner for possible scale actions, creates warm or active replicas, waits for readiness, then updates routing weights.

### Explain The Data Flow

User requests enter serving queues. Metrics from queues and model workers feed autoscaling. New replicas load model artifacts and join routing. Usage and latency events flow to metering and observability.

### Deep Dive Topics And Questions

#### Topic: Warm Pool Versus Cold Start

Problem: Large models can take minutes to load, but idle GPUs are expensive.

Options:

- Large warm pool
  - Pros: excellent latency during bursts.
  - Cons: high cost.
- Cold start only
  - Pros: cost efficient.
  - Cons: poor burst handling.
- Predictive warm pool
  - Pros: balances cost and latency.
  - Cons: forecast errors.

Recommended answer: maintain small baseline warm pools for critical models and predictive warming for known traffic patterns.

#### Topic: Scaling Signal Choice

Problem: GPU utilization alone can be misleading for LLM serving.

Options:

- GPU utilization
  - Pros: easy and intuitive.
  - Cons: misses queueing and token-level bottlenecks.
- Queue depth and wait time
  - Pros: closer to user experience.
  - Cons: can overreact to short bursts.
- Tokens/sec plus latency
  - Pros: model-aware and better for LLMs.
  - Cons: requires app-level metrics.

Recommended answer: combine queue wait, p95/p99 latency, tokens/sec, batch saturation, and GPU memory pressure.

---

## 12. Question: Design A Multi-Tenant GPU Cloud Control Plane

## Answer

### Scope

Design the control plane for a GPU cloud that manages clusters, storage, networking, reservations, IAM, billing, and observability across many tenants.

### Functional Requirements

- Tenant/project/account hierarchy.
- APIs for clusters, node pools, networks, storage, reservations, and usage.
- IAM, RBAC, audit logs, and policy enforcement.
- Tenant-scoped dashboards and APIs.
- Internal operator tooling.
- Control-plane eventing and reconciliation.

### Non Functional Requirements

- Strong isolation and security.
- Highly available APIs.
- Idempotent operations.
- Disaster recovery.
- Auditable mutations.
- Backward-compatible API evolution.

### High Level Design And Diagram

```text
Portal/API/CLI
    |
    v
API Gateway ---> IAM/AuthZ ---> Audit Log
    |
    v
Resource APIs
    |
    v
Desired State Store
    |
    v
Reconcilers
    |
    +--> Compute
    +--> Network
    +--> Storage
    +--> Observability
    +--> Billing/Metering
```

### Explain The Blocks

- API Gateway: common auth, validation, rate limiting, and versioning.
- IAM/AuthZ: validates tenant and project permissions.
- Audit Log: immutable record of mutations.
- Resource APIs: typed APIs for cloud resources.
- Desired State Store: durable source of truth.
- Reconcilers: converge actual infrastructure to desired state.

### Explain The Control Flow

Customers and operators issue API requests. AuthZ and policy checks run first. Valid mutations write desired state and audit records. Reconcilers asynchronously provision or update infrastructure. Status is written back to resource objects.

### Explain The Data Flow

Customer workload data does not flow through the control plane. The control plane handles metadata, configuration, and lifecycle events. Runtime traffic flows through customer networks, storage paths, and compute nodes.

### Deep Dive Topics And Questions

#### Topic: Synchronous Versus Asynchronous Resource Creation

Problem: Cluster and network provisioning can take minutes and partially fail.

Options:

- Synchronous API
  - Pros: simple client experience for fast operations.
  - Cons: timeouts and poor partial-failure handling.
- Async operation resource
  - Pros: robust for long-running workflows.
  - Cons: clients must poll or subscribe.
- Declarative resource status
  - Pros: Kubernetes-like, resilient, auditable.
  - Cons: more complex API model.

Recommended answer: use declarative desired state plus operation/status resources for long-running provisioning.

#### Topic: Auditability

Problem: Cloud mutations must be explainable during incidents or customer disputes.

Options:

- App logs only
  - Pros: simple.
  - Cons: weak audit integrity.
- Immutable audit log
  - Pros: strong compliance and debugging.
  - Cons: extra storage and privacy handling.
- Event-sourced control plane
  - Pros: full reconstruction.
  - Cons: high implementation complexity.

Recommended answer: use immutable audit logs for all mutations; event sourcing only if the team already has the maturity.

---

## 13. Question: Design GPU Usage Metering And Billing

## Answer

### Scope

Design a metering system that records GPU, CPU, storage, network, reservation, and inference-token usage for billing and customer dashboards.

### Functional Requirements

- Collect usage from scheduler, Kubernetes, node agents, storage systems, and inference gateways.
- Attribute usage to tenant, project, cluster, job, model, region, SKU, and reservation.
- Support near-real-time dashboards.
- Support monthly billing reconciliation.
- Handle missing, duplicate, and late events.
- Support discounts, credits, and committed-use contracts.

### Non Functional Requirements

- Billing accuracy.
- Idempotent ingestion.
- Tamper-resistant records.
- Scalable event processing.
- Reconciliation and backfill.
- Clear lineage from raw usage to invoice.

### High Level Design And Diagram

```text
Usage Producers
    |
    v
Usage Event Gateway
    |
    v
Durable Event Stream
    |
    +--> Real-time Aggregator --> Customer Dashboard
    +--> Batch Reconciler -----> Billing Ledger
    |
    v
Invoice/Reporting System
```

### Explain The Blocks

- Usage Producers: emit raw measurements.
- Event Gateway: validates schema and idempotency keys.
- Durable Event Stream: preserves ordered-ish event history.
- Real-time Aggregator: powers dashboards and alerts.
- Batch Reconciler: creates authoritative billing totals.
- Billing Ledger: stores finalized charges and adjustments.

### Explain The Control Flow

Operators define SKUs, pricing rules, contracts, discounts, and billing periods. Producers register schemas. Reconciliation jobs close billing windows, detect anomalies, and produce invoices after validation.

### Explain The Data Flow

Raw usage events flow into the stream. Real-time consumers aggregate approximate current spend. Batch jobs deduplicate, correct late events, apply pricing, and write final ledger entries.

### Deep Dive Topics And Questions

#### Topic: Real-Time Versus Authoritative Billing

Problem: Customers want live spend visibility, but invoices must be exact.

Options:

- Real-time only
  - Pros: simple and fresh.
  - Cons: prone to late/duplicate event errors.
- Batch only
  - Pros: accurate and reconcilable.
  - Cons: poor customer visibility.
- Dual path
  - Pros: live estimates plus accurate bills.
  - Cons: two systems to reconcile.

Recommended answer: use real-time estimates for dashboards and batch reconciliation for invoices.

#### Topic: Idempotency

Problem: Usage producers can retry and emit duplicates.

Options:

- Trust producers
  - Pros: simple.
  - Cons: overbilling risk.
- Idempotency keys
  - Pros: reliable duplicate suppression.
  - Cons: producers must generate stable keys.
- Windowed dedupe
  - Pros: handles imperfect producers.
  - Cons: stateful and may miss old duplicates.

Recommended answer: require stable usage event IDs and also perform windowed dedupe in aggregation.

---

## 14. Question: Design A Distributed Job Queue For Scarce GPU Resources

## Answer

### Scope

Design a job queue for GPU workloads with fairness, priority, preemption, backfill, and starvation prevention.

### Functional Requirements

- Accept jobs with tenant, priority, resource shape, deadline, and preemption policy.
- Support multiple queues by GPU type, region, tenant, and workload class.
- Support fairness and quota enforcement.
- Support backfill for small jobs.
- Support job cancellation, reprioritization, and status.
- Support delayed scheduling and reservations.

### Non Functional Requirements

- Durable queue state.
- High availability.
- Predictable scheduling.
- Explainable queue position.
- Low scheduling latency for interactive jobs.
- Scales to high submission volume.

### High Level Design And Diagram

```text
Job API
    |
    v
Durable Job Store
    |
    v
Queue Manager <---- Quota/Priority/Reservation Policy
    |
    v
Scheduling Candidates
    |
    v
Scheduler / Placement Engine
```

### Explain The Blocks

- Job API: creates, updates, cancels, and queries jobs.
- Durable Job Store: authoritative job state.
- Queue Manager: orders jobs and applies fairness rules.
- Policy Services: define tenant priority and allowed capacity.
- Scheduler: turns candidates into placements.

### Explain The Control Flow

Policy owners configure queues, priority classes, and fairness rules. The queue manager continuously computes eligible candidates. The scheduler consumes candidates and returns placement results. Failed placement may requeue or reserve future capacity.

### Explain The Data Flow

Jobs flow from submission to durable state to queue ordering to scheduler candidates. Runtime state flows back from the scheduler and nodes, updating job status and queue metrics.

### Deep Dive Topics And Questions

#### Topic: Fairness Versus Priority

Problem: Strategic or urgent jobs may need priority, but lower-priority tenants should not starve.

Options:

- Strict priority
  - Pros: simple and business-aligned for top customers.
  - Cons: starvation risk.
- Fair sharing
  - Pros: predictable across tenants.
  - Cons: urgent jobs may wait.
- Weighted fair queueing with priority boosts
  - Pros: balances business priority and fairness.
  - Cons: policy complexity.

Recommended answer: use weighted fair sharing with caps, reservations, and bounded priority boosts.

#### Topic: Backfill

Problem: Large jobs can block the queue while waiting for a full gang allocation.

Options:

- No backfill
  - Pros: simple and predictable.
  - Cons: idle capacity.
- Conservative backfill
  - Pros: improves utilization without delaying reserved jobs.
  - Cons: requires runtime estimates.
- Aggressive backfill
  - Pros: highest utilization.
  - Cons: can delay important jobs.

Recommended answer: use conservative backfill with runtime limits and preemptible classes.

---

## 15. Question: Design Driver And Kubernetes Upgrade Orchestration

## Answer

### Scope

Design a safe upgrade system for Kubernetes versions, GPU drivers, CNI, CSI, firmware, and platform add-ons across customer clusters.

### Functional Requirements

- Track current and target versions.
- Support maintenance windows, canaries, phased rollout, and rollback.
- Drain nodes safely.
- Respect disruption budgets and job checkpointing.
- Validate post-upgrade health.
- Block incompatible version combinations.

### Non Functional Requirements

- Minimize customer disruption.
- Avoid fleet-wide regressions.
- Fast security patch rollout when needed.
- Strong auditability.
- Clear customer communication.
- Automated rollback where safe.

### High Level Design And Diagram

```text
Version Policy Service
    |
    v
Upgrade Planner
    |
    +--> Compatibility Matrix
    +--> Maintenance Window Service
    +--> Canary Controller
    |
    v
Drain/Upgrade/Validate Controller
    |
    v
Clusters / Node Pools / Add-ons
```

### Explain The Blocks

- Version Policy Service: defines supported versions and deadlines.
- Upgrade Planner: decides upgrade order and rollout batches.
- Compatibility Matrix: prevents invalid combinations.
- Maintenance Window Service: honors customer timing.
- Canary Controller: tests changes on limited scope.
- Drain/Upgrade/Validate Controller: executes and checks upgrades.

### Explain The Control Flow

Operators publish a target version. The planner selects eligible clusters, schedules canaries, checks compatibility, and creates upgrade operations. Controllers drain nodes, apply upgrades, validate health, and proceed or pause.

### Explain The Data Flow

Upgrade artifacts flow from artifact stores to nodes. Health and validation metrics flow back into the upgrade controller. Customer workloads should continue flowing through remaining healthy capacity during rolling upgrades.

### Deep Dive Topics And Questions

#### Topic: Upgrade Speed Versus Safety

Problem: Security fixes may need speed; platform upgrades can break workloads.

Options:

- Fast global rollout
  - Pros: closes vulnerabilities quickly.
  - Cons: high blast radius.
- Slow phased rollout
  - Pros: catches regressions.
  - Cons: prolonged version drift.
- Risk-tiered rollout
  - Pros: match speed to urgency and risk.
  - Cons: more operational complexity.

Recommended answer: use canary plus phased rollout; emergency patches can accelerate with tighter rollback and monitoring.

#### Topic: Draining GPU Jobs

Problem: Draining a node may kill expensive long-running training jobs.

Options:

- Immediate drain
  - Pros: simple and fast.
  - Cons: high customer disruption.
- Maintenance windows only
  - Pros: customer-friendly.
  - Cons: slow vulnerability remediation.
- Checkpoint-aware drain
  - Pros: reduces lost work.
  - Cons: requires workload integration.

Recommended answer: combine maintenance windows, disruption budgets, and checkpoint-aware termination where supported.

---

## 16. Question: Design Cross-Cloud Dataset Ingestion

## Answer

### Scope

Design a system to move large datasets from other clouds or customer environments into AI storage efficiently, securely, and reliably.

### Functional Requirements

- Connect to source object stores and file systems.
- Support resumable, parallel transfer.
- Validate checksums and object counts.
- Support encryption, IAM, and temporary credentials.
- Provide progress, errors, and retry status.
- Support scheduled or continuous sync.

### Non Functional Requirements

- High throughput.
- Reliable recovery after failures.
- Minimal customer setup.
- Secure credential handling.
- Cost visibility.
- Avoid overwhelming source or destination.

### High Level Design And Diagram

```text
Customer Source Cloud
    |
    v
Transfer Planner
    |
    v
Transfer Workers / Agents
    |
    v
Destination Object Storage
    |
    v
Validation + Catalog Update
```

### Explain The Blocks

- Transfer Planner: lists source data, partitions work, and creates transfer manifests.
- Workers/Agents: perform parallel copy with retries.
- Destination Storage: durable target.
- Validation: checks hashes, sizes, counts, and manifests.
- Catalog Update: registers dataset versions after successful transfer.

### Explain The Control Flow

The customer creates an ingestion job with source credentials, destination, and sync policy. The planner creates a manifest and assigns chunks to workers. The controller tracks progress, retries failures, throttles transfer, and marks the dataset ready only after validation.

### Explain The Data Flow

Data flows directly from source storage to transfer workers and then to destination storage. Metadata and progress events flow to the ingestion controller and customer dashboard.

### Deep Dive Topics And Questions

#### Topic: Agentless Versus Customer-Hosted Agents

Problem: Some customers prefer managed transfers; others need private network access.

Options:

- Fully managed pull
  - Pros: easy customer experience.
  - Cons: needs source access from provider.
- Customer-hosted agent
  - Pros: works in private environments.
  - Cons: customer installation burden.
- Hybrid
  - Pros: covers both cases.
  - Cons: more support surface.

Recommended answer: offer managed pull for cloud sources and customer-hosted agents for private or restricted networks.

#### Topic: Consistency During Transfer

Problem: Source data may change while transfer is running.

Options:

- Best-effort copy
  - Pros: simple.
  - Cons: inconsistent snapshots.
- Snapshot or versioned source
  - Pros: consistent result.
  - Cons: source-specific complexity.
- Manifest-based finalization
  - Pros: detects drift.
  - Cons: may require retries or freeze windows.

Recommended answer: prefer versioned/snapshot sources; otherwise use manifests and post-copy validation.

---

## 17. Question: Design Network Isolation For Customer GPU Clusters

## Answer

### Scope

Design secure network isolation for customer GPU clusters while preserving low-latency, high-bandwidth paths needed by AI workloads.

### Functional Requirements

- Tenant VPC or private network abstraction.
- Private subnets, routing, security groups or network policies.
- Controlled ingress and egress.
- Private access to storage and control APIs.
- Support peering or private connectivity.
- Support audit logs and flow logs.

### Non Functional Requirements

- Strong tenant isolation.
- Low network overhead.
- High throughput for training traffic.
- Clear operational visibility.
- Secure defaults.
- Minimal blast radius.

### High Level Design And Diagram

```text
Tenant Network API
    |
    v
Network Control Plane
    |
    +--> IPAM
    +--> Policy Engine
    +--> Route Controller
    +--> Flow Log Pipeline
    |
    v
Cluster Network Data Plane
    |
    +--> CNI / Overlay or Underlay
    +--> Security Policies
    +--> Private Endpoints
```

### Explain The Blocks

- Network API: lets customers define networks and policies.
- IPAM: allocates address space.
- Policy Engine: validates and compiles security rules.
- Route Controller: programs routing.
- Flow Logs: provide observability and audit.
- CNI/Data Plane: enforces connectivity on nodes.

### Explain The Control Flow

Customers create networks, subnets, routes, and policies. The control plane validates conflicts, allocates IPs, and programs data-plane components. Operators define default-deny or managed platform access policies.

### Explain The Data Flow

Customer traffic flows between pods, nodes, storage endpoints, and external destinations according to route and security policy. Flow metadata is exported to logs without sending customer payloads through the control plane.

### Deep Dive Topics And Questions

#### Topic: Overlay Versus Underlay

Problem: AI workloads need performance, but cloud networks need isolation.

Options:

- Overlay network
  - Pros: flexible isolation and easier multi-tenant abstraction.
  - Cons: overhead and possible performance limits.
- Underlay with hardware isolation
  - Pros: high performance.
  - Cons: more complex provisioning.
- Hybrid
  - Pros: use high-performance paths for training, overlay for control traffic.
  - Cons: operational complexity.

Recommended answer: use the lowest-overhead isolation that meets security goals; avoid putting high-bandwidth training traffic through unnecessary overlays.

#### Topic: Egress Control

Problem: Customers need internet or private egress, but uncontrolled egress creates security and cost risk.

Options:

- Open egress
  - Pros: easy.
  - Cons: insecure and hard to bill.
- Deny by default
  - Pros: secure.
  - Cons: more setup friction.
- Policy-based egress
  - Pros: balanced.
  - Cons: needs good UX and debugging.

Recommended answer: default to controlled egress with explicit policies, private endpoints, and flow logs.

---

## 18. Question: Design A Fault-Tolerant Checkpointing System

## Answer

### Scope

Design a checkpointing system for long-running distributed training jobs to reduce lost work after node, GPU, network, or storage failures.

### Functional Requirements

- Periodically write checkpoints.
- Support distributed checkpoint shards.
- Mark complete checkpoints atomically.
- Retain multiple versions.
- Resume jobs from latest valid checkpoint.
- Expose checkpoint health and cost metrics.

### Non Functional Requirements

- Low training overhead.
- Durable storage.
- Fast restore.
- Consistent checkpoint metadata.
- Scalable for very large models.
- Efficient cleanup.

### High Level Design And Diagram

```text
Training Workers
    |
    v
Checkpoint Library/Sidecar
    |
    v
Checkpoint Coordinator
    |
    +--> Metadata Store
    +--> Durable Storage
    +--> Retention Manager
    |
    v
Restart Controller
```

### Explain The Blocks

- Checkpoint Library/Sidecar: integrates with framework or captures files.
- Coordinator: manages checkpoint epochs and completion.
- Metadata Store: records checkpoint manifests and status.
- Durable Storage: stores checkpoint shards.
- Retention Manager: deletes old checkpoints safely.
- Restart Controller: resumes jobs from valid checkpoints.

### Explain The Control Flow

Job policy defines checkpoint interval, retention, and destination. The coordinator starts checkpoint epochs and records metadata. Only after all required shards are written and validated does it publish a complete marker.

### Explain The Data Flow

Workers write checkpoint shards to storage. Metadata flows to the coordinator. On restart, workers read the latest complete manifest and load shards from durable storage.

### Deep Dive Topics And Questions

#### Topic: Checkpoint Frequency

Problem: Frequent checkpoints reduce lost work but consume I/O and slow training.

Options:

- Frequent fixed interval
  - Pros: simple and low recovery loss.
  - Cons: high overhead.
- Infrequent fixed interval
  - Pros: low overhead.
  - Cons: more lost work.
- Adaptive interval
  - Pros: optimizes based on failure rate and job cost.
  - Cons: more complex.

Recommended answer: default interval based on expected failure rate and checkpoint cost; allow adaptive tuning for large jobs.

#### Topic: Atomic Completion

Problem: Partial checkpoints can corrupt resumes.

Options:

- Write directly to final path
  - Pros: simple.
  - Cons: readers may see partial state.
- Temporary path plus commit marker
  - Pros: robust and common.
  - Cons: cleanup required.
- Transactional metadata service
  - Pros: strong correctness.
  - Cons: metadata dependency.

Recommended answer: write shards to temporary/versioned paths and publish a manifest commit marker only after validation.

---

## 19. Question: Design A GPU Fleet Incident Detection System

## Answer

### Scope

Design a system that detects incidents across GPU fleet infrastructure, identifies blast radius, correlates symptoms, and triggers safe remediation.

### Functional Requirements

- Ingest events from jobs, nodes, GPUs, network, storage, Kubernetes, and control plane.
- Detect anomalies and correlated failures.
- Determine blast radius by tenant, region, rack, GPU type, driver version, or storage system.
- Open incidents and page owners.
- Trigger safe remediation or rollback.
- Provide incident timeline and root-cause hints.

### Non Functional Requirements

- Low detection latency.
- Low alert noise.
- High confidence for automated actions.
- Resilient during outages.
- Explainable detections.
- Strong audit trail.

### High Level Design And Diagram

```text
Telemetry/Event Sources
    |
    v
Event Stream
    |
    v
Correlation Engine
    |
    +--> Anomaly Detection
    +--> Dependency Graph
    +--> Blast Radius Analyzer
    |
    v
Incident Manager
    |
    +--> Alerting
    +--> Remediation Orchestrator
    +--> Timeline/Reports
```

### Explain The Blocks

- Event Sources: produce symptoms and health events.
- Event Stream: durable transport.
- Correlation Engine: groups related symptoms.
- Dependency Graph: maps workloads to infra dependencies.
- Blast Radius Analyzer: finds common failing dimensions.
- Incident Manager: creates incidents and coordinates response.

### Explain The Control Flow

Operators define alert rules, anomaly thresholds, ownership, and remediation guardrails. The correlation engine evaluates events against these rules. Incidents are created with severity, scope, owners, and suggested actions.

### Explain The Data Flow

Events flow from infrastructure and workloads into the stream. Correlated incident facts flow to alerting, dashboards, and remediation systems. Remediation outcomes flow back into the incident timeline.

### Deep Dive Topics And Questions

#### Topic: Rule-Based Versus ML Detection

Problem: Static rules miss novel incidents; ML can be noisy.

Options:

- Rule-based detection
  - Pros: explainable and reliable for known failures.
  - Cons: misses unknown patterns.
- ML/anomaly detection
  - Pros: catches unexpected behavior.
  - Cons: noisy and hard to trust.
- Hybrid
  - Pros: rules for paging, ML for hints and early warning.
  - Cons: more systems to operate.

Recommended answer: page on high-confidence rules and use anomaly detection to enrich diagnosis and detect emerging issues.

#### Topic: Automated Remediation

Problem: Automation can reduce MTTR but can also worsen outages.

Options:

- Human-only remediation
  - Pros: safer for complex incidents.
  - Cons: slow.
- Fully automated remediation
  - Pros: fast.
  - Cons: risky if diagnosis is wrong.
- Guarded automation
  - Pros: fast for safe actions, human approval for risky ones.
  - Cons: requires action taxonomy.

Recommended answer: automate low-risk actions like cordon or rollback canary; require approval for broad drains or fleet-wide changes.

---

## 20. Question: Design A Control Plane API For Provisioning GPU Clusters

## Answer

### Scope

Design an API and backend workflow that provisions GPU clusters with compute, storage, networking, Kubernetes version, identity, and observability add-ons.

### Functional Requirements

- Create cluster requests with region, GPU type, node count, Kubernetes version, network, storage, and add-ons.
- Validate quota and capacity.
- Provision network, control plane, node pools, storage mounts, and observability.
- Expose operation status and cluster health.
- Support idempotent retries.
- Support update and delete workflows.

### Non Functional Requirements

- Reliable partial-failure recovery.
- Auditable operations.
- Idempotent APIs.
- Backward-compatible schema evolution.
- Secure defaults.
- Clear customer-visible states.

### High Level Design And Diagram

```text
Customer API/CLI
    |
    v
Cluster Provisioning API
    |
    +--> Auth/Policy/Quota
    +--> Capacity Check
    |
    v
Desired State Store
    |
    v
Provisioning Workflow Engine
    |
    +--> Network Reconciler
    +--> Control Plane Reconciler
    +--> Node Pool Reconciler
    +--> Storage Reconciler
    +--> Add-on Reconciler
    |
    v
Cluster Status API
```

### Explain The Blocks

- Provisioning API: validates and records requested cluster state.
- Auth/Policy/Quota: ensures the customer may create the cluster.
- Capacity Check: verifies GPU availability or reservation.
- Desired State Store: durable record of cluster intent.
- Workflow Engine: coordinates multi-step provisioning.
- Reconcilers: own specific resource domains.
- Cluster Status API: exposes progress and health.

### Explain The Control Flow

The customer submits a create request with an idempotency key. The API validates and writes desired state. The workflow engine creates dependent resources in order, while reconcilers handle retries and partial failures. Status moves from pending to provisioning to ready or degraded/failed.

### Explain The Data Flow

Provisioning metadata flows through the control plane. Once ready, customer workload traffic flows directly through the cluster data plane, including pod networking, storage I/O, and ingress/egress. Telemetry flows back to status and observability.

### Deep Dive Topics And Questions

#### Topic: Workflow Engine Versus Independent Reconcilers

Problem: Cluster provisioning has ordered dependencies but also needs robust reconciliation.

Options:

- Central workflow engine
  - Pros: easy to visualize ordering and status.
  - Cons: can become a monolith.
- Independent reconcilers only
  - Pros: Kubernetes-native and resilient.
  - Cons: harder to present unified progress.
- Workflow plus reconcilers
  - Pros: clear customer operation state with robust domain controllers.
  - Cons: coordination complexity.

Recommended answer: use a workflow for customer-visible operation orchestration and reconcilers for each resource domain.

#### Topic: Idempotency And Partial Failure

Problem: A cluster create may fail after network creation but before node pool creation.

Options:

- Roll back everything on any failure
  - Pros: simple final state.
  - Cons: rollback can fail too.
- Retry until success
  - Pros: good for transient failures.
  - Cons: can get stuck on permanent errors.
- Declarative desired state with compensating cleanup
  - Pros: robust and debuggable.
  - Cons: requires careful state modeling.

Recommended answer: store desired state, make every step idempotent, retry transient failures, and use explicit cleanup workflows for terminal failures.

---

## 21. Question: Design A Cross-Cloud AI Training Control Plane

## Answer

### Scope

Design a control plane that lets AI research teams run large training, reinforcement-learning, evaluation, and agent-development jobs across CoreWeave regions, other cloud GPU pools, and customer-owned clusters while preserving familiar Slurm-like workflows, Kubernetes-native operations, shared storage access, identity, observability, and cost controls.

Public CoreWeave context: 2026 public material emphasizes SUNK as Slurm on Kubernetes for long-running AI training, SUNK Anywhere for consistent operation across clouds and on-prem environments, LOTA Cross-Cloud for high-throughput data access without full dataset replication, CoreWeave Interconnect for secure low-latency cross-cloud networking, and Mission Control observability for GPU straggler and rack-scale infrastructure visibility.

### Functional Requirements

- Register GPU environments across CoreWeave, partner clouds, and on-prem clusters with capacity, topology, scheduler, storage, network, and trust metadata.
- Submit jobs through one API or CLI with Slurm-compatible queue semantics, Kubernetes-native execution, tenant quotas, reservation hints, image/runtime requirements, data locality, and placement constraints.
- Support long-running distributed training, burst capacity, multi-region experimentation, pause/resume, checkpoint-aware retry, and strict gang start semantics.
- Provide cross-cloud dataset access through cache or remote data-plane services without forcing full data replication for every job.
- Synchronize users, groups, SSH keys, service identities, POSIX accounts, and Slurm accounts from enterprise identity providers.
- Expose unified job status, queue state, GPU health, straggler signals, network/storage bottlenecks, cost, and audit events.
- Support safe failover, draining, preemption, quota enforcement, and policy-based selection between local, remote, and dedicated capacity.

### Non Functional Requirements

- Preserve training goodput under node, GPU, rack, network, storage, and cross-cloud link failures.
- Avoid a global control-plane dependency in the hot path of running jobs.
- Keep tenant isolation, data boundary, and audit guarantees clear across independently operated environments.
- Make placement decisions explainable from job spec, policy version, inventory snapshot, and data-locality state.
- Bound control-plane convergence time after capacity or connectivity changes.
- Keep cross-cloud data movement cost and latency visible before admission.
- Support staged rollout of new clusters, scheduler adapters, identity mappings, and storage-cache policies.

### High Level Design And Diagram

```text
Researcher / CI / Agent Workflow
    |
    v
Global Training API + Slurm-Compatible CLI
    |
    +--> Identity + User Provisioning Broker
    +--> Policy / Quota / Reservation Service
    +--> Data Locality + Cost Planner
    |
    v
Global Job Queue + Placement Broker
    |
    +--> CoreWeave Region Adapter ---> Local SUNK / K8s / Slurm Runtime
    +--> Partner Cloud Adapter ------> Remote SUNK / K8s Runtime
    +--> On-Prem Adapter ------------> Site Runtime Agent
    |
    v
Per-Environment Controllers
    |
    +--> GPU Nodes / Racks / Network Fabric
    +--> Dataset Cache / Object Storage / Parallel FS
    +--> Mission Control / Telemetry / Audit
```

### Explain The Blocks

- Global Training API + Slurm-Compatible CLI: accepts job specs, exposes queue/status commands, and maps familiar research workflows onto portable execution objects.
- Identity + User Provisioning Broker: synchronizes enterprise identity into global IAM, POSIX users, SSH keys, groups, and per-cluster scheduler accounts.
- Policy / Quota / Reservation Service: enforces tenant limits, committed capacity, priority, region allowlists, data-boundary rules, and preemption policy.
- Data Locality + Cost Planner: estimates where the dataset, checkpoints, model artifacts, and cache warmth make a job cheapest or fastest to run.
- Global Job Queue + Placement Broker: chooses an eligible environment and sends an admitted job attempt to exactly one local controller.
- Environment Adapter: translates global job and policy objects into local SUNK, Kubernetes, Slurm, storage, network, and observability primitives.
- Per-Environment Controller: owns local reconciliation so already-running jobs continue if the global control plane is temporarily unreachable.
- Dataset Cache / Object Storage / Parallel FS: provides local or near-local reads for training data, checkpoint writes, and artifact distribution.
- Mission Control / Telemetry / Audit: aggregates job progress, GPU health, stragglers, fabric behavior, queue events, and operator actions.

### Core Components And Low-Level Design

The most important component is the Placement Broker. It should make an environment-level decision, not individual pod-level GPU placement. Local schedulers keep ownership of node-level binding because they understand live topology, reservations, health, and local readiness.

Key objects:

- `Environment`: region, provider, cluster id, GPU SKUs, topology domains, supported runtimes, scheduler type, storage endpoints, identity realm, trust level, health, and policy version.
- `TrainingJob`: workers, GPUs per worker, image, command, checkpoint URI, dataset URIs, queue, priority, tenant, deadline, restart policy, elastic or strict gang mode, and runtime constraints.
- `PlacementIntent`: selected environment, expected start window, capacity class, data access mode, checkpoint target, identity mapping, and local adapter contract.
- `AttemptLease`: idempotency key that prevents two environments from starting the same strict training attempt.
- `DataPlan`: cache hit estimate, remote read path, expected throughput, egress or interconnect cost, checkpoint write path, and prefetch policy.

Placement algorithm:

```text
for job in admitted_queue:
  envs = filter_by_policy_identity_runtime(job, environments)
  envs = filter_by_capacity_and_health(job, envs)
  plans = build_data_plans(job.datasets, job.checkpoints, envs)
  scored = score(envs, capacity, topology, data_plan, cost, queue_delay)
  selected = first_environment_with_valid_attempt_lease(scored)
  dispatch_to_local_adapter(job, selected, selected.data_plan)
```

Local adapters should be versioned. A `v1` adapter might support basic job submit/status/cancel. A richer adapter adds checkpoint-aware restart, elastic workers, straggler hints, rack drain, or custom Slurm account sync. The global control plane must degrade gracefully when an environment lacks a newer feature instead of assuming a uniform fleet.

### Explain The Control Flow

Platform operators register environments and define policy: which tenants can use which clouds, which datasets can cross a boundary, which capacity is committed, which clusters are burst-only, and which identity mappings are valid. The broker continuously imports environment inventory, queue delay, data-cache warmth, link health, and rack/cluster health. Researchers submit jobs through the global API or Slurm-like CLI. The broker validates policy, selects an environment, creates an attempt lease, and hands the job to the local adapter. Local controllers reconcile execution and stream status back.

Control-plane decisions are versioned and auditable: admission policy, placement score, data plan, identity mapping, and adapter version should all appear in the job's explain output. Operators can drain an environment by changing capacity health or policy, which stops new placements while existing local jobs continue or checkpoint and migrate according to job policy.

### Explain The Data Flow

Training data should not flow through the global control plane. Workers read from local storage, remote object storage through a cross-cloud cache, or a parallel file system mounted in the selected environment. The Data Locality Planner can prewarm shards, tensorized model artifacts, and container images before job start. Checkpoints write to the closest durable target, then replicate or catalog asynchronously so a retry can resume in another environment when policy allows.

Runtime telemetry flows from GPU/node/rack agents, local schedulers, storage caches, and network systems into per-environment collectors. The global observability layer joins these signals by `jobId`, `attemptId`, `tenant`, `environment`, `rank`, `gpu`, and `rack`, with cardinality limits and sampling for high-volume metrics. User-facing status should distinguish `Queued`, `Admitted`, `Dispatched`, `LocalQueued`, `Reserved`, `Starting`, `Running`, `Checkpointing`, `Migrating`, `Completed`, and `Failed`.

### Deep Dive Topics And Questions

#### Topic: Unified Control Plane Versus Local Autonomy

Problem: Cross-cloud training needs one user experience, but running jobs cannot depend on a global service for every heartbeat or worker restart.

Options:

- Central scheduler owns every placement
  - Pros: global optimization and simpler quota accounting.
  - Cons: fragile during WAN partitions; hard to model every local topology and scheduler edge case.
- Fully independent clusters with manual federation
  - Pros: strong local autonomy and easy failure isolation.
  - Cons: fragmented queues, inconsistent policy, duplicate account setup, and poor cross-cloud utilization.
- Global broker plus local reconcilers
  - Pros: unified admission and explainability with local resilience.
  - Cons: requires crisp contracts between global intent and local execution state.

Recommended answer: use a global placement broker for environment selection, quota, identity, data plan, and audit, then delegate node-level scheduling and failure recovery to local SUNK/Kubernetes/Slurm controllers. Running jobs should continue during global-control-plane outages, but new cross-environment placements can pause until policy and quota state are safe.

#### Topic: Data Locality Versus Capacity Availability

Problem: The cheapest available GPUs may be far from the dataset, and the closest data may sit near a saturated GPU pool.

Options:

- Always move compute to data
  - Pros: avoids repeated dataset transfer and maximizes storage locality.
  - Cons: can strand jobs behind a saturated local queue.
- Always burst to available GPUs
  - Pros: lower queue time and better capacity utilization.
  - Cons: remote reads, cache misses, or checkpoint writes can erase the benefit.
- Cost-aware placement with prefetch and cache contracts
  - Pros: exposes the real tradeoff between queue delay, data throughput, and cost.
  - Cons: requires accurate cache, link, and job-throughput models.

Recommended answer: score each environment with queue delay, GPU topology, dataset locality, expected cache hit rate, interconnect health, checkpoint target, and cost. Admit the job only if the selected environment can meet a minimum data-throughput contract or can prewarm enough data before gang start.

#### Topic: Identity And Researcher Workflow Portability

Problem: Researchers expect familiar Slurm accounts, POSIX users, SSH keys, shared directories, and experiment tooling, while platform teams need centralized IAM and audit.

Options:

- Per-cluster manual account setup
  - Pros: simple for one cluster.
  - Cons: unsafe and slow across many environments.
- Pure cloud IAM only
  - Pros: centralized policy and audit.
  - Cons: does not match existing HPC training workflows.
- Identity broker with generated local accounts
  - Pros: keeps central policy while preserving local Slurm/POSIX semantics.
  - Cons: account lifecycle bugs can block jobs or leave stale access.

Recommended answer: synchronize users and groups from enterprise identity into a broker, then generate local POSIX, SSH, and Slurm account state per environment with reconciliation, drift detection, expiry, and audit. Job attempts should carry a resolved identity snapshot so retries are deterministic.

#### Topic: Straggler Detection And Cross-Environment Debugging

Problem: In a thousand-GPU job, one slow GPU, rack, link, cache node, or remote data path can reduce goodput without causing a clean failure.

Options:

- Job-level metrics only
  - Pros: low cardinality and easy dashboards.
  - Cons: hides rank-level and infrastructure-level root cause.
- Full per-GPU, per-rank, per-link telemetry everywhere
  - Pros: powerful debugging.
  - Cons: expensive, high-cardinality, and hard to secure across tenants.
- Tiered telemetry with straggler-focused drilldown
  - Pros: keeps normal cost bounded while enabling deep diagnosis.
  - Cons: needs trigger logic and temporary high-resolution capture.

Recommended answer: default to bounded job and environment metrics, then trigger high-resolution capture when step time, all-reduce latency, cache wait, GPU utilization, or network retry distributions show outliers. Join telemetry by attempt, rank, GPU, node, rack, and data path, but enforce tenant isolation and retention limits.

---

# Quick Mock Interview Rotation

Use these six first if time is limited:

1. GPU cluster scheduler.
2. LLM inference serving platform.
3. Cross-cloud AI training control plane.
4. AI dataset storage.
5. Bare-metal GPU node health monitoring.
6. Topology-aware placement.

# L7 Evaluation Checklist

For every answer, make sure you cover:

- Why control plane and data plane are separated.
- What is the scarce resource and how it is protected.
- What fails first at scale.
- What signals drive automation.
- What actions are safe to automate versus require human review.
- What customer-visible SLO you would offer.
- How you would roll it out without breaking existing workloads.

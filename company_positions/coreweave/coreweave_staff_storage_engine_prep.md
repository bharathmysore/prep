# CoreWeave Staff Storage Engine: Seven-Day Intensive Tracker

This is a public-prep artifact and a personal progress ledger, not a claim about CoreWeave's private interview process. It targets the Storage Engine role—the managed-storage **data plane**—while retaining enough control-plane fluency to demonstrate staff-level end-to-end ownership.

The two linked roles point to high-performance AI storage: distributed file/object/block systems; performance technologies such as RDMA, GPU Direct Storage, and SPDK; reliability, durability, and observability; Kubernetes; and cross-functional technical leadership. See the [Storage Engine posting](https://www.efinancialcareers.it/jobs-United_States-Northfield-Staff_Engineer_Storage_Engine.id24588834) and the related [File & Block posting](https://www.efinancialcareers-norway.com/jobs-United_States-Seattle-Staff_Software_Engineer_Storage.id24642820). Third-party mirrors are used because the provided career URLs were not reliably retrievable during preparation.

## How To Use This Tracker

- This is a **seven-day, six-to-eight-hour/day** intensive. Timeboxes are focused work, excluding breaks.
- Start with every checkbox open. A box is checked only after you personally confirm that you can repeat the exercise without notes.
- For each task record: date, artifact/notes location, confidence (`0` unfamiliar to `4` can defend alternatives after delay), and the next gap. Do not infer completion from reading, generated answers, or elapsed time.
- Revisit in this order: reopen the lowest-confidence item, perform it closed-book, then update evidence and confidence. Preserve old evidence rather than rewriting history.
- Use sanitized descriptions only. Say what problem you solved, the invariants, alternatives, outcome category, and lesson. Never disclose internal topology, customer/workload identities, unreleased behavior, exact capacity, thresholds, configurations, or incident details.

## Role Thesis And Ownership Translation

Your interview thesis: **I have built and operated distributed block-storage capabilities across the hot path and lifecycle, and I make staff-level decisions by tying correctness and durability invariants to latency, cost, operability, and safe rollout. I would apply that discipline to CoreWeave's AI storage engine.**

| Owned feature | Safe evidence to extract | CoreWeave Storage Engine translation |
| --- | --- | --- |
| Replication | ordering, quorum/ack point, retry ambiguity, repair | highly available dataset/checkpoint/object data paths |
| Leader election | epochs/terms, leases, fencing, recovery ownership | failover without split-brain writes or stale repair |
| Snapshots | consistency boundary, manifest/COW strategy, restore proof | training checkpoints, clones, artifact/version recovery |
| Backup | durability tier, retention, verification, restore RTO/RPO | data protection across failures and regions |
| XRR | cross-region lag, promotion, failback, conflict policy | geo-resilient AI data and disaster recovery |
| XRRVG | multi-volume consistency and coordinated cutover | checkpoint groups and application-consistent recovery |
| Filesystem work | metadata/data path, namespace, cache/consistency semantics | shared training data and POSIX-like access patterns |
| Background compression | scheduling, write amplification, resource isolation | capacity efficiency without corrupting tail-latency SLOs |

## Completion And Scoring Rules

Each daily exercise requires a named artifact: a drawing, code/test result, failure matrix, recorded answer, or one-page decision note. Score the daily design/debugging answer on eight dimensions: requirements, normal path, invariants, failure handling, performance, observability, rollout/rollback, and staff decision. Score `0` absent/unsafe, `1` plausible, `2` explicit and defensible. A daily gate is evidence-ready at `12/16` with no zero; it is confirmed only when you say it is repeatable.

## Seven-Day Calendar

| Day | Primary theme | Required ownership threads |
| --- | --- | --- |
| 1 | Hot-path mechanics and performance | filesystem, compression, replication |
| 2 | Replication correctness and leadership | replication, leader election, snapshots |
| 3 | Checkpoints, snapshots, backup, DR | snapshots, backup, XRR, XRRVG |
| 4 | AI shared-storage semantics | filesystem, replication, compression, snapshots |
| 5 | Reliability and incident command | all eight threads |
| 6 | Staff-level design and coding simulations | all eight threads |
| 7 | Full loop and durable recall | all eight threads |

## Day 1 — AI Storage Fast Path And Performance

**Target:** Explain how a training read or checkpoint write becomes I/O, where the copies and queues are, and which layer owns correctness versus latency.

- [x] **D1-DOM (100m):** Draw an end-to-end read path for a GPU training worker through client/runtime, filesystem or object adapter, metadata lookup, cache, transport, storage service, and media. Mark queue boundaries, buffer ownership, backpressure, retry point, checksum point, and application-visible completion. Then compare buffered I/O, direct I/O, `io_uring`, SPDK, RDMA/RoCE, and GPU Direct Storage: state what each removes or moves, and what it does **not** guarantee.
- [x] **D1-CODE (120m):** Implement a bounded C++ request scheduler with cancellation, deadlines, and joined shutdown. Use [Bounded Blocking Queue](../../coding/cpp/concurrency/solutions.md#1-bounded-blocking-queue) as the warm-up and [Producer-Consumer Pipeline](../../coding/cpp/concurrency/questions.md#8-implement-a-producer-consumer-pipeline-with-cancellation-and-backpressure) as the main prompt. Test capacity zero/one, cancellation while blocked, backend error, late completion, and shutdown with queued work. State safety: no lost work is reported successful; liveness: all waiters eventually unblock after shutdown.
- [x] **D1-DESIGN (90m):** Re-answer [Design AI Dataset Storage For Training](../../system_design/coreweave_l7_system_design_prep.md#6-question-design-ai-dataset-storage-for-training). Deep dive on how filesystem metadata and a replication protocol interact on the read path; include a compression policy that protects P99 latency.
- [x] **D1-PERF (60m):** Given a 64-worker job that reads shards and checkpoints periodically, create a bottleneck tree: client CPU/copies, metadata, queue depth, network, storage CPU, media, and background compression. For each, name one discriminating metric and one safe mitigation.
- [x] **D1-STORY (30m):** Record a two-minute sanitized story connecting your filesystem/compression work to AI storage. Use: context → performance/correctness tension → invariant → decision → measurable outcome category → what you would change at CoreWeave.
- [x] **D1-GATE (20m):** Explain the complete path and answer: “Why did throughput fall but median latency remain steady?” Record confidence and one gap.

## Day 2 — Replication, Leadership, And Recovery Ownership

**Target:** Defend write correctness under lost replies, partitions, leader changes, and stale repair.

- [x] **D2-DOM (100m):** Write a state machine for `Follower → Candidate → Leader → Draining/Recovering`, with term/epoch transitions, durable evidence, lease limits, and fencing. For each transition define who may accept writes and how a stale leader is prevented from doing so.
- [x] **D2-CODE (120m):** Implement the [simplified term-based leader election model](../../coding/cpp/distributed_systems_algorithms/questions.md#6-implement-leader-election-in-a-simplified-term-based-cluster-model). Add deterministic events for delayed vote, stale heartbeat, partition heal, crash after term persistence, and a late old-leader write. Test: at most one leader per term, no vote twice per term, and writes require the current fenced term.
- [x] **D2-DESIGN (90m):** Design a replicated AI checkpoint/object write service: acknowledge point, idempotency key, checksum, replica selection, quorum behavior, leader failure, and repair. Compare leader-based versus quorum/coordinator-less options; make an explicit recommendation.
- [x] **D2-FAILURE (60m):** Produce six timelines: lost client reply, leader crash before/after quorum, lease partition, replica corruption, membership change, and stale repair. Each row must say detection, safe customer-visible result, recovery, and residual risk.
- [x] **D2-STORY (30m):** Prepare a three-minute replication/leader-election story. Avoid code internals; make the staff signal the decision boundary, cross-team alignment, proof strategy, and post-launch observability.
- [x] **D2-GATE (20m):** Closed-book: explain why a timeout, a transport completion, a replicated log entry, and durable customer data are different facts.

## Day 3 — Snapshot, Backup, XRR, And XRRVG

**Target:** Make recovery behavior concrete, consistent, and customer comprehensible.

- [ ] **D3-DOM (90m):** Compare copy-on-write, redirect-on-write, and log/manifest snapshots. Define the snapshot linearization point, reference/lifetime rules, garbage-collection safety, and restore verification.
- [ ] **D3-CODE (120m):** Implement an immutable checkpoint manifest publisher: write content-addressed shards, verify checksums, publish a generation atomically, and recover the latest valid generation. Test crash before/after each publication boundary, missing shard, corrupt shard, duplicate publish, and concurrent reader. State what filesystem atomic rename alone does not prove about device durability.
- [ ] **D3-DESIGN (90m):** Re-answer [Design A Fault-Tolerant Checkpointing System](../../system_design/coreweave_l7_system_design_prep.md#18-question-design-a-fault-tolerant-checkpointing-system). Include group consistency for multi-rank workloads, bandwidth isolation, retention, restore drills, and the change in design for elastic training.
- [ ] **D3-DR (75m):** Create an XRR/XRRVG table for asynchronous versus synchronous replication: lag visibility, group consistency boundary, promotion authority, failback, split-brain prevention, RPO/RTO wording, and test evidence. Explain what a customer may safely assume after regional loss.
- [ ] **D3-STORY (30m):** Prepare one snapshot/backup story and one XRR/XRRVG story. Each must include a rejected alternative and a meaningful decision you personally influenced.
- [ ] **D3-GATE (20m):** Draw a restore proof: how you know the recovered application checkpoint is valid, not merely mountable or readable.

## Day 4 — Shared Filesystem Semantics And Multi-Tenant AI Workloads

**Target:** Bridge your filesystem experience to dataset distribution, metadata scaling, tenant isolation, and GPU-cluster topology.

- [ ] **D4-DOM (100m):** Compare S3, NFS-like, FUSE-like, and POSIX-like interfaces for training datasets, model artifacts, and checkpoints. Address directory/listing semantics, overwrite/rename, locks, cache invalidation, small-file metadata pressure, and failure behavior. Identify which semantics are contractual and which are an implementation optimization.
- [ ] **D4-CODE (120m):** Implement a concurrency-safe metadata cache with immutable snapshot publication. Start from [Readers-Writer Cache](../../coding/cpp/concurrency/solutions.md#5-readers-writer-cache); add generation checks, bounded invalidation, and a test for reader/writer races. Explain when a shared mutex is worse than a versioned immutable map.
- [ ] **D4-DESIGN (90m):** Design a multi-tenant shared training filesystem. Explicitly separate control plane (provisioning, policy, quotas) from data plane (metadata/data reads and writes). Include tenant isolation, topology-aware placement, hot namespace mitigation, snapshots, replication, and background compression throttling.
- [ ] **D4-ANALYSIS (60m):** Diagnose “GPU utilization dropped, aggregate storage bandwidth is below nominal capacity.” Produce four competing hypotheses involving metadata, client cache, topology, throttling/noisy neighbors, and background maintenance; list evidence that distinguishes them.
- [ ] **D4-STORY (30m):** Turn filesystem work into a staff story: clarify a product or operational contract, name the hidden edge case, show how you aligned dependent teams, and describe a safe rollout.
- [ ] **D4-GATE (20m):** Explain why an AI-data system can have high aggregate bandwidth yet still starve training workers.

## Day 5 — Durability, Observability, Compression, And Incident Command

**Target:** Demonstrate operational judgment, not just implementation fluency.

- [ ] **D5-DOM (90m):** Specify a durability strategy: checksums, corruption containment, replica/EC choice, scrub, repair priority, failure-domain placement, and backup. Explain how background compression changes write amplification, CPU use, capacity, and tail latency.
- [ ] **D5-CODE (120m):** Implement a tenant-aware admission controller with token buckets and bounded queues. Use [Concurrent Token Bucket](../../coding/cpp/concurrency/solutions.md#3-concurrent-token-bucket) as the base. Test monotonic time, tenant fairness, queue saturation, cancellation, and a maintenance/compression class that cannot consume reserved foreground capacity.
- [ ] **D5-INCIDENT (100m):** Incident: a partial network failure coincides with checkpoint surge and compression backlog; some replicas become unavailable and P99 rises. Write a command timeline: scope evidence, containment, data-integrity posture, customer communication, leader/failover decisions, repair order, rollback/roll-forward condition, and follow-ups. Explicitly invoke replication, leader election, snapshot/backup, XRR/XRRVG, filesystem, and compression impacts.
- [ ] **D5-OBS (45m):** Define a dashboard and alerts for client impact, queues, storage/media, network, metadata, replication/lag, snapshot/backup health, XRR/XRRVG health, and compression backlog. For every alert give an action, not merely a threshold.
- [ ] **D5-STORY (30m):** Prepare an incident-leadership narrative with uncertainty, reversible mitigations, and how you prevented recurrence without blaming individuals.
- [ ] **D5-GATE (20m):** Answer: “Would you pause compression, throttle checkpoint writes, or fail over?” State the evidence needed before each action.

## Day 6 — Timed Staff Interview Simulations

**Target:** Practice delivery under time pressure and integrate every ownership thread.

- [ ] **D6-CODE-A (50m):** Timed C++ implementation: [Leader Election](../../coding/cpp/distributed_systems_algorithms/questions.md#6-implement-leader-election-in-a-simplified-term-based-cluster-model). Spend five minutes on state/invariants, 35 on code/tests, ten on failure and complexity discussion.
- [ ] **D6-CODE-B (50m):** Timed C++ implementation: a cancellation-safe pipeline from [Producer-Consumer Pipeline](../../coding/cpp/concurrency/questions.md#8-implement-a-producer-consumer-pipeline-with-cancellation-and-backpressure). Explain shutdown and backpressure before coding.
- [ ] **D6-DESIGN-A (60m):** Design a high-throughput AI training storage engine. Cover metadata/data separation, data placement, read path, checkpoint writes, replication, caching, filesystem semantics, compression, fault domains, and observability.
- [ ] **D6-DESIGN-B (60m):** Design cross-region consistent checkpoint protection for a volume group. Cover snapshot creation, XRR/XRRVG, promotion, restore, backup, lag, and leader/fencing behavior.
- [ ] **D6-REVIEW (50m):** Score both designs using the eight dimensions. Re-answer the weakest ten minutes without notes. Prepare two follow-ups: “Why not use object storage only?” and “What breaks at ten times the fleet?”
- [ ] **D6-STORY (30m):** Deliver four two-minute stories: replication/leadership, snapshot/backup, XRR/XRRVG, filesystem/compression. Each needs staff scope, conflict/tradeoff, action, outcome, and learning.

## Day 7 — Full Loop, Recall, And Progress Baseline

**Target:** Establish a truthful baseline for recurring use of this tracker.

- [ ] **D7-MOCK (180m):** Run a continuous mock: 45m coding, 45m Storage Engine system design, 30m incident/debugging, 30m project deep dive, and 30m critique. Do not consult notes during the four interview sections.
- [ ] **D7-CODING (45m):** Choose the weaker of D3 manifest publisher and D5 admission controller; implement its key invariant from a clean file and create tests before checking earlier code.
- [ ] **D7-DESIGN (60m):** Re-answer [Design AI Dataset Storage For Training](../../system_design/coreweave_l7_system_design_prep.md#6-question-design-ai-dataset-storage-for-training), adding a concrete XRRVG checkpoint-recovery deep dive and compression/noisy-neighbor policy.
- [ ] **D7-OWNERSHIP (45m):** Produce eight answer cards—one each for replication, leader election, snapshot, backup, XRR, XRRVG, filesystem, and compression. Each card has: problem, invariant, decision, alternative, safe result category, operations, CoreWeave translation, and a likely interviewer follow-up.
- [ ] **D7-LEDGER (30m):** For every `D1`–`D7` task record `not-started`, `practicing`, `evidence-ready`, or `confirmed`; add confidence and next gap. Select the three lowest-confidence tasks for the next revisit.
- [ ] **D7-GATE (20m):** State readiness accurately: strengths, unproven areas (especially hardware-specific claims), and the next three practice actions. Do not call the plan complete merely because the week elapsed.

## Recurring Revisit Protocol

Use this after the first week or before interviews:

1. Spend ten minutes reading only the ledger, not old answers.
2. Select one low-confidence coding task, one ownership story, and one design/incident task.
3. Repeat each closed-book under its original timebox; compare against the rubric afterward.
4. Reopen any box with a new gap; append the date and gap rather than deleting a prior confidence score.
5. End with a five-minute CoreWeave thesis: one data-path detail, one correctness invariant, one tradeoff, one operational signal, and one staff-level decision.

## Useful Public References

- [CoreWeave-style system design catalog](../../system_design/coreweave_l7_system_design_prep.md)
- [CoreWeave storage product context](https://www.coreweave.com/products/storage)
- [CoreWeave Kubernetes Service](https://www.coreweave.com/products/coreweave-kubernetes-service)
- [Kubernetes scheduling framework](https://kubernetes.io/docs/concepts/scheduling-eviction/scheduling-framework/)
- [NVIDIA GPU Direct Storage overview](https://docs.nvidia.com/gpudirect-storage/overview-guide/)
- [SPDK documentation](https://spdk.io/doc/)

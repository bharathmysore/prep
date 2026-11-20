# Microsoft Position 200047514 Interview Prep

This packet is tailored to **Principal Software Engineer — Azure Storage Geo Replication**, position `200047514`, and the stated four-hour loop. The supplied description makes the target clear: geo-resiliency, replication, data protection, business continuity, regional failover, operational excellence, and AI-assisted engineering for exabyte-scale Azure Storage.

## Interview map

| Time | Interviewer / label | Likely focus | What to demonstrate |
| --- | --- | --- | --- |
| 9:00–10:00 | Kris Fritz — PSDE AA | Technical architecture and implementation | Clarify requirements, establish an invariant, write clean C++, then reason about scale and failures. |
| 10:00–11:00 | Bhagyashree Nayak — Customer Obsession | Customer-centered technical judgment | Begin with user harm, use evidence, own the decision, and explain the tradeoff. |
| 11:00–12:00 | Venkates Paramasivam Balakrishnan — One Microsoft | Cross-org leadership | Align teams, make decisions legible, share credit, and reduce future friction. |
| 1:00–2:00 | Kaifeng Zhu — Growth Mindset | Learning and adaptation | Name a gap honestly, describe the learning loop, seek feedback, and show changed behavior. |

`PSDE AA` is not expanded because its meaning is not public or verified. Confirm the format with the recruiter if possible; prepare for a blended architecture-and-coding conversation regardless.

## Role thesis: the four promises to repeat

Use these as the through-line for both design and behavioral answers:

1. **Durability:** a committed write is not silently lost; the replication contract states its failure boundary honestly.
2. **Recoverability:** failover is a controlled, observable state transition with a bounded and customer-visible RPO/RTO—not merely a DNS or traffic switch.
3. **Blast-radius control:** an unhealthy region, replica, feature rollout, or repair workflow cannot turn a local fault into fleet-wide corruption or unavailability.
4. **Operability:** telemetry, reconciliation, game days, automation, and AI-assisted operations reduce customer-impact duration without authorizing unsafe side effects.

Your resume is unusually well aligned. Lead with asynchronous cross-region volume-group replication, application-controlled snapshots, data protection, storage fault containment, capacity efficiency, and region-build automation. Frame RDMA, GPU/HPC debugging, and hardware diagnostics as additional systems depth, not the center of this Azure Storage role.

## Public reference context

- [Microsoft interview tips](https://careers.microsoft.com/v2/global/en/hiring-tips/interview-tips): make assumptions and reasoning visible; use STAR(R) for behavioral responses.
- [Microsoft culture interview areas](https://careers.microsoft.com/v2/global/en/hiring-tips/interview-tips.html): explicitly names growth mindset, One Microsoft, and customer obsession.
- [Microsoft L7 system-design guide](../../system_design/microsoft_l7_system_design_prep.md): representative prompts, not an official interview bank.

## Operating rules

Open technical answers with: “I’ll pin down the customer, correctness target, scale, and failure model. Then I’ll propose a simple baseline, state the invariant, and deepen the highest-risk component.” Do not silently jump into code or architecture.

Use **STAR(R)** for every behavioral response:

1. **Situation / task** — customer, constraint, and your direct accountability.
2. **Action** — your decision, alignment work, and hard tradeoff.
3. **Result** — measured outcome, using only defensible metrics.
4. **Reflection** — what you changed afterward and the mechanism that preserves the learning.

## 9:00–10:00 — Technical architecture and coding

### Design drill: geo-replicated Azure Storage with regional failover

Prompt: *Design geo replication and disaster recovery for an Azure Storage-like service that protects exabytes of tenant data across paired regions. Support asynchronous replication by default, customer-visible replication status, planned and unplanned failover, safe failback, and auditable recovery.*

Practice sequence:


- Clarify: blob/object versus block semantics, permitted data loss, read availability during outage, regulatory/geography limits, customer-triggered versus platform-triggered failover, and whether cross-object consistency is required.
- Separate the **data plane** (write quorum/local durability, log shipping or changed-block replication, apply, checksums, anti-entropy) from the **control plane** (pairing policy, failover epochs, fencing, customer workflow, status, and audit).
- State the core invariant: for a partition/volume generation, at most one region accepts writes. A promotion epoch fences the old primary before the new primary is writable.
- Define replication states: `Healthy -> Lagging -> ProtectedDegraded -> FailoverPreparing -> PrimaryPromoted -> FailbackCatchingUp -> Healthy`. Every transition is idempotent, durable, and externally observable.
- Explain the async contract plainly: acknowledge a write after primary-region durability; expose a replication watermark and RPO estimate; never imply zero data loss after an unplanned regional loss.
- Deep dive on ordering: replicate an ordered per-partition log or versioned changed blocks, apply only contiguous sequence numbers, use checksums and Merkle/segment repair for gaps, and retain tombstones long enough to prevent resurrection.
- Close with telemetry: replication lag by partition and tenant, recovery-point age, replication backlog, checksum/repair rate, fenced-write attempts, failover time by stage, and restore correctness checks.

```text
Client -> Regional write front end -> Primary durable log/data
                                      |  ordered async replication
                                      v
                              Secondary apply + durable replica
                                      |
                         Replication watermark / integrity ledger

Customer + Azure control plane -> Failover orchestrator -> epoch/fencing -> promoted region
```

Use [Design Azure Blob Storage / Object Storage](../../system_design/microsoft_l7_system_design_prep.md#5-design-azure-blob-storage-object-storage), [Design a Globally Distributed Database Like Cosmos DB](../../system_design/microsoft_l7_system_design_prep.md#6-design-a-globally-distributed-database-like-cosmos-db), and [Design Azure Resource Manager / Cloud Provisioning Control Plane](../../system_design/microsoft_l7_system_design_prep.md#20-design-azure-resource-manager-cloud-provisioning-control-plane) as practice baselines.

### Coding drill: ordered replication apply with gap detection

Prompt: *Implement an in-memory replica apply component. It accepts `(partition, sequence, payload, checksum)` records, applies each partition in sequence order, buffers bounded out-of-order records, exposes the highest contiguous applied sequence, and asks for repair on a gap or checksum failure.*

Cover:

- Data model: `next_expected`, bounded pending map keyed by sequence, durable-applied watermark, and repair state per partition.
- Safety: never apply a sequence twice; never advance the watermark past a gap; reject a checksum mismatch before mutation.
- Liveness: when a missing sequence arrives, drain the newly contiguous suffix; when the pending buffer reaches its cap, stop accepting farther-ahead data and request a bounded repair range.
- Follow-ups: concurrent partitions, duplicate delivery, crash/replay, tombstones, snapshots plus logs, checksum/Merkle repair, and backpressure to the primary.

Implement in 35 minutes with `std::unordered_map<Partition, PartitionState>` and `std::map<uint64_t, Record>` for ordered pending records; reserve 10 minutes for duplicate, gap, corruption, buffer-cap, and recovery tests. Then practice the existing [Thread Pool](../../coding/cpp/concurrency/solutions.md#2-thread-pool) and [Bounded Blocking Queue](../../coding/cpp/concurrency/solutions.md#1-bounded-blocking-queue) because replication applies often need bounded asynchronous execution.

| Secondary prompt | Signal | Practice |
| --- | --- | --- |
| Concurrent single-flight cache | Coalesce duplicate repair/read work; ownership and timeout paths | [Single-Flight Duplicate Suppression](../../coding/cpp/concurrency/solutions.md#4-single-flight-duplicate-suppression) |
| Top-K telemetry across shards | Surface hot partitions and lag sources under bounded memory | [Parallel Top K](../../coding/cpp/parallel_algorithms/solutions.md#4-parallel-top-k) |
| Per-customer ordered scheduler | Per-partition ordering, fairness, idempotency, backpressure | [Per-Customer Ordered Scheduler](../../coding/cpp/systems_style/solutions.md#10-per-customer-ordered-scheduler) |
| Retry executor with jitter | Repair/reconciliation retry budgets without amplifying an outage | [Bounded Exponential Retry With Jitter](../../coding/cpp/distributed_systems_algorithms/solutions.md#12-bounded-exponential-retry-with-jitter) |

## 10:00–11:00 — Customer Obsession

Prepare these evidence-bounded stories. Add the precise customer, personal role, metric, and reflection before interviewing; do not invent details.

| Story | Customer problem to foreground | Resume evidence | Likely probe |
| --- | --- | --- | --- |
| Cross-region volume-group replication and application-controlled snapshots | Recovery must be usable for the application, not merely durable | You designed/delivered asynchronous cross-region volume-group replication and application-controlled snapshots | How did you balance RPO, cost, operator burden, and consistency? |
| Capacity reclamation and cold-data compression | Customers need predictable capacity and cost without data-risk surprises | 20–30% sellable-capacity recovery; 6–7% unreferenced-data reclamation | What safeguard stopped optimization becoming a data-loss risk? |
| Failure containment and automated disk/host handling | A failure should degrade a small scope, not become an outage | Refactored fault boundaries; automated failed-disk and single-host handling | What was the blast radius before/after, and how was it proven? |
| iSCSI / network-path redesign | Scale and attach latency constrain customer growth | Multiprocessing and reduced network hops | What measurement changed your initial hypothesis? |

Rehearse: “Tell me about a time you changed a plan after customer/operator evidence,” “when did a metric look healthy but customers were unhappy?”, and “when did you reject a shortcut due to customer risk?” Strong answers name the user, quantify harm, explain discovery, make the decision and dissent path clear, and end with prevention.

## 11:00–12:00 — One Microsoft

| Story | Collaboration tension | Principal-level signal |
| --- | --- | --- |
| Unified object, block, and file storage platform | Product paths and ownership boundaries | Created a shared technical contract while preserving necessary local autonomy. |
| Touchless region builds and qualification | Release velocity versus safety and operations | Aligned service, infrastructure, test, and operations around repeatable gates. |
| Azure Storage RDMA proof of concept | Platform, networking, and storage boundaries | Translated a cross-stack performance idea into testable interfaces and shared evidence. |
| GPU/HPC debugger programs | Product, developer tools, runtime, and customer workflows | Built team capability through mentoring, test infrastructure, and a common diagnosis model. |

Rehearse: “Tell me about a decision where you did not own every required team,” “how did you handle a platform standard versus immediate product need?”, and “when did you change your roadmap to improve the combined customer outcome?” Explain the stakeholder map, decision mechanism, interface/contract, rollout sequencing, risk ownership, and durable capability—not only the meeting cadence.

## 1:00–2:00 — Growth Mindset

Rehearse: “Where were you initially not the expert?”, “which feedback changed your behavior?”, “what failure do you now teach others to avoid?”, and “how would you apply AI-assisted operations safely in your first 90 days?”

Use a concrete learning loop: read design and operational evidence, shadow experts, build a measurement harness, run a bounded experiment, then publish the decision and remaining risks. For AI-assisted operations, recommend read-only anomaly triage, runbook retrieval, correlated-signal summarization, and candidate remediation plans first; require deterministic guardrails, approvals, and audit trails before any mutation. Be exact about adjacent versus direct experience: GPU/HPC debugging, RDMA, storage, and hardware validation are strong foundations; they are not direct ownership of modern LLM-serving systems. Show feedback as a changed mechanism—review structure, rollout gate, test automation, observability, or interface—not merely a changed opinion.

## 48-hour plan

### Day 1

1. Deliver the geo-replication and failover design aloud in 45 minutes; record it.
2. Implement ordered replication apply with gaps and duplicates in C++ in 35 minutes, including recovery tests.
3. Draft four STAR(R) stories: replication/snapshot, capacity optimization, failure containment, and cross-team automation.
4. For each story, write one sentence each for customer, personal role, result, tradeoff, and reflection.

### Day 2

1. Run one mock technical hour: 10-minute requirements, 20-minute design, 20-minute coding/follow-ups, 10-minute retrospective.
2. Run the three behavioral hours with a two-minute initial answer and five-minute probes.
3. Prepare three panel questions: customer problem/measure of success, cross-team interfaces, and the highest-risk technical decision in the first six months.
4. Remove any resume claim you cannot defend with a concrete example, metric, artifact, or named collaborator.

## Day-of checklist

- Ask a clarifying question before proposing a solution.
- State an invariant before writing concurrency or distributed code.
- Separate customer outcome from implementation detail in behavioral answers.
- Give credit specifically and own your decision points.
- End every story with reflection and a repeatable improvement mechanism.
- If you do not know a fact, say what you would verify and how; do not guess.

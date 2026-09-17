# USP and OCI Block Storage: Project-Backed Screening Preparation

Companion to the [ramp tracker](./linux_storage_principal_ramp.md) and [exercise book](./linux_storage_principal_exercises.md). Use your own project experience to explain implementation, correctness, performance, and operational judgment during the 90-minute NVIDIA screening. This is local preparation material, not a public description of Oracle's architecture or a verified interview question bank.

**Teaching mode:** follow the [ramp's teaching-first contract](./linux_storage_principal_ramp.md#teaching-mode--applies-to-the-entire-ramp). The coach explains each feature, walks the source and a worked failure scenario, then helps you construct the answer. Prompts below are lesson topics, not prerequisite quizzes. Notes, examples, and hints are welcome; strict independent rehearsal requires explicit opt-in after teaching.

## Ownership and evidence boundaries

On September 15, 2026, you confirmed ownership in **replication, leader election, snapshot, backup, XRR, XRRVG, parts of the filesystem, and background compression**. That establishes the areas to emphasize. It does not identify your exact changes, incidents, quantitative results, or which subcomponents you owned in each project.

Keep four categories separate:

- **User-confirmed ownership:** the eight areas above, across the projects you identified.
- **Source evidence:** the particular contracts and code paths cited below exist in the inspected local checkouts.
- **Interview inference:** a generalizable design lesson or failure scenario derived from those paths, not necessarily a production incident.
- **Unknown:** personal implementation details, deployed configuration, measured outcomes, actual device guarantees, and untraced boundaries.

Do not copy internal source, service names, identifiers, exact topology, customer information, unpublished numbers, or internal incident links into an interview artifact. Use generic component names and only details you are authorized to disclose. Local source links are for private preparation; a sanitized interview answer should stand without them. No source repositories were modified and no builds, tests, faults, or live queries were run for this review.

## Workspace snapshots and architecture boundaries

The two paths are multi-repository workspaces, not Git roots. Selected source snapshots inspected on September 15, 2026:

| Workspace / repository | Revision | Scope inspected |
| --- | --- | --- |
| `/Users/bmysoren/workspace/usp/blockstorage` | `ccf0864e2a79` | Shared leader/recovery and chunk-side documentation |
| `/Users/bmysoren/workspace/usp/usp-client` | `5834eec81f35` | Chunk I/O, discovery, ownership, EC reads, and tests |
| `/Users/bmysoren/workspace/usp/usp-frontend-service` | `16bae3d33e76` | Extent creation boundary |
| `/Users/bmysoren/workspace/usp/usp-media` | `f3a789434bfa` | Asynchronous filesystem interface |
| `/Users/bmysoren/workspace/bs/blockstorage` | `08ec5487e` | Block client, replication, snapshot, compression, LogFS, SPDK/NVMe |
| `/Users/bmysoren/workspace/bs/bsv2_java` | `15097daf18fa` | Snapshot/backup and cross-region orchestration |

Selected repositories were clean apart from unrelated untracked files in `usp-client`, which were not read or changed. These revisions are local evidence, not statements about production versions. File line numbers can drift after a checkout update; re-resolve symbols before a later detailed walkthrough.

**USP inspected segment — conceptual synthesis, not a complete object PUT trace:**

```text
Service using the USP client
  -> cached namespace / extent discovery
  -> chunk replica I/O and configured protection
  -> separate close / commit interaction with the elected leader

Storage-side interface examined separately:
  asynchronous filesystem commands -> completion / descriptor lifetime
```

Object ingress, object-to-chunk mapping, the exact placement selection path, and the complete server-to-filesystem call chain were not traced. Do not fill those gaps from memory without labeling them and verifying the relevant code.

**OCI Block Storage inspected components — synthesized data-path sketch:**

```text
Block client: alignment / range mapping / extent fanout
  -> leader: sequencing / replication / quorum / ordered commit
  -> acceptor: persisted state and checkpoint processing
  -> asynchronous filesystem / device interface
  -> selected backend: userspace SPDK / NVMe queues and callbacks

Snapshot / backup / cross-region controllers
  -> generation-fenced workflows and completion reconciliation
```

This is a component map, not proof that every deployment uses that backend. In particular, the inspected SPDK path is **not** the kernel `read/write -> VFS -> filesystem -> bio -> blk-mq` path. Be able to draw both and identify their different memory, queue, scheduling, and completion owners. NVMe command completion alone does not prove the application's durability contract.

The USP checkout also contains block-oriented snapshot, backup, and XRR files. Their presence there does not establish those features as object/chunk APIs. This guide uses the separate Block Storage checkout for those feature anchors. A USP background-compression implementation was not established by the bounded review.

## Source evidence ledger

These are implementation/document observations, not claims of your authorship or of successful execution.

| ID | Observation | Local source |
| --- | --- | --- |
| U1 | Extent creation constructs a management request with a retry token; client discovery separately manages cached metadata. | [ExtentService.java](/Users/bmysoren/workspace/usp/usp-frontend-service/usp-frontend-service-api/src/main/java/com/oracle/pic/usp/frontendservice/service/ExtentService.java:82), [ExtentDiscovery.cpp](/Users/bmysoren/workspace/usp/usp-client/usp-client/ExtentDiscovery.cpp:185) |
| U2 | Chunk close is documented as committing data persisted on a replica quorum; the close is sent to the elected leader, with rediscovery after timeout. This does not independently establish power-loss durability. | [ChunkRemoteExtent.cpp](/Users/bmysoren/workspace/usp/usp-client/usp-client/ChunkRemoteExtent.cpp:488) |
| U3 | Replica-failure policy and completion of an already submitted request are tracked separately; batch generation is also recorded. | [ChunkRemoteExtent.cpp](/Users/bmysoren/workspace/usp/usp-client/usp-client/ChunkRemoteExtent.cpp:810) |
| U4 | EC decode collects sufficient successful shard responses and schedules codec work; read deadlines/hedges depend on separate completion and latency state. | [decode](/Users/bmysoren/workspace/usp/usp-client/usp-client/ChunkRemoteExtent.cpp:2556), [read timing](/Users/bmysoren/workspace/usp/usp-client/usp-client/ChunkRemoteExtent.cpp:2831), [hedge timer](/Users/bmysoren/workspace/usp/usp-client/usp-client/ChunkRemoteExtent.cpp:2903) |
| U5 | Leader prepare/recovery processes accepted work; the no-pending-work case establishes leadership with an empty proposal. | [prepare](/Users/bmysoren/workspace/usp/blockstorage/replication/leader/Leader.cpp:2351), [recovery](/Users/bmysoren/workspace/usp/blockstorage/replication/leader/Leader.cpp:2703) |
| U6 | The filesystem interface is asynchronous and documents descriptor lifetime across incomplete operations; it also exposes scheduling distinctions. | [FSApi.h](/Users/bmysoren/workspace/usp/usp-media/fs/FSApi.h:31), [queue interface](/Users/bmysoren/workspace/usp/usp-media/fs/FSApi.h:417) |
| B1 | The block client validates alignment, takes the request buffer, and maps block requests into extent operations. | [Client.cpp](/Users/bmysoren/workspace/bs/blockstorage/client/Client.cpp:1671), [range mapping](/Users/bmysoren/workspace/bs/blockstorage/client/Client.cpp:1904) |
| B2 | The leader waits for replica quorums, including membership-transition requirements, and commits proposals in order. | [quorum](/Users/bmysoren/workspace/bs/blockstorage/replication/leader/Leader.cpp:2223), [ordered commit](/Users/bmysoren/workspace/bs/blockstorage/replication/leader/Leader.cpp:2299) |
| B3 | Recovery precedes new requests; a committed watermark is persisted to prevent minority-accepted writes resurfacing later. | [prepare](/Users/bmysoren/workspace/bs/blockstorage/replication/leader/Leader.cpp:1202), [recovery boundary](/Users/bmysoren/workspace/bs/blockstorage/replication/leader/Leader.cpp:1594) |
| B4 | Stale-ballot read checks and client access/reservation checks are separate mechanisms. | [BlockAcceptor.cpp](/Users/bmysoren/workspace/bs/blockstorage/replication/block/acceptor/BlockAcceptor.cpp:742), [BlockLeader.cpp](/Users/bmysoren/workspace/bs/blockstorage/replication/block/leader/BlockLeader.cpp:2502) |
| B5 | Snapshot prepare validates generation and idempotence, creates a sequence boundary, and schedules a timeout while acknowledgments are held; commit updates state, wakes waiters, and saves state before replying. | [prepare](/Users/bmysoren/workspace/bs/blockstorage/replication/block/leader/SnapshotLeaderComponent.cpp:181), [commit](/Users/bmysoren/workspace/bs/blockstorage/replication/block/leader/SnapshotLeaderComponent.cpp:298) |
| B6 | Compression uses bounded background work; compressed writes and checkpoint metadata handling precede release of original blocks when unmapping is enabled in the examined scan. | [limits](/Users/bmysoren/workspace/bs/blockstorage/replication/block/acceptor/BlockCompressionScan.cpp:30), [write and metadata ordering](/Users/bmysoren/workspace/bs/blockstorage/replication/block/acceptor/BlockCompressionScan.cpp:210) |
| B7 | LogFS queues asynchronous commands and performs mirrored metadata recovery before accepting new writes, followed by data-log replay. | [command path](/Users/bmysoren/workspace/bs/blockstorage/logfs/LogFS.cpp:599), [recovery](/Users/bmysoren/workspace/bs/blockstorage/logfs/LogFS.cpp:1093) |
| B8 | The NVMe backend documents one-thread queue-pair ownership, polls completions, submits SPDK commands, and checks buffer/thread constraints before scheduler admission. | [queue pair](/Users/bmysoren/workspace/bs/blockstorage/async/BlockDeviceNvme.cpp:1184), [completion](/Users/bmysoren/workspace/bs/blockstorage/async/BlockDeviceNvme.cpp:1239), [submission](/Users/bmysoren/workspace/bs/blockstorage/async/BlockDeviceNvme.cpp:1544) |
| J1 | The snapshot coordinator prepares all extents before commit, using prepared sequence numbers and generations. | [SnapshotController.java](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service/src/main/java/com/oracle/pic/blockstorage/management/snapshot/SnapshotController.java:1345) |
| J2 | Backup validates snapshot/parent-manifest state and starts extent uploads; an initial response can still mean uploading. Existing manifests and partial retries are validated separately. | [workflow](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service/src/main/java/com/oracle/pic/blockstorage/management/snapshot/SnapshotController.java:393), [identity and retry](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service/src/main/java/com/oracle/pic/blockstorage/management/snapshot/SnapshotController.java:551) |
| J3 | Cross-region enable/disable checks generations/epochs, disables XRR at the extent-zero coordinator before the remaining extents, and resumes transitional operations after bootstrap. | [XRRVolumeController.java](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service/src/main/java/com/oracle/pic/blockstorage/management/xrr/XRRVolumeController.java:231), [resume](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service/src/main/java/com/oracle/pic/blockstorage/management/xrr/XRRVolumeController.java:443) |
| J4 | Group activation selects a complete group delta and validates response identities, member counts, status, and requested delta. The code uses the term XRR Volume Group. | [API terminology](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service-spec/specs/api.yaml:10057), [activation](/Users/bmysoren/workspace/bs/bsv2_java/ad-manager/blockstorage-ad-manager/src/main/java/com/oracle/pic/blockstorage/admanager/xrr/XrrGroupController.java:72), [validation](/Users/bmysoren/workspace/bs/bsv2_java/ad-manager/blockstorage-ad-manager/src/main/java/com/oracle/pic/blockstorage/admanager/xrr/XrrGroupController.java:238) |

XRR is cross-region replication in the inspected terminology. XRRVG is treated here as your shorthand for cross-region replication volume groups; the literal concatenated acronym was not found in the bounded Java search. Correct that mapping if your ownership used a different meaning.

## Eight owned-feature drills

For each row, spend **20 minutes inside the existing day's domain/source block**: five on a teacher-led source walkthrough, five drawing and explaining normal/recovery flow together, five on a worked failure example, and five constructing a supported two-minute answer and follow-up. Model the answer before requesting a retelling. These replace generic examples; they add no study hours and do not replace kernel labs or coding. All eight fit into 160 minutes across Days 1–6; record unfinished teaching rather than rush or claim completion.

| Owned area | Day / existing todo | Question and required reasoning | Evidence |
| --- | --- | --- | --- |
| Replication | Day 5 / `D5-DOM`, `D5-CODE` | A write reached a quorum but its reply was lost. What is known? Separate accepted, committed, acknowledged, retry identity, recovery, and application completion. Compare a chunk close with a block overwrite without assuming identical contracts. | U2, U3, B2 |
| Leader election | Day 2 / `D2-DESIGN` | Why can't a newly elected leader immediately serve writes? Explain authority/ballot checks, recovery of accepted history, committed-prefix protection, stale-leader behavior, and fencing at the place mutations are accepted. A routing update alone is insufficient. | U5, B3, B4 |
| Snapshot | Day 4 / `D4-DOM`, `D4-DESIGN` | One extent prepared; another timed out. What boundary is safe to expose? Explain per-extent sequence boundaries, generation fencing, coordinator recovery, idempotence, and release of blocked acknowledgments. Do not equate storage crash consistency with application quiescence. | B5, J1 |
| Backup | Day 4 / `D4-DOM`, `D4-CODE` | A manifest exists but not all extents uploaded. Is the backup usable? Separate submitted, uploading, complete, and restore-validated; check parent lineage, identity, retry scope, and retention/GC dependencies. | J2 |
| XRR | Day 5 / `D5-DOM`, `D5-DESIGN` | The controller crashes during partial enable or disable. Show durable operation identity, epoch/generation checks, reconciliation, a safe final boundary, and source/destination lag. RPO and RTO require measurements, not an enabled flag. | J3 |
| XRRVG | Day 5 / `D5-DOM`, `D5-DESIGN` | Members return different recovery deltas or one member fails activation. Define a common recoverable boundary and verify identities/membership before success. Explain partial activation recovery and why a common storage point is not automatically an application-consistent transaction. | J4 |
| Filesystem | Day 1 / `D1-DOM`, `D1-GATE` | Trace command ownership, descriptor/buffer lifetime, metadata recovery, and device completion. Then redraw the standard Linux kernel path and explicitly identify where this selected userspace backend differs. | U6, B1, B7, B8 |
| Background compression | Day 6 / `D6-DOM`, `D6-LAB` | Crash after compressed data is written but before the old representation is released. State publication/reclamation ordering, restart behavior, CPU/memory/I/O limits, and how foreground P99 constrains compression admission. | B6, B7 |

These are training questions, not claims that you experienced those exact incidents. The thought experiments are not instructions to inject faults into either project.

### Required eight-part answer card

Use one compact card per feature; keep each field to one or two sentences so it remains speakable. Fill historical facts only from your experience and evidence. When proposing a new design, label the assumptions and recommendation.

1. **Requirements:** actual API contract and failure tolerance; relevant throughput/latency, RPO/RTO, consistency, or space targets. Unknown numbers remain unknown.
2. **Steady-state path:** request, owner, state transition, persistence/publication boundary, completion. Distinguish control and data paths.
3. **Invariants:** state the property, enforcement point, and what a stale callback/controller/writer must not do.
4. **Failure matrix:** at least three cuts in the sequence, detection, safe action, recovery evidence, and unresolved risk.
5. **Observability:** workload impact plus the responsible stage; name counters/timestamps rather than relying only on aggregate P99.
6. **Rollout:** compatibility, small representative cohort, failure tests, acceptance threshold, and stop condition.
7. **Rollback:** disable/drain/reconcile or restore/rebuild procedure as appropriate; explain state/format changes that cannot be undone by a binary rollback.
8. **Leadership decision:** recommendation, rejected alternative, cost or availability tradeoff, and evidence that would change the choice.

Use these prompts to make the cards concrete:

| Feature | Failure cut / observability focus | Rollout and rollback question | Decision to ask leadership |
| --- | --- | --- | --- |
| Replication | Quorum success with lost reply; committed position versus response latency | Mixed versions and retry semantics; what preserves the committed history during reversal? | Availability versus the required durability/consistency contract |
| Leader election | Old leader remains alive; recovery duration and stale-request rejection | Exercise failover under load; never "rollback" by reviving unfenced ownership | Maximum permitted outage to preserve safe ownership |
| Snapshot | Coordinator failure with pending members; prepare/commit age and write stalls | Bound stalls and reconcile pending generations before reverting orchestration | Snapshot consistency level versus pause cost |
| Backup | Partial upload or broken lineage; completion age and restore verification | Canary restore and retention compatibility; preserve dependencies when disabling new uploads | Retention/cost versus independently tested recoverability |
| XRR | Partial transition or growing destination lag; recovery-point age and transfer backlog | Failback requires ownership and reconciliation, not just reversing traffic | RPO/RTO budget, bandwidth cost, and promotion authority |
| XRRVG | One stale/failed member; group completeness and per-member delta | Prove partial activation recovery and membership compatibility | Group recovery semantics versus independent-volume availability |
| Filesystem | Torn/incomplete metadata transition; replay correctness and queue/service delay | Compatibility of persisted formats; an old binary may not understand new state | Format/performance benefit versus recovery and migration risk |
| Compression | Crash between new data and reclamation; CPU, memory, queue delay, compression gain | Stop new work and drain safely; disabling compression does not restore freed original blocks | Space savings versus foreground latency and recovery complexity |

## Project stories and coding bridges

Prepare **one concrete USP story and one concrete Block Storage story**, selecting actual changes/incidents you can defend. Cover a diagnosis and a design/correctness decision across the pair. Do not combine separate projects or years of work into a fictional single incident.

**USP candidate:** replication/leader recovery, late callbacks, or foreground latency under repair/EC work. Open with the customer-visible symptom or contract, trace the critical ownership/commit boundary, explain your actual change, and defend the failure tests and rollout. U2–U6 are technical prompts, not proof you changed those lines.

**Block Storage candidate:** safe recovery, a snapshot/backup/XRR/XRRVG transition, or compression/recovery ordering. Choose one as the main story and use the others as short follow-up examples. B2–B8 and J1–J4 let you connect controller state to data-plane safety without claiming the controller alone proves durability.

Keep a 30-second version, a two-minute version, and a five-minute version of each. In the five-minute version, budget 30 seconds for the problem, 45 for the path/invariant, 90 for your implementation/diagnosis, 60 for failures/testing, 45 for change safety/results, and 30 for the decision and limitations.

The existing coding exercises remain generic C++/C practice, not copied proprietary code. Derive the design and invariants together, explain a worked implementation, and guide a variation; independently implemented versions are a later opt-in readiness check, recorded separately from assisted work:

| Existing exercise | Project concept to explain afterward | Counterexample / edge case |
| --- | --- | --- |
| Extent mapper and partial-I/O harness | Logical block range versus chunk/object mapping | Boundary-crossing range, hole, unaligned request, partial completion |
| Bounded executor and request tracker | Replica failure policy versus in-flight ownership | Late completion after timeout; stale generation; shutdown with retained buffers |
| Kernel C lifecycle helper | Reference ownership and asynchronous teardown | Callback remains possible after caller timeout; sleeping in the wrong context |
| Checkpoint publisher | Snapshot boundary versus complete backup/manifest publication | Manifest visible before referenced data is recoverable |
| Fenced replica model | Recovery before serving, epoch checks, retry ambiguity | Accepted minority state resurrected; old writer; incomplete group recovery |
| Tenant admission and telemetry | Foreground versus repair/compression work | Background work consumes shared CPU/memory/device capacity despite separate queue labels |

For XRRVG, use a paper event trace with three anonymous volumes: one delta complete, one stale, one failed. Identify the allowed reported group outcome and recovery steps. This is a variation inside `D5-DOM`, not an additional distributed-consensus implementation or a new coding requirement.

### Personal evidence still to supply

Area ownership is confirmed; the following details are not. Keep this as an evidence table, not a second checkbox tracker.

| Project / selected story | Exact component/change you owned | Hardest bug or design choice | Your code/test/review contribution | Authorized result/measurement | Sharing limits |
| --- | --- | --- | --- | --- | --- |
| USP | To supply | To supply | To supply | To supply; do not invent numbers | To confirm |
| OCI Block Storage | To supply | To supply | To supply | To supply; distinguish team result from personal work | To confirm |

Suggested introduction skeleton, not a finalized biography: "My storage work spans object and block systems. My ownership has included replication and leader election, snapshots and backup, cross-region replication including volume groups, and parts of filesystem/background-compression work. A representative contribution was [specific change I implemented], where the hard problem was [invariant or measured bottleneck]. I can walk through the implementation, the failure cases, and the rollout tradeoff."

## Integration into the dated plan

- **Days 1–6:** use the eight 20-minute source drills inside existing domain blocks. On Days 1, 4, and 6, use the already allocated 15-minute story sessions for selection, evidence, and concise delivery.
- **Day 7 / rehearsal A, September 22:** build an introduction to the two projects without unsupported metrics. Use a real project to teach the lifetime/incident follow-up, while coding the generic queue with support. Independent execution is opt-in.
- **September 24 / rehearsal B:** work through a complete design connected to the owned features. Discuss the general architecture, not private source or internal product details. Teach unused features as adversarial follow-ups within existing design time.
- **September 25 / rehearsal C:** work through one ownership claim at code/test level and one failure boundary with notes and guidance as needed. Both project stories must distinguish personal work, repository observations, and unknowns. Record independent evidence only for an opted-in unassisted attempt.
- **September 26–27:** review the two story cards and eight feature cards alongside the existing diagrams. No new source tour or first-time project reconstruction is planned for the review weekend.

The tracker stays at **53 todos and 85 focused hours before September 25**. `SC-STORY`, `P0-SOURCE`, the linked daily items, and the mock IDs carry this work. Area ownership is not a confidence confirmation and does not check any readiness item.

## Test and source limitations

Observed tests are useful reading targets, **not passing results**:

- USP late-replica success handling: [ChunkRemoteExtentTest.cpp](/Users/bmysoren/workspace/usp/usp-client/usp-client/test/ChunkRemoteExtentTest.cpp:163).
- Block leader-failover/read-quorum cases: [LeaderTest.cpp](/Users/bmysoren/workspace/bs/blockstorage/replication/block/test/LeaderTest.cpp:1438).
- Snapshot timeout and repeated prepare/commit: [SnapshotLeaderTest.cpp](/Users/bmysoren/workspace/bs/blockstorage/replication/block/test/SnapshotLeaderTest.cpp:283).
- Compression concurrency limit: [CompressionLeaderTest.cpp](/Users/bmysoren/workspace/bs/blockstorage/replication/block/test/CompressionLeaderTest.cpp:410).
- Partial backup retry: [BackupCreateTest.java](/Users/bmysoren/workspace/bs/bsv2_java/management-service/blockstorage-management-service/src/test/java/com/oracle/pic/blockstorage/management/snapshot/BackupCreateTest.java:121).
- Failed or mismatched group activation: [XrrGroupControllerTest.java](/Users/bmysoren/workspace/bs/bsv2_java/ad-manager/blockstorage-ad-manager/src/test/java/com/oracle/pic/blockstorage/admanager/xrr/XrrGroupControllerTest.java:535).

Do not claim this review proved application-consistent quiescing, complete rollback after partial group activation, end-to-end power-loss durability, actual RPO/RTO, or production latency gains. A bounded C++ search did not find `io_uring`/`liburing`; that is not proof of absence from all repositories. Userspace SPDK code does not establish personal kernel-driver, RDMA, DPU, or GDS experience. Keep those practical qualifications separate.

Reference context: the [official JR2024421 posting](https://nvidia.wd5.myworkdayjobs.com/nvidiaexternalcareersite/job/us-ca-santa-clara/principal-block-file-storage-software-engineer--linux---dgx-cloud_jr2024421) was rechecked September 15, 2026 for role alignment. It emphasizes hands-on C/kernel/userspace, Linux storage, NVMe, distributed systems, and production debugging; it does not prescribe these project questions or confirm the screen format. Project evidence above comes only from the local sources explicitly supplied by the user, not from public claims about Oracle deployments.

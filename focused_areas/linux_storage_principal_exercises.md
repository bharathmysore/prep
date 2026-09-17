# Linux Storage Principal Ramp: Detailed Exercises

Use the [ramp tracker](./linux_storage_principal_ramp.md) for all checkboxes and confidence updates. This book contains exercise specifications; it does not claim implementations or experiments have been completed. Workload numbers, costs, thresholds, and assessment scores are illustrative training assumptions, not NVIDIA requirements or measured results.

Calendar: setup September 15, 2026; Days 1–7 on September 16–22; gap closure and qualification September 23–25; review only September 26–27. The readiness target is September 25. See the [dated tracker](./linux_storage_principal_ramp.md#deadline-calendar) and [qualification/review sessions](#deadline-qualification-and-review-sessions). The seven-day core retains its full scope; the added days provide remediation and delayed recall, not additional curriculum.

Project grounding: use the [USP / OCI Block Storage companion](./linux_storage_project_evidence.md) for all eight confirmed ownership areas: replication, leader election, snapshot, backup, XRR, XRRVG, filesystem work, and background compression. Its source drills replace generic examples inside the existing domain blocks; they do not add hours or substitute repository access for personal implementation/lab evidence.

## Teaching mode contract

The entire ramp uses teaching mode: setup, baseline, domain study, coding, labs, source reading, designs, project stories, rehearsals, qualification, and review. For each topic, the coach explains the fundamentals and vocabulary, works through an example, and shows a model answer before asking for a comprehension check. For code, walk the relevant lines, ownership changes, invariants, failure paths, and complexity before guided implementation and tests. For labs, explain each command's purpose, exact target, expected observation, and limits before the user performs an authorized step.

Use this sequence inside the existing time allocation: **explain → worked example/model answer → guided practice → one comprehension check → feedback and reteaching**. Notes, source references, prior code, diagrams, hints, and pauses for explanation are allowed throughout. Ask only one check at a time and wait for the response; if it exposes a gap, explain the missing concept and work another example before continuing. The numbered prompts and test tables below are teaching agendas, not batches of questions to answer before help is provided. All durations are learning budgets, not speed requirements; report unfinished coverage if teaching needs more time.

The 90-minute rehearsals and all design/incident practice are guided by default. Strict timed, closed-book, or independent assessment occurs only when the user explicitly opts in to that specific assessment after the material has been taught. Historic headings containing “independent,” “timed,” or “closed-book” are retained for links; their default activities follow this contract.

Record whether evidence was **taught/modelled**, **completed with guidance**, or **independently demonstrated**, including any notes or hints used. A guided correct answer can demonstrate learning progress; it cannot establish independent interview performance. Retain all technical correctness, execution, safety, and delayed-revisit gates. A coached explanation does not replace code execution or a required real lab, and a guided revisit does not establish unaided recall. If independent readiness has not been assessed, report it as unassessed rather than a pass or failure. No status is completed merely by converting this plan to teaching mode.

## Ninety-minute screening rehearsals

Target: the user's 90-minute screen for [NVIDIA JR2024421](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Principal-Block-File-Storage-Software-Engineer--Linux---DGX-Cloud_JR2024421). These are **our chosen practice formats, not a verified NVIDIA agenda or question bank**. Recruiter guidance should override the mix when available. No live interview date is inferred from the September 25 readiness deadline.

| Mock | Introduction | Coding | Linux/storage oral | Design / incident | Candidate questions | Total |
| --- | --- | --- | --- | --- | --- | --- |
| A — Sep 22 | 5m | 45m: C++ bounded queue | 20m: I/O path, lifetime, durability | 15m: release-incident challenges | 5m | 90m |
| B — Sep 24 | 5m | 25m: plain-C bounded-span parser | 10m: weakest kernel/NVMe topic | 45m: one complete existing design | 5m | 90m |
| C — Sep 25 | 5m | 40m: fresh variant of a practiced C/C++ core | 20m: mixed domain and delayed retests | 20m: two adversarial design follow-ups | 5m | 90m |

“Mock” A/B/C names identify the same 90-minute allocations; run each as a guided rehearsal by default. Begin each segment with a model explanation or worked variant, guide the implementation or discussion, then ask one comprehension check at a time. Pause for teaching, use notes and prior code, and explain changed assumptions before practicing their consequences. Compilers and references support learning; actual interview permissions remain unknown. Collect code and a recording or transcript with the assistance used, then review gaps together. A partial implementation is recorded as partial, even if the remaining scheduled work completes it later. Only an explicit opt-in after teaching switches a particular rehearsal to a continuous independent mock with agreed tool and note rules.

The normal 45-minute design rubric still applies to every full design. Follow-up answers should state the relevant requirement, path, invariant, failure, measurement, change plan, and decision concisely rather than repeat the entire architecture.

**Screening practice rubric:** use five dimensions to organize teaching feedback: problem framing/communication; implementation and tests; ownership/concurrency safety; Linux/storage explanation; debugging/design judgment. Record what was explained, practiced, and still needs help. For an explicitly chosen independent mock only, score each dimension 0–4: `0` absent or unsafe, `1` heavily prompted, `2` plausible but incomplete, `3` independently correct, `4` precise, efficient, and handles changed assumptions. The independent target remains at least 16/20 with every dimension at least 3 in mocks B and C, with no unresolved hard correctness error. Guided B/C sessions do not satisfy or fail that independent target; leave it unassessed. This is a coaching threshold, not a hiring prediction. A shorter coding sample is judged against its stated subtask, not falsely counted as an entire finished project.

Time is counted once:

- **September 22 (540m):** mock A 90m; remaining request-tracker coding/repair 105m; practical replay 120m; two designs plus critique 120m; remaining incident simulation/challenges/note 75m; remaining oral and gap review 30m. The mock contains the existing 45-minute queue exercise, 20 oral minutes, and 15 incident-challenge minutes. Ten minutes of the original gap-review budget fund the introduction/questions.
- **September 24 (480m):** mock B 90m; six remaining full designs 270m; C continuation/tests 35m; remaining domain retest 20m; pooled critique 45m; revision pack 20m. The mock's 45-minute design counts as one of seven, not an eighth. Its 25-minute C sample plus the 35-minute continuation is the day's 60-minute C exercise.
- **September 25 (240m):** mock C 90m; targeted coding/tests 35m; remaining oral 25m; practical replay/audit 45m; remaining design follow-ups 20m; final evidence/confirmation audit 25m. Across the mock and remaining blocks, retain 75m coding, 45m oral, and 40m design follow-ups.

### Plain-C screening practice

This supplements, rather than replaces, the C++ workbench and actual kernel-C exercise. It is role-aligned practice; do not assume the interviewer requires or permits a particular language until confirmed.

**Two 15-minute guided defect reviews**, charged to Days 1 and 2 recall blocks. The coach first explains the defect and demonstrates the correction; walk each line and invariant together before one comprehension check:

1. Explain why validating a range with unchecked `offset + length <= size` can fail. Write a C helper that first proves `offset <= size`, then checks `length <= size - offset`, and does no pointer arithmetic until validation succeeds. Test zero length, offset equal to size, out-of-range offset, and lengths near `SIZE_MAX`. Explain why the caller must also supply a valid underlying object; integer checks cannot validate an arbitrary pointer. State constant time/space.
2. Review a request whose timeout callback frees a payload while completion can still access it. Draw caller/backend/reference ownership and fix the lifetime protocol in C-style pseudocode. Separate caller notification from backend quiescence, handle a late completion and shutdown, and identify allowed lock/callback contexts. Do not claim `volatile` or a reference counter alone solves all ordering and synchronization problems. Tie this to the existing kernel-C tests.

**Mock B C coding task: bounded record parser, 60 minutes total (25 rehearsal + 35 continuation/tests).** Begin with a worked record sequence and model implementation, then trace the bounds checks line by line before guided coding. Given a valid byte span of length `n`, parse a sequence of `[4-byte little-endian payload length][payload bytes]` records. Define an explicit error enum; return a record count only on success. Do not allocate memory or cast an unaligned byte pointer to an integer pointer. Define whether `(NULL, 0)` is accepted, reject null with nonzero length, and leave the output unchanged on failure.

**Pattern / Idea:** scan a binary span while maintaining validated bounds. **Tags:** `linear-scan`, `bounds-checking`. **Company signal:** domain fit only; no reported NVIDIA question-frequency claim. **Implementation status:** pending; this is a guided application of previously taught C bounds/lifetime skills, not an implemented solution or a new subsystem.

Implement validation before each access: at least four remaining bytes for the header; decode using explicitly unsigned byte-to-integer conversions; check payload length against remaining bytes using subtraction; advance only after proof. State the loop invariant: the current offset is within the input, and every prior record is fully validated. Time O(n) upper bound, O(number of records) when payloads are skipped; O(1) auxiliary space. No concurrency is required; explain how borrowed-buffer lifetime would change for asynchronous use.

Concrete cases: empty span → zero records; one zero-length record → one; two valid records → two; one to three trailing header bytes → truncated-header error; declared payload longer than remaining input → truncated-payload error; maximum 32-bit length with a tiny buffer → error without overflow/read; valid prefix followed by malformed tail → error and unchanged output. Compile with warnings and appropriate sanitizers in the existing lab. Explain error cleanup and why no allocation/free is needed here. Save the 25-minute state and assistance separately so the continuation cannot disguise what was completed in that segment.

### Project communication for the screen

Use 15 minutes of guided recall on Days 1, 4, and 6; polish in the existing revision-pack block. Teach the story structure with a clearly labelled illustrative model before building the user's two-minute introduction and two five-minute stories together. All accomplishments must come from the user's real experience; leave placeholders where facts or metrics have not been supplied.

The projects are now specified: **Unified Storage Pool (object storage)** and **OCI Block Storage**. Ownership areas are user-confirmed, so use one story from each and the [eight owned-feature drills](./linux_storage_project_evidence.md#eight-owned-feature-drills) as follow-ups. Day 1 selects actual changes; Day 4 fills evidence and outcome limitations; Day 6 rehearses concise delivery. Keep all private source references in the local companion, not in the spoken or shareable interview material.

- **Introduction:** current systems scope; one relevant storage/distributed-systems contribution; one example of hands-on debugging/implementation; why this specific role fits. Avoid substituting a list of technologies for demonstrated ownership.
- **Production diagnosis:** symptom and customer impact → your competing hypotheses → decisive measurements → code/path-level cause → containment and fix → measured result and remaining uncertainty.
- **Design/correctness decision:** requirement → alternatives → invariant → implementation detail you personally owned → failure testing → rollout/rollback → outcome and what you would change.

Teach and rehearse follow-ups one at a time: "What exactly did you write?", "Which evidence ruled out the other explanation?", "What failed?", "How did you persuade the other team?", and "What would change at ten times the load?" Distinguish your contribution from the team's. Prepare questions about current block/file ownership boundaries, host/DPU split, dominant correctness/performance challenges, and first-project expectations; do not pretend to know NVIDIA's proprietary architecture.

## Environment and evidence contract

Use a disposable Linux development VM or host, with a second isolated Linux VM for the NVMe/TCP lab. A useful starting allocation is 8 vCPUs, 16 GiB RAM, and 60 GiB free build/test space; adjust for the chosen kernel build and dataset. Record the actual resources. A macOS C++ run can validate portable code but cannot satisfy Linux kernel/block-stack gates.

Prepare these tools in the selected lab: C/C++ compilers, a build system, a Linux source checkout and its documented build prerequisites, fio, nvme-cli, nvmetcli, strace, perf, and a supported tracing tool such as trace-cmd or bpftrace. Inventory first; follow the distribution/upstream installation instructions for missing components. For kernel tests, choose supported UML or QEMU execution and record the configuration. Tool installation and target configuration are future lab steps, not actions performed by creating this plan.

All write tests use newly created synthetic files or explicitly identified disposable virtual disks. Scope memory pressure to the lab process/cgroup or VM. Network interruption applies only to the isolated test transport. Do not export an existing mounted filesystem's backing block device or run formatting/raw-device writes against an unidentified device. Start the kernel baseline build early on Day 1.

Record one environment sheet: OS/architecture, `uname -r`, source commit, compiler/build flags, kernel config, RAM/vCPU count, filesystem and mount options, virtual or physical storage, NVMe model/firmware if present, CPU/NUMA placement, transport, and tool versions.

Evidence levels must remain distinct:

| Level | What it demonstrates | What it cannot establish |
| --- | --- | --- |
| Deterministic C++ model | State transitions, ordering assumptions, tested interleavings | Real DMA, fabric behavior, complete consensus correctness |
| Linux file/VM lab | Syscall behavior, filesystem protocol, tracing methodology | Bare-metal SSD latency or physical power-loss protection |
| Kernel C with KUnit | Actual kernel compilation/execution, kernel object lifetime and synchronization | Correctness of an entire NVMe driver |
| Actual NVMe/TCP lab | Initiator/target setup, transport I/O, interruption/reconnect | RDMA or multi-target durable ownership failover |
| Physical NVMe/RDMA/DPU/GPU | Measured behavior on the recorded topology/configuration | Unmeasured hardware, firmware, or fleet-scale generalization |

Missing hardware remains visible in the tracker's hardware ledger. A synthetic null block device is useful for block dispatch/completion experiments, but its default completion-only mode is not durable storage and is not a data-integrity oracle. See [null_blk](https://docs.kernel.org/block/null_blk.html).

## Baseline diagnostic

Use the existing 30 minutes as an introductory teaching session. No quiz precedes the explanations. In each segment, explain the vocabulary, show a worked example/model answer, and then offer one supported comprehension check:

1. **8 minutes:** teach a buffered cache miss and a direct write using an annotated diagram of memory owners and completion boundaries; guide a short walkthrough.
2. **7 minutes:** model a capacity-one blocking queue, including shutdown while a producer waits; trace the predicate and wakeup before a guided sketch.
3. **5 minutes:** compare `write` success, `fsync` success, flush, FUA, and RDMA completion using a worked completion-versus-durability timeline.
4. **5 minutes:** work a partitioned-writer example and show where current authority is enforced after failover.
5. **5 minutes:** model an investigation of P99 rising while device service time stays flat; explain the missing intervals and discriminating measurements.

Record what was introduced, understood with help, and needs more teaching, with a concrete reason. Revisit the same topics with explanations and guided checks on Day 7. This is a learning baseline, not an independent score; unfinished introductory coverage remains visible.

## Workbench and artifact contract

Build one small C++20 project across the week. Suggested modules are `extent_map`, `file_io`, `executor`, `request_tracker`, `checkpoint`, `replica_model`, and `tenant_admission`. Keep a deterministic fake clock and injectable backend operations separate from real file and network adapters. Kernel C lives in a separately recorded Linux source tree.

For each coding attempt, save:

1. One-sentence problem, input/output contract, and explicit non-goals.
2. Pattern/idea, tagged below; implementation status; ownership and transition diagram.
3. Source, tests, commands/compiler flags, and raw results.
4. Explanation of relevant code lines and main blocks; safety and liveness arguments; model answers, notes, and hints used.
5. Time/space complexity including queues, retained requests, logs, and threads.
6. One runtime optimization and one memory optimization with tradeoffs.
7. Edge cases, one production follow-up, and a short list of unresolved gaps.

These exercises use **domain-fit relevance to NVIDIA/Excelero**, with no measured interview-frequency claim. Existing solution links are pattern references, not complete solutions to the expanded lab specifications. All new workbench implementations are pending until you write and test them. Concrete test scenarios are in this exercise book; do not copy them into a canonical solution file if you later promote an exercise into the coding catalog.

Use separate sanitizer builds: ASan plus UBSan for memory/undefined behavior, and TSan for data races where supported. Keep compiler warnings enabled. A passing sanitizer run is evidence about exercised paths, not a proof. Prefer RAII ownership, bounded queues, joined threads, deterministic failure injection, and callbacks outside internal locks. Record unsupported sanitizer configurations instead of claiming they ran.

Recommended evidence per day: source diff; test log; raw measurement data; one annotated trace; one diagram; a design decision note; a 5–10 minute recording/transcript; gap updates. Create artifact paths when work starts, then link the exact paths and lab host in the tracker.

Use the first 20 minutes of each coding block for a taught warm-up. Open the linked reference, explain the pattern, walk a worked example and the relevant code lines, then derive the invariant and time/space complexity together. Follow with one guided comprehension check; notes and hints remain available:

| Day | Warm-up | Main recognition signal |
| --- | --- | --- |
| 1 | [Merge Intervals](../coding/cpp/arrays_strings/solutions.md#5-merge-intervals) | Normalize overlapping ranges before mapping/scheduling. |
| 2 | [TTL Cache](../coding/cpp/advanced_data_structures/solutions.md#3-ttl-cache) | Expiry ordering, stale heap entries, and bounded retained state. |
| 3 | [LRU Cache](../coding/cpp/advanced_data_structures/solutions.md#1-lru-cache) | Hash lookup plus list ownership/iterator lifetime; discuss kernel C translation. |
| 4 | [Config Snapshot Manager](../coding/cpp/systems_style/solutions.md#4-config-snapshot-manager) | Immutable state and atomic publication; distinguish memory from disk durability. |
| 5 | [Rendezvous Hashing](../coding/cpp/distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | Deterministic replica ranking; add a failure-domain constraint. |
| 6 | [Concurrent Token Bucket](../coding/cpp/concurrency/solutions.md#3-concurrent-token-bucket) | Monotonic time, explicit rate/burst units, synchronized admission. |

## Source-reading map

Pin one kernel commit for code reading and testing. The links here use Linux v6.12 as a stable reading example; they are not an instruction to replace a deployed kernel. API details in current documentation may differ; reconcile against the source actually built.

| Area | Read narrowly | Teach with a model answer, then check together |
| --- | --- | --- |
| Syscall/VFS | [fs/read_write.c](https://github.com/torvalds/linux/blob/v6.12/fs/read_write.c) | Where do fd, position, flags, and buffer iterator enter filesystem code? |
| Cached I/O | [mm/filemap.c](https://github.com/torvalds/linux/blob/v6.12/mm/filemap.c) | Who waits for a folio, and who copies data into userspace? |
| Filesystem entry | [fs/xfs/xfs_file.c](https://github.com/torvalds/linux/blob/v6.12/fs/xfs/xfs_file.c) | Which branch chooses buffered/direct I/O and which locks apply? |
| Extent/direct I/O | [fs/iomap/direct-io.c](https://github.com/torvalds/linux/blob/v6.12/fs/iomap/direct-io.c) | Who aggregates bios, holds pages, and finishes metadata work? |
| Block dispatch | [block/blk-mq.c](https://github.com/torvalds/linux/blob/v6.12/block/blk-mq.c) | When can dispatch be immediate and when are tags/queues needed? |
| NVMe PCIe | [drivers/nvme/host/pci.c](https://github.com/torvalds/linux/blob/v6.12/drivers/nvme/host/pci.c) | Where are DMA mapping, command publication, and completion cleanup? |
| Reclaim/writeback | [mm/vmscan.c](https://github.com/torvalds/linux/blob/v6.12/mm/vmscan.c), [mm/page-writeback.c](https://github.com/torvalds/linux/blob/v6.12/mm/page-writeback.c) | Why can an I/O workload stall before reaching the device? |

For each file, the coach explains the selected path and relevant lines before asking for a walkthrough. Bookmark only the relevant entry, one allocation/locking site, one wait, one error path, and one completion path. Record actual symbol names and line references. Do not spend the week reading whole subsystems sequentially.

Add the selected [project source anchors](./linux_storage_project_evidence.md#source-evidence-ledger) to this reading map. Explain where shared replication code serves distinct chunk/block contracts. Draw the inspected userspace SPDK/NVMe backend separately from the Linux VFS/blk-mq path; neither source tree alone proves the active production backend or device durability policy.

## Common design exercise contract

For every design/incident drill, first teach the requirements and tradeoffs using a worked diagram, calculations, and model answer. Build the following eight sections together, then ask one comprehension check at a time. They remain the required answer structure within the allocated learning budget:

1. **Requirements:** functional scope, scale, latency/throughput, durability/consistency, tenancy, and failure assumptions.
2. **Steady-state path:** block diagram, entities/APIs, control versus data flow, write/read or normal operating sequence, and critical component internals.
3. **Invariants:** at least three exact properties and where each is enforced.
4. **Failure matrix:** failure → detection → safe action → recovery → remaining risk; include ambiguous failures.
5. **Observability:** customer impact plus queue, resource, correctness, and recovery signals.
6. **Rollout:** compatibility, cohort selection, acceptance thresholds, and stop conditions.
7. **Rollback:** actual reversal/failback mechanism, data-format limits, and evidence that the old path is usable.
8. **Leadership decision:** recommendation, cost, rejected alternative, and the evidence that would change the choice.

Use the 45-minute guided discussion budget: requirements/sizing 5m; diagram and normal flow 10m; internals/invariants 10m; failures/tradeoffs 10m; operations/change plan 7m; decision 3m. Teaching and supported practice occur inside each segment. Follow with 30 minutes of guided challenges and 15 minutes updating the note. Explain the consequences of each changed assumption before asking the user to practice the revised answer.

Score the resulting content in each section `0` absent/unsafe, `1` plausible but incomplete, or `2` explicit and defensible. Content-quality target: **14/16 or higher, no zero, and no unresolved correctness error**. Record assistance separately: a guided answer meeting this target is guided evidence, not an independent performance pass. This is a training rubric, not a company hiring rubric. Your confidence confirmation still controls completion, with unassessed independence and unresolved evidence stated explicitly.

## Day 1 — I/O path and file harness

### Domain exercise

Draw four paths: cached read hit, read miss, ordinary buffered write followed by writeback, and direct read/write. For each annotate the executing context, file offset, device offset, buffer location, reference/pin owner, possible wait, error recipient, and application-visible completion.

Explain these distinctions aloud: folio versus bio versus request; payload versus descriptor; file offset versus LBA versus DMA address; page pin versus DMA mapping; Dirty versus Writeback; device completion versus filesystem completion. Explain why a hole can return zeroes without SSD data I/O, why metadata lookup may cause separate reads, and why an ordinary read hit can still fault its destination memory.

Use the [VFS overview](https://docs.kernel.org/filesystems/vfs.html), [iomap operations](https://docs.kernel.org/filesystems/iomap/operations.html), and [blk-mq guide](https://docs.kernel.org/block/blk-mq.html). Do not draw mandatory software queues at every layer; direct dispatch and caller-context execution are valid paths.

### Coding exercise — map ranges and perform exact file I/O

**Pattern / Idea:** split a logical interval against an ordered extent map; use checked arithmetic and explicit I/O progress. **Tags:** `intervals`, `ordered-map`. Warm-up: [Merge Intervals](../coding/cpp/arrays_strings/solutions.md#5-merge-intervals), 20 minutes.

**Specification:** an immutable map contains `{logical_start, length, backend_offset, Mapped|Hole}` entries over a finite logical file. Implement `map_read(offset, length) -> segments`, plus `read_exact_at` and `write_exact_at` over a newly created file. Return a structured short-read/EOF/error result; do not loop forever on zero progress. Hole writes are outside this day's mapper scope and must be rejected explicitly.

Implementation steps:

1. Define whether the map must fully cover the logical file; either insert explicit holes or reject unexplained gaps. Reject overlap, negative/invalid conversions, and overflowing range ends.
2. Use binary search/ordered lookup to locate the first extent, then emit segments across boundaries. Zero-fill holes without issuing payload reads.
3. Implement RAII descriptors and buffers; use `pread`/`pwrite` so independent operations do not share a mutable file position.
4. Inject syscall behavior to test short reads/writes, `EINTR`, zero-progress writes, and `EIO`. Preserve byte counts on partial success.
5. Add an aligned direct-I/O mode only after discovering supported alignment using the selected filesystem's documented interface, such as `STATX_DIOALIGN` when available. Record unsupported cases. Do not silently label buffered fallback as direct I/O.
6. Write a deterministic byte pattern, reopen, read, and compare exact bytes/checksums. Report setup time separately from transfer time.

| Concrete scenario | Expected result |
| --- | --- |
| Extents: `[0,4096)` mapped to backend 8192; `[4096,8192)` hole; `[8192,12288)` mapped to backend 0. Read offset 2048, length 8192. | Three segments: 2048 bytes at backend 10240; 4096 zero bytes; 2048 bytes at backend 0. |
| Zero-length request | Empty result; no I/O. |
| Range beyond logical EOF | Explicit EOF/short result per API; no fabricated full success. |
| Unsigned offset near maximum plus nonzero length | Rejected before wrapping. |
| Injected transfers of 1024, `EINTR`, then remainder | Correct buffer/offset advancement; no duplication. |
| Failure after some bytes transfer | Preserve accepted byte count and error; never report full completion. |
| Buffer/offset violating discovered direct alignment | Rejected by harness or recorded kernel error; no false direct-path claim. |

**Invariants:** every output byte has one source; no buffer access beyond the supplied span; descriptors close exactly once. **Complexity target:** mapping `O(log E + K)`, transfer/zeroing `O(B)` plus I/O; `O(K)` segment storage plus bounded buffers. Discuss batching adjacent segments for runtime and streaming segments for memory.

### Experiment — cache versus device evidence

Create a small correctness fixture and a separately sized benchmark file on the selected filesystem. Prepopulate the benchmark file with real data; record its size relative to effective memory. Run warm-up and three measured windows, recording tool options, bytes verified, CPU, throughput, and latency distribution.

Compare sequential buffered first-read, buffered reread, direct sequential read, ordinary buffered write, and buffered write followed by synchronization. Keep file size/block size/concurrency comparable. A suggested starting block size is 128 KiB and QD 1; adjust direct alignment to the discovered contract. Measure sync time separately and end-to-end. Do not call a first run 'cold' without evidence; host/guest/device caches can remain warm. File-scoped advisory eviction is best effort, not proof of cache absence.

Use fio's reported achieved depth and latency fields, not only requested settings. A synchronous I/O engine does not magically produce QD 32 because `iodepth=32` is specified. Preserve the full job configuration and raw output. [fio manual](https://fio.readthedocs.io/en/latest/fio_doc.html)

### Design drill — multi-tenant distributed block storage

**Question/requirements:** 64 training hosts, each demanding 2 GiB/s checkpoint writes; 1 PiB usable capacity; three replicas in separate racks; one active read-write attachment per volume. Define durability and consistency before promising an SLO.

**Required calculations:** logical burst 128 GiB/s; eventual three-copy media traffic at least 384 GiB/s before overhead; raw capacity at 70% utilization `3 / 0.7 = 4.286 PiB`. For 4 MiB allocation units and 32-byte map entries, the 1 PiB map is 8 GiB before indexes/replication. Explain cache scope and network topology; quorum acknowledgement does not remove eventual third-copy traffic.

**Steady-state/internals:** draw API/auth, attachment authority, placement metadata, endpoint, replication groups, and devices. Define the write request fields, ordering/commit point, how readers avoid stale replicas, and where stale ownership is rejected. Discuss per-tenant IOPS, bandwidth, queue depth, and background rebuild budgets.

**Invariants/failure matrix:** acknowledged durable writes survive the agreed failure; stale writers cannot commit; repair cannot replace a newer version. Walk primary failure, lost reply, rack loss, metadata quorum loss, slow replica, and tenant overload. Separate ownership epoch from placement-map version.

**Observability/change plan:** require tenant latency, commit wait, degraded bytes, stale-epoch rejections, and rebuild competition. Canary one storage class/cohort; preserve metadata/protocol compatibility; identify irreversible data-format changes before rollout. Rollback must retain access to existing data.

**Leadership decision:** replication first versus EC, with capacity cost and a trigger for introducing EC. **Challenge:** two replicas contain bytes—what makes them committed, and how will a new leader know?

Canonical reading: [block-store design](../system_design/apple_l7_system_design_prep.md#21-design-elastic-disk--ebs-class-block-storage). This is a storage-focused drill using that pattern, not an assertion about NVIDIA's internal architecture.

**Day gate:** after the taught examples, use ten minutes for a guided explanation of all four paths; point to one real source-code wait/completion; show exact harness tests and interpret a measurement with notes available. Record the assistance used.

## Day 2 — protocols and asynchronous ownership

### Domain exercise

Draw CPU memory, NVMe SQ/CQ, controller, and payload pages. Explain producers/consumers, doorbells, command identifiers, queue wrap/phase, PRP/SGL, DMA direction, IOMMU, and interrupt/polling costs. Then draw NVMe/TCP and NVMe/RDMA, naming what changes at the transport and what remains an NVMe command.

Explain RDMA QP, CQ, memory registration, local/remote keys, SEND versus READ/WRITE, and the specific meaning of a completion. Identify work hidden by claims such as 'zero copy' and 'zero target CPU'. Compare kernel NVMe with an SPDK-style polled userspace path as an architectural discussion; do not claim every path uses blk-mq.

Distinguish path selection (including ANA), frontend availability, attachment ownership, and backend commit authority. See [NVMe multipath](https://docs.kernel.org/admin-guide/nvme-multipath.html) and [DMA mapping](https://docs.kernel.org/core-api/dma-api-howto.html).

### Coding exercise — bounded executor plus request lifetime

**Pattern / Idea:** bounded producer-consumer admission plus a synchronized lifecycle. **Tags:** `bounded-blocking-queue`, `condition-variable`, `backpressure`, `cancellation`, `state-machine`, `min-heap`. References: [bounded queue](../coding/cpp/concurrency/solutions.md#1-bounded-blocking-queue), [thread pool](../coding/cpp/concurrency/solutions.md#2-thread-pool), [delayed scheduler](../coding/cpp/systems_style/solutions.md#1-delayed-job-scheduler).

**Specification:** `submit(id, generation, owned_buffer, deadline) -> Accepted|Full|Closed`, `cancel(handle)`, `backend_complete(handle, result)`, `advance_time(t)`, and `shutdown`. Start with fixed workers, finite outstanding capacity, a fake monotonic clock, and a backend that can withhold/reorder/duplicate completions.

Practical budget: 20-minute warm-up, about 45 minutes adapting the existing executor, two hours for the lifecycle core, one hour for deterministic tests/sanitizers, and 60–85 minutes for the already-prepared transport lab. This uses Day 2's extra coding hour. Initial Linux/network provisioning must be completed in P0; otherwise record that dependency and carry the unfinished gate rather than omitting tests.

1. Write the state table first. Track **user-visible outcome** separately from **backend no-longer-accesses-buffer**. A timeout can finish the caller's wait without making reclamation safe.
2. Transfer task ownership only on successful admission; execute tasks and callbacks outside the queue mutex.
3. Serialize competing terminal outcomes, reject old generations, and keep the backend's buffer reference until completion or a proven quiescence acknowledgement. Retain the outstanding-capacity slot until that point, or impose a separate finite retained-buffer/request budget. A logical timeout must not reopen unlimited admission while old buffers remain held.
4. Use a bounded deadline structure; if a heap contains stale entries, define when they are removed and bound their growth under long-running churn.
5. Close admission before draining. Wake sleepers, join workers, and define behavior when the backend never acknowledges cancellation. Return an explicit incomplete teardown state or retain ownership; do not free active buffers to meet a timeout.
6. Catch task exceptions at worker boundaries. Record admission time, queue wait, backend time, callback time, and release time.

| Adversarial test | Expected result |
| --- | --- |
| Capacity 1, concurrent producers | Queue never exceeds 1; rejected work stays with caller. |
| Complete before timeout / timeout before complete | Exactly one user-visible terminal result; late backend completion still releases its hold safely. |
| Cancellation races completion | One outcome; no use-after-free or double release. |
| ID 7 generation 1 completes after ID 7 generation 2 is submitted | Generation 2 unaffected. |
| Callback resubmits another request | No callback-under-lock deadlock. |
| Shutdown races submission | Each task is either rejected or fully accounted for; no orphaned accepted work. |
| Backend never finishes | Explicit non-quiescent state; bounded new admission; no false successful shutdown. |
| Repeated submit then timeout; backend never becomes quiescent | Retained requests/buffers reach the configured bound; further admission rejects instead of growing memory. |
| Repeated completed requests with long deadlines | Timer bookkeeping remains bounded. |

**Complexity:** queue operations `O(1)` under lock; hash lookup average `O(1)`; heap insertion/removal `O(log N)`; memory `O(capacity + workers + outstanding + bounded timers)`. Explain hash worst cases and scheduling contention. Optimize batching/sharding only after the simple model passes. Ordinary mutexes do not establish a starvation-freedom proof.

### Experiment — real NVMe/TCP plus deterministic races

Using the selected lab's documented nvmetcli/nvme-cli workflow, configure one target namespace backed by a newly created disposable file or virtual disk, and one separate initiator. Record exact namespace, subsystem NQN, endpoint, host NQN, backing object, and discovered client device before writing. Keep all endpoints private to the lab. [Upstream nvmetcli](https://github.com/linux-nvme/nvmetcli)

Write a known pattern, read/verify it, capture discovery/controller/path state, and interrupt only the test connection. Record submitted operations, completions/errors, reconnect duration, and post-reconnect verification. Record reconnect/controller-loss policies because they change what the caller observes. A timeout has an unknown write outcome unless recovery establishes otherwise.

This single-target experiment validates transport behavior; it does not demonstrate durable multi-target failover or fencing. Run the coding race table separately with an event log and fake clock. If real transport setup is unavailable, `D2-LAB` remains open.

### Design drill — failover without split-brain writers

**Question/requirements:** two frontends, three backend replicas, preserve acknowledged durable writes after one failure, and target a defined failover case below five seconds. State the failure model and what happens without quorum.

**Steady-state/internals:** distinguish frontend paths, backend leader, and attachment owner. Define the epoch protocol and exact enforcement at storage mutation/commit. Explain how the chosen frontend conveys authority; standard NVMe commands do not automatically carry an application-specific epoch or reconnect-stable deduplication ID.

**Required timeline:** detection 1.0s + fencing 1.5s + ownership commit 0.1s + recovery 0.4s + reconnect 0.8s = 3.8s nominal sequential budget. Name the assumptions; this is not a worst-case guarantee. Draw a second timeline where fencing fails and writes remain unavailable.

**Invariants/failure matrix:** only current authority commits; acknowledged state survives recovery; old in-flight operations cannot overwrite newer state. Walk asymmetric partition, stale frontend restart, lagging survivor, lost response, path-only failure, and quorum loss. Explain how previously admitted operations and delayed DMA/RDMA access are drained or rejected, not just future authorization revoked.

**Observability/change plan:** log owner/epoch, fence proof, commit/recovery position, stale-operation rejection, path state, and phase durations. Qualify scripted partitions before enabling automation. During rollback/failback the former owner rejoins stale, catches up, and undergoes a new controlled ownership transition.

**Leadership decision:** accept a write outage when authority cannot be proven. **Challenges:** why is ANA insufficient; what if the old process pauses and resumes; how does a retried old write interact with a newer overlapping write?

**Day gate:** show the exact line/state transition preventing double callback and the exact mechanism preventing stale writes. Explain why a software cancellation result does not prove a device stopped DMA.

## Day 3 — kernel C lifecycle and latency

### Domain exercise

Make a context/primitive table for process context, interrupt handling, deferred work, and polling. For each state whether your particular path may sleep or allocate, which lock protects shared state, and who owns the object. Discuss kernel-version/PREEMPT_RT differences instead of assuming every spinlock behaves identically everywhere.

Explain mutex/spinlock/atomics/RCU/kref tradeoffs; why `volatile` is not synchronization; CPU versus device ordering; `kmalloc` versus `vmalloc` versus DMA allocation; GFP constraints and reclaim recursion; page faults/pinning; scheduler run queues; softirq/workqueue load; NUMA placement and false sharing. RCU read-side access does not automatically serialize writers or replace all lifetime references.

Read one allocation, one wait, and one completion site in the selected source map. Draw how memory pressure can delay I/O before device issue, and how completion can precede the application's next timeslice.

### Coding exercise — actual kernel C lifecycle

**Pattern / Idea:** port Day 2's bounded ownership state into kernel C with real kernel primitives. **Tags:** `state-machine`, `cancellation`, `lock-ordering`. This is a test-only helper, not a new storage driver. Its backend is mock work; do not add physical-device DMA.

**Specification:** a request has a generation, terminal-result state, `kref`, `work_struct` or `delayed_work`, a short lock-protected transition, and a completion/wait primitive. Implement creation, queueing, logical finish, cancellation, and teardown, plus a KUnit suite and Kconfig/Makefile wiring.

1. Write a reference ledger: creator hold, successfully queued work hold, test/caller hold, final release. State which path drops each reference exactly once.
2. Serialize queueing with shutdown. Allocate outside the critical section where possible and select flags from execution context. Handle allocation failure before publication.
3. Publish a terminal outcome once; perform notification outside locks if it can re-enter or sleep. Keep result storage alive until its consumer finishes.
4. Never wait for/cancel work while holding a lock required by that work. Decide who releases the queued-work reference when cancellation prevents execution.
5. Integrate tests and run the built kernel through the selected KUnit/UML/QEMU route. Save source commit, diff, config, build command, and KTAP output.
6. Exercise lockdep and at least one applicable memory-safety configuration. Add KCSAN where the selected execution mode supports it; record omissions and follow-up. Keep instrumented correctness results separate from performance measurements.

| Kernel test | Required observation |
| --- | --- |
| Work completes normally | One terminal result and one final object release. |
| Cancellation before work starts | Work cannot later use freed state; work reference accounted for. |
| Cancellation while callback executes | Proper synchronization; no wait-under-needed-lock deadlock. |
| Two paths attempt logical finish | One wins; loser does not duplicate notification or release. |
| Shutdown concurrently with enqueue | No new work escapes teardown accounting. |
| Allocation fails before publication | Clean error return and no leaked partial object. |
| Stress with delayed callbacks | No warnings in exercised paths; deterministic tests still define the expected outcomes. |

**Gate:** compiling alone is insufficient. Show the modified code executing inside the test kernel, then explain context, lock order, lifetime, and liveness. Complexity should remain `O(1)` per local transition and `O(N)` retained request state. Real-kernel testing may need six hours total including the scheduled later follow-up.

References: [KUnit](https://docs.kernel.org/dev-tools/kunit/start.html), [kref](https://docs.kernel.org/core-api/kref.html), [workqueues](https://docs.kernel.org/core-api/workqueue.html), [KASAN](https://docs.kernel.org/dev-tools/kasan.html), [KCSAN](https://docs.kernel.org/dev-tools/kcsan.html).

### Experiment — correlate real block events

Run a bounded fio workload on the disposable file/device. Enumerate tracepoints available in the selected kernel before choosing the command. Capture request issue/completion plus application timestamps; add insert/merge/split, scheduler, reclaim, and writeback events only when available and needed.

Correlate by actual request identity/lifetime and device/range, accounting for tag reuse, merging, and splitting. Do not assume one application operation equals one block request. A direct-dispatch path may have no scheduler-insert interval. Explain what the trace cannot observe, such as SSD firmware scheduling. A synthetic null_blk run is a useful comparison, not a substitute for real media.

### Design drill — P99 under memory pressure and high queue depth

**Question/requirements:** application P99 rises from 1ms to 20ms at QD 256 under memory pressure; device issue-to-completion looks unchanged. Preserve throughput and correctness while locating the missing time.

**Steady-state/internals:** define timestamps for admission, syscall, mapping/pinning, request dispatch, device completion, filesystem completion, and task running. Compare hypotheses: reclaim, dirty/writeback interference, tags, locks, IRQ CPU saturation, and scheduler delay.

**Experiment design:** low/high scoped memory pressure × QD 16/256. Record dataset/cache state, operation mix, offered load, CPU placement, swap policy, and achieved depth. Use warm-up and three repeated windows per cell. First run only enough to validate instrumentation; perform the larger sweep on Day 6.

**Required calculation:** at 400,000 completed IOPS and mean latency 250 microseconds, average in-flight work is `lambda × W = 100`. Do not substitute P99 for the mean or sum stage P99s into an end-to-end percentile.

**Invariants/failure matrix:** bounded admission, no premature buffer release, no hidden tenant starvation. Walk reclaim pressure, writeback burst, tag exhaustion, slow device, overloaded completion CPU, and measurement blind spots. Name one result that would falsify each leading hypothesis.

**Observability/change plan:** application queue time, throughput/errors, PSI, faults/reclaim, block wait, device service, completion-to-run delay. PSI reports stall time, not percent memory occupied. Canary one admission/placement change; roll back if latency gains violate throughput or fairness goals. [PSI documentation](https://docs.kernel.org/accounting/psi.html)

**Leadership decision:** memory/CPU headroom versus higher density with a weaker latency promise. **Challenge:** a QD cap halves throughput while improving latency—what claim can you honestly make?

**Day gate:** identify a missing interval, show real-kernel evidence, and defend a falsifiable investigation rather than listing tuning knobs.

## Day 4 — filesystem durability and checkpoints

### Domain exercise

Compare ext4 and XFS at the level of inode/extent metadata, allocation, logging/journaling, writeback, and recovery; identify which details depend on mount mode/version. Explain sparse/unwritten ranges, rename atomicity, file versus directory synchronization, and why multiple hosts cannot simply mount the same ordinary filesystem read-write without a coherence protocol.

Draw the promises of ordinary write, synchronous file I/O, direct I/O, file synchronization, preflush, and FUA. Explain overlapping buffered/direct I/O and why `io_uring` describes an interface rather than an automatic durability or cache-bypass guarantee.

### Coding exercise — durable checkpoint publication

**Pattern / Idea:** immutable data plus explicit publication and recovery. **Tags:** `immutable-snapshot`, `atomic-publish`, `idempotency`. Warm-up/reference: [Config Snapshot Manager](../coding/cpp/systems_style/solutions.md#4-config-snapshot-manager); explain why atomic in-memory pointer publication does not make a filesystem transaction durable.

**Specification:** one coordinator, one local filesystem, four 1 MiB synthetic shards per generation. Implement `publish(generation, shards)` and `recover_latest()`. Use Day 1's short-I/O-safe file helpers, streaming checksums, immutable names, and a manifest with expected count, lengths, and hashes. Reject a duplicate generation containing different bytes.

1. Create/validate a persistent checkpoint root. Handle persistence of newly created directories explicitly.
2. Write shards under unique names, synchronize contents, and persist their directory entries before a durable manifest can reference them.
3. Persist the manifest contents and pathname. Write the new generation ID to `latest.tmp`, synchronize that temporary file's contents, rename it over `latest` on the same filesystem, then synchronize the containing directory before reporting durable success. Directory synchronization does not replace synchronization of the pointer file's contents.
4. Keep the prior verified generation. On recovery, validate the pointer, manifest, expected shard set, lengths, and hashes; report corruption instead of accepting incomplete state.
5. Inject failure after each protocol step. Separate a process being killed from a kernel/VM crash and from physical power loss.
6. Surface failed file sync, directory sync, rename, short write, and ENOSPC. An error after publication can leave an uncertain outcome; recovery must determine what is visible and valid.

| Cut / corruption | Allowed outcome |
| --- | --- |
| Before or during a shard write | Prior committed generation remains usable; partial new data is not advertised. |
| All shard files written, synchronization incomplete | No durable-success claim for the new checkpoint. |
| Shards durable, manifest not published | Old generation is discoverable; new orphaned data can be handled later. |
| Manifest durable, latest update not complete | Old or new pointer depending on cut; any selected generation must validate. |
| Writing or synchronizing `latest.tmp` fails | No rename/publication success; preserve the prior pointer and surface the error. |
| Rename completed, directory sync pending | Process-crash test may show new state; this does not prove crash durability. |
| Directory sync complete, response lost | New valid generation may exist despite caller uncertainty; retry must reconcile. |
| Flip one byte / truncate one shard / remove one referenced shard | Verification fails explicitly; retain access to a verified older generation per policy. |
| Inject synchronization error | Publish does not report durable success. |

**Invariants:** published durable success names a complete valid generation; old committed data is preserved until retention permits removal; publication is idempotent under the defined single-coordinator contract. **Complexity:** `O(B)` streaming write/checksum and `O(S)` manifest entries; memory `O(buffer + S)`. Recovery with full validation reads `O(B)`; describe faster startup's weaker immediate integrity coverage. Keep GC disabled until reachability/reader-lifetime rules are defined.

File synchronization and directory-entry persistence are distinct obligations. [fsync manual](https://man7.org/linux/man-pages/man2/fsync.2.html)

### Experiment — crash cuts and recovery

Run every injection point repeatedly with a fixed seed/log. Restart the publisher/recovery process and verify which generation is selected. Save the directory listing, manifest IDs, hashes, error result, and allowed versus observed outcome.

If using abrupt VM-stop testing, state the virtual disk cache mode and host persistence assumptions; a guest crash may leave host caches alive. No physical power-loss qualification is implied. A negative process-crash test can expose a publication bug; a successful one cannot establish honest device flush behavior.

### Design drill — high-throughput checkpoint filesystem

**Question/requirements:** 256 ranks each write 4 GiB every ten minutes; publish the whole checkpoint within 30 seconds. Support parallel restore, tenants, and retention. Define how ranks capture one logically consistent training step before storage begins.

**Calculations:** 1 TiB/checkpoint; required burst at least 34.13 GiB/s versus average 1.707 GiB/s; two durable copies require at least 68.27 GiB/s media writes during the burst, before overhead. At 128 bytes/shard, the 256-entry manifest is 32 KiB before encoding. Explain metadata-rate and straggler limits separately from bytes/sec.

**Steady-state/internals:** metadata service, immutable chunk/shard store, coordinator, manifest publisher, restore scheduler, and GC. Define PREPARING/COMMITTED/RETIRED, conditional publication, coordinator generation, checksum validation, placement, and restore admission. State the actual POSIX semantics offered.

**Invariants/failure matrix:** complete generation visibility; immutable referenced data; no collection of data reachable by retained checkpoints or active readers. Walk rank loss, coordinator restart, publication reply loss, two coordinators, shard corruption, GC/restore race, and synchronized restore storms.

**Observability/change plan:** critical-path checkpoint time, slowest-rank contribution, metadata P99, throughput, checksum failures, restore stalls, and GC backlog. Deploy compatible readers before new manifest writers. Roll back by publishing a retained verified generation with the supported protocol; preserve formats and lineage.

**Leadership decision:** checkpoint-specific immutable semantics versus full distributed POSIX coherence. **Challenge:** every worker said 'done'—what persistence and logical-training-state guarantees did that mean?

Canonical reading: [checkpointing and recovery](../system_design/nvidia_l7_system_design_prep.md#13-design-checkpointing-and-recovery-for-distributed-training).

**Day gate:** reconstruct the durable order, demonstrate all process-crash cuts, and explain the remaining power-loss qualification gap.

## Day 5 — replication, fencing, and corruption

### Domain exercise

Define failure detection versus authority, ordered log versus replicated bytes, quorum intersection versus consensus recovery, committed versus uncertain writes, and application request identity versus transport command ID. Explain why retries of older overlapping writes can corrupt newer state without ordering/deduplication rules.

Discuss replication versus EC under small writes, degraded reads, rebuild traffic, and failure domains. Draw a membership change and identify why the exercise below intentionally does not implement it. Explain checksums as integrity evidence, not proof of freshness or committed history.

### Coding exercise — fenced replica model

**Pattern / Idea:** deterministic state-machine simulation of ownership, persistence, commit, and repair. **Tags:** `quorum`, `versioning`, `term`, `idempotency`, `deduplication`, `state-machine`. References: [quorum simulator](../coding/cpp/distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator), [rendezvous hashing](../coding/cpp/distributed_systems_algorithms/solutions.md#2-rendezvous-hashing). Warm-up: choose replica hosts while respecting rack diversity; report infeasibility rather than silently violating policy.

**Bounded specification:** three replicas, static membership, one logical block, one serialized log, externally supplied ownership epochs, and a deterministic event scheduler. The trusted authority supplies a verified recovered committed prefix when installing a new owner. This explicit oracle substitutes for a consensus election/recovery implementation and must be named in every conclusion.

Implement `receive_write(epoch, sequence, request_id, value)`, `persist(replica, entry)`, `commit(entry)`, `install_epoch(epoch, recovered_prefix)`, and `repair(replica, expected_version, value)`.

1. Separate received, persisted, and committed states; keep old log records separate from the current block value. Define the exact acknowledgement contract.
2. Install new authority through the model's ordered fence/recovery barrier. Validate epoch at mutation/commit, not only when a request enters a frontend.
3. Track `(epoch, sequence)` ordering and request identity. A delayed old operation must not revert a newer committed value.
4. Use conditional repair against the version observed when copying began. Reject stale repair and retry from the current committed source.
5. Add delayed, dropped, duplicated, and reordered events. Use a fixed event limit and seed; record complete failing schedules.
6. Describe retained log/dedupe state and safe reclamation assumptions. Bounded retention needs a stated retry/recovery horizon; deleting history arbitrarily is not safe.

| Deterministic schedule | Expected observation |
| --- | --- |
| Epoch 7 old writer paused; epoch 8 installed; old write resumes | Old operation cannot commit under the new authority. |
| Old write received before fencing, persistence callback delayed until after epoch 8 | No overwrite of the newer committed state; barrier/validation handles in-flight work. |
| Commit succeeds but reply is lost; retry same ID | Same logical outcome, without replaying old content over a later value. |
| Repair reads version 10; write 11 commits; repair attempts version 10 | Conditional repair fails or restarts; version 11 survives. |
| Minority isolated | The minority cannot commit. A current/fenced owner on the connected majority can commit when recovery and persistence preconditions hold; run both histories. |
| New owner requested without recovered-prefix/fence proof | Reject promotion; do not invent safe state. |
| Same request ID with different payload | Explicit protocol error. |

**Invariants:** authority monotonicity, committed-order preservation, no stale repair, no duplicate logical effect under the specified retry contract. **Complexity:** write messaging `O(R)`; scheduled events `O(log M)` with a heap; memory includes `O(R × retained history + M + dedupe records)`. Discuss batching for runtime and snapshots/log truncation for memory, without implementing an unsafe approximation.

The existing quorum exercise is a conceptual reference. `R + W > N` alone does not specify leader selection, ordering, durable commit recovery, or fencing.

### Experiment — model counterexamples

Save one successful trace per row, then intentionally disable the fence or repair version check in a local test variant and show the corresponding test fails. Restore the correct variant and rerun. This demonstrates the test targets a real property rather than echoing implementation details. Label results as model evidence; real-device cancellation/fencing remains a separate integration claim.

### Design drill — corruption after a power event

**Question/requirements:** 16 ranks wrote 512 MiB each; after a power event the filesystem mounts but the newest 8 GiB checkpoint is unusable. Establish the promised acknowledgement and failure scope before selecting a repair.

**Steady-state/internals:** application generation commit → file/directory synchronization → filesystem data/metadata order → block flush/FUA → transport/target cache → media. Preserve original logs and data images before repair. Reconstruct eight cuts spanning shard writes, file sync, directory persistence, manifest creation/publication, and lost acknowledgement.

**Invariants/failure matrix:** no partial generation accepted, synchronization errors surfaced, repair uses verified committed data, failure domains match the contract. Compare unsynchronized application data, missing directory persistence, flush failure, dropped flush, torn writes, lying firmware, wrong replica version, and common power-domain loss.

**Observability/change plan:** application IDs/commit records, sync errors, cache settings, flush/FUA evidence, device logs, checksums, replica versions, scrub outcomes. Reproduce with bounded fault injection, qualify the fix with the crash matrix, and retain compatible readers and verified prior checkpoints. Software rollback does not repair corrupted bytes.

**Calculation:** a separate full validation read of 1 TiB at 4 GiB/s adds 256 seconds; compare streaming checksums, sampled validation, and full recovery validation while stating coverage limits.

**Leadership decision:** explicit strict-durability tier, qualifying hardware/configuration, and acceptable commit latency. **Challenges:** journal recovery succeeded—what did it prove; does FUA on one record persist all prior writes; does checksum validity establish freshness?

Use [block write-cache controls](https://docs.kernel.org/block/writeback_cache_control.html): preflush concerns prior completed writes within scope; FUA concerns the associated write. Correct ordering and propagation must survive the entire stack.

**Day gate:** present an unsafe schedule and its corrected outcome; identify the precise assumption your model depends on; explain a complete corruption investigation without guessing which layer failed.

## Day 6 — performance, fairness, and DPU judgment

### Domain exercise

Explain IOPS versus bandwidth; service time versus queueing; mean versus percentiles; steady-state Little's Law; open-loop arrivals versus closed-loop concurrency; coordinated omission; IRQ/CPU affinity; memory bandwidth; PCIe/NUMA topology; and how rebuild traffic affects foreground latency.

Draw host, DPU, RNIC, GPU memory, and storage paths. Identify where buffers, copies, queueing, authorization, fencing, and recovery reside. Explain how to prove the actual GDS path and recognize fallback. Successful API execution alone does not establish direct DMA. [GDS overview](https://docs.nvidia.com/gpudirect-storage/overview-guide/index.html)

### Coding exercise — admission and measurement extension

**Pattern / Idea:** bounded per-tenant admission with explicit budgets and fair selection. **Tags:** `token-bucket`, `rate-limiter`, `backpressure`, `admission-control`, `rolling-window`. References: [Concurrent Token Bucket](../coding/cpp/concurrency/solutions.md#3-concurrent-token-bucket), [Rolling Metrics Window](../coding/cpp/advanced_data_structures/solutions.md#6-rolling-metrics-window).

**Specification:** reuse the executor with tenants A/B, separate IOPS and byte-rate buckets, bounded tenant and global outstanding limits, a fake monotonic clock, and fair eligible-tenant selection. Use fixed bounded histogram storage for queue, service, and end-to-end latency. Report rejected/offered/completed work separately.

1. Define where a token is charged and whether cancellation refunds it. State initial burst tokens, rate, and maximum burst in units.
2. Avoid head-of-line blocking by an ineligible tenant. Do not promise proportional fairness from token buckets alone; define the scheduler.
3. Record ingress before admission so rejection and queueing cannot disappear from the report.
4. Keep telemetry memory bounded and sampling overhead measured. Merge compatible histogram counts; do not average percentiles.
5. Apply Day 2's shutdown/lifetime tests with admission enabled.

| Test | Expected result |
| --- | --- |
| Bucket starts with 100 operations, refills 100/s; all operations available in first second | No more than 200 operations charged by t=1s, subject to boundary convention. |
| No time advancement | No refill. |
| Clock input decreases | Reject or clamp according to documented contract; no token creation. |
| Request exceeds maximum byte burst | Explicit rejection or documented special policy; no permanent queue blockage. |
| Tenant A floods, tenant B submits below its provisioned share | B makes progress under the specified healthy-backend/scheduler assumptions; measure its tail latency. |
| Backend saturated, clients continue offering work | Admission stays bounded; reject/throttle is visible; no unbounded memory growth. |

**Complexity:** bucket check `O(1)`; scheduling cost must match chosen active-tenant structure; memory `O(tenants + bounded queues + histogram bins + outstanding requests)`. Discuss per-core sharding versus global precision, batching versus latency, and fixed-bin accuracy versus memory.

### Experiment — controlled saturation and interference

Complete the Day 3 2×2 experiment, then a limited QD sweep `{1, 8, 32, 128}` under baseline memory conditions. Use a supported asynchronous engine for depth greater than one. Choose one request size/mix first; vary request size only after that baseline. Use 20s warm-up and 60s measured windows as initial settings, three repetitions; lengthen if there are too few tail observations.

Run A alone, B alone, A+B without admission, and A+B with admission. Keep the backend and placement fixed. Record offered/completed IOPS, bytes/sec, errors/rejections, P50/P99/P99.9 with sample counts, actual depth, CPU, and PSI. Compare an admission cap at equal offered load as well as achieved throughput. Low latency obtained by rejecting most work must be described accurately.

Use cgroup-scoped or VM-scoped memory pressure with a chosen cap and stop condition; record achieved PSI rather than calling any allocator loop 'high pressure'. Preserve raw results and note virtualization/caching limitations. Reserve remaining experiment time for Day 3 kernel instrumentation gaps; unresolved gates remain open.

### Design drill — decide whether to offload to a DPU

**Question/requirements:** at the same 8 GiB/s workload, host CPU falls from 18 to 8 cores with a DPU prototype, but P99 rises from 500 to 550 microseconds. Evaluate 500 nodes; the contract permits at most 5% P99 regression.

**Calculations:** 55.6% host-core reduction, 5,000 cores across the fleet, and 10% P99 regression, which fails the stated gate. Assume incremental capital $2,000/node over three years, incremental power 100W/node, electricity $0.12/kWh, and $150,000/year operations cost. Annual capital is about $333,333; energy $52,560; total $535,893 before cooling/other omissions. That requires about $107.18 of usable annual value per freed core just to cover the modeled cost. Idle saved cores have no automatic business value.

**Steady-state/internals:** draw both paths and state exactly which functions move. Account for DPU CPU/memory, PCIe traversal, copies, queueing, firmware/software versions, policy distribution, and backend limits. Measure sustainable throughput under the latency SLO, not unconstrained peak throughput.

**Invariants/failure matrix:** offload cannot bypass access control or fencing; resets cannot create stale ownership; compatible versions must be enforced. Walk DPU reset, resource saturation, observability loss, host/DPU mismatch, alternate-path loss, and bad policy distribution.

**Observability/change plan:** host/DPU utilization, per-stage latency, resets/reconnects, useful training throughput, power, and operator effort. Prove fallback feasibility before including it in the rollback plan; switching paths may require fencing and reconnect, not a live toggle. Canary representative hardware/software cohorts and reject the sample prototype's latency gate violation.

**Leadership decision:** retain host path, narrow offload, or invest in correcting the prototype; identify what measured gain would justify rollout. **Challenges:** where did CPU work move; why are freed cores useful; what proves a bounce copy disappeared?

Public architecture context: [DOCA SNAP service guide](https://docs.nvidia.com/doca/sdk/doca-snap-4-service-guide/).

**Day gate:** after reviewing the worked calculation, defend a conclusion with actual measurements and limitations, calculate the tradeoff with notes available, and identify a falsifying experiment. Record the assistance used.

## Day 7 — independent assessment

This heading retains its existing anchor; **Day 7 is guided consolidation by default**. Independent assessment is conditional on an explicit user opt-in after teaching. On September 22, use guided rehearsal A and the exact remaining-block allocation in [screening rehearsals](#ninety-minute-screening-rehearsals). The sections below describe the underlying work, including work already inside the rehearsal; they are not additional sessions.

### Timed coding: 150 minutes

“Timed” denotes the retained learning allocation, not an independent speed test.

1. **45 minutes:** review the bounded-queue model and close/drain invariants line by line, then implement with guidance and run boundary/shutdown tests; prior code is available.
2. **60 minutes:** revisit the request-lifecycle model, then implement together with timeout, cancellation, late completion, generation reuse, and explicit backend ownership. Use a simple deterministic backend; do not spend the slot recreating the whole harness.
3. **45 minutes:** inspect failures together, teach the violated invariants/complexity, repair the weakest issue, and compare against the earlier implementation throughout as useful. Ask one comprehension check after each worked correction.

Coding evidence-ready requires a correct executable core, meaningful failure tests, and a correct lifetime explanation. Faster incorrect code does not pass. Label guided implementations and explanations accurately; independent coding readiness remains unassessed unless separately demonstrated. If a feature is unfinished, record it precisely rather than judging the whole exercise by confidence alone.

### Practical replay: 120 minutes

Review the saved instructions and expected results with the coach, then reproduce a file-I/O trace, the kernel C test suite, one checkpoint crash/recovery case, and one stale-writer rejection. Check that environment and version details are sufficient for a clean repeat. Explain how instrumentation changes timing and why the model/VM/hardware evidence differs. Leave a failing or unavailable replay open.

### Two design mocks: 120 minutes

Use 45 minutes for guided distributed block storage or fenced failover, 45 minutes for guided checkpoint filesystem or DPU choice, then 30 minutes combined critique. Review model answers before supported practice; notes and solutions stay available. Apply all eight sections and the 14/16 content-quality, no-zero, no-correctness-error rule, recording assistance separately. Teach the effect of one changed assumption in each discussion, then revise the design together.

### Incident simulation: 90 minutes

**Question/requirements:** 512 eight-GPU nodes; 64 upgraded; 16 degraded. Affected nodes' GPU utilization falls from 90% to 60%, and checkpoints slow down. No corruption has been observed yet. Teach the incident response using the worked example below and guide the user's practice inside the 45-minute simulation, then use 30 minutes of guided challenges and 15 minutes for the decision note.

**Calculations:** 25% of upgraded nodes affected, 3.125% of the fleet; 30 percentage-point utilization drop, a 33.3% relative reduction. Across 128 affected GPUs, the difference is 38.4 GPU-equivalents of utilization. This is not proof of equal lost training throughput; measure job progress.

**Steady-state/internals:** draw training → checkpoint library/GDS or normal I/O → filesystem/memory → block/transport → backend. Compare affected upgraded, healthy upgraded, and healthy old cohorts. Record kernel, filesystem, GPU/GDS, NIC/DPU firmware, hardware SKU, topology, and workload mix.

**Invariants/failure matrix:** protect data integrity; preserve workload state during drain; verify rollback compatibility. Walk performance-only regression, new corruption signal, only one SKU affected, remote-only degradation, memory-pressure interaction, GDS fallback, and incompatible rollback. Correlation with a version is a hypothesis, not root cause.

**Observability/change plan:** freeze rollout expansion; identify a matching rollback canary and diagnostic cohort; measure checkpoint duration, job progress, P99, errors, integrity checks, resource pressure, transport behavior, and actual GDS path. Pick one A/B or A/B/A experiment. Define restoration and reintroduction gates, and stop retaining a diagnostic cohort if its risk becomes unacceptable.

**Leadership decision:** rollback scope and pace, permitted diagnostic exposure, and required evidence before re-release. Deliver a two-minute spoken update using: impact → known evidence → containment → next discriminating test → next update owner/time.

**Challenges:** why are 48 upgraded nodes healthy; healthy backend graphs but poor jobs; rollback helps latency but breaks driver compatibility; when does service restoration take priority over root-cause work?

### Closed-book oral and final review: 50 minutes

This retained heading does not require closed-book work. Spend 30 minutes on taught, guided oral review (20 inside rehearsal A and 10 afterward), then 20 minutes writing the gap ledger and confirmations. Revisit the model answer before each comprehension check and ask only one at a time, with notes and hints available. The other ten minutes from the original hour fund rehearsal A's introduction and candidate questions:

1. Where can a page-cache hit still block?
2. Why can a page-cache miss produce no SSD data read?
3. Why are Dirty and Writeback distinct?
4. Why is pinning not the same as DMA mapping?
5. When can a request bypass software staging queues?
6. What may remain after an NVMe command completes?
7. Why is timeout not proof of backend quiescence?
8. Why can a storage-path allocation deadlock during reclaim?
9. What does a file sync leave unresolved about a directory entry?
10. What do flush and FUA each promise?
11. Why is ANA not a writer-ownership protocol?
12. How can stale repair corrupt a current replica?
13. What additional rules are missing from quorum intersection alone?
14. What is the difference between queue wait and device service time?
15. Why can DPU offload save CPU and still be a poor decision?
16. What is known versus inferred when a release affects only one cohort?

Content target: at least 14 complete answers, no unresolved unsafe claim about durability, fencing, memory lifetime, or locking, and a successful later revisit of corrected answers. Record the assistance used for each answer; model answers alone do not count as the user's demonstrated understanding. Repeat at least two topics taught/practiced 24 hours earlier, with a recap and one guided check at a time. If the last corrections were made today, delayed-revisit confirmation stays pending until a later revisit. Guided success establishes supported understanding; unaided recall remains unassessed unless the user opts in and demonstrates it.

## Deadline qualification and review sessions

Use the existing exercises and evidence; do not add modules or unrelated interview topics. Dates are in 2026, America/Los_Angeles time. Teaching mode continues through qualification and review: explain or recap, show the model, practice together, then ask one supported check. The tracker remains the only checkbox list. These sessions are planned work, not scheduled automation or authorization to mutate a lab.

### September 23 — gap closure: 8 hours

1. **30 minutes:** triage every open required item together. Rank unsafe correctness/lifetime/fencing/durability issues first, then missing actual kernel/block/NVMe-TCP evidence, then reproducible coding, then incomplete explanations. Assign a concrete reproduction and expected result to each selected gap; keep independent-readiness evidence separate.
2. **180 minutes:** teach the cause of the weakest coding defects, guide repairs, and run focused regression tests. Explain why the earlier version failed and which invariant now holds. Keep raw failing and passing results.
3. **120 minutes:** review the procedure and expected observations, then finish or reproduce missing practical evidence using the recorded disposable environments. Do not substitute a simulator for required kernel or transport execution. A blocked lab remains a schedule risk.
4. **90 minutes:** reteach weak domain topics with worked examples and model answers, then practice explanations with notes and one comprehension check at a time. Save correction timestamps and schedule guided revisits at least 24 hours later.
5. **60 minutes:** rehearse the weakest design with guidance for 45 minutes and apply the content rubric for 15. Update the gap ledger and September 24 priorities.

Blocks may be reassigned to the highest-risk existing gap, but preserve the eight-hour budget and document the change. If no gaps remain in a block, use it for guided adversarial variants and clean replays. Output: a reduced gap ledger, reproducible evidence with assistance recorded, and a timestamped revisit queue; no completion is inferred from the time spent.

### September 24 — complete coverage stress test: 8 hours

This day's explicit allocation replaces the common design contract's per-answer 30-minute challenge plus 15-minute note follow-up: guided challenges occur inside each 45-minute discussion, followed by one pooled 45-minute critique block. “Stress test” is the retained heading for coverage of changed assumptions; teaching mode remains the default. Rehearsal B integrates one full design and portions of the coding/oral work; none of its minutes are extra.

1. **90 minutes:** guided rehearsal B, including a 25-minute plain-C teaching/practice segment, 10-minute guided oral, one complete 45-minute guided design, introduction, and candidate questions.
2. **270 minutes:** the other six guided 45-minute discussions, reviewing the model before practice with notes available. Across this block and the rehearsal, cover all seven: multi-tenant block store; checkpoint filesystem; P99 under memory pressure/high queue depth; fenced NVMe-oF failover; DPU decision; power-event corruption; kernel/storage-release incident. Explain and practice one changed assumption in each and preserve the eight-section structure. Take breaks outside focused time.
3. **35 minutes:** finish/test the C exercise after preserving the first segment and its assistance record; explain every bound and memory-lifetime assumption. Its combined 60-minute evidence does not turn an incomplete 25-minute sample into a completed first-segment solution or independent demonstration.
4. **20 minutes:** remaining guided domain revisits whose 24-hour interval has elapsed. Recap before one check at a time. Timestamp new corrections for September 25 or flag that a qualifying revisit would fall after the deadline.
5. **45 minutes:** review all seven designs and rehearsal B, recording exact gaps and assistance. Every design requires at least 14/16 for content quality, no zero, and no unresolved correctness error; any independent mock score requires the explicit opt-in and independent evidence. A prior good attempt does not erase a new gap.
6. **20 minutes:** freeze the compact revision pack: I/O/ownership diagram, durability ordering, fencing invariant, memory-pressure diagnostic tree, code/lab links, seven design summaries, sizing formulas, introduction/project stories, and incident update. Record remaining risks explicitly.

Do not bury failed gates inside the revision pack. If remediation needs more time, reallocate the qualification sessions and explicitly report any lost coverage or deadline risk; all required designs still need evidence before final acceptance.

### September 25 — readiness acceptance: 4 hours

1. **90 minutes:** guided rehearsal C: 40 minutes coding teaching/practice, 20 minutes guided oral, 20 minutes guided design follow-ups, introduction, and candidate questions. Explain changed inputs/failures with a model before supported practice. Preserve the work and assistance used before correction.
2. **35 minutes:** finish testing with guidance and explain the selected coding core's ownership, shutdown, complexity, and a kernel-C translation or lifecycle walkthrough. Combined coding time is 75 minutes; this sample does not replace evidence for other required exercises.
3. **25 minutes:** finish the 16-topic guided oral review, using 45 oral minutes across the rehearsal and this block. Include corrected topics at least 24 hours after correction; recap and ask one supported check at a time. Require at least 14 complete answers with assistance recorded and no unresolved required-scope gap; incomplete answers must be resolved and revisited, not waived by the numerical count. Independent oral readiness is a separate, opt-in assessment.
4. **45 minutes:** explain the replay and expected observations, then replay the weakest practical case and audit reproducible records for the required kernel C, block trace, and actual NVMe/TCP gates. Missing evidence keeps the relevant gate open.
5. **20 minutes:** remaining guided adversarial design follow-ups (40 minutes total with the rehearsal); work through failure safety, rollout/rollback, and the leadership decision. Revisit corrected arguments only after the required delay, recording support used.
6. **25 minutes:** review rehearsal C and audit all required IDs/gaps, distinguishing taught material, guided work, independently demonstrated screening performance, software evidence, and untested hardware qualifications. Obtain explicit `F-*` confirmations only for what their evidence supports. The user—not the clock or an automated score—confirms confidence; unassessed independent readiness remains visible.

Exit means the applicable evidence and confidence gates are met, not merely that all pages were read. State teaching completion and independent readiness separately: guided completion cannot support an independent-ready claim, and choosing teaching mode does not automatically satisfy any gate. A newly discovered material gap is retaught, corrected, and revisited; if the delay or work cannot fit by September 25, report the deadline as missed for that scope. Do not label September 26–27 as review while depending on those days for unfinished first-time work.

### September 26 — technical review: 4 hours

1. **30 minutes:** review the model diagrams, then redraw buffered-hit, buffered-miss, writeback, and direct I/O together with queues, owners, waits, and completion boundaries.
2. **60 minutes:** walk the existing C++/kernel code and tests line by line with the explanation available; revisit one worked cancellation race, teardown race, and recovery boundary before supported checks.
3. **90 minutes:** two guided 45-minute design discussions selected from the frozen pack, with model answers and notes available.
4. **30 minutes:** review and redo the existing capacity, throughput, queue-depth, and DPU-cost calculations with the worked examples; check units and assumptions together.
5. **30 minutes:** review the error/recall cards and confidence ledger; log regressions and their consequences.

No planned new code, new tools, or first-time topics. A material regression is named and corrected transparently; prior confirmation is not proof that the gap can be ignored.

### September 27 — final review: 4 hours

1. **45 minutes:** guided review of an existing design selected from the pack; explain changed scale or failure assumptions before practicing the revision.
2. **45 minutes:** guided incident rehearsal, including a model two-minute leadership update and supported practice of a containment/rollback decision.
3. **45 minutes:** recap kernel, filesystem, NVMe/DMA/RDMA, and memory-pressure concepts from the frozen pack before one supported recall check at a time.
4. **45 minutes:** review previously solved coding sketches and line-by-line ownership/failure explanations before guided practice; use existing tests to check an uncertain claim.
5. **30 minutes:** revisit eligible corrections with teaching support and review remaining limitations honestly; label any independent evidence separately.
6. **30 minutes:** model and practice concise summaries together: one-minute role introduction, five-minute I/O walkthrough, three-minute design recommendation, and the questions you would ask the interviewer about the team's storage architecture.

Finish with the evidence-backed confidence summary, including assistance used and unassessed independence, and the next interview's short reference sheet. The goal is increasingly fluent explanation of already-practiced material, with no late expansion of scope.

## Hard correctness gates

Do not sign off final readiness while any of these explanations remain wrong:

- Timeout/cancellation automatically permits reclaiming memory still accessible by a backend or device.
- `write`, `O_DIRECT`, RDMA completion, and durable application commit all mean the same thing.
- ANA or a control-plane lease alone fences every path to storage.
- A quorum count alone defines a safe replicated storage protocol.
- A checksum proves data freshness or membership in the committed history.
- Journal recovery guarantees arbitrary application transactions.
- Shared block access makes an ordinary filesystem safe for concurrent read-write mounts.
- A queue-depth number or a stage P99 alone explains application latency.
- Passing a simulator/VM test establishes physical NVMe, RDMA, DPU, or power-loss behavior.

These are remediation topics, not reasons to abandon the plan. Teach the exact failed concept again, show a worked correction, guide practice, and then revisit it with one comprehension check at a time. Record support used; required code/lab results and safety properties still need their original evidence.

## Reference context

Public sources checked when preparing this plan on 2026-09-15. Use the version corresponding to the actual lab; source examples and product support matrices evolve.

- [Excelero joined NVIDIA](https://blogs.nvidia.com/blog/excelero-storage-software/) — historical company context.
- [Historical NVMesh architecture brief](https://network.nvidia.com/files/related-docs/solutions/SB_Excelero.pdf) — block pooling, RDDA, and distributed data-path context; not a current performance guarantee.
- [NVIDIA principal block/file storage role](https://nvidia.wd5.myworkdayjobs.com/nvidiaexternalcareersite/job/us-ca-santa-clara/principal-block-file-storage-software-engineer--linux---dgx-cloud_jr2024421) — role relevance, not an interview question bank.
- [JR2024421 official indexed posting](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Principal-Block-File-Storage-Software-Engineer--Linux---DGX-Cloud_JR2024421) — official indexed text checked September 15, 2026 for C/kernel/userspace and domain emphasis; Workday's directly opened page did not expose readable body text. It does not specify the 90-minute interview split.
- [VFS](https://docs.kernel.org/filesystems/vfs.html), [iomap](https://docs.kernel.org/filesystems/iomap/operations.html), [blk-mq](https://docs.kernel.org/block/blk-mq.html), [DMA](https://docs.kernel.org/core-api/dma-api-howto.html), [page pinning](https://docs.kernel.org/core-api/pin_user_pages.html) — kernel mechanism references.
- [KUnit](https://docs.kernel.org/dev-tools/kunit/start.html), [kref](https://docs.kernel.org/core-api/kref.html), [workqueues](https://docs.kernel.org/core-api/workqueue.html), [KASAN](https://docs.kernel.org/dev-tools/kasan.html), [KCSAN](https://docs.kernel.org/dev-tools/kcsan.html) — implementation and testing references.
- [open/O_DIRECT](https://man7.org/linux/man-pages/man2/open.2.html), [fsync](https://man7.org/linux/man-pages/man2/fsync.2.html), [write-cache controls](https://docs.kernel.org/block/writeback_cache_control.html) — API and persistence semantics.
- [fio](https://fio.readthedocs.io/en/latest/fio_doc.html), [PSI](https://docs.kernel.org/accounting/psi.html), [null_blk](https://docs.kernel.org/block/null_blk.html) — measurements and synthetic block experiments.
- [nvmetcli](https://github.com/linux-nvme/nvmetcli), [NVMe multipath](https://docs.kernel.org/admin-guide/nvme-multipath.html) — lab transport and path selection.
- [DOCA SNAP](https://docs.nvidia.com/doca/sdk/doca-snap-4-service-guide/), [GDS](https://docs.nvidia.com/gpudirect-storage/overview-guide/index.html) — offload and GPU-storage architecture context.

Return to the [progress tracker](./linux_storage_principal_ramp.md).

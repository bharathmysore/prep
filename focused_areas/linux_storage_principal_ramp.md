# Linux Storage Principal Ramp: NVIDIA / Excelero

An aggressive preparation plan for the **90-minute screening for NVIDIA JR2024421: Principal Block and File Storage Software Engineer, Linux — DGX Cloud**. It retains Linux block I/O, filesystems, kernel C, NVMe/RDMA, distributed storage, performance debugging, and principal-level design depth. **Readiness deadline: Friday, September 25, 2026. Saturday/Sunday, September 26–27, are reserved for review.** Dates use America/Los_Angeles time.

Keep the seven-day core sprint at **9 focused hours per day, September 16–22 (63 hours)**. Add setup on September 15 and gap closure/qualification on September 23–25: **85 focused hours through the readiness deadline**, plus breaks. Review adds four hours each on September 26 and 27, for 93 planned hours overall. These are time budgets, not evidence of completion.

This assumes existing professional systems/C++ experience and availability for the stated hours. The target is independent performance across the agreed software/interview scope, not an automatic claim of mastery or untested hardware expertise. Reading an answer or receiving generated code does not establish readiness. An unmet gate on September 25 means the target is not yet met; it must not be silently deferred into the review weekend or checked off to fit the date.

These are representative preparation exercises, not verified Excelero interview questions. Excelero joined NVIDIA in 2022; historical NVMesh documents provide architecture context, not proof of the current team's implementation or interview process. See [reference context](./linux_storage_principal_exercises.md#reference-context).

## Teaching mode — applies to the entire ramp

User instruction, September 16: **all ramp sessions are teaching-first**, including coding, domain depth, labs, designs, project stories, review, and screening rehearsals. This supersedes the earlier quiz-first Day 1 prompt. No answer is required before receiving the lesson.

For each topic, the coach follows this sequence:

1. **Explain:** start with the purpose and mental model; define unfamiliar terms and prerequisites.
2. **Demonstrate:** walk through a concrete example, diagram, or event trace; explain each ownership boundary, wait, invariant, and failure case.
3. **Connect:** relate the concept to the relevant USP or OCI Block Storage feature, keeping source observations and personal ownership claims separate.
4. **Build together:** derive the algorithm and invariants before code; explain code line by line, tests, complexity, and alternatives. For labs, explain each command, safety boundary, expected result, and interpretation before execution.
5. **Practice with support:** work through a variation with hints and a detailed model answer. Offer one comprehension check at a time after teaching; confusion triggers another explanation, not a grade or a barrier to continuing.
6. **Summarize and track:** give the interview-ready explanation, record what was taught or practiced, and ask for confidence only when appropriate.

Notes, worked solutions, pauses, and questions are welcome. Strict timed, closed-book, or independent assessments happen **only after relevant teaching and explicit user opt-in**; dates do not automatically switch the mode. Default mocks are guided rehearsals. Teaching and discussion use the existing timeboxes; unfinished scope stays open and is reported, not silently dropped or counted complete.

The independent-performance target remains, but **taught**, **guided practice**, and **independently demonstrated** are different evidence levels. User confirmation controls checkboxes; it does not manufacture missing independent or hardware evidence. If an assessment is deferred, record independent readiness as unverified and keep teaching. Every design walkthrough retains requirements, steady-state path, invariants, failure matrix, observability, rollout, rollback, and the leadership decision.

## Screening target and priorities

The [official JR2024421 posting](https://nvidia.wd5.myworkdayjobs.com/en-US/NVIDIAExternalCareerSite/job/Principal-Block-File-Storage-Software-Engineer--Linux---DGX-Cloud_JR2024421) emphasizes hands-on C work in kernel and userspace, C/C++ proficiency, Linux block/filesystem I/O, NVMe, distributed storage, host/DPU development, and production debugging. Kernel coding, memory/scheduling internals, and filesystem experience are differentiators. Checked September 15, 2026.

**Known:** the user reports a 90-minute screen. **Unknown:** interviewer, coding language/platform, live-coding requirement, and time split. The posting does not establish the interview format. Ask for recruiter guidance once; until it arrives, use the explicitly hypothetical mixed rehearsals below. No interview outcome is guaranteed.

| Preparation priority | What you must demonstrate | Existing work / added rehearsal |
| --- | --- | --- |
| Hands-on coding | Build toward independently correct code; check bounds/overflow, memory ownership, cleanup, partial I/O, concurrency, and shutdown | Taught and guided C++ workbench, kernel C, and plain-C practice in `SC-C`; independent attempts by opt-in |
| Linux/storage depth | Explain the complete I/O path and reason about waits, NVMe, DMA, memory pressure, and durability | `D1-DOM` through `D6-DOM`; explanations and worked oral answers before comprehension checks |
| Debugging judgment | Separate evidence from hypotheses; propose a discriminating test and safe mitigation | P99, corruption, and release-incident drills |
| Principal-level scope | Connect implementation to distributed correctness, performance, failure handling, rollout, and a decision | All seven existing designs, plus truthful project examples in `SC-STORY` |
| Screening fluency | Answer directly, write while explaining, handle interruptions, and fit the available time | Three guided 90-minute rehearsals: `SC-M1`, `SC-M2`, `SC-M3`; strict mocks only by opt-in |

C++ remains the general algorithm-practice default; kernel code uses C, and this role adds explicit userspace-C practice. The full practical/depth curriculum is retained because the requested ramp is uncompromising; those lab gates are our preparation requirements, **not requirements claimed for NVIDIA's screen**. Do not spend mock time demonstrating a lab setup unless asked. Report screening performance and hands-on evidence separately.

## Your USP and OCI Block Storage experience

Use the [project evidence and feature drills](./linux_storage_project_evidence.md) throughout the plan. Your confirmed ownership areas are replication, leader election, snapshot, backup, XRR, XRRVG, parts of the filesystem, and background compression. Local source supports concrete rehearsal prompts; exact personal changes, incidents, and outcome metrics remain to be supplied.

Replace generic examples with eight 20-minute project-source drills inside existing domain blocks: filesystem on Day 1; leader election on Day 2; snapshot and backup on Day 4; replication, XRR, and XRRVG on Day 5; compression on Day 6. Keep all existing coding/lab gates. These 160 minutes are part of the current budget, not additional hours.

Prepare one USP and one Block Storage story, together covering diagnosis and a design/correctness decision. Keep source details local and use sanitized abstractions in the interview. Distinguish object/chunk paths from block-volume paths, userspace SPDK from kernel blk-mq, and source observations from deployed behavior. The source and ownership tables are evidence aids; this file remains the only completion checklist.

## Start here

1. Use this file as the **only completion checklist**.
2. Open the [detailed exercise book](./linux_storage_principal_exercises.md) for specifications, adversarial tests, experiments, design scenarios, and oral questions.
3. Use September 15 for `P0-ENV`, `P0-BASE`, `P0-SOURCE`, and `P0-TRACK`, then start Day 1 on September 16. Unfinished setup uses Day 1's domain/lab time and is recorded as a schedule risk.
4. On each revisit, report task IDs, evidence, confidence, and the next weak point. Example: `D2-CODE: tests pass; confident; timeout versus cancellation explanation still needs practice`.
5. A checked box means **you explicitly confirmed confidence in that specific item**. Evidence review and your confidence are recorded separately. Conflicting evidence becomes a named gap, even if you feel confident.

## Progress and confirmation rules

Initial state: **all items unconfirmed; no exercises or labs have been executed as part of creating this plan**. Earlier conversation explanations do not count as completed practice.

There are **53 tracked items: 51 readiness items due by September 25 and two review items due September 26–27**. All previous 48 IDs are preserved; five `SC-*` IDs add C fluency, project communication, and full-length screening rehearsals. Shared evidence is linked to each relevant ID; the work is not scheduled twice.

| State | Meaning | Checkbox |
| --- | --- | --- |
| Not started | No attempt/evidence recorded | Open |
| Session started | Teaching session opened; lesson or worked example underway | Open |
| Taught | Explanation and worked example delivered; no learner-performance claim | Open |
| Practicing | Guided or independent attempt recorded explicitly; gaps remain | Open |
| Evidence ready | Required artifacts and checks exist; awaiting your confidence confirmation | Open |
| Confirmed | You explicitly say the item is understood and repeatable | Checked |
| Needs revisit | Previously confirmed knowledge has a demonstrated gap | Record gap and reopen when you request or agree to it |

Confidence vocabulary: `0` unfamiliar; `1` recognize with notes; `2` can explain with prompts; `3` can implement/explain independently; `4` can defend alternatives, handle failures, and repeat after a delay. Target `3` for each exercise and `4` for final domain/design gates. Ratings are self-assessments, not generated scores.

For future updates: record the exact IDs and date confirmed; preserve history; never infer completion from a passing test, a confident-sounding answer, an assistant's code, or elapsed time. Do not create reminders or run lab mutations merely because this tracker exists.

## Deadline calendar

| Date (2026) | Focused budget | Required outcome / task IDs |
| --- | --- | --- |
| Tue Sep 15 | 2h | Disposable lab inventoried, guided baseline orientation, source/build route pinned, tracker initialized; record any recruiter format guidance: `P0-*` |
| Wed Sep 16 | 9h | Day 1: end-to-end I/O, extent mapper, buffered/direct lab, block-store design: `D1-*` |
| Thu Sep 17 | 9h | Day 2: NVMe/DMA/RDMA, asynchronous lifetime, actual NVMe/TCP, fenced failover: `D2-*` |
| Fri Sep 18 | 9h | Day 3: kernel C/KUnit, reclaim/scheduling, block trace, P99 investigation: `D3-*` |
| Sat Sep 19 | 9h | Day 4: filesystem durability, checkpoint publisher, crash recovery, filesystem design: `D4-*` |
| Sun Sep 20 | 9h | Day 5: replication/fencing model, adversarial schedules, corruption investigation: `D5-*` |
| Mon Sep 21 | 9h | Day 6: tenant admission, controlled performance experiment, DPU decision: `D6-*` |
| Tue Sep 22 | 9h | Day 7: integrated guided 90-minute rehearsal A, supported coding, practical replay, design/incident work, gap inventory: `D7-*`, `SC-M1` |
| Wed Sep 23 | 8h | Close implementation, lab, and explanation gaps; no new curriculum: `S23-FIX` |
| Thu Sep 24 | 8h | Integrated guided 90-minute rehearsal B, all seven full designs, plain-C exercise, review/retests, revision pack: `S24-MOCK`, `SC-M2`, `SC-C` |
| Fri Sep 25 | 4h | Integrated guided 90-minute rehearsal C, retention review, evidence audit, explicit readiness confirmation if supported: `S25-EXIT`, `SC-M3`, `F-*` |
| Sat Sep 26 | 4h | Review only: I/O, code ownership, two designs, sizing, recall: `R26-REVIEW` |
| Sun Sep 27 | 4h | Review only: random design/incident, kernel internals, coding, concise summaries: `R27-REVIEW` |

The original `D1`–`D7` IDs remain stable. Day 7 is the first integrated coached review; independent assessment is available only by opt-in after teaching. Detailed timeboxes and exit checks are in [deadline qualification and review sessions](./linux_storage_principal_exercises.md#deadline-qualification-and-review-sessions).

At each daily revisit, spend the final 15 minutes of the existing recall/review block recording: attempted IDs, evidence-ready IDs, user-confirmed IDs, remaining gaps, and tomorrow's first task. This is part of the stated budget, not extra work or an automatic reminder.

Within existing recall blocks, use 15 minutes on each of Days 1 and 2 for plain-C defect reviews, and 15 minutes on each of Days 1, 4, and 6 for the introduction/project stories. The three mocks reuse scheduled coding, oral, design, and review minutes; follow the [screening rehearsal accounting](./linux_storage_principal_exercises.md#ninety-minute-screening-rehearsals) to avoid adding 4.5 hours to the plan.

Treat the deadline as **at risk** if the Linux lab/build route is still unavailable after September 16; if required kernel C/block-trace/NVMe-TCP evidence is missing after September 22; or if a hard correctness gap remains at the end of September 24. Reallocate September 23–24 to those dependencies first and report the tradeoff. Do not remove required scope or turn September 26–27 into planned first-time learning. For an opted-in independent retention claim, retest corrections at least 24 hours later and record timestamps; a supported review is useful learning but not an independent retest.

Detect environment gaps on September 15, not on the first affected lab day: verify the existing KUnit baseline, block tracing availability, disposable storage, NVMe/TCP module support, and isolated network route. Start the baseline build as soon as prerequisites permit. The two-hour setup budget assumes these resources are largely available; otherwise record the missing dependency and recovery allocation immediately. September 16 is the escalation checkpoint, not permission to delay discovery.

## Seven-day schedule

This is the daily block allocation for September 16–22; use the deadline calendar above for setup, qualification, and review dates.

| Day | Domain / source study: 2h | Coding: 2.5h | Experiments: 2h | Design: 1.5h | Recall / review: 1h |
| --- | --- | --- | --- | --- | --- |
| 1 | VFS, folios, extents, bios, requests, completion | Extent mapper and POSIX I/O harness | Buffered/direct comparison; start kernel baseline build | Multi-tenant block store | Guided recap of paths and ownership |
| 2 | NVMe queues, DMA, RDMA, NVMe-oF (1h) | Bounded executor and request lifecycle (3.5h) | Deterministic race schedules; NVMe/TCP connection lab | Fenced target failover | Defend timeout, cancellation, and fencing |
| 3 | Kernel contexts, memory reclaim, scheduling, NUMA | Kernel C request lifecycle and KUnit integration | Run kernel tests and block tracing | P99 latency under memory pressure and queue depth | Explain locks, lifetime, context, and waits |
| 4 | VFS/FS consistency, ext4/XFS, writeback, fsync | Durable checkpoint publisher/recovery | Crash injection and checksum validation | Checkpoint filesystem | Worked durability protocol, then guided reconstruction |
| 5 | Replication ordering, epochs, recovery, repair | Fenced replica model with deterministic events | Partition, late writes, lost replies, stale repair | Corruption after a power event | Prove a safety property and name the model's limits |
| 6 | Saturation, fairness, PCIe/NUMA, DPU/GDS | Tenant admission and measurement extension | Mixed-load experiment; finish kernel instrumentation evidence | DPU offload decision | Quantify benefit, cost, and failure tradeoffs |
| 7 | Two 45-minute guided designs + 30-minute feedback | Guided coding + tests + repair of weak sections | Replay hardest lab/race failures from clean state | Coached release-regression incident + feedback | Taught oral review and final gap ledger |

For Days 1–6, use the design block as 45 minutes of a taught/guided answer and diagram, 30 minutes of worked adversarial follow-ups, and 15 minutes writing the decision together. Reserve 20 minutes of the coding block for a demonstrated, then guided algorithm warm-up. All columns use teaching mode. Day 2 shifts one hour from domain study to coding, retaining the nine-hour total and giving practical work 5.5 hours. Kernel compilation can run while you study; compilation waiting is not focused study time. Day 3 kernel work may use Day 6's planned follow-up and Day 7 replay time.

No optional skipping of correctness, tests, or review to meet the clock. If a block overruns, record the unfinished ID, its dependency impact, and a September 23–24 recovery slot. If the work cannot fit before September 25, report the readiness risk immediately rather than extending the schedule or reducing scope without agreement. Protect the September 26–27 review allocation.

## Prerequisite todos

- [ ] **P0-ENV** — Record the exact disposable Linux environment, available storage/network hardware, kernel build route, and tool availability; select the applicable lab evidence levels. [Environment contract](./linux_storage_principal_exercises.md#environment-and-evidence-contract)
- [ ] **P0-BASE** — Complete the 30-minute teacher-led baseline orientation with worked examples; record unfamiliar concepts without a prerequisite quiz. [Baseline](./linux_storage_principal_exercises.md#baseline-diagnostic)
- [ ] **P0-SOURCE** — Record kernel commit, configuration, compiler, filesystem, mount options, fio version, and source-reading bookmarks; successfully run one existing kernel test. Record the USP/Block Storage source revisions and selected feature anchors separately; repository reading is not lab execution. [Source map](./linux_storage_principal_exercises.md#source-reading-map), [project evidence](./linux_storage_project_evidence.md#source-evidence-ledger)
- [ ] **P0-TRACK** — Create your evidence log, acknowledge the September 15–27 calendar and September 25 readiness target, and adopt the confirmation rules above. Record unavailable hardware as a limitation, not as a successful lab.

## Day 1 todos — trace and execute I/O

- [ ] **D1-DOM** — Draw buffered-hit, buffered-miss, buffered-write/writeback, and direct-I/O paths; explain extents, holes, folios, bios, requests, tags, DMA, completion, and durability. [Day 1](./linux_storage_principal_exercises.md#day-1--io-path-and-file-harness)
- [ ] **D1-CODE** — Derive, implement, and test the extent mapper and file-I/O harness with guidance; handle partial I/O, EOF, alignment, overflow, and ownership; study then practice the merge-interval warm-up.
- [ ] **D1-LAB** — Collect comparable buffered/direct runs with recorded cache state and verified data; explain why warm-cache throughput is not SSD throughput; start the kernel build baseline.
- [ ] **D1-DESIGN** — Defend the multi-tenant block-store design with numerical sizing, a write contract, failure handling, per-tenant admission, and rollout/rollback.
- [ ] **D1-GATE** — After the lesson, work through a 10-minute supported path recap and explain a byte's location/lifetime at every boundary; record uncertainties and assistance used. Independent recall is opt-in.

## Day 2 todos — queues, protocols, and request lifetime

- [ ] **D2-DOM** — Explain SQ/CQ ownership, doorbells, tags, PRP/SGL, DMA/IOMMU, interrupts/polling, RDMA QP/CQ/MR, NVMe/TCP versus RDMA, and ANA versus fencing. [Day 2](./linux_storage_principal_exercises.md#day-2--protocols-and-asynchronous-ownership)
- [ ] **D2-CODE** — Build a bounded executor and request tracker with one user-visible outcome, generation-safe IDs, retained backend buffer ownership, and explicit shutdown; pass deterministic races and separate sanitizer runs.
- [ ] **D2-LAB** — Exercise an actual Linux NVMe/TCP initiator/target in the disposable lab and capture path interruption/reconnection; separately execute fake-backend timeout/late-completion schedules.
- [ ] **D2-DESIGN** — Defend target failover when the old target is alive but partitioned; identify the storage-side fence, recovery evidence, unknown outcomes, and unavailable states.
- [ ] **D2-GATE** — Work through why timeout, RDMA completion, ANA state, and durable storage commit are four different facts; review Day 1's path summary with support as needed.

## Day 3 todos — real kernel C, memory, and scheduling

- [ ] **D3-DOM** — Explain process/IRQ context, allowed sleeping, spinlock versus mutex, kref, RCU limits, memory barriers, reclaim/allocation flags, scheduler delay, IRQ affinity, NUMA, and page pinning. [Day 3](./linux_storage_principal_exercises.md#day-3--kernel-c-lifecycle-and-latency)
- [ ] **D3-CODE** — Implement the test-only kernel C lifecycle helper and KUnit suite, wire Kconfig/Makefile, and justify every reference, lock, callback, and teardown transition.
- [ ] **D3-LAB** — Build and run the modified kernel tests, save KTAP and relevant diagnostics, and capture a real block-layer trace; record enabled instrumentation and any unavailable checks.
- [ ] **D3-DESIGN** — Diagnose a memory-pressure-plus-queue-depth P99 regression with competing hypotheses, timestamp boundaries, a controlled experiment, and a reversible mitigation.
- [ ] **D3-GATE** — Explain a teardown deadlock and a premature-free bug at code level; show how the tests expose them and how correct ownership prevents them.

## Day 4 todos — filesystem durability and checkpoints

- [ ] **D4-DOM** — Explain inode/dentry/cache roles, delayed allocation, unwritten extents, journaling versus application transactions, fsync/fdatasync, directory durability, flush/FUA, and mixed direct/buffered access. [Day 4](./linux_storage_principal_exercises.md#day-4--filesystem-durability-and-checkpoints)
- [ ] **D4-CODE** — Implement immutable checkpoint shards, a manifest, atomic publication, and verified recovery using Day 1's file helpers; explicitly handle all synchronization failures.
- [ ] **D4-LAB** — Inject process failure at every publication boundary, corrupt/truncate a shard, test recovery, and distinguish these results from VM crash and physical power-loss evidence.
- [ ] **D4-DESIGN** — Defend checkpoint filesystem throughput, metadata scaling, consistency across training ranks, manifest publication, read recovery, and retention/GC safety.
- [ ] **D4-GATE** — Study then redraw the durable-publication ordering with support as needed; explain how a mountable filesystem can still contain an unusable application checkpoint.

## Day 5 todos — distributed correctness and recovery

- [ ] **D5-DOM** — Explain ordered replication, quorum intersection limits, leader recovery, fencing epochs, retry ambiguity, deduplication scope, membership changes, checksums, and repair ordering. [Day 5](./linux_storage_principal_exercises.md#day-5--replication-fencing-and-corruption)
- [ ] **D5-CODE** — Build the deliberately bounded replica model and enumerate adversarial event schedules; prove stale epochs and stale repair cannot overwrite current committed state under the stated assumptions.
- [ ] **D5-LAB** — Save traces for minority isolation, delayed old writes, acknowledgement loss, stale rebuild, and unsafe-promotion rejection; demonstrate both progress and deliberate refusal to proceed.
- [ ] **D5-DESIGN** — Lead the power-event corruption investigation: evidence preservation, acknowledgement boundaries, ordering, checksums, replica choice, repair approval criteria, and durability policy.
- [ ] **D5-GATE** — Explain exactly what your model does not implement; give a counterexample to the claim that `R + W > N` alone creates a correct failover protocol.

## Day 6 todos — performance, tenant fairness, and offload

- [ ] **D6-DOM** — Explain Little's Law, queue saturation, histogram interpretation, coordinated omission, CPU/NUMA cost, tenant fairness, RDMA resource limits, DPU placement, and GDS fallback. [Day 6](./linux_storage_principal_exercises.md#day-6--performance-fairness-and-dpu-judgment)
- [ ] **D6-CODE** — Add tenant admission, separate queue/service/total latency, and bounded telemetry to the workbench; prove limits and shutdown still hold under a noisy tenant.
- [ ] **D6-LAB** — Run a controlled mixed-load/queue-depth experiment with repeat trials, retain raw results, quantify regressions and gains, and finish outstanding kernel instrumentation evidence.
- [ ] **D6-DESIGN** — Present a quantified host-versus-DPU decision including end-to-end performance, CPU savings, fleet cost, telemetry, compatibility, failover, and a credible rollback path.
- [ ] **D6-GATE** — Defend why your measurement supports the conclusion; identify one confounder, one falsifying result, and all hardware claims not tested.

## Day 7 todos — coached integration and principal review

- [ ] **D7-DOM** — Work through two 45-minute guided designs; one must cover distributed block storage/failover and one checkpoints or DPU tradeoffs. Notes and model answers are allowed; strict assessment is opt-in. [Day 7](./linux_storage_principal_exercises.md#day-7--independent-assessment)
- [ ] **D7-CODE** — Rebuild a bounded queue and request-lifetime core with teaching support inside the allocated blocks, test adversarial cases, explain complexity, and fix the weakest coding gap; label assisted versus independent work.
- [ ] **D7-LAB** — Reproduce one real I/O trace, one kernel test result, one checkpoint failure/recovery, and one stale-writer rejection from recorded instructions.
- [ ] **D7-DESIGN** — Lead the kernel/storage release-regression incident, issue a concise status update, select a mitigation, and defend rollout/rollback gates and the leadership decision.
- [ ] **D7-GATE** — Complete the taught domain review and supported retention checks; populate the final gap ledger, record the evidence level, and leave independent readiness unverified unless demonstrated in an opted-in check.

## Screening-specific todos

- [ ] **SC-C** — Study then complete the two plain-C defect reviews and the userspace-C exercise with its continuation/tests; explain pointers, lengths, overflow, cleanup, and ownership, and work through the kernel-C code. Strict timing and independent defense are opt-in. [C fluency](./linux_storage_principal_exercises.md#plain-c-screening-practice)
- [ ] **SC-STORY** — Deliver a two-minute introduction and two truthful technical stories, one from USP and one from OCI Block Storage, covering diagnosis and a design/correctness decision. Prepare follow-ups for all eight confirmed ownership areas; distinguish personal work, code observations, team results, and limitations. [Project story cards](./linux_storage_project_evidence.md#project-stories-and-coding-bridges)
- [ ] **SC-M1** — September 22: complete and review guided 90-minute rehearsal A, saving code, worked answers, assistance level, and exact gaps. Use a strict mock only by opt-in. [Mock formats](./linux_storage_principal_exercises.md#ninety-minute-screening-rehearsals)
- [ ] **SC-M2** — September 24: complete and review guided 90-minute rehearsal B, including a full 45-minute design; finish and test the C exercise in its remaining allocated block. Record any opted-in independent attempt separately.
- [ ] **SC-M3** — September 25: complete and review guided 90-minute rehearsal C; revisit correctness and concise reasoning. Independent retention claims require an opted-in retest of corrected material after at least 24 hours.

## Deadline and review todos

- [ ] **S23-FIX** — September 23: reproduce and fix remaining required coding/lab/domain failures, rerun their tests, and record correction timestamps and delayed-retest slots. [Qualification sessions](./linux_storage_principal_exercises.md#deadline-qualification-and-review-sessions)
- [ ] **S24-MOCK** — September 24: work through all seven 45-minute design/incident answers with the eight-section rubric, one inside rehearsal B; complete the remaining code/domain blocks and freeze the revision pack; record guidance used and any readiness risk explicitly.
- [ ] **S25-EXIT** — September 25: review code, domain, practical evidence, and retention with teaching support; audit every required gap. Offer independent checks only by opt-in and request final confirmation only if the readiness claim is supported.
- [ ] **R26-REVIEW** — September 26: complete the four-hour review session from the frozen pack; record any regression rather than treating review as first-time completion.
- [ ] **R27-REVIEW** — September 27: complete the four-hour taught final review, including worked design/incident follow-ups and supported ownership/correctness explanations; independent checks remain opt-in.

## Final readiness todos

Target: September 25, after qualification. `P0-*`, `D1-*` through `D7-*`, `SC-*`, and `S23-FIX`/`S24-MOCK`/`S25-EXIT` need supporting evidence and your confidence confirmation. Required practical evidence must be reproducible, and no [hard correctness gate](./linux_storage_principal_exercises.md#hard-correctness-gates) may be waived. Guided completion is recorded as learning progress, not proof of the independent final targets below. To claim independently verified screening readiness, opt into assessments after teaching: all seven designs must meet the rubric without assistance, the final two screening rehearsals must meet the practice rubric as strict mocks, and earlier gaps need successful retests (at least 24 hours after corrections for retention claims). If those assessments are not requested or completed, keep independent readiness unverified; continue teaching without forcing a mode change. The two review todos are not prerequisites for September 25 readiness.

- [ ] **F-CODE** — I can implement, test, debug, and explain the required C++, userspace-C, and kernel-C exercises independently; no required coding gap remains open.
- [ ] **F-DOM** — I can explain the complete I/O stack and reason about memory, scheduling, DMA, filesystems, NVMe, and RDMA without material correctness errors.
- [ ] **F-DESIGN** — I can lead all seven design/incident topics using requirements, steady-state paths, invariants, failure matrices, observability, rollout, rollback, and a leadership decision.
- [ ] **F-CONFIRM** — I explicitly confirm readiness for the 90-minute JR2024421 screening and the agreed depth curriculum. Screening rehearsal results, hardware evidence level, and any unclaimed practical exposure are stated separately alongside this confirmation.

## Evidence ledger

Add one row per exercised or confirmed ID. A path can be local to the selected Linux lab; record the host as well so it remains retrievable. Keep large logs in artifacts and link them here.

| ID | State | Evidence / environment | Last attempted | Confidence 0–4 | Gap / next retest | User confirmation / date |
| --- | --- | --- | --- | --- | --- | --- |
| D1-DOM | Session started | Session opened September 15; September 16 user instruction switches it to teaching-first. Earlier quiz is superseded; no learner answer, code, or lab execution evidence yet. | Teaching mode adopted 2026-09-16 | Not assessed | Coach explains buffered-read mental model and worked syscall-to-NVMe path before any comprehension check; prerequisites remain open | None |

## Hardware evidence ledger

| Capability | Required evidence | Current status |
| --- | --- | --- |
| Linux kernel C | Modified test code compiled and executed in a recorded Linux/UML/QEMU kernel | Unconfirmed |
| Linux block path | Kernel trace tied to real submitted requests; null_blk labeled synthetic | Unconfirmed |
| NVMe/TCP | Actual initiator/target connection, I/O, and interruption/reconnect behavior | Unconfirmed |
| Physical NVMe performance | Bare-metal device/topology/firmware and repeatable measurements | Access unknown; no claim |
| RDMA | Real RNIC/fabric setup, registered memory, completion/error observations | Access unknown; no claim |
| BlueField/DPU or GDS | Actual supported hardware/software versions and verified active data path | Access unknown; no claim |

The first three are required practical gates for this software ramp. The last three are additional evidence required before claiming hands-on expertise on those hardware paths. If access is missing, keep that qualification open; source analysis is useful preparation but does not replace the experiment.

## Review protocol

At the start of a revisit, read the checklist, evidence ledger, and gap ledger. Select the oldest unconfirmed dependency or weakest previously attempted topic. Begin with a brief recap and worked explanation, then guide practice or inspect supplied evidence. Spend 10–15 minutes reviewing one earlier day; offer one comprehension check after teaching. Never start with a quiz, withhold the answer, or switch to assessment because a date has arrived. Strict retests require explicit opt-in.

Helpful update messages:

- `Teach D1-DOM from fundamentals, then walk through an example with me.`
- `I am ready for an independent D1-DOM check; quiz me now.`
- `Review D2-CODE: here are my source and tests.`
- `D3-CODE and D3-LAB confident; mark confirmed. D3-DOM still weak on reclaim.`
- `Reopen D4-GATE; I cannot explain directory fsync clearly.`
- `Show all open items and give me the next 90-minute session.`

## Gap ledger

| ID / area | Specific gap | Why it matters | Next action / evidence | Retest date |
| --- | --- | --- | --- | --- |
| P0-ENV | Linux lab and hardware access not yet recorded | Determines which practical claims can be validated | Inventory the disposable lab and pin versions | 2026-09-15; risk checkpoint 2026-09-16 |
| SC-STORY | Ownership areas confirmed; exact personal changes/incidents and authorized outcomes not yet supplied | Strong project answers need defensible personal contribution, not just architecture knowledge | Complete the two personal-evidence rows in the project companion | Day 1 selection; Day 4 evidence; Day 6 rehearsal |

## Change log

| Date | Change | Confidence confirmations |
| --- | --- | --- |
| 2026-09-15 | Created seven-day plan and detailed exercise specifications | None; all checkboxes open |
| 2026-09-15 | Set September 25 readiness deadline; dated core sprint September 16–22; added qualification September 23–25 and protected review September 26–27 | None; 48 checkboxes open |
| 2026-09-15 | Aligned to JR2024421's 90-minute screen; added role-specific C practice, truthful project stories, and three integrated mocks without increasing the time budget | None; 53 checkboxes open |
| 2026-09-15 | Incorporated USP and OCI Block Storage source evidence and all eight user-confirmed ownership areas into existing daily drills, stories, and mocks | Ownership areas confirmed, not readiness; all 53 checkboxes remain open |
| 2026-09-15 | Started Day 1 coaching at the user's request, ahead of its scheduled September 16 slot; first D1-DOM answer pending. Calendar/deadline unchanged; lab prerequisites remain open. | None; all 53 checkboxes remain open |
| 2026-09-16 | User requested teaching mode for the entire ramp. Replaced quiz-first baseline and mandatory closed-book sessions with explanations, worked examples, guided coding/labs/designs, and supported rehearsals. Independent assessment is opt-in; scope, dates, hours, and evidence standards remain unchanged. | None; all 53 checkboxes remain open |

## Related prep

- [Exercise book and public sources](./linux_storage_principal_exercises.md)
- [USP / OCI Block Storage project evidence and feature drills](./linux_storage_project_evidence.md)
- [NVIDIA design catalog](../system_design/nvidia_l7_system_design_prep.md)
- [C++ concurrency questions](../coding/cpp/concurrency/questions.md)
- [C++ systems-style questions](../coding/cpp/systems_style/questions.md)
- [C++ distributed-systems questions](../coding/cpp/distributed_systems_algorithms/questions.md)
- [Root index](../INDEX.md)

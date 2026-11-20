# Research and architectural leadership program

[Expertise path](README.md) · [Research method](research-method.md) · [Paper study](paper-study.md)

The goal is to create and defend new storage architecture, not only understand existing implementations. Build breadth across engines, databases, time-series systems and large-scale infrastructure, with original depth at a carefully chosen intersection. Industry recognition is an outcome of useful work and sustained influence; it cannot be guaranteed by a reading list, schedule or paper acceptance.

This is a proposed program, not an assessment that the learner has already met its gates. Prior experience and available time remain unspecified. The existing source-grounded knowledge base is the starting map; implementation labs, reproductions and research claims still need to be executed and validated.

## Two connected forms of achievement

**Architectural leadership:** choose sound boundaries under uncertain requirements, quantify costs and failure behavior, evolve deployed systems safely, communicate decisions, and help other engineers make better decisions. Evidence includes successful designs, migration and recovery outcomes, usable abstractions, and lessons that survive changing workloads and organizations.

**Research contribution:** establish a consequential problem or insight, position it against prior work, support claims through the appropriate model/proof/implementation/evaluation, and explain applicability and limitations. A paper can contribute a mechanism, an analysis, a rigorous measurement study, a validation method or deployed experience; it need not invent a whole database.

The FAST research criteria emphasize a significant problem, a compelling solution, benefits and drawbacks, sound evaluation and advances over previous work. Use the [FAST call](https://www.usenix.org/conference/fast27/call-for-papers) as a concrete reference, while checking the actual target venue/category when submitting.

## The expanded foundation

| Foundation | Required capability | Concrete application |
|---|---|---|
| Algorithms and external-memory models | Derive asymptotic and byte/I/O costs under explicit assumptions | Tree fanout, runs/levels, scan/lookup cost, amortized maintenance |
| Probability and statistics | Reason about filters, skew, distributions, variance and inference | Bloom allocation, tail latency, experiment uncertainty |
| Queueing and feedback | Separate stable capacity, bursts, backlog, admission and control loops | Compaction debt, repair scheduling, replay after outages |
| Information representation | Explain encoding, entropy, compression and metadata lower-level costs | Time-series chunks, index footprint, flash traffic |
| OS and hardware | Trace cache/memory/I/O/persistence and identify the hardware contract | mmap faults, sync boundaries, CPU/NUMA and storage concurrency |
| Concurrency and memory safety | Separate publication, visibility and lifetime | Iterators, reader epochs, concurrent flush/compaction |
| Distributed reasoning | State safety, liveness, authority, causal order and failure assumptions | Commit, replication, offline merge, membership and reclamation |
| Database theory and execution | Connect isolation, dependencies, indexes and query plans | Write skew, range conflicts, analytical/transactional layouts |
| Scientific argument | Distinguish evidence, causal interpretation and extrapolation | Paper claims, experiment design, negative results |

Acquire these through derivations and experiments attached to the projects, not as indefinitely postponed prerequisites. For example, learn constrained optimization while deriving a filter-memory allocation, and learn liveness assumptions while specifying offline rejoin.

## Milestone 1 Explain and derive

Finish the relevant [engine](engine-internals.md), [time-series](time-series.md), [offline](constrained-and-disconnected.md) and distributed architecture routes. For each mechanism, produce an invariant, an operation timeline, a cost model and a counterexample to an overbroad claim.

**Gate:** explain why the mechanism works, derive when it becomes expensive, and identify which assumption would invalidate the explanation. A source tour alone is insufficient.

## Milestone 2 Build and break

Complete selected [labs](labs.md) with a declared failure model. Extend at least one educational implementation enough to expose real publication, retention and maintenance problems. Trace the same mechanism in a production codebase.

**Gate:** use an oracle and minimized histories to explain failures; distinguish a proven property from a bounded test result. Produce resource and recovery evidence rather than only passing functional tests.

## Milestone 3 Reproduce and critique

Choose a foundational result that fits available hardware and artifact access. Reproduce a central model, figure or mechanism, documenting revisions and unavoidable differences. Select an adjacent result with different assumptions and explain why the conclusions differ.

**Gate:** present the claim, experimental setup, reproduction, discrepancies, and sensitivity analysis. If reproduction fails, investigate fairly; failure is not automatically evidence that the original work is wrong.

## Milestone 4 Characterize a boundary

Identify a situation where a model or mechanism becomes insufficient: finite flash/space, late samples, stale peers, metadata cardinality, expensive repair, or resource interference. Establish that the boundary matters using measurements or a precise counterexample.

**Gate:** describe the limitation without relying on a deliberately bad configuration. Search the closest literature and implementation alternatives before calling it an open problem.

## Milestone 5 Make one original contribution

Write a narrow claim with a falsifier. Propose the smallest mechanism/model/measurement study that can establish it. Implement only what the claim needs and compare against strong relevant alternatives.

**Gate:** explain what is new, why it matters, how it works, what evidence supports it, and where it loses. Novelty is unresolved until a focused related-work review establishes the difference. Candidate questions below are not novelty claims.

## Milestone 6 Exercise architectural judgment

Turn the contribution into a deployable design proposal. Include compatibility, migration/rollback, failure containment, observability, resource isolation, capacity economics, security boundaries and ownership of operational decisions. Model what happens as hardware, workload, organization and retention requirements change.

**Gate:** defend the proposal in an adversarial design review and change it when counterevidence warrants. Research speedups and production usefulness need separate evidence. Mentoring, clear interfaces and documented tradeoffs are technical leadership outputs too.

## Milestone 7 Communicate, review and contribute

Develop the paper and artifact together. Seek critical feedback from researchers and experienced implementers when available. Give a technical talk, write clear design/experiment notes, and practice reviewing another paper. Contributions to shared systems can demonstrate generality and maintainability, but must follow the relevant project process and authorization.

**Gate:** another reader can understand the contribution and rerun its central evidence. Submission, acceptance, adoption and industry leadership remain distinct outcomes.

## Candidate research questions at your intersection

### A Resource-bounded maintenance for late-arriving telemetry

**Question:** can a storage layout/scheduler handle a specified amount of late data and outage replay while bounding memory and temporary space, without unacceptable foreground interference?

**First probe:** characterize lateness, series churn, physical rewrites and queue growth for simple baseline designs before proposing a controller. Compare equal persistence and lateness semantics.

**Possible contribution:** a cost model, a policy with explicit conditions, or a measurement result showing where common assumptions fail.

**Early rejection test:** an existing time-window/overlay/compaction policy already provides the same tradeoff, or the benefit disappears at sustainable steady state. Include recovery and overflow behavior.

### B Bounded-history deletion and rejoin

**Question:** under an explicit maximum absence, membership or forced-resync rule, how can a system reclaim history while preventing deleted data from reappearing?

**First probe:** formalize the peer state and allowed messages; construct the stale-peer counterexample before implementing optimization.

**Possible contribution:** a protocol with safety/liveness argument and measured metadata/rejoin costs, or a precise boundary showing which promises cannot coexist under the chosen model.

**Early rejection test:** the proposal silently prevents rejoin, drops intended updates, or depends on trusted wall clocks without stating it. Compare relevant causal-history and snapshot-resync designs.

### C Joint budgeting of ingestion, replay and maintenance

**Question:** can a device or service allocate I/O/CPU/energy among fresh ingestion, outage catch-up and reclamation while maintaining explicit backlog and latency objectives?

**First probe:** measure service budgets and queue interaction; derive the conditions for a finite catch-up time.

**Possible contribution:** a model/controller and reproducible evaluation across workload shifts, with clear infeasible regions.

**Early rejection test:** a simpler static reservation or existing admission scheduler matches the result, or claimed gains hide rejection, data loss, uncounted energy or deferred maintenance.

These are starting hypotheses. Do not select a paper title first and then search for a favorable benchmark. Begin with a reproducible observation or contradiction.

## First research deliverable

Produce a short **technical study**, not a purported novel paper: compare LSM and COW B+ tree designs for an intermittently connected telemetry workload. Include the schema, acknowledgment/failure model, memory/space/write budgets, late data and old-reader behavior, and a proposed experiment matrix. Reconstruct one mechanism from the readings in [Paper study](paper-study.md).

The next research step is to challenge one prediction with an experiment. Let the strongest supported unresolved result determine the first original project. Write continuously, beginning with claims, assumptions and failed hypotheses rather than waiting until the end for prose.

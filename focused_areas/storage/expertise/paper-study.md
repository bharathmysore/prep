# Paper study and research apprenticeship

[Research program](research-program.md) · [Research method](research-method.md)

Read papers as arguments to reconstruct and challenge. These seed readings were located using primary paper texts or author/institution sources on 2026-10-27. They are an entry sequence, not an exhaustive related-work review or evidence that a proposed idea is novel. Foundational papers must be followed by newer work and implementations around the eventual research question.

## Seed reading sequence

| Reading | Question to investigate | Artifact to produce |
|---|---|---|
| [Organization and Maintenance of Large Ordered Indexes](https://rtheunissen.github.io/bst/docs/references/1972_bayer_mccreight.pdf) — Bayer and McCreight | How do page organization and occupancy shape search/update costs? | Derive fanout/height and simulate a split; distinguish the original structure from modern variants |
| [The Log-Structured Merge-Tree](https://db.cs.berkeley.edu/cs286/papers/lsm-acta1996.pdf) — O'Neil and coauthors | What workload and I/O assumptions make deferred merging attractive? | Reconstruct the cost argument and compare with local LevelDB; full-text retrieval needs rechecking because the fetch timed out during this update |
| [ARIES](https://www.csd.uoc.gr/~hy460/pdf/aries.pdf) — Mohan and coauthors | What recovery obligations arise with fine-grained locking and partial rollback? | Trace a small logging/restart history; contrast with COW-root publication rather than assuming every engine implements ARIES |
| [Designing Access Methods: The RUM Conjecture](https://research.ibm.com/publications/designing-access-methods-the-rum-conjecture) | How do read, update and memory/storage overheads constrain design choices? | Draw tradeoff axes and state assumptions; treat a conjecture as a conjecture, not a universal proved theorem |
| [Monkey: Optimal Navigable Key-Value Store](https://nivdayan.github.io/monkeykeyvaluestore.pdf) — Dayan, Athanassoulis and Idreos | How does filter-memory allocation interact with LSM lookup/update cost? | Reconstruct one optimization/model prediction and test sensitivity to workload and memory budget |
| [In Search of an Understandable Consensus Algorithm](https://raft.github.io/raft.pdf) — Ongaro and Ousterhout | Which rules establish replicated-log safety, and which conditions are needed for progress? | Reconstruct a leader-change history and connect persistent state to etcd/Cornerstone code |
| [FoundationDB: A Distributed Unbundled Transactional Key Value Store](https://www.foundationdb.org/files/fdb-paper.pdf) — Zhou and coauthors | Why separate transaction, logging and storage roles, and how does testing support the design? | Trace one transaction/failure through the paper and current checkout, recording historical differences |
| [Gorilla: A Fast, Scalable, In-Memory Time Series Database](https://www.vldb.org/pvldb/vol8/p1816-teller.pdf) — Pelkonen and coauthors | Which monitoring assumptions justify its representation and availability choices? | Reconstruct timestamp/value encoding and identify a workload or loss contract where the design is unsuitable |

The paper rows name study questions, not a completed replication of their results. Some scans or fetches are imperfect; obtain and read the actual relevant sections before making technical claims or citing them in original research.

For constrained/offline practice, pair these with the [littlefs design](https://github.com/littlefs-project/littlefs/blob/master/DESIGN.md) and [Automerge concepts](https://automerge.org/docs/reference/concepts/). They are living implementation documents, not interchangeable substitutes for a research paper. Use them to connect bounded persistence and synchronization abstractions to real engineering decisions.

## A paper is not finished after a summary

First identify the problem, claim and evidence. Then reconstruct the mechanism/model in enough detail to generate a prediction. Finally challenge an assumption or reproduce a central result when feasible. Reading depth should match the claim you intend to make; no fixed ritual replaces understanding.

For each core reading keep one record:

1. Problem and why it matters to a specific workload.
2. Contribution stated without marketing language.
3. Assumptions about hardware, failures, consistency, resources and data.
4. Mechanism or argument, with an independently drawn diagram/derivation.
5. Evidence supporting each major claim.
6. Strongest alternative explanation and missing comparison.
7. One case where the result should fail or stop applying.
8. Connection to a local source path and any implementation difference.
9. Reproduction status, exact revision, observed discrepancies and uncertainty.
10. Follow-up papers/artifacts and a narrowly phrased new question.

## Build a literature graph rather than a citation pile

Group work by the problem and assumptions: update/read/space tradeoffs; recovery and version retention; scheduling and interference; temporal layout and lateness; causal history and disconnected rejoin. Link competing solutions and identify what changed between them.

When choosing a project, extend its cluster with recent publications, backward references, forward citations and implementation evidence. The seed list cannot establish the current frontier. Record search terms, dates and unresolved closest-work leads before making a novelty claim.

## The first reproduction

Prefer a small model or mechanism with inspectable inputs over an unaffordable full-system claim. A filter-allocation prediction or encoding result may be more tractable initially than reproducing a large deployed system. Decide based on artifact accessibility, mathematical clarity and available hardware, not prestige.

Reconstruct the original result first, then vary one meaningful assumption. Label a reimplementation, analytical reconstruction and artifact rerun differently. Preserve negative results and explain why a discrepancy does or does not challenge the original claim.

## Writing practice

Write a critical review, a mechanism explanation, a reproduction report and a research brief before treating the output as an original paper. These exercises develop different abilities. A well-supported negative or boundary result can guide the next project even if it is not a standalone publication.

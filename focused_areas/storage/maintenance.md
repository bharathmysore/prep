# Evidence, provenance, and maintenance

[Contents](README.md)

## Source scope

The baseline is the local directory `/Users/bmysoren/tpt` on 2026-10-27 in America/Los_Angeles. All 14 top-level projects are represented. Thirteen are Git checkouts; `rockslab-week1` is a non-Git educational directory. [source-snapshot.json](source-snapshot.json) records exact commits, branches, origins, commit dates and observed worktree status. Commit dates describe commits, not feature release dates.

Paxos had an existing untracked `.idea/` directory. The remaining Git checkouts were clean at inspection. No source checkout was edited, refreshed, built or started. Documentation is written separately under `/Users/bmysoren/prep/focused_areas/storage`.

## Evidence levels

- **Direct evidence:** a source contract, implementation branch, test or document at the recorded revision.
- **Synthesis/inference:** explanation connecting evidence, including likely bottlenecks and conceptual operation sequences.
- **Forecast:** a conditional future direction with confidence, counterpressure and a falsification test.
- **Unknown:** an untraced boundary, untested behavior or configuration not established by the evidence.

A citation to a header supports an interface, not every implementation detail. A historical design document can explain intent while containing outdated defaults. A test describes what it checks, not a complete proof. Directory or feature-name presence alone establishes neither maturity nor deployment.

## Source links and index

Dossiers use absolute local paths with one-based line anchors. [source-index.json](source-index.json) records the cited path, line, source text, full-file SHA-256 and referring chapters. The snapshot commit identifies the source revision for each repository. The non-Git lab's cited files are covered by hashes rather than a commit.

These links target the working checkouts and can drift when files change. Hashes reveal drift; they do not preserve the complete source. For a missing old source, use the recorded commit in the existing repository to inspect the historical content with a read-only command, rather than switching or resetting the checkout.

## Updating after source changes

1. Inspect local guidance and worktree state before any source operation. Respect user changes.
2. Record the new commit and diff against the baseline. Do not silently replace the baseline before reviewing changed claims.
3. Re-read changed contracts, code paths, tests and relevant docs. Update anchors from the new content, not by line-number arithmetic.
4. Revise the system dossier, then any affected fundamentals/comparisons/forecast. Name edition, API and option boundaries where they matter.
5. Refresh the snapshot and source index as one coherent evidence baseline. Validate local targets, line bounds and section links.
6. Record newly performed verification separately from conceptual exercises. Update [research gaps](research-gaps.md) when a gap is actually resolved.

Updating these knowledge artifacts does not authorize source code changes, remote writes, benchmarks, cluster operations or recurring monitoring.

## External sources

The evolution research checked selected primary sources on 2026-10-27: RocksDB's remote-compaction wiki, Ceph's development architecture overview, FoundationDB's technical overview, Redis vector-set documentation, and the February 2026 OceanBase Bacchus paper. Links appear beside the claims they support. Online documentation can change independently of the local source snapshot; release/edition applicability must be checked before operational use.

## Validation scope

[Validation report](validation-report.md) records local document coverage and link/hash checks. No claim of dynamic system validation is made. The knowledge base's study scope is architectural explanation with selected implementation traces; [research gaps](research-gaps.md) lists the work needed for deeper operational assurance.

## Expertise expansion

The expertise tracks were added on 2026-10-27 after the learning goal expanded to deep LSM/B+ tree internals, databases, large-scale infrastructure, time-series systems and constrained/disconnected environments. Supplemental sources checked for this expansion: Prometheus storage/data model/instrumentation, the Gorilla VLDB paper, SQLite atomic commit/WAL, littlefs design, Automerge concepts and Ink & Switch local-first principles. Direct links accompany the supported claims. These sources are outside the 14-project local snapshot; the educational labs remain future work.

## Research and leadership expansion

On 2026-10-27 the target expanded to original research and architectural leadership. Added research milestones, experimental/argument standards and seed paper study. Selected paper texts/abstracts and FAST 2027/PVLDB Volume 20 guidance were checked for context. Their dated venue requirements are examples, not a selected submission plan. Some seed full-text retrieval was incomplete and is identified in the paper-study chapter; no exhaustive literature review, novelty determination, experiment or publication is claimed.

## Hands-on learning expansion

Added an implementation workbook on 2026-10-27 after the learner explicitly requested implementing core mechanisms. The workbook connects existing lab specifications to small buildable steps and defines an explainable-lookup starting exercise against the inspected RocksLab API. Collaboration mode remains a learner preference; no new implementation or test pass is claimed. The documented CMake prerequisite was unavailable on the inspected PATH, so no build or alternative command was attempted.

## Canonical storage location

Saved to `/Users/bmysoren/prep/focused_areas/storage` at the user's request on 2026-10-27. This is the canonical location for subsequent documentation, exercises and samples. The earlier copy under `/Users/bmysoren/randoms/storage-knowledge-base` is retained as an earlier snapshot. The existing RocksLab source was copied into `samples/rockslab-week1`; all six copied files are byte-identical to their source at save time. Larger production repositories remain in `/Users/bmysoren/tpt`, and architectural evidence links continue to target those checkouts. `save-manifest.json` records the saved package hashes and original inputs.

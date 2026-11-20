# Validation report

[Contents](README.md)

Validation date: 2026-10-27, America/Los_Angeles. Rechecked after the implementation-workbook expansion.

- Project dossiers: **14/14** discovered projects.
- Markdown chapters including this report: **41**.
- Expertise chapters: **9**, including the incremental implementation workbook, research chapters, specialization tracks and lab portfolio.
- Approximate words excluding this report: **37,462**.
- Local links checked: **626**, including target existence, cited source line bounds and explicit document heading fragments.
- Unique cited source anchors: **324** across **135** source files.
- Source checkout commits/worktree states and previously indexed file hashes match the baseline.
- Link, coverage and source-state errors: **0**.

## Source anchors by project

| Project | Unique source anchors |
|---|---:|
| ceph | 28 |
| cornerstone | 14 |
| cubefs | 36 |
| etcd | 21 |
| foundationdb | 23 |
| leveldb | 28 |
| lmdb | 18 |
| oceanbase | 27 |
| paxos | 13 |
| redis | 22 |
| rocksdb | 22 |
| rockslab-week1 | 21 |
| seaweedfs | 27 |
| tikv | 24 |

## Content review and limits

The original architecture study received a separate bounded source-aware review of shared foundations, comparisons, architecture map, performance reasoning and evolution. Its CubeFS protocol/subsystem correction was applied. The new expertise tracks were checked against selected primary-source readings and reviewed for scope, resource arithmetic, prerequisite order, pass criteria and the distinction between specified and executed labs.

The five new chapters are educational material and experiment specifications. No lab implementation, benchmark, crash test, service startup, deployment or fault injection was performed. Supplemental examples are external readings rather than new local audited repositories.

Citation checks establish path existence and line bounds, not independent proof of every interpretation. External links were read selectively for supported claims, not exhaustively checked for future availability. See [research gaps](research-gaps.md), [maintenance](maintenance.md) and [source index](source-index.json).

## Research expansion verification

The three research chapters were reviewed for conditional claims, prerequisite order, evidence standards and consistency with the existing learning tracks. Candidate questions are explicitly unvalidated for novelty; foundational reading is distinguished from a current exhaustive literature review. Venue guidance is dated and category-specific. Local references, heading links, source anchor bounds, source hashes and checkout states were rechecked with no errors. This verifies the documentation, not research results or competency attainment.

## Implementation workbook verification

Inspected the existing RocksLab README, API header, CMake configuration and test. Checked the first exercise against the API and teaching model: a high memtable limit avoids unintended auto-flush, deletion differs from a never-written key, and current compaction retains tombstones. No new implementation was written. CMake was not found on the inspected PATH; the documented build/test sequence was not executed. Local links, line anchors, indexed source hashes and checkout states passed static verification.

## Save location verification

The complete learning package was prepared for `/Users/bmysoren/prep/focused_areas/storage` on 2026-10-27. It contains 41 Markdown documents, including exercise/sample navigation, and the six original RocksLab files. All relative targets, explicit heading fragments and source line bounds were checked in staging; RocksLab file bytes match their original source. Production source links and the original source snapshot/index retain their evidence targets under `/Users/bmysoren/tpt`. This save does not imply new lab implementations or executed builds/tests. Root prep index coverage has four unrelated pre-existing transcript omissions, which are outside this save's scope.

## Exercise code reference verification

Added direct code references for the implementation ladder and a function-level map of the saved RocksLab sample. Each referenced file and one-based source line was checked, and indexed source hashes were verified. The ladder explicitly marks local time-series/codec/outbox sample gaps; analogues such as TiKV safe-point GC are not presented as complete offline protocols. No sample or production source code was modified, built or executed. The save manifest now preserves initial-save hashes while recording current package hashes.

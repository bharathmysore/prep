# Filesystem learning progress

## Baseline

- Background: experienced engineer.
- Goal: broad storage architecture expertise with implementation depth and AI/ML specialization.
- Started: October 27, 2026.
- Language: C++17 for this prep pack; core concepts are language-independent.

## Milestones

- [ ] Trace namespace, cache, allocation and persistence paths.
- [ ] E1: implement and explain a virtual block device.
- [ ] E2: persistent namespace and checker.
- [ ] E3: deterministic crash recovery and interrupted replay.
- [ ] E4: bounded cache and extent experiments.
- [ ] E5: remote retry and restart semantics.
- [ ] E6: replication, stale epochs, integrity and repair.
- [ ] E7: striped data access and metadata service.
- [ ] E8: dataset layout and checkpoint publication.
- [ ] Compare ext4, XFS, ZFS, NFS, Lustre and CephFS using the same architecture questions.
- [ ] Defend a complete architecture with normal/degraded evidence and cost.

The supplied starter samples are examples, not completion evidence for the learner milestones.

## Session record template

```text
Date / time spent:
Topic / source:
Prediction:
Implementation or experiment:
Observed result:
Invariant or guarantee established:
What remains unproven:
Design consequence:
Next bounded step:
```

## Architecture decision template

```text
Problem and constraints:
Workload and failure model:
Alternatives:
Decision and quantitative rationale:
Durability / consistency / availability / integrity:
Performance and capacity consequences:
Operational and security consequences:
Evidence and limitations:
When to revisit:
```

## Initial checkpoint

Saved the course notes, exercise specifications and three C++ starter examples. Full filesystem, networking, replication, caching and AI workload implementations are learner exercises. Start with README's first session, then E1/E2. Record your own verification results and questions here.

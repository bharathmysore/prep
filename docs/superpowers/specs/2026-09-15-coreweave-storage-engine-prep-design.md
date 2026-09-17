# CoreWeave Staff Storage Engine Prep Design

## Purpose

Create a one-week, repeatable preparation tracker for CoreWeave's Staff Storage Engine role. It must turn the user's practical experience with Unified Storage Pool (USP) and OCI Block Storage into interview-safe, evidence-driven explanations—not a generic storage reading list.

## Audience and scope

The audience is an experienced staff-level storage engineer. The tracker is for seven intensive days (six to eight focused hours per day) and remains useful afterward as a progress ledger. It covers C++ coding, distributed-storage domain depth, AI-storage design, production debugging, and staff-level leadership communication.

## Content model

- The single user-facing tracker lives at `company_positions/coreweave/coreweave_staff_storage_engine_prep.md`.
- It uses stable IDs, unchecked boxes, and explicit evidence/confidence rules. Nothing is marked complete at creation.
- Each day includes: source-to-CoreWeave translation, detailed exercises, timeboxes, required artifacts, a score gate, and a revisit procedure.
- Ownership is threaded through every day: replication, leader election, snapshots, backup, XRR, XRRVG, filesystem work, and background compression.
- Proprietary implementation, host, customer, configuration, and metric details are excluded. The tracker teaches outcome, decision, tradeoff, and invariant-based narratives.

## External role framing

The linked roles emphasize a storage data plane for AI workloads: scalable object/distributed storage, low-latency/high-throughput paths, RDMA/GPU Direct Storage-adjacent technologies, distributed filesystem protocols, durability, observability, Kubernetes, and cross-functional staff leadership. The tracker links to the canonical CoreWeave design guide and existing C++ practice rather than duplicating them.

## Integration requirements

- Add the pack to `company_positions/README.md`, root `INDEX.md`, and `LATEST_UPDATES.md`.
- Do not modify the pre-existing, independently dirty CoreWeave system-design guide.
- Run heading checks, changed-link checks, and root index coverage after editing.

## Success criteria

At the end of the week, the user has a transparent ledger of attempted work and can independently: explain their owned systems safely; solve and test timed C++ exercises; defend AI-storage design choices; diagnose failure/performance scenarios from evidence; and present staff-level decisions with tradeoffs, rollout, and operational posture.

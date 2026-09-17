# CoreWeave Storage Engine Prep Tracker Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a maintainable, one-week CoreWeave Staff Storage Engine preparation tracker grounded in the user's USP and OCI Block Storage ownership.

**Architecture:** A single role-specific tracker contains the active curriculum, exercise specifications, and progress rules. Existing canonical C++ and CoreWeave system-design materials remain the source of detailed solutions and design prompts; workspace navigation files link to the new tracker.

**Tech Stack:** Markdown and existing workspace links.

**Spec:** `docs/superpowers/specs/2026-09-15-coreweave-storage-engine-prep-design.md`

## Global Constraints

- Do not expose proprietary design, customer, operational, or metric details from USP or OCI Block Storage.
- Preserve unrelated existing changes, especially the dirty CoreWeave system-design guide.
- New Markdown files must be linked from `INDEX.md`; material prep updates must be listed in `LATEST_UPDATES.md`.

---

### Task 1: Create the repeatable role tracker

**Files:**
- Create: `company_positions/coreweave/coreweave_staff_storage_engine_prep.md`

**Interfaces:**
- Consumes: the role requirements and the user's stated ownership areas.
- Produces: stable daily exercise IDs and links for future progress updates.

- [ ] Write the tracker with a seven-day calendar, all ownership threads, exercises, rubrics, and evidence rules.
- [ ] Ensure every day has coding, domain, design/debugging, story, and review work.
- [ ] Verify no checklist is checked by default and no proprietary implementation detail appears.

### Task 2: Link the tracker into navigation

**Files:**
- Modify: `company_positions/README.md`
- Modify: `INDEX.md`
- Modify: `LATEST_UPDATES.md`

**Interfaces:**
- Consumes: tracker path and title from Task 1.
- Produces: discoverable workspace navigation.

- [ ] Add the pack to the company-position table and root company-position section.
- [ ] Add a dated, concise design/coding update row that links to the tracker.
- [ ] Check all new relative links resolve from their containing documents.

### Task 3: Verify documentation integrity

**Files:**
- Verify: `company_positions/coreweave/coreweave_staff_storage_engine_prep.md`
- Verify: `INDEX.md`

- [ ] Run `rg '^## ' company_positions/coreweave/coreweave_staff_storage_engine_prep.md` and inspect section coverage.
- [ ] Run the root Markdown coverage check from `AGENTS.md`; expect no missing file paths.
- [ ] Review `git diff --check` and report only files intentionally changed for this task.

# Latest Coding And Design Question Updates

Use this as the quick navigation page for the most recently expanded coding and system-design question material in this workspace.

## Coding Updates

| Update | What Changed | Start Here |
| --- | --- | --- |
| Circular queue implementations — 2026-09-17 | C++17 single-threaded and mutex-protected MPMC FIFOs, shared ring logic, snapshot/lifetime rules, wraparound walkthrough, and 18 tests. Both suites passed normal, optimized, and ASan/UBSan builds; the concurrent suite also passed TSan. | [Teaching solution](./coding/cpp/stacks_queues/solutions.md#7-fixed-capacity-circular-queue) and [test cases](./coding/cpp/stacks_queues/test_cases.md#7-fixed-capacity-circular-queue) |
| Runnable leader-election simulator — 2026-09-16 | Implemented the Day 2 term-based C++ simulator with executable checks for delayed votes, stale heartbeats, no-quorum partitions, crash-persisted votes, and former-leader write fencing. | [Implementation](./coding/cpp/distributed_systems_algorithms/leader_election_simulator.cpp) and [exercise entry](./coding/cpp/distributed_systems_algorithms/solutions.md#6-simplified-term-based-leader-election) |
| Linux storage ramp teaching mode — 2026-09-16 | Made explanations, worked examples, guided coding/labs, and supported practice the default across all days. Strict independent checks are opt-in; taught, guided, and independently demonstrated evidence remain separate. All 53 IDs, dates, scope, and hour budgets are preserved. | [Teaching contract](./focused_areas/linux_storage_principal_ramp.md#teaching-mode--applies-to-the-entire-ramp) and [exercise book](./focused_areas/linux_storage_principal_exercises.md) |
| CoreWeave Staff Storage Engine intensive — 2026-09-15 | Added a seven-day, repeatable staff-storage tracker with timed C++ exercises, AI storage design and incident drills, rubrics, and safe translations of USP/OCI Block Storage ownership across replication, leader election, snapshots, backup, XRR/XRRVG, filesystem, and compression. | [Seven-day tracker](./company_positions/coreweave/coreweave_staff_storage_engine_prep.md) |
| Linux storage principal ramp — 2026-09-15 | Aligned to JR2024421's 90-minute screening: 53 tracked items, C/C++ and kernel-C practice, three budget-integrated mocks, September 25 readiness, and September 26–27 review. | [Dated ramp tracker](./focused_areas/linux_storage_principal_ramp.md#deadline-calendar) and [screening rehearsals](./focused_areas/linux_storage_principal_exercises.md#ninety-minute-screening-rehearsals) |
| USP / OCI Block Storage coding bridges — 2026-09-15 | Added local source evidence for asynchronous ownership, replication, filesystem/SPDK boundaries, snapshot publication, and compression; retained existing coding scope and time budgets. | [Project evidence](./focused_areas/linux_storage_project_evidence.md#source-evidence-ledger) and [coding bridges](./focused_areas/linux_storage_project_evidence.md#project-stories-and-coding-bridges) |
| Weekly coding catalog coverage refresh | Added missing `questions.md` prompts and mirrored pattern-map tags for already-implemented arrays/strings, dynamic programming, graphs, heaps/ordered structures, and trees/tries solutions. | [C++ category index](./coding/cpp/README.md#categories) |
| Apple coding code index | Added an Apple-specific map from every applicable coding question to the exact implemented C++ code heading and tests. | [Apple Coding Code Index](./company_positions/apple/apple_coding_code_index.md) |
| Focused rate limiter coding pack | Added a focused area covering fixed window, sliding window log, sliding window counter, token bucket, leaky bucket, GCRA, keyed buckets, complexities, performance improvements, and C++ implementations. | [Rate Limiters](./focused_areas/rate_limiters.md) |
| Apple Elastic Disk coding/design shortlist | Moved the company/position-specific top-20 sheet into the dedicated company-position prep tree and linked each coding/design row back to canonical prompts. | [Apple Elastic Disk Staff/Principal Top 20 Questions](./company_positions/apple/apple_elastic_disk_staff_principal_top_20.md) |
| Top 20 coding focus | Added a short-list view for top questions across categories, across tracked companies, and per company. | [C++ Top 20 Coding Question Focus](./coding/cpp/top_20.md) |
| Company-specific coding focus | Each implemented solution has company-frequency tags, and the company index ranks public signals separately from L7 domain-fit signals. | [C++ Company Frequency Focus Index](./coding/cpp/company_frequency.md) |
| Question summaries in solutions | Every implemented C++ solution now states the actual prompt before the code. | [C++ L7 Coding Prep](./coding/cpp/README.md) |
| External test cases | Concrete test cases live in per-category `test_cases.md` files and are linked from solution entries. | [C++ category index](./coding/cpp/README.md#categories) |

## Coding Question Index

| Category | Questions | Solutions | Test Cases |
| --- | --- | --- | --- |
| Advanced data structures | [questions](./coding/cpp/advanced_data_structures/questions.md) | [solutions](./coding/cpp/advanced_data_structures/solutions.md) | [test cases](./coding/cpp/advanced_data_structures/test_cases.md) |
| Arrays and strings | [questions](./coding/cpp/arrays_strings/questions.md) | [solutions](./coding/cpp/arrays_strings/solutions.md) | [test cases](./coding/cpp/arrays_strings/test_cases.md) |
| Backtracking | [questions](./coding/cpp/backtracking/questions.md) | [solutions](./coding/cpp/backtracking/solutions.md) | [test cases](./coding/cpp/backtracking/test_cases.md) |
| Binary search | [questions](./coding/cpp/binary_search/questions.md) | [solutions](./coding/cpp/binary_search/solutions.md) | [test cases](./coding/cpp/binary_search/test_cases.md) |
| Concurrency | [questions](./coding/cpp/concurrency/questions.md) | [solutions](./coding/cpp/concurrency/solutions.md) | [test cases](./coding/cpp/concurrency/test_cases.md) |
| Distributed systems algorithms | [questions](./coding/cpp/distributed_systems_algorithms/questions.md) | [solutions](./coding/cpp/distributed_systems_algorithms/solutions.md) | [test cases](./coding/cpp/distributed_systems_algorithms/test_cases.md) |
| Dynamic programming | [questions](./coding/cpp/dynamic_programming/questions.md) | [solutions](./coding/cpp/dynamic_programming/solutions.md) | [test cases](./coding/cpp/dynamic_programming/test_cases.md) |
| Graphs | [questions](./coding/cpp/graphs/questions.md) | [solutions](./coding/cpp/graphs/solutions.md) | [test cases](./coding/cpp/graphs/test_cases.md) |
| Hashing | [questions](./coding/cpp/hashing/questions.md) | [solutions](./coding/cpp/hashing/solutions.md) | [test cases](./coding/cpp/hashing/test_cases.md) |
| Heaps and ordered structures | [questions](./coding/cpp/heaps_ordered_structures/questions.md) | [solutions](./coding/cpp/heaps_ordered_structures/solutions.md) | [test cases](./coding/cpp/heaps_ordered_structures/test_cases.md) |
| Linked lists | [questions](./coding/cpp/linked_lists/questions.md) | [solutions](./coding/cpp/linked_lists/solutions.md) | [test cases](./coding/cpp/linked_lists/test_cases.md) |
| Parallel algorithms | [questions](./coding/cpp/parallel_algorithms/questions.md) | [solutions](./coding/cpp/parallel_algorithms/solutions.md) | [test cases](./coding/cpp/parallel_algorithms/test_cases.md) |
| Stacks and queues | [questions](./coding/cpp/stacks_queues/questions.md) | [solutions](./coding/cpp/stacks_queues/solutions.md) | [test cases](./coding/cpp/stacks_queues/test_cases.md) |
| Systems-style coding | [questions](./coding/cpp/systems_style/questions.md) | [solutions](./coding/cpp/systems_style/solutions.md) | [test cases](./coding/cpp/systems_style/test_cases.md) |
| Trees and tries | [questions](./coding/cpp/trees_tries/questions.md) | [solutions](./coding/cpp/trees_tries/solutions.md) | [test cases](./coding/cpp/trees_tries/test_cases.md) |

## Design Updates

| Update | What Changed | Start Here |
| --- | --- | --- |
| Linux storage coached design and project lessons — 2026-09-16 | Changed designs, eight project-feature drills, reviews, and screening rehearsals to teaching-first. Retained the eight-part answer structure and evidence requirements; no automatic switch to closed-book assessment. | [Teaching contract](./focused_areas/linux_storage_principal_ramp.md#teaching-mode--applies-to-the-entire-ramp) and [project lessons](./focused_areas/linux_storage_project_evidence.md#eight-owned-feature-drills) |
| Linux storage principal design drills — 2026-09-15 | Preserved all seven design/incident drills; integrated full-length screening mocks, concise technical stories, September 24 all-seven stress test, and September 25 acceptance within the existing budget. | [Screening priorities](./focused_areas/linux_storage_principal_ramp.md#screening-target-and-priorities) and [qualification/review sessions](./focused_areas/linux_storage_principal_exercises.md#deadline-qualification-and-review-sessions) |
| USP / OCI Block Storage ownership drills — 2026-09-15 | Incorporated all eight user-confirmed areas: replication, leader election, snapshot, backup, XRR, XRRVG, filesystem work, and background compression; added failure/rollout/rollback/leadership prompts and personal-evidence boundaries. | [Owned-feature drills](./focused_areas/linux_storage_project_evidence.md#eight-owned-feature-drills) and [dated integration](./focused_areas/linux_storage_project_evidence.md#integration-into-the-dated-plan) |
| Kubernetes DRA GPU scheduler refresh | Deepened the NVIDIA and CoreWeave GPU scheduler prompts with Kubernetes Dynamic Resource Allocation, `DeviceClass`/`ResourceClaimTemplate` workflows, Kueue quota accounting, MIG/shared-device charging, binding-timeout recovery, and topology/preemption caveats. | [NVIDIA topology-aware GPU scheduler](./system_design/nvidia_l7_system_design_prep.md#12-design-a-topology-aware-gpu-job-scheduler) |
| OpenAI agent-ready workflow automation platform | Added a Codex-style prompt for reusable workflow automation across CLI jobs, SDK/API callers, app-server sessions, WebMCP browser tools, durable notebooks, approvals, and run history. | [Design An Agent-Ready Workflow Automation Platform](./system_design/openai_l7_system_design_prep.md#22-design-an-agent-ready-workflow-automation-platform) |
| AWS AgentCore policy and traffic gateway refresh | Deepened the AWS enterprise agent platform prompt with stateful temporal policy, gateway-level AI traffic limits, layered quotas, retry-after signals, and high-risk tool gates. | [Design An Enterprise AI Agent Platform](./system_design/aws_l7_system_design_prep.md#21-design-an-enterprise-ai-agent-platform) |
| NVIDIA shared GPU tenant and scheduler refresh | Deepened the NVIDIA multi-tenant GPU cloud and topology-aware scheduler prompts with KAI-style queue guarantees, virtual tenant control planes, fractional GPU policy, stable scheduler snapshots, consolidation/reclaim/preemption, and DCGM health-aware placement. | [Design A Topology-Aware GPU Job Scheduler](./system_design/nvidia_l7_system_design_prep.md#12-design-a-topology-aware-gpu-job-scheduler) |
| Anthropic channel-scoped team agent deep dive | Deepened the long-running agentic coding platform prompt with Claude Tag-style channel identities, scoped memory, ambient follow-ups, spend caps, tool grants, and audit logs. | [Design An Agentic Coding Platform For Long-Running Cloud Workflows](./system_design/anthropic_l7_system_design_prep.md#21-design-an-agentic-coding-platform-for-long-running-cloud-workflows) |
| Snowflake governed enterprise agent data platform | Added a Snowflake-style prompt covering Cortex Agents, Snowflake Intelligence, CoCo/Cortex Code, governed structured and unstructured tools, code sandboxes, MCP/custom tools, thread state, evaluations, cost controls, and user-role execution boundaries. | [Design A Governed Enterprise Agent Data Platform](./system_design/snowflake_l7_system_design_prep.md#21-design-a-governed-enterprise-agent-data-platform) |
| Microsoft Foundry agent runtime and tool platform | Added a Microsoft-style prompt covering hosted agent sessions, Toolboxes/MCP governance, Entra Agent ID, managed memory and knowledge, model routing, Teams/Copilot publishing, trace-linked evaluations, and high-risk tool approval. | [Design A Microsoft Foundry Agent Runtime And Tool Platform](./system_design/microsoft_l7_system_design_prep.md#21-design-a-microsoft-foundry-agent-runtime-and-tool-platform) |
| CoreWeave cross-cloud AI training control plane | Added a CoreWeave-style prompt covering SUNK/Slurm-on-Kubernetes federation, cross-cloud placement, identity provisioning, data locality, LOTA-style cache planning, local reconcilers, straggler observability, and rack-scale failure boundaries. | [Design A Cross-Cloud AI Training Control Plane](./system_design/coreweave_l7_system_design_prep.md#21-question-design-a-cross-cloud-ai-training-control-plane) |
| AWS enterprise AI agent platform | Added an AWS AgentCore-style platform prompt covering hosted agent runtime, tool gateway, identity broker, memory, evaluation, rollout, audit, and side-effect controls. | [Design An Enterprise AI Agent Platform](./system_design/aws_l7_system_design_prep.md#21-design-an-enterprise-ai-agent-platform) |
| Meta heterogeneous AI inference platform | Added a Meta-style AI infrastructure prompt for routing ranking, recommendation, and GenAI inference across GPUs, MTIA-style accelerators, CPU fallback, model artifacts, capacity policy, observability, and rollout. | [Design A Heterogeneous AI Inference Platform](./system_design/meta_l7_system_design_prep.md#21-design-a-heterogeneous-ai-inference-platform) |
| Anthropic agentic coding platform | Added an Anthropic-style cloud coding-agent prompt covering long-running sessions, routines, subagents, agent teams, hooks, MCP tools, permission gates, worktree isolation, resumable memory, audit, and artifact handoff. | [Design An Agentic Coding Platform For Long-Running Cloud Workflows](./system_design/anthropic_l7_system_design_prep.md#21-design-an-agentic-coding-platform-for-long-running-cloud-workflows) |
| Databricks Lakeflow Jobs scheduler refresh | Deepened the Databricks job scheduler prompt with current Lakeflow-style DAG definitions, queueing, concurrency limits, repair runs, task leases, compute dispatch, streaming backlog observability, and system-table analysis. | [Design A Data Pipeline Job Scheduler](./system_design/databricks_l7_system_design_prep.md#8-design-a-data-pipeline-job-scheduler) |
| Google TPU accelerator control plane | Added a Google-style L7 design prompt for TPU/Ironwood-style reservations, topology-aware placement, health visibility, maintenance, inference collections, and all-capacity tradeoffs. | [Design A TPU / AI Accelerator Cluster Control Plane](./system_design/google_l7_system_design_prep.md#21-design-a-tpu-ai-accelerator-cluster-control-plane) |
| NVIDIA Dynamo disaggregated inference refresh | Deepened the prefill/decode serving prompt with current Dynamo-style routing, KV transfer, planner/autoscaler, topology-aware placement, fallback semantics, and low-level invariants. | [Design Disaggregated LLM Serving](./system_design/nvidia_l7_system_design_prep.md#4-design-disaggregated-llm-serving-with-separate-prefill-and-decode-workers) |
| Focused rate limiter design pack | Added a cross-company design drill set plus a distributed quota-leasing answer for global rate limiting, tenant fairness, failure behavior, and observability. | [Rate Limiters design drills](./focused_areas/rate_limiters.md#design-drill-set) |
| Apple Elastic Disk top-20 | Added a ranked mixed coding/design prep sheet for Apple Staff/Principal Elastic Disk interviews, including coding solutions, complexities, performance improvements, tradeoffs, and canonical question links. | [Apple Elastic Disk Staff/Principal Top 20 Questions](./company_positions/apple/apple_elastic_disk_staff_principal_top_20.md) |
| Apple Elastic Disk Staff/Principal focus | Added a role-specific prep section plus storage design drills for EBS-class block storage, replication and metadata, snapshots, scrub and repair, I/O QoS, and cross-org replication foundations. | [Apple Elastic Disk focus](./system_design/apple_l7_system_design_prep.md#apple-elastic-disk-staffprincipal-focus) |
| All design questions TOC | Added a question-level table of contents for every tracked company system-design prompt. | [System Design Question TOC](./system_design/design_questions_toc.md) |
| OpenAI agent tool-use control plane | Added an OpenAI-style L7 prompt for governing hosted tools, remote MCP, customer functions, file/web search, computer-use sandboxes, policy, secrets, approvals, and audit. | [Design An Agent Tool-Use Control Plane](./system_design/openai_l7_system_design_prep.md#21-design-an-agent-tool-use-control-plane) |
| Low-level component detail | Design answers should now include core component internals: APIs, ownership, state, schemas, algorithms, invariants, concurrency, and failure handling. | [Design question answer guidance](./AGENTS.md#design-question-answer-guidance) |
| GPU scheduler deep dive | The CoreWeave GPU scheduler question now has detailed Scheduler Core, Placement Planner, gang scheduling, bin packing, and multi-GPU start semantics. | [Design a GPU Cluster Scheduler](./system_design/coreweave_l7_system_design_prep.md#1-question-design-a-gpu-cluster-scheduler) |
| Scheduler Core specifics | Details queue policy, atomic reservations, job state transitions, gang admission, and start semantics. | [Scheduler Core Deep Dive](./system_design/coreweave_l7_system_design_prep.md#scheduler-core-deep-dive) |
| Placement Planner specifics | Details candidate filtering, topology-aware scoring, fragmentation control, and bin-packing tradeoffs. | [Placement Planner Deep Dive](./system_design/coreweave_l7_system_design_prep.md#placement-planner-deep-dive) |

## Design Question Index

- [All System Design Questions TOC](./system_design/design_questions_toc.md)

| Company | Design Question Guide |
| --- | --- |
| Anthropic | [Anthropic L7 System Design Prep](./system_design/anthropic_l7_system_design_prep.md) |
| Apple | [Apple L7 System Design Prep](./system_design/apple_l7_system_design_prep.md) |
| AWS | [AWS L7 System Design Prep](./system_design/aws_l7_system_design_prep.md) |
| CoreWeave | [CoreWeave-Style L7 System Design Prep](./system_design/coreweave_l7_system_design_prep.md) |
| Databricks | [Databricks L7 System Design Prep](./system_design/databricks_l7_system_design_prep.md) |
| Google | [Google L7 System Design Prep](./system_design/google_l7_system_design_prep.md) |
| Meta | [Meta L7 System Design Prep](./system_design/meta_l7_system_design_prep.md) |
| Microsoft | [Microsoft L7 System Design Prep](./system_design/microsoft_l7_system_design_prep.md) |
| NVIDIA | [NVIDIA L7 System Design Prep](./system_design/nvidia_l7_system_design_prep.md) |
| OpenAI | [OpenAI L7 System Design Prep](./system_design/openai_l7_system_design_prep.md) |
| Oracle | [Oracle L7 System Design Prep](./system_design/oracle_l7_system_design_prep.md) |
| Snowflake | [Snowflake L7 System Design Prep](./system_design/snowflake_l7_system_design_prep.md) |
| Stripe | [Stripe L7 System Design Prep](./system_design/stripe_l7_system_design_prep.md) |

## Maintenance Checklist

- When adding or materially changing coding question catalogs, solutions, company tags, top-20 lists, or test cases, update the Coding Updates section.
- When adding or materially changing system-design question content or answer-format guidance, update the Design Updates section.
- During the weekly review, inspect all existing coding and design question files and update this index before finishing so it reflects the newest useful prep entry points.
- Keep entries focused on surfaces that changed recently or are currently most useful for practice; avoid turning this into a duplicate of the full root index.
- Keep this file linked from [L7 Interview Prep Index](./INDEX.md).

# Cornerstone: Raft consensus and integration interfaces

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: `/Users/bmysoren/tpt/cornerstone`, HEAD `eb053fcff3ffa271be2c5f31d6a58c511b493f85`, read on 2026-10-27. This is a C++ Raft library, not a finished storage service. Its README explicitly says the filesystem log store is not for production ([warning](/Users/bmysoren/tpt/cornerstone/README.md:11)) and that an application supplies the state machine ([boundary](/Users/bmysoren/tpt/cornerstone/README.md:27)). Static review only.

## Build from consensus fundamentals

Terms identify election epochs. An elected leader orders entries. Followers reconcile logs by position/term. Majority replication determines a committed prefix under Raft's rules. Applying that prefix to deterministic application state creates a replicated service. Snapshots replace an already-applied log prefix; durable term/vote, log, snapshot and application state must agree during restart.

Cornerstone separates consensus core, log store, state manager, state machine, RPC, timer and logger. `raft_server` coordinates these interfaces rather than implementing a B+tree/LSM engine itself. Its application contract includes pre-commit, commit, rollback, snapshot creation/loading and the last committed index ([state machine](/Users/bmysoren/tpt/cornerstone/include/state_machine.hxx:26)). Pre-commit is not proof of final consensus commit.

## Lifecycle

- **Election:** timeouts lead to candidate transitions and vote requests; pre-vote is an optional path ([timeout](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:174), [candidate](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:238)). A term alone does not mean the node may safely acknowledge an application write.
- **Write:** `append_entries` submits application buffers through the leader path ([client entry](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:960)); the leader constructs append requests and tracks follower matched indexes ([requests](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:638), [response bookkeeping](/Users/bmysoren/tpt/cornerstone/src/raft_server_resp_handlers.cxx:104)).
- **Apply:** background commit advances from the applied index toward the committed index and invokes application commit callbacks for application entries; configuration entries take a separate path ([apply loop](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:882)). An integration must define whether a client success means appended, replicated, committed or applied.
- **Recover:** constructor obtains saved server state, log and state-machine commit index, then checks uncommitted configuration history ([startup](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:49)). Durable state manager/log/application implementations remain part of the crash-safety boundary.
- **Delete/reclaim:** user deletes would be application commands. Consensus log reclamation uses snapshots and compaction, which must preserve recovery and lagging-follower catch-up ([snapshot coordination](/Users/bmysoren/tpt/cornerstone/src/raft_server.cxx:540), [log compact](/Users/bmysoren/tpt/cornerstone/src/fs_log_store.cxx:644)). These are different operations.
- **Read:** a storage read API and its consistency contract must be supplied by the application. No claim of automatic linearizable local reads follows from having a Raft leader.

## Durability, cost and evolution

The demo store calls C++ stream `flush` after appending ([append](/Users/bmysoren/tpt/cornerstone/src/fs_log_store.cxx:276)); that alone does not guarantee physical persistence. The README explicitly cites missing file/data sync and transactional consistency across three files ([reason](/Users/bmysoren/tpt/cornerstone/README.md:40)). A production adapter is therefore an architectural requirement, not a tuning option.

Latency depends on leader work, quorum network/storage completion and application apply speed. Slow followers increase retained-log/snapshot traffic; snapshot creation competes with foreground work. Membership changes complicate quorum interpretation and cannot be reduced to editing a server list.

Exercises: label each write milestone, compare pre-commit and commit callbacks, then draw restart after snapshot publication but before log-prefix deletion. Conditional evolution: production use requires stronger persistence adapters, explicit read/client completion contracts, bounded apply queues and validated membership/snapshot recovery. These are integration pressures; correctness of the full membership algorithm and transport was not proved here.

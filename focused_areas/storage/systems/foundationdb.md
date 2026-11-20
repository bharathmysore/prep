# FoundationDB: a transactional ordered keyspace with separated roles

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

## Scope and evidence

Local snapshot `1f6fd50214ce2d93fbc9e6305246a5992ae69b22`, clean when inspected. Bounded source/doc review only; no builds, simulation, or live-cluster checks. Local documentation includes historical details: architecture.rst still describes Redwood as an upcoming 7.0 engine. Use current source and the local feature-status document to qualify that text rather than projecting its release wording forward. [historical wording](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:124)

## Primitives before architecture

- **Ordered bytes:** keys and values form a global ordered KV space. Higher-level data models are application layers over that interface, not a separate mandatory SQL server. [layer model](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:5)
- **Version:** a transaction reads a consistent read version; mutations remain client-side until commit. Read-your-writes overlays local mutations. [client flow](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/kv-architecture.rst:61)
- **Conflict range:** optimistic concurrency checks a transaction's read conflict ranges against writes committed after its read version. A read-only transaction does not need a normal commit. Default transactions offer strict serializability; snapshot reads omit read conflict ranges and require care. [ACID/MVCC](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/developer-guide.rst:339), [conflict controls](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/developer-guide.rst:605)
- **Mutation log and materialized storage are separate:** TLogs make mutations durable; storage servers consume mutations and serve versioned reads. Commit durability need not wait for every storage server's local engine to materialize the update. [logs](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:94)
- **Generation:** a transaction subsystem is replaced through coordinated recovery. This is distinct from moving a key range or replacing an ordinary storage server. [recovery triggers](/Users/bmysoren/tpt/foundationdb/design/recovery-internals.md:15)

## Compose the roles

Coordinators bootstrap discovery and elect the cluster controller; the controller recruits roles and distributes topology. GRV proxies supply read versions and admission control; the master sequences commit versions; commit proxies batch transactions; resolvers detect conflicts; TLogs persist mutations; storage servers serve key ranges. Data Distributor manages storage placement and repair, while Ratekeeper controls admission to avoid runaway backlog. [role topology](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:24), [distribution and admission](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:136)

The system therefore scales different duties independently, but logical separation does not imply unlimited scaling: sequencing, metadata distribution, fanout, and shared queues remain coordination costs. This is an **inference** from the role interfaces.

## Read, write, and recovery paths

**Read:** obtain a GRV → locate storage replicas for a key range using cached metadata → read at the transaction version → merge local writes. Stale location metadata causes refresh/retry. Storage must retain that version; an old transaction can fail with `transaction_too_old`. The docs' five-second examples explain bounded history but should not be treated as immutable values across configurations. [routing](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:85), [retention](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:124)

**Commit:** client sends mutations plus conflict ranges → commit proxy gets a commit version → resolvers validate conflicts → accepted mutations become durable in TLogs → success is returned. Storage servers apply the log stream independently. This is transaction coordination over a decoupled log/storage architecture, not a per-key-range Raft group. [commit sequence](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:71), [resolver request construction](/Users/bmysoren/tpt/foundationdb/fdbserver/commitproxy/CommitProxyServer.cpp:104)

**Recovery:** read/lock coordinated state, stop old logs from accepting new writes, determine a safe recovery boundary, recruit a new generation, and copy required log history. Exclusive coordinated state and log locking prevent competing generations accepting commits. A storage-server failure is handled by data distribution and need not force transaction-system recovery. [recovery phases](/Users/bmysoren/tpt/foundationdb/design/recovery-internals.md:48), [fencing](/Users/bmysoren/tpt/foundationdb/design/recovery-internals.md:67), [log-boundary example](/Users/bmysoren/tpt/foundationdb/design/recovery-internals.md:99)

**Client uncertainty:** a failed response does not prove the commit failed. Retrying non-idempotent application side effects can duplicate work; the client retry loop and unknown-result handling belong in the architecture. [unknown results](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/developer-guide.rst:566)

## Engine and runtime boundaries

Current `openKVStore` selects SQLite B-tree, memory, Redwood, RocksDB, sharded RocksDB, and memory radix-tree variants; engine existence does not establish the selected production engine. [factory](/Users/bmysoren/tpt/foundationdb/fdbserver/kvstore/IKeyValueStore.cpp:25)

The local repo instructions document cooperative single-threaded C++ coroutines and deterministic simulation. Concurrency safety depends on yield/cancellation and object lifetime as well as distributed protocol safety. This checkout uses ordinary `.cpp` coroutines; older actor-file descriptions need translation. [runtime and subsystem map](/Users/bmysoren/tpt/foundationdb/AGENTS.md:68)

## Delete and version reclamation

Deletes are transactional mutations, including range clears. The storage-server path handles `ClearRange` mutations, while its MVCC state tracks `oldestVersion` and explicitly forgets earlier versions. A logical clear, history expiration, local-engine byte reclamation, and TLog popping are separate lifecycle events. [clear mutation handling](/Users/bmysoren/tpt/foundationdb/fdbserver/storageserver/StorageServer.cpp:11572), [version retention and forgetting](/Users/bmysoren/tpt/foundationdb/fdbserver/storageserver/StorageServer.cpp:1627). **Inference:** a successful clear does not imply an immediate proportional decrease in disk usage; engine layout and retained versions matter. The source review here has not audited each engine's physical reclamation behavior. Compare Redwood and RocksDB before forecasting delete-heavy space amplification.

## Bottlenecks and predictions

**Derived hypotheses:** hot conflict ranges amplify retries even if disk is idle; delayed storage persistence retains TLog data and eventually throttles the transaction system; a slow commit pipeline can dominate despite fast storage reads. Ratekeeper's GRV admission point makes overload control visible as time obtaining a read version, not necessarily a disk latency spike. [Ratekeeper](/Users/bmysoren/tpt/foundationdb/documentation/sphinx/source/architecture.rst:146)

**Observed local signals:** feature-status labels RocksDB production, sharded RocksDB experimental with file-copy motivations, range-oriented backup evolution, bulk loading/dumping, and version-vector work intended to reduce TLog broadcast. These labels are local author statements rather than independently verified release support. [feature status](/Users/bmysoren/tpt/foundationdb/design/feature-status.md:8)

**Forecast, medium confidence:** reducing movement/restore amplification through files and key-range partitioning is a strong direction; reducing transaction-system fanout is another. Maturity and maintenance ownership can veto technically attractive features: the same status document lists removed experimental features and ownership gaps. No dates or guaranteed roadmap are inferred. [future intents and removals](/Users/bmysoren/tpt/foundationdb/design/feature-status.md:17)

## Reading route and exercises

Read the API transaction contract → NativeAPI/read-your-writes → commit proxy/resolver → TLog/logsystem → storage-server MVCC/engine → recovery → Data Distributor/Ratekeeper. Treat AI-generated subsystem maps as navigation aids and verify claims in primary code.

1. Explain why a committed mutation can be durable before storage-server materialization.
2. Draw two transactions that conflict through a range read, then show how snapshot reads can remove the conflict and weaken the invariant.
3. Compare a failed storage server with a failed commit proxy. Which causes repair and which changes transaction generation?
4. Derive why log retention grows if storage servers stop advancing durable versions.
5. Design an idempotent application operation for `commit_unknown_result`; identify state outside the database that retries cannot roll back.

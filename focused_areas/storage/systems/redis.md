# Redis: an in-memory data-structure server with persistence and replication

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: `/Users/bmysoren/tpt/redis`, HEAD `33c2a2a145d3f09b42503df27d3ffbc76f34e32c`, read on 2026-10-27. Production network server, not an embedded disk lookup engine. The README describes data structures, caching, modules and query capabilities ([scope](/Users/bmysoren/tpt/redis/README.md:37)). Static review covers core storage/persistence paths; optional search modules are not fully analyzed.

## Build from fundamentals

A key dictionary maps names to typed objects; those objects use encodings suited to size and operations. The command layer supplies operations such as counters, lists and sorted sets. An event loop schedules network requests. Expiration and eviction enforce logical lifetime and memory policy. Persistence serializes snapshots or command history; replication distributes a command stream. These are separate mechanisms with different guarantees.

Core command execution is coordinated by `processCommand` and `call` ([dispatch](/Users/bmysoren/tpt/redis/src/server.c:4691), [execution](/Users/bmysoren/tpt/redis/src/server.c:4182)). Background I/O, persistence children and module work mean “everything is single threaded” is too broad; the command/state path remains the key serialization boundary to inspect.

## Follow data through the system

- **Read:** a command resolves its key through lookup helpers, including expiration/lookup policies, then operates on its object's encoding ([lookup](/Users/bmysoren/tpt/redis/src/db.c:372)). Ordinary core reads access RAM structures, not an SST or B+tree on disk.
- **Write:** the command mutates memory and propagation machinery feeds replicas and, when configured, the AOF ([AOF feed](/Users/bmysoren/tpt/redis/src/aof.c:1671), [replication](/Users/bmysoren/tpt/redis/src/replication.c:656)). A command reply is not universally equivalent to disk sync or quorum durability.
- **Persistence:** RDB writes a snapshot, including a background save path ([RDB save](/Users/bmysoren/tpt/redis/src/rdb.c:2317)). AOF serializes commands, buffering and syncing under policy ([flush](/Users/bmysoren/tpt/redis/src/aof.c:1409)). The local config describes a multipart base/incremental-file manifest and defaults `appendonly no` ([format](/Users/bmysoren/tpt/redis/redis.conf:1545)). AOF `everysec` is the displayed fsync policy when AOF is enabled ([policy](/Users/bmysoren/tpt/redis/redis.conf:1603)); it is not a hard loss bound under every failure/stall.
- **Recovery:** load the selected RDB or AOF dataset; multipart AOF loading is explicit ([AOF load](/Users/bmysoren/tpt/redis/src/aof.c:2132), [RDB load](/Users/bmysoren/tpt/redis/src/rdb.c:5258)). Loading/rewrite cost depends on dataset and chosen format.
- **Delete/expire:** dictionary removal may free synchronously or delegate freeing; logical absence can precede memory reclamation ([delete](/Users/bmysoren/tpt/redis/src/db.c:883)). Expiration combines lookup-time handling with active work ([active expiration](/Users/bmysoren/tpt/redis/src/expire.c:295)). Eviction under memory pressure is another policy, distinct from TTL.

## Internal structures and execution ownership

In this checkout, `redisDb` contains `kvstore *keys` for the keyspace, a separate `kvstore *expires` for key timeouts, subkey expiration state, and dictionaries for blocked and watched keys ([database layout](/Users/bmysoren/tpt/redis/src/server.h:1268)). The exact type matters: describing this tree solely using older Redis releases' `dict *dict` field would miss its current layout. Conceptually, key indexing, expiration scheduling, blocking clients and transaction watches are different indexes over related state.

The object behind a key is a typed value, and its physical encoding may differ from the API's logical type. Compact representations save allocations and improve locality for small objects; more general representations support larger or more dynamic objects. Encoding transitions can therefore create nonlinear latency and memory behavior even when a client keeps issuing the same command family. Strings demonstrate the separation: `GET` looks up a key, verifies string type and emits the object as a bulk reply ([GET path](/Users/bmysoren/tpt/redis/src/t_string.c:456)). Its semantics do not expose the allocation strategy to clients.

`aeMain` repeatedly processes readiness events and before/after-sleep work ([event loop](/Users/bmysoren/tpt/redis/src/ae.c:497)). Core command execution and key mutation are coordinated through the server's execution path, while I/O, freeing and persistence can use other execution contexts. A long command can monopolize the command path even if network reads are accelerated elsewhere. A background RDB/AOF operation can avoid executing all serialization inline but still create disk contention and copy-on-write memory pressure. This is why CPU utilization across several cores does not, by itself, establish that one hot shard can execute commands in parallel.

Redis ordinarily treats RAM as the serving dataset, rather than an explicitly bounded disk-block cache. Eviction removes dataset keys according to policy; it is not just throwing away a reconstructible SST block. Whether that is acceptable depends on the application treating Redis as a cache, a primary store, or a mixed role. Persistence and replication do not erase the semantic difference between eviction and a read-cache miss.

## Worked history: a TTL write and two kinds of acknowledgment

Assume a primary with one replica, AOF enabled with the local `everysec` policy, and a client issuing commands sequentially.

1. `SET session:7 token PX 5000` enters the string command path. The implementation parses expiry, checks write conditions and resolves the existing key before assigning the new value ([SET implementation](/Users/bmysoren/tpt/redis/src/t_string.c:87)). The five seconds is the key's logical lifetime, not a persistence interval.
2. The command changes RAM state and is propagated to the AOF/replication machinery. A successful SET response alone does not prove that an AOF fsync or replica acknowledgment has completed.
3. The client issues `GET session:7` before expiration and gets `token`. Lookup also participates in expiration handling, so checking a key is not simply dereferencing a value pointer forever.
4. `WAIT 1 timeout` can observe that one replica processed the client's write offset. It does not ask that the replica's AOF bytes were synced. `WAITAOF` expresses that separate disk-sync wait, subject to its returned counts and timeout.
5. The primary machine crashes before its relevant AOF sync. Some acknowledged in-memory state can be lost locally. A replica may have a newer copy, but safe failover still depends on which replica is selected and what state it durably retained; the WAIT count is not an election proof.
6. Without a crash, the expiry deadline passes. A lookup or active expiration cycle can make the key absent and propagate the expiration's effects. Physical memory freeing may occur separately. An expired session should not become live merely because the application has not issued an explicit DEL.

This history distinguishes logical time, execution order, replica progress and stable storage. Applications need to select and test the combination that matches their loss and failover requirements.

## Consistency and scaling boundaries

The local config explicitly calls replication asynchronous ([contract](/Users/bmysoren/tpt/redis/redis.conf:616)). `WAIT` counts replica acknowledgments for a client's write offset; `WAITAOF` separately waits for disk-sync acknowledgments ([WAIT](/Users/bmysoren/tpt/redis/src/replication.c:4911), [WAITAOF](/Users/bmysoren/tpt/redis/src/replication.c:4946)). Neither observation alone proves end-to-end linearizable failover. Cluster placement, failover selection, client retries and transactions across keys require separate analysis.

RAM capacity, object overhead, long-running commands, hot keys, replication backlog, fork copy-on-write memory and persistence I/O shape the cost model. More threads do not automatically solve a single hot key's serialized state transitions.

## Exercises, forecast and gaps

Trace `SET`, `GET`, `DEL` and expiration through dispatch, key lookup, propagation and persistence. Compare RDB-only, AOF-everysec and AOF-always crash windows. Explain why replica acknowledgment and replica disk acknowledgment differ.

Conditional forecast: growing working sets make tiering/encoding efficiency attractive; richer query workloads shift bottlenecks toward indexes and CPU; stricter durability pushes acknowledgment and failover semantics into the application contract. These are architectural pressures, not a product roadmap. Cluster routing, Sentinel election, module indexes, I/O thread ownership and transaction semantics remain follow-up chapters.

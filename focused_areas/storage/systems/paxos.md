# paxos: a small consensus implementation, not a storage engine

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: `/Users/bmysoren/tpt/paxos`, HEAD `b8a8040feec37b0382bab2aa7654d2aca94fdeb3`, read on 2026-10-27. The README calls this a simple C++ Paxos implementation and identifies acceptor, proposer/client, learner and persistence interfaces ([README](/Users/bmysoren/tpt/paxos/README.md:1)). Treat it as a protocol study library; no production safety certification follows from its name. Static reading, no tests executed.

## Fundamentals and layering

Consensus chooses a value despite some participants being unavailable. Proposal numbers order attempts. A majority intersects other majorities. Promises constrain future acceptance; accepted state must survive the failures assumed by the protocol. A learner applies a chosen value to application state. Consensus alone supplies neither key indexing nor a database's read API, deletion policy, snapshots or application transactions.

Here the path is `PaxosClient -> PaxosPeer/Thrift -> PaxosBrain -> PaxosState -> PaxosStateLogger`, plus learner callbacks. The filesystem logger is a persistence adapter, not a queryable key/value engine.

## Actual paths and limits in this checkout

- **Proposal/accept:** `recvPropose` compares the highest proposal, reports unfinished work or promises a higher version; `recvAccept` rejects lower proposals or stores pending state ([acceptor](/Users/bmysoren/tpt/paxos/PaxosBrain.cpp:15)).
- **Proposer:** `submit` contacts peers sequentially, collects replies, retries higher proposals and attempts replay of pending transactions ([submit](/Users/bmysoren/tpt/paxos/PaxosClient.cpp:65)). `sendAccept` first requires more than half to reply, then rejects the attempt if any received reply rejects ([accept count](/Users/bmysoren/tpt/paxos/PaxosClient.cpp:27)). This is the implementation's behavior; it should not be substituted for a textbook proof of Paxos.
- **Learning:** the client sends learning callbacks after its accept path reports success; exceptions are ignored per peer ([learn](/Users/bmysoren/tpt/paxos/PaxosClient.cpp:55)). A complete durable, deduplicated replicated application is not demonstrated by this callback alone.
- **Persistence:** pending state is serialized and written as a framed Thrift record, followed by an unchecked direct `fsync` call ([logger](/Users/bmysoren/tpt/paxos/PaxosFileLogger.cpp:148)). Startup obtains the latest transaction from the logger and restores proposal/pending state ([restore](/Users/bmysoren/tpt/paxos/PaxosState.cpp:6), [tail read](/Users/bmysoren/tpt/paxos/PaxosFileLogger.cpp:197)).
- **Read/delete:** no general storage read/delete path exists. They would need application-defined state-machine commands and read-consistency rules. Log rotation is distinct from a safe snapshot/truncation protocol.

## Safety questions supported by code

`setHighestProposalSeen` only updates memory ([promise state](/Users/bmysoren/tpt/paxos/PaxosState.cpp:33)); promise-only durability across restart is therefore not established. `commitLatestTransaction` changes a byte without an explicit sync ([commit marker](/Users/bmysoren/tpt/paxos/PaxosFileLogger.cpp:272)). `PaxosServiceHandler::accept` calls `sentAcceptResponse` before returning from the handler, despite the latter's comment saying the response has already been successfully sent ([handler](/Users/bmysoren/tpt/paxos/PaxosServiceHandler.cpp:12), [clear](/Users/bmysoren/tpt/paxos/PaxosBrain.cpp:48)). These are concrete reasons to avoid assuming the implementation has textbook crash semantics. They are research findings, not a complete adversarial proof or executed failure reproduction.

Sequential RPCs make slow peers affect proposal latency. Per-record sync limits throughput. Checksums and nonblocking sends appear explicitly as unfinished work ([TODO](/Users/bmysoren/tpt/paxos/TODO:1)).

## Exercises and conditional evolution

Construct a promise-only restart timeline and ask what lower proposal can be accepted afterward. Separate “an acceptor accepted,” “a majority accepted,” “the learner applied,” and “the client observed success.” Trace lost responses and retry duplication.

A production evolution would need a precise protocol/state specification, durable promises/accepted values, checked partial-I/O and sync handling, recovery validation, catch-up/snapshotting and application deduplication. This is a requirements inference, not a recommendation to patch this library incrementally or proof that these are its only gaps.

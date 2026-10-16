# Distributed Systems Algorithms Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`. These are single-process simulations, not production networking implementations.

## 1. Consistent Hashing Ring

* **Pattern / Idea**: Sorted hash ring with virtual nodes.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `Amazon/AWS: High`, `Google: High`, `Microsoft: High`, `Databricks: High`, `Snowflake: Medium`, `OpenAI: Medium`, `CoreWeave: High`, `Stripe: Medium`.
* **Question**: Implement a consistent hashing ring with virtual nodes and lookup.
* **Test Cases**: [Test cases](./test_cases.md#1-consistent-hashing-ring).
* **C++ Code**
  ```cpp
  class ConsistentHashRing {
      map<size_t, string> ring;
      int virtualNodes;
      hash<string> h;
  public:
      explicit ConsistentHashRing(int vnodes = 100) : virtualNodes(vnodes) {}

      void addNode(const string& node) {
          for (int i = 0; i < virtualNodes; ++i)
              ring[h(node + "#" + to_string(i))] = node;
      }

      void removeNode(const string& node) {
          for (int i = 0; i < virtualNodes; ++i)
              ring.erase(h(node + "#" + to_string(i)));
      }

      optional<string> getNode(const string& key) const {
          if (ring.empty()) return nullopt;
          size_t x = hash<string>{}(key);
          auto it = ring.lower_bound(x);
          if (it == ring.end()) it = ring.begin();
          return it->second;
      }
  };
  ```
* **Code Explanation**: A key maps to the first virtual node clockwise from its hash.
* **Invariants**: Ring keys are sorted; every lookup on a non-empty ring returns exactly one owner.
* **Complexity**: Lookup `O(log V)`, add/remove `O(vnodes log V)`, space `O(V)`.
* **Optimizations**: Runtime: use a sorted vector for read-heavy rings. Memory: tune virtual node count to balance smoothness and footprint.
* **Edge Cases To Consider**: Empty ring, wraparound lookup, node removal, skew with few virtual nodes.
* **L7 Follow-ups**: Real systems need stable hashing, weights, replica selection, and membership rollout.

## 2. Rendezvous Hashing

* **Pattern / Idea**: Highest score wins for each key.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `Amazon/AWS: High`, `Google: High`, `Microsoft: High`, `Databricks: High`, `Snowflake: Medium`, `OpenAI: Medium`, `CoreWeave: High`, `Stripe: Medium`.
* **Question**: Implement rendezvous hashing to choose the top `r` replicas for a key.
* **Test Cases**: [Test cases](./test_cases.md#2-rendezvous-hashing).
* **C++ Code**
  ```cpp
  vector<string> rendezvousReplicas(const string& key, const vector<string>& nodes, int replicas) {
      using Item = pair<size_t, string>;
      priority_queue<Item, vector<Item>, greater<Item>> best;
      hash<string> h;
      for (const string& node : nodes) {
          size_t score = h(key + "|" + node);
          best.push({score, node});
          if (static_cast<int>(best.size()) > replicas) best.pop();
      }
      vector<string> ans;
      while (!best.empty()) {
          ans.push_back(best.top().second);
          best.pop();
      }
      reverse(ans.begin(), ans.end());
      return ans;
  }
  ```
* **Code Explanation**: Score each node independently for the key; top scores become owners.
* **Invariants**: Heap contains the highest-scoring `replicas` nodes seen so far.
* **Complexity**: Time `O(N log r)`, space `O(r)`.
* **Optimizations**: Runtime: for one replica, keep only max in `O(N)`. Memory: no ring or virtual nodes.
* **Edge Cases To Consider**: No nodes, replicas greater than nodes, membership change, deterministic scoring.
* **L7 Follow-ups**: Weighted rendezvous hashing handles heterogeneous capacity.

## 3. Quorum Read/Write Simulator

* **Pattern / Idea**: Versioned replicas with intersecting read/write quorums.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `Amazon/AWS: High`, `Google: High`, `Microsoft: High`, `Databricks: High`, `Snowflake: Medium`, `OpenAI: Medium`, `CoreWeave: Medium`, `Stripe: Medium`.
* **Question**: Simulate quorum reads and writes over `N` replicas with versions.
* **Test Cases**: [Test cases](./test_cases.md#3-quorum-readwrite-simulator).
* **C++ Code**
  ```cpp
  struct ReplicaValue {
      string value;
      long long version = 0;
  };

  class QuorumStore {
      vector<ReplicaValue> replicas;
      int R, W;
      long long nextVersion = 0;
  public:
      QuorumStore(int n, int readQ, int writeQ) : replicas(n), R(readQ), W(writeQ) {}

      bool write(const string& value, const vector<int>& available) {
          if (static_cast<int>(available.size()) < W) return false;
          long long version = ++nextVersion;
          for (int i = 0; i < W; ++i) replicas[available[i]] = {value, version};
          return true;
      }

      optional<string> read(const vector<int>& available) const {
          if (static_cast<int>(available.size()) < R) return nullopt;
          ReplicaValue best;
          for (int i = 0; i < R; ++i) {
              const auto& cur = replicas[available[i]];
              if (cur.version > best.version) best = cur;
          }
          return best.value;
      }
  };
  ```
* **Code Explanation**: Reads return the highest version observed among a read quorum.
* **Invariants**: With `R + W > N`, every read quorum intersects every successful write quorum.
* **Complexity**: Write `O(W)`, read `O(R)`, space `O(N)`.
* **Optimizations**: Runtime: stop after quorum replies. Memory: store compact version metadata.
* **Edge Cases To Consider**: Insufficient replicas, stale replicas, `R + W <= N`, equal versions.
* **L7 Follow-ups**: Real quorum systems need read repair, hinted handoff, conflict resolution, and durable logs.

## 4. Heartbeat Failure Detector

* **Pattern / Idea**: Last-seen timestamp with suspicion threshold.
* **Company Frequency Tags**: Public signal: `Databricks: High (6m 100.0)`, `Apple: High (6m 62.9)`, `Snowflake: Medium (6m 49.9)`; Domain fit: `Amazon/AWS: High`, `Google: High`, `Microsoft: High`, `OpenAI: Medium`, `CoreWeave: High`, `Stripe: Medium`.
* **Question**: Implement a heartbeat-based failure detector.
* **Test Cases**: [Test cases](./test_cases.md#4-heartbeat-failure-detector).
* **C++ Code**
  ```cpp
  class FailureDetector {
      long long timeoutMs;
      unordered_map<string, long long> lastSeen;
  public:
      explicit FailureDetector(long long timeout) : timeoutMs(timeout) {}

      void heartbeat(const string& node, long long now) {
          lastSeen[node] = now;
      }

      vector<string> suspected(long long now) const {
          vector<string> out;
          for (const auto& [node, ts] : lastSeen)
              if (now - ts > timeoutMs) out.push_back(node);
          return out;
      }
  };
  ```
* **Code Explanation**: A node is suspected once no heartbeat has been observed within the configured timeout.
* **Invariants**: `lastSeen[node]` is the latest heartbeat timestamp accepted for that node.
* **Complexity**: Heartbeat `O(1)` average, sweep `O(N)`, space `O(N)`.
* **Optimizations**: Runtime: min-heap by expiry can avoid full sweeps. Memory: compact node ids.
* **Edge Cases To Consider**: New node, clock going backward policy, just-at-timeout, delayed heartbeat.
* **L7 Follow-ups**: Failure detectors produce suspicion, not truth; tune for false positives vs detection latency.

## 5. Vector Clock Comparison

* **Pattern / Idea**: Partial ordering by component-wise dominance.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `Amazon/AWS: High`, `Google: High`, `Microsoft: High`, `Databricks: High`, `Snowflake: Medium`, `OpenAI: Medium`, `CoreWeave: Medium`, `Stripe: Medium`.
* **Question**: Implement vector clocks and compare two events as before, after, equal, or concurrent.
* **Test Cases**: [Test cases](./test_cases.md#5-vector-clock-comparison).
* **C++ Code**
  ```cpp
  enum class ClockOrder { Equal, Before, After, Concurrent };

  ClockOrder compareVectorClocks(const vector<int>& a, const vector<int>& b) {
      bool less = false, greater = false;
      int n = max(a.size(), b.size());
      for (int i = 0; i < n; ++i) {
          int av = i < static_cast<int>(a.size()) ? a[i] : 0;
          int bv = i < static_cast<int>(b.size()) ? b[i] : 0;
          less |= av < bv;
          greater |= av > bv;
      }
      if (!less && !greater) return ClockOrder::Equal;
      if (less && !greater) return ClockOrder::Before;
      if (!less && greater) return ClockOrder::After;
      return ClockOrder::Concurrent;
  }

  void localEvent(vector<int>& clock, int node) {
      ++clock[node];
  }

  void receiveEvent(vector<int>& local, const vector<int>& remote, int node) {
      if (local.size() < remote.size()) local.resize(remote.size(), 0);
      for (int i = 0; i < static_cast<int>(remote.size()); ++i)
          local[i] = max(local[i], remote[i]);
      ++local[node];
  }
  ```
* **Code Explanation**: A clock is before another only if every component is `<=` and at least one is `<`.
* **Invariants**: Each node's component monotonically increases for events known to that node.
* **Complexity**: Update/compare `O(N)`, space `O(N)` per timestamp.
* **Optimizations**: Runtime: sparse maps for active nodes. Memory: prune inactive members with epochs.
* **Edge Cases To Consider**: Equal clocks, before/after, concurrent clocks, different vector lengths.
* **L7 Follow-ups**: Vector clocks identify concurrency but do not resolve conflicts by themselves.

## 6. Simplified Term-Based Leader Election

* **Pattern / Idea**: Monotonic terms, one durable vote per node per term, and a majority to become leader.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `CoreWeave: High`.
* **Question**: Implement a single-process simulation of term-based leader election and fence a former leader's write.
* **Test Cases**: [Test cases](./test_cases.md#6-simplified-term-based-leader-election).
* **C++ Code**: [Implementation](./leader_election_simulator.cpp) and [public API](./leader_election_simulator.h); [executable tests](./leader_election_simulator_test.cpp).

  ```bash
  c++ -std=c++17 -Wall -Wextra -Werror leader_election_simulator.cpp \
      leader_election_simulator_test.cpp -o leader_election_simulator_test
  ./leader_election_simulator_test
  ```

* **Code Explanation**: `currentTerm` and `votedFor` model durable state; role, leader identity, and collected votes are volatile. Elections persist a self-vote before issuing vote requests. A response whose term is older than the candidate's current term is discarded. Directed links model partitions, and write acceptance requires both the recipient and sender to agree on the current leader term.
* **Invariants**:
  - A node never grants votes to two different candidates in one term.
  - A candidate becomes leader only after a strict majority of unique votes in its current term.
  - A higher term demotes a node and clears only its volatile election state.
  - A replica rejects writes whose term is not its current term or whose sender is not the leader for that term.
* **Complexity**: An election sends at most `O(N)` vote requests; node state is `O(1)` plus the candidate's `O(N)` vote set; the simulator stores `O(N + E)` state for nodes and directed blocked links.
* **Optimizations**: Runtime: stop vote fanout after a majority in a real implementation, batch heartbeats, and use randomized timers. Memory: persist only term, vote, log/commit evidence, and compact membership configuration; do not retain unbounded vote history.
* **Edge Cases To Consider**: Delayed responses, old-term heartbeats, duplicate vote responses, no-quorum partitions, restart after durable vote, membership changes, and a repair/write race with a newer generation.
* **L7 Follow-ups**: This simulator does not implement a replicated log, durable device flush semantics, leases, membership joint consensus, or real network delivery. In production, the acknowledgment point is a configured durable quorum, not an RPC or RDMA completion.

## 7. Gossip Membership Simulator

* **Pattern / Idea**: `gossip`, `membership`; merge a fixed registry by incarnation, with a tombstone winning ties.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Simulate peer-to-peer membership propagation despite duplicated, delayed, reordered, or lost messages.
* **Test Cases**: [Test cases](./test_cases.md#7-gossip-membership-simulator).
* **API Contract / Failure Model**: Single-threaded, non-Byzantine peers share a fixed, unique node registry and initially believe every member alive at incarnation zero. Only a node may issue its own higher incarnation through `rejoin`; detectors may mark any member dead. A round snapshots every sender before delivering the explicitly supplied directed edges. Missing edges model loss or partitions; repeated edges model duplication. Tombstones are never deleted, so arbitrarily delayed gossip cannot revive the same or an older incarnation. A higher self-incarnation deliberately rejoins. Unknown IDs and invalid edges throw before delivery. Restart must restore incarnation/tombstones from durable storage; persistence, authentication, timers, and automatic failure detection are outside this model.
* **C++ Code**
  ```cpp
  class GossipMembership {
  public:
      struct Record {
          uint64_t incarnation = 0;
          bool dead = false;
      };
      using Snapshot = map<string, Record>;
  private:
      string self_;
      Snapshot members_;
  public:
      GossipMembership(string self, const vector<string>& registry)
          : self_(std::move(self)) {
          for (const auto& id : registry) {
              if (id.empty() || !members_.emplace(id, Record{}).second)
                  throw invalid_argument("invalid registry");
          }
          if (members_.count(self_) == 0)
              throw invalid_argument("self missing from registry");
      }

      const Snapshot& snapshot() const { return members_; }

      void markDead(const string& node) { members_.at(node).dead = true; }

      uint64_t rejoin() {
          auto& own = members_.at(self_);
          if (own.incarnation == numeric_limits<uint64_t>::max())
              throw overflow_error("incarnation exhausted");
          // A newer self-incarnation can override an older death announcement.
          ++own.incarnation;
          own.dead = false;
          return own.incarnation;
      }

      void merge(const Snapshot& packet) {
          // Validate the entire packet before changing any membership record.
          for (const auto& entry : packet)
              if (members_.count(entry.first) == 0)
                  throw invalid_argument("unknown member");
          for (const auto& [node, incoming] : packet) {
              auto& local = members_.at(node);
              // Newer incarnations win; at equal incarnation, dead beats alive.
              // Replaying stale alive messages therefore cannot remove a tombstone.
              if (incoming.incarnation > local.incarnation ||
                  (incoming.incarnation == local.incarnation && incoming.dead))
                  local = incoming;
          }
      }
  };

  void gossipRound(vector<GossipMembership>& peers,
                   const vector<pair<size_t, size_t>>& edges) {
      vector<GossipMembership::Snapshot> sent;
      // Freeze senders so one round cannot accidentally deliver multiple hops.
      for (const auto& peer : peers) sent.push_back(peer.snapshot());
      for (const auto& [source, target] : edges) {
          if (source >= peers.size() || target >= peers.size())
              throw out_of_range("invalid gossip edge");
          const auto& a = sent[source];
          const auto& b = sent[target];
          if (a.size() != b.size()) throw invalid_argument("registry mismatch");
          for (const auto& entry : a)
              if (b.count(entry.first) == 0)
                  throw invalid_argument("registry mismatch");
      }
      for (const auto& [source, target] : edges)
          peers[target].merge(sent[source]);
  }
  ```
* **Code Explanation**: Each member record is a join-semilattice ordered lexicographically by `(incarnation, dead)`. Local suspicion advances the dead bit; only `rejoin` advances an incarnation. Frozen snapshots prevent same-round multi-hop propagation. Callers choose fanout and delivery order rather than hiding a network scheduler in the algorithm.
* **Invariants**: Accepted records never decrease in this order; duplicate merges are idempotent; dead wins every equal-incarnation conflict. A stale alive record cannot remove a tombstone. Convergence requires eventual dissemination of each update to every peer after updates stop, for example through recurring strongly connected directed delivery paths. Weak connectivity or one-way delivery alone is insufficient; a permanent partition can prevent convergence.
* **Complexity**: For `N` registry members, one full merge takes `O(N log N)` time and retains `O(N)` state per peer. A round with `P` peers and `E` edges takes `O(PN + EN log N)` time, sends `E` messages containing `O(EN)` records, and uses `O(PN)` snapshot space. All-peer fanout `f` gives `E = O(Pf)`.
* **Optimizations**: Runtime: indexed node IDs and delta dissemination reduce lookup and payload costs. Memory: fixed membership bounds tombstones by `N`; dynamic membership requires durable retired-incarnation floors or proven dissemination/epoch barriers before tombstone garbage collection.
* **Edge Cases To Consider**: Empty/duplicate registry, missing self, unknown member, stale alive messages, newer rejoin, incarnation exhaustion, and partitions.
* **L7 Follow-ups**: Add suspicion/refutation timers, durable restart epochs, authenticated self-updates, bounded fanout, convergence-lag metrics, and deterministic partition/recovery schedules. Suspicion is not proof of death; this provides neither consensus nor exclusive ownership.

## 8. Lamport Timestamps

* **Pattern / Idea**: `lamport-clock`, `logical-clock`; increment locally and advance past the maximum received timestamp.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Assign scalar timestamps to local, send, and receive events across simulated nodes.
* **Test Cases**: [Test cases](./test_cases.md#8-lamport-timestamps).
* **API Contract / Failure Model**: One `LamportClock` per node, used serially. `send` timestamps a send event; a local event calls `tick`; every delivered message calls `receive`, including duplicates. Initial values may restore persisted clocks. Overflow throws before mutation; timestamps never wrap. Messages may be delayed, reordered, duplicated, or lost; no physical clock is used. State loss on restart requires a durable clock or a fresh node epoch.
* **C++ Code**
  ```cpp
  class LamportClock {
      uint64_t value_;

      uint64_t advance(uint64_t base) {
          // Check before incrementing: wrapping would violate logical ordering.
          if (base == numeric_limits<uint64_t>::max())
              throw overflow_error("Lamport clock exhausted");
          value_ = base + 1;
          return value_;
      }
  public:
      explicit LamportClock(uint64_t initial = 0) : value_(initial) {}
      uint64_t value() const { return value_; }
      uint64_t tick() { return advance(value_); }
      uint64_t send() { return tick(); }
      // Receiving is itself an event, later than both local and remote time.
      uint64_t receive(uint64_t remote) { return advance(max(value_, remote)); }
  };
  ```
* **Code Explanation**: A send counts as a local event. Receive first incorporates the sender's logical time and then counts the receiving event. Construct a vector of clocks to model multiple nodes.
* **Invariants**: Every successful event strictly increases its node's clock; a receive timestamp exceeds both the previous local clock and the message timestamp. Happens-before implies timestamp order, but the converse is false.
* **Complexity**: Time `O(1)` per event and `O(events)` overall; space `O(1)` per node or `O(N)` across `N` nodes, excluding any caller-retained event log.
* **Optimizations**: Runtime: scalar updates need no vector merge. Memory: retain only the current scalar unless events must be audited.
* **Edge Cases To Consider**: Initial zero, out-of-order delivery, duplicate messages, very large remote timestamps, and exhaustion without mutation.
* **L7 Follow-ups**: `(timestamp, stable node ID)` gives a deterministic total ordering compatible with causality, but does not detect concurrency or establish a replicated order. Compare vector clocks, hybrid logical clocks, durable epochs, and serialized/atomic updates for concurrent callers.

## 9. Idempotency Key Store

* **Pattern / Idea**: `idempotency`, `deduplication`; reserve an in-flight key, fence completion with a unique ticket, and cache the final result.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Distinguish a new request, an in-flight duplicate, a cached response, and conflicting reuse of an idempotency key.
* **Test Cases**: [Test cases](./test_cases.md#9-idempotency-key-store).
* **API Contract / Failure Model**: Single-threaded in-memory store. Keys must include tenant/operation scope; the request string is the complete canonical request identity, compared exactly. `begin` returns `Started` with a ticket only for new ownership; callers execute a side effect only in that case. `complete` accepts one matching ticket and caches any final response, including a terminal failure. Completed records expire at elapsed time `>= ttl` after completion, after which duplicate execution is allowed. In-flight records never expire automatically: an uncertain side effect cannot safely be retried. Capacity and per-string size limits bound storage; full admission returns `Full`. Expired unrelated keys require explicit `sweep`, avoiding a hidden linear scan per request. All time arguments are nondecreasing unsigned monotonic ticks; rollback throws. There is no transaction with an external side effect and no crash durability or exactly-once guarantee.
* **C++ Code**
  ```cpp
  class IdempotencyStore {
  public:
      enum class Status { Started, InFlight, Cached, Conflict, Full };
      struct Reply {
          Status status;
          uint64_t ticket = 0;
          string response;
      };
  private:
      struct Entry {
          string request;
          uint64_t ticket;
          optional<string> response;
          uint64_t completedAt = 0;
      };
      size_t capacity_, maxBytes_;
      uint64_t ttl_, now_ = 0, nextTicket_ = 0;
      unordered_map<string, Entry> entries_;

      void time(uint64_t now) {
          if (now < now_) throw invalid_argument("time moved backwards");
          now_ = now;
      }
      void sizeCheck(const string& value) const {
          if (value.size() > maxBytes_) throw length_error("string too large");
      }
      bool expired(const Entry& entry, uint64_t now) const {
          // Never expire uncertain in-flight work and accidentally admit it twice.
          return entry.response.has_value() && now - entry.completedAt >= ttl_;
      }
  public:
      IdempotencyStore(size_t capacity, uint64_t ttl, size_t maxBytes)
          : capacity_(capacity), maxBytes_(maxBytes), ttl_(ttl) {
          if (capacity == 0 || ttl == 0 || maxBytes == 0)
              throw invalid_argument("limits must be positive");
      }

      Reply begin(const string& key, const string& request, uint64_t now) {
          if (key.empty()) throw invalid_argument("empty key");
          sizeCheck(key);
          sizeCheck(request);
          time(now);
          auto it = entries_.find(key);
          if (it != entries_.end() && expired(it->second, now)) {
              entries_.erase(it);
              it = entries_.end();
          }
          if (it != entries_.end()) {
              const auto& entry = it->second;
              // Check identity before exposing a cached response for a reused key.
              if (entry.request != request) return {Status::Conflict, 0, {}};
              if (!entry.response) return {Status::InFlight, 0, {}};
              return {Status::Cached, 0, *entry.response};
          }
          if (entries_.size() == capacity_) return {Status::Full, 0, {}};
          if (nextTicket_ == numeric_limits<uint64_t>::max())
              throw overflow_error("tickets exhausted");
          // Only the Started owner receives a ticket and may execute the side effect.
          uint64_t ticket = ++nextTicket_;
          entries_.emplace(key, Entry{request, ticket, nullopt, 0});
          return {Status::Started, ticket, {}};
      }

      bool complete(const string& key, uint64_t ticket,
                    const string& response, uint64_t now) {
          sizeCheck(response);
          time(now);
          auto it = entries_.find(key);
          // A stale owner cannot complete a later reservation of the same key.
          if (it == entries_.end() || it->second.ticket != ticket ||
              it->second.response.has_value()) return false;
          it->second.response = response;
          // Retention starts at completion, not at reservation or duplicate lookup.
          it->second.completedAt = now;
          return true;
      }

      size_t sweep(uint64_t now) {
          time(now);
          size_t removed = 0;
          for (auto it = entries_.begin(); it != entries_.end();) {
              if (expired(it->second, now)) {
                  it = entries_.erase(it);
                  ++removed;
              } else ++it;
          }
          return removed;
      }
      size_t size() const { return entries_.size(); }
  };
  ```
* **Code Explanation**: Reservation and completion are serialized operations. Conflicts are detected before exposing cached data. Only the successful owner receives its ticket; old completions cannot fill a later reservation. Expiry uses elapsed subtraction, so computing an overflowing absolute expiry is unnecessary. Allocation exceptions propagate without a general strong exception guarantee; after such a failure, stop admission and reconcile uncertain external effects before reconstructing state. Allocation-failure recovery is outside this simulation.
* **Invariants**: At most one active ticket per retained key; only that ticket can finalize it once. In-flight records survive sweeps. Completed retention is measured from completion, never extended by a duplicate read. Tickets never wrap or repeat within the store.
* **Complexity**: With `K <= capacity` and maximum string size `B`, begin/complete take average `O(B)` time including hashing/comparison/copy, worst-case `O(KB)` under hash collisions. Sweep takes `O(K)` record visits, excluding string destruction costs. Space is `O(KB)` including keys, requests, and responses; fixed-size payloads recover the usual average `O(1)` request target.
* **Optimizations**: Runtime: shard atomic reservations or use a transactional unique index; use a timing wheel for expiry if sweeps dominate. Memory: cap response/request bytes and admission count; retaining in-flight uncertainty deliberately trades capacity for duplicate-side-effect safety.
* **Edge Cases To Consider**: Empty key/response, conflicting in-flight and completed requests, exact expiry, full store, stuck owner, stale ticket, backward time, oversized payload, and ticket exhaustion.
* **L7 Follow-ups**: Integrate an outbox or same-database side-effect transaction, durable request status, authorized recovery of uncertain work, retry-horizon contracts, quotas, and conflict/full/oldest-in-flight metrics. A distributed deployment needs transactional reservation and fencing across process restarts.

## 10. Grow-Only CRDT Counter

* **Pattern / Idea**: `crdt`, `g-counter`; one monotonic component per writer, merged by component-wise maximum.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Implement local increments, replica-state merge, and a checked total for a grow-only counter.
* **Test Cases**: [Test cases](./test_cases.md#10-grow-only-crdt-counter).
* **API Contract / Failure Model**: Fixed membership and stable component indices; one serial writer owns each component. Every replica may merge other replicas of the same registry size. Duplicate/out-of-order/lost state messages are allowed; peers must eventually exchange state for convergence. `increment` changes only the owner's component. Component overflow throws without mutation; `value` separately throws if the mathematical sum exceeds `uint64_t`. Merge remains defined even when the total is too large, preserving the lattice laws. Matching sizes do not authenticate matching identities; the caller supplies that guarantee. Forked writers, reset counters, reused node IDs, crashes without persisted state, and Byzantine states are unsupported.
* **C++ Code**
  ```cpp
  class GCounter {
      vector<uint64_t> counts_;
      size_t owner_;
  public:
      GCounter(size_t nodes, size_t owner) : counts_(nodes, 0), owner_(owner) {
          if (nodes == 0 || owner >= nodes)
              throw invalid_argument("invalid writer registry");
      }
      const vector<uint64_t>& components() const { return counts_; }

      void increment(uint64_t delta = 1) {
          // Each replica writes only its own component; other components are merged.
          auto& own = counts_[owner_];
          if (delta > numeric_limits<uint64_t>::max() - own)
              throw overflow_error("component exhausted");
          own += delta;
      }

      void merge(const GCounter& remote) {
          if (counts_.size() != remote.counts_.size())
              throw invalid_argument("registry size mismatch");
          // Max, rather than addition, makes repeated state delivery idempotent.
          for (size_t i = 0; i < counts_.size(); ++i)
              counts_[i] = max(counts_[i], remote.counts_[i]);
      }

      uint64_t value() const {
          uint64_t total = 0;
          for (uint64_t count : counts_) {
              // Components can fit individually even when their total does not.
              if (count > numeric_limits<uint64_t>::max() - total)
                  throw overflow_error("total exhausted");
              total += count;
          }
          return total;
      }
  };
  ```
* **Code Explanation**: Increment adds only to the writer's slot; replaying an old snapshot cannot lower it. A merge does not sum received values, which would double-count duplicated messages. Total calculation checks each addition before performing it.
* **Invariants**: Components never decrease. Max-merge is associative, commutative, and idempotent. With a single writer per component and eventual dissemination, replicas converge to the sum of all retained increments.
* **Complexity**: Time `O(1)` per increment and `O(N)` per merge/value query; space `O(N)` per replica, `O(N)` records per full-state message, and `O(N^2)` for an `N`-replica full simulation.
* **Optimizations**: Runtime: delta-state propagation reduces payloads; a cached checked total can speed reads but complicates overflow/merge handling. Memory: sparse components help large mostly inactive registries; removing components needs coordinated epochs and cannot silently discard increments.
* **Edge Cases To Consider**: Empty registry, invalid owner, zero increment, duplicate/self/stale merge, registry mismatch, component maximum, and aggregate overflow.
* **L7 Follow-ups**: Support arbitrary-precision counters, PN-counters, durable writer epochs, membership change, anti-entropy convergence metrics, and serialized updates for shared-process access. This counter cannot decrement or provide linearizable reads during partitions.

## 11. Merkle Tree Comparison

* **Pattern / Idea**: `merkle-tree`, `anti-entropy`; compare subtree hashes and descend only when they differ.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Build two hash trees over aligned chunks and return the indices of differing chunks.
* **Test Cases**: [Test cases](./test_cases.md#11-merkle-tree-comparison).
* **API Contract / Failure Model**: Immutable, single-threaded snapshots must have equal chunk counts and identical chunk boundaries; size mismatch throws. Empty snapshots compare equal. Strings contain arbitrary bytes. A specified 64-bit FNV-1a-style hash with domain tags and fixed little-endian child encoding provides deterministic simulation hashes. Unsigned hash wraparound is intentional. This hash is non-cryptographic: equal hashes are treated as equal data only under an explicit no-collision assumption; collisions can hide differences. No adversarial integrity guarantee or cross-snapshot synchronization is provided. Allocation/size exhaustion throws.
* **C++ Code**
  ```cpp
  class MerkleTree {
      static constexpr uint64_t seed_ = 14695981039346656037ULL;
      size_t count_, leaves_ = 1;
      vector<uint64_t> tree_;

      static uint64_t byte(uint64_t hash, unsigned char value) {
          return (hash ^ value) * 1099511628211ULL;
      }
      static uint64_t leaf(const string& chunk) {
          // Domain tags distinguish actual chunks, internal nodes, and padding.
          uint64_t hash = byte(seed_, 0);
          for (unsigned char value : chunk) hash = byte(hash, value);
          return hash;
      }
      static uint64_t parent(uint64_t left, uint64_t right) {
          uint64_t hash = byte(seed_, 1);
          // Fixed byte order makes parent hashes independent of machine endianness.
          for (uint64_t child : {left, right}) {
              for (unsigned shift = 0; shift < 64; shift += 8)
                  hash = byte(hash, static_cast<unsigned char>(child >> shift));
          }
          return hash;
      }
      void compare(const MerkleTree& other, size_t node, size_t lo, size_t hi,
                   vector<size_t>& out) const {
          // Equal hashes prune whole subtrees under the no-collision assumption.
          if (lo >= count_ || tree_[node] == other.tree_[node]) return;
          if (hi - lo == 1) {
              out.push_back(lo);
              return;
          }
          size_t mid = lo + (hi - lo) / 2;
          compare(other, node * 2, lo, mid, out);
          compare(other, node * 2 + 1, mid, hi, out);
      }
  public:
      explicit MerkleTree(const vector<string>& chunks) : count_(chunks.size()) {
          const size_t limit = tree_.max_size();
          while (leaves_ < count_) {
              if (leaves_ > limit / 2) throw length_error("tree too large");
              leaves_ *= 2;
          }
          if (leaves_ > limit / 2) throw length_error("tree too large");
          tree_.assign(leaves_ * 2, byte(seed_, 2));
          for (size_t i = 0; i < count_; ++i) tree_[leaves_ + i] = leaf(chunks[i]);
          // Children occupy higher indices, so reverse order builds bottom-up.
          for (size_t i = leaves_ - 1; i > 0; --i)
              tree_[i] = parent(tree_[i * 2], tree_[i * 2 + 1]);
      }

      vector<size_t> differences(const MerkleTree& other) const {
          if (count_ != other.count_) throw invalid_argument("chunk count mismatch");
          vector<size_t> out;
          compare(other, 1, 0, leaves_, out);
          return out;
      }
  };
  ```
* **Code Explanation**: Pad to a power of two using a domain distinct from real leaves. Build parents bottom-up; equal subtree hashes stop traversal. Left-first recursion returns indices in ascending order. Store only hashes, so callers retain actual chunks separately for repair.
* **Invariants**: Every parent hashes its ordered children; both snapshots use the same tree geometry. Under the no-collision assumption, skipped subtrees contain no differing chunk. Padding never appears in results.
* **Complexity**: Build time `O(total input bytes + n)` and tree space `O(n)` for `n > 0` (constant for empty). Comparison uses `O(1 + k log(n + 1))` hash visits for `k` differences, bounded by `O(n)` worst case, with `O(k + log(n + 1))` output/stack space. Identical roots compare in `O(1)`.
* **Optimizations**: Runtime: incrementally update an ancestor path or compare remote hashes in batches. Memory: larger chunks shrink trees but increase repair traffic; retain internal levels on disk for large replicas.
* **Edge Cases To Consider**: No chunks, empty chunk versus absent chunk, single leaf, non-power-of-two size, binary content, all chunks changed, count mismatch, and hash collisions.
* **L7 Follow-ups**: Replace the demonstration hash with a vetted cryptographic hash, version its encoding, compare consistent snapshots, authenticate roots, and measure repair bytes and false equality risk. Arbitrary insertions need aligned ranges or a different tree layout.

## 12. Bounded Exponential Retry With Jitter

* **Pattern / Idea**: `retry`, `backoff`; bound calls, classify failures, and sample full jitter from a saturating exponential delay ceiling.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Retry transient failures with capped exponential backoff, injected jitter/waiting, cancellation, and a maximum attempt count.
* **Test Cases**: [Test cases](./test_cases.md#12-bounded-exponential-retry-with-jitter).
* **API Contract / Failure Model**: Serial callbacks: `operation(oneBasedAttempt)` returns success/transient/permanent; `random(ceiling)` must sample uniformly in the inclusive range `[0, ceiling]`; `wait(delay)` returns false on cancellation; `cancelled()` is polled before calls and before waiting. Timing units are caller-defined. Policy requires positive attempts and `base <= cap`; zero delay is valid. Only transient results retry. Exceptions propagate without automatic retries; an out-of-range random sample throws. The caller owns the RNG and timer, allowing deterministic tests without real sleeps. Callbacks must terminate/cooperate with cancellation; attempt limits do not bound total wall time or make non-idempotent side effects safe.
* **C++ Code**
  ```cpp
  enum class AttemptResult { Success, Transient, Permanent };
  enum class RetryStop { Succeeded, PermanentFailure, Exhausted, Cancelled };
  struct RetryPolicy {
      size_t maxAttempts;
      uint64_t baseDelay, maxDelay;
  };
  struct RetryReport {
      RetryStop stop;
      size_t attempts;
  };

  template<class Operation, class Random, class Wait, class Cancelled>
  RetryReport boundedRetry(const RetryPolicy& policy, Operation operation,
                           Random random, Wait wait, Cancelled cancelled) {
      if (policy.maxAttempts == 0 || policy.baseDelay > policy.maxDelay)
          throw invalid_argument("invalid retry policy");
      uint64_t ceiling = policy.baseDelay;
      for (size_t used = 0; used < policy.maxAttempts; ++used) {
          if (cancelled()) return {RetryStop::Cancelled, used};
          const size_t attempt = used + 1;
          AttemptResult result = operation(attempt);
          if (result == AttemptResult::Success) return {RetryStop::Succeeded, attempt};
          if (result == AttemptResult::Permanent)
              return {RetryStop::PermanentFailure, attempt};
          // Do not sleep after the last allowed call: there will be no next attempt.
          if (attempt == policy.maxAttempts) return {RetryStop::Exhausted, attempt};
          if (cancelled()) return {RetryStop::Cancelled, attempt};
          // Full jitter selects any delay from zero through the current ceiling.
          uint64_t delay = random(ceiling);
          if (delay > ceiling) throw invalid_argument("jitter outside range");
          if (!wait(delay)) return {RetryStop::Cancelled, attempt};
          // Saturate before doubling so unsigned arithmetic cannot wrap.
          ceiling = ceiling > policy.maxDelay / 2 ? policy.maxDelay : ceiling * 2;
      }
      throw logic_error("unreachable retry state");
  }
  ```
* **Code Explanation**: The first retry uses the base ceiling, later retries double it up to the cap. Jitter changes the actual delay, so individual delays need not increase. The doubling guard avoids multiplication overflow. No delay is requested after success, permanent failure, or the final allowed call.
* **Invariants**: Operation calls never exceed `maxAttempts`; every delay lies within the current ceiling and cap; terminal results stop further work. Cancellation before a call prevents that call, subject to the caller's cooperative execution model.
* **Complexity**: Time `O(A)` policy steps for `A` attempts, plus callback and waiting time; space `O(1)` policy state, excluding callback state. At most `A - 1` waits, each bounded by `maxDelay`.
* **Optimizations**: Runtime: honor retry budgets, deadlines, and server retry hints; avoid retrying at every stack layer. Memory: pass callbacks using reference wrappers if their state is large; a production RNG should be seeded once rather than per attempt.
* **Edge Cases To Consider**: One allowed attempt, zero delays, cap saturation, invalid policy/randomness, early permanent failure, cancellation during wait, callback exceptions, and unsigned maximum delay.
* **L7 Follow-ups**: Add a monotonic deadline with checked remaining-time arithmetic, per-attempt timeouts, safe replay keys, shared retry budgets, and metrics for attempts, sleep, and exhaustion. Independent jitter reduces synchronized retries but does not guarantee fairness or recovery.

## 13. Circuit Breaker With Rolling Failure Window

* **Pattern / Idea**: `circuit-breaker`, `state-machine`; count failures in a rolling time window and gate recovery through one uniquely identified probe.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Implement closed/open/half-open admission with a rolling failure threshold and stale-completion fencing.
* **Test Cases**: [Test cases](./test_cases.md#13-circuit-breaker-with-rolling-failure-window).
* **API Contract / Failure Model**: Single-threaded calls use nondecreasing unsigned monotonic ticks. A failure completed at `t` contributes while `now - t < window`; this is a failure-count threshold, not a failure ratio. `admit` returns a unique ticket or rejects. `complete` accepts each currently registered ticket once; unregistered/duplicate/stale tickets return false. Opening invalidates all older tickets, and tickets never repeat, so an older generation cannot close or retrip a recovered breaker. After cooldown, exactly one half-open probe may be admitted until its completion. Lost probe completions deliberately stall recovery; the owner must report timeout as failure. The limit bounds current-generation bookkeeping, not outstanding physical calls across generations; a separate executor must bound/cancel actual work. No networking, locking, automatic timeout worker, or crash persistence is provided.
* **C++ Code**
  ```cpp
  class CircuitBreaker {
  public:
      enum class State { Closed, Open, HalfOpen };
  private:
      size_t threshold_, inFlightLimit_, failures_ = 0;
      uint64_t window_, cooldown_, now_ = 0, openedAt_ = 0, nextTicket_ = 0;
      State state_ = State::Closed;
      deque<pair<uint64_t, size_t>> buckets_;
      unordered_set<uint64_t> active_;

      void advance(uint64_t now) {
          if (now < now_) throw invalid_argument("time moved backwards");
          now_ = now;
          while (!buckets_.empty() && now - buckets_.front().first >= window_) {
              failures_ -= buckets_.front().second;
              buckets_.pop_front();
          }
      }
      void clearFailures() { buckets_.clear(); failures_ = 0; }
      void open(uint64_t now) {
          state_ = State::Open;
          openedAt_ = now;
          // Invalidate older calls so their late completions cannot change recovery.
          active_.clear();
          clearFailures();
      }
  public:
      CircuitBreaker(size_t threshold, uint64_t window, uint64_t cooldown,
                     size_t inFlightLimit)
          : threshold_(threshold), inFlightLimit_(inFlightLimit),
            window_(window), cooldown_(cooldown) {
          if (threshold == 0 || window == 0 || cooldown == 0 || inFlightLimit == 0)
              throw invalid_argument("limits must be positive");
      }
      State state() const { return state_; }
      size_t failures() const { return failures_; }
      size_t inFlight() const { return active_.size(); }

      optional<uint64_t> admit(uint64_t now) {
          advance(now);
          if (state_ == State::Open && now - openedAt_ < cooldown_) return nullopt;
          // Exactly one probe may remain active while recovery is half-open.
          if (state_ == State::HalfOpen && !active_.empty()) return nullopt;
          if (state_ == State::Closed && active_.size() >= inFlightLimit_)
              return nullopt;
          if (nextTicket_ == numeric_limits<uint64_t>::max())
              throw overflow_error("tickets exhausted");
          const uint64_t ticket = nextTicket_ + 1;
          active_.insert(ticket);
          nextTicket_ = ticket;
          if (state_ == State::Open) state_ = State::HalfOpen;
          return ticket;
      }

      bool complete(uint64_t ticket, bool success, uint64_t now) {
          advance(now);
          // Erasing the ticket makes every completion effective at most once.
          if (active_.erase(ticket) == 0) return false;
          if (state_ == State::HalfOpen) {
              if (success) {
                  state_ = State::Closed;
                  clearFailures();
              } else open(now);
              return true;
          }
          if (!success) {
              // Open on the threshold failure; otherwise retain it in the window.
              if (failures_ == threshold_ - 1) open(now);
              else {
                  if (buckets_.empty() || buckets_.back().first != now)
                      buckets_.push_back({now, 0});
                  ++buckets_.back().second;
                  ++failures_;
              }
          }
          return true;
      }
  };
  ```
* **Code Explanation**: Equal-tick failures share a bucket; expired buckets are removed lazily. Success in closed state does not erase recent failures. Tripping clears all active tickets, implicitly fencing that generation without a wrapping epoch counter. A half-open completion can change state only if it matches the one active probe. Allocation exceptions are fatal to this simulated instance: stop admission, drain or cancel external work, and reconstruct the breaker. Multi-step completion does not promise strong exception safety under allocation failure.
* **Invariants**: Open admits no call before cooldown. Half-open has at most one active ticket. Every completion affects policy at most once, and an invalidated ticket never affects later generations. Closed-state failure count stays below threshold; reaching threshold opens and clears the window.
* **Complexity**: Admission/completion time is average amortized `O(1)` for hash lookup and bucket expiry. One call can expire `O(B)` buckets or clear `O(L + B)` state when opening; adversarial hashing also costs `O(L)`. Space is `O(L + B)`, where `L` is the current-generation in-flight limit and `B <= min(window ticks, threshold - 1)` retained buckets.
* **Optimizations**: Runtime: a fixed bucket ring provides predictable expiry costs with coarser time resolution. Memory: cap current permits and aggregate same-tick failures. Use a bounded executor independently to control actual network concurrency.
* **Edge Cases To Consider**: Zero limits, exact rolling-window expiry, exact cooldown, multiple simultaneous probe admissions, duplicate/late completions, probe timeout/failure, backward time, and ticket exhaustion.
* **L7 Follow-ups**: Add failure-ratio/minimum-volume policy, bounded probe deadlines, per-tenant/dependency breakers, executor cancellation, durable ownership if needed, and metrics for rejects, state durations, outstanding work, and ignored completions. Multiple processes need coordinated probe budgets if one global recovery limit is required.

## Useful Public References

These references provide algorithm and production-design context, not interview-frequency evidence. The local simulations deliberately implement smaller contracts than the full protocols.

* [SWIM membership paper](https://www.cs.cornell.edu/projects/Quicksilver/public_pdfs/SWIM.pdf): background for gossip dissemination and suspicion; the membership sample does not implement SWIM's probe/suspicion protocol.
* [Amazon Dynamo paper](https://www.allthingsdistributed.com/files/amazon-dynamo-sosp2007.pdf): context for replica reconciliation, causality, and Merkle-tree anti-entropy; the local examples are not a Dynamo implementation.
* [AWS Builders' Library: Timeouts, retries, and backoff with jitter](https://aws.amazon.com/builders-library/timeouts-retries-and-backoff-with-jitter/): context for retry budgets, idempotency, timeouts, and randomized backoff.

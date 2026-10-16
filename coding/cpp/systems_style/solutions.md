# Systems-Style Coding Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Delayed Job Scheduler

* **Pattern / Idea**: Min-heap by due time.
* **Company Frequency Tags**: Public signal: `Databricks: High (6m 100.0)`, `Apple: High (6m 62.9)`, `Snowflake: Medium (6m 59.1)`, `Oracle: Medium (6m 56.9)`; Domain fit: `Amazon/AWS: High`, `CoreWeave: High`.
* **Question**: Implement a delayed job scheduler that runs jobs at or after their due time.
* **Test Cases**: [Test cases](./test_cases.md#1-delayed-job-scheduler).
* **C++ Code**
  ```cpp
  class DelayedScheduler {
      struct Job {
          long long due;
          int id;
          function<void()> fn;
          bool operator>(const Job& o) const { return due > o.due; }
      };
      priority_queue<Job, vector<Job>, greater<Job>> pq;
  public:
      void schedule(int id, long long dueMillis, function<void()> fn) {
          pq.push({dueMillis, id, move(fn)});
      }

      vector<int> runDue(long long now) {
          vector<int> ran;
          while (!pq.empty() && pq.top().due <= now) {
              Job job = pq.top();
              pq.pop();
              job.fn();
              ran.push_back(job.id);
          }
          return ran;
      }
  };
  ```
* **Code Explanation**: The earliest due job is always at the heap top; all due jobs are popped and run.
* **Invariants**: Heap order is by next eligible execution time.
* **Complexity**: Schedule `O(log n)`, dispatch each job `O(log n)`, space `O(n)`.
* **Optimizations**: Runtime: worker can sleep until heap top due time. Memory: store payload references for large jobs.
* **Edge Cases To Consider**: Same due time, no due jobs, overdue jobs, function failure policy.
* **L7 Follow-ups**: Add cancellation, persistence, retry policy, and graceful shutdown.

## 2. Retry Queue With Backoff

* **Pattern / Idea**: Delayed heap with attempt count.
* **Company Frequency Tags**: Public signal: `Snowflake: Medium (6m 59.1)`, `Oracle: Medium (6m 56.9)`, `Apple: Medium (6m 47.7)`; Domain fit: `Stripe: High`, `Amazon/AWS: High`, `OpenAI: Medium`.
* **Question**: Implement a retry queue that schedules failed jobs using bounded exponential backoff and retry limits.
* **Test Cases**: [Test cases](./test_cases.md#2-retry-queue-with-backoff).
* **C++ Code**
  ```cpp
  struct RetryTask {
      int id;
      int attempt;
      long long due;
      bool operator>(const RetryTask& o) const { return due > o.due; }
  };

  class RetryQueue {
      priority_queue<RetryTask, vector<RetryTask>, greater<RetryTask>> pq;
      int maxAttempts;
      long long baseDelay;
  public:
      RetryQueue(int maxA, long long base) : maxAttempts(maxA), baseDelay(base) {}

      void add(int id, long long now) { pq.push({id, 0, now}); }

      optional<RetryTask> popDue(long long now) {
          if (pq.empty() || pq.top().due > now) return nullopt;
          RetryTask t = pq.top(); pq.pop();
          return t;
      }

      void retry(const RetryTask& t, long long now) {
          if (t.attempt + 1 >= maxAttempts) return;
          long long delay = baseDelay << min(t.attempt, 20);
          pq.push({t.id, t.attempt + 1, now + delay});
      }
  };
  ```
* **Code Explanation**: Failed tasks are reinserted with exponentially increasing due times until attempts are exhausted.
* **Invariants**: Heap top is the next retry eligible to run.
* **Complexity**: Push/pop `O(log n)`, space `O(n)`.
* **Optimizations**: Runtime: cap delay and add jitter. Memory: max attempts bounds long-lived retries.
* **Edge Cases To Consider**: Max attempts, immediate first attempt, delay overflow, many tasks same due time.
* **L7 Follow-ups**: Add idempotency keys before retrying side effects.

## 3. Message Broker With Visibility Timeout

* **Pattern / Idea**: Ready queue, inflight map, timeout heap.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `Amazon/AWS: High`, `Stripe: High`, `Microsoft: Medium`.
* **Question**: Implement a message broker with enqueue, consume, ack, and visibility timeout.
* **Test Cases**: [Test cases](./test_cases.md#3-message-broker-with-visibility-timeout).
* **C++ Code**
  ```cpp
  class Broker {
      struct Msg { int id; string body; };
      queue<Msg> ready;
      unordered_map<int, Msg> inflight;
      priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> timeouts;
      long long visibilityMs;

      void expire(long long now) {
          while (!timeouts.empty() && timeouts.top().first <= now) {
              auto [_, id] = timeouts.top(); timeouts.pop();
              auto it = inflight.find(id);
              if (it != inflight.end()) {
                  ready.push(it->second);
                  inflight.erase(it);
              }
          }
      }
  public:
      explicit Broker(long long visibility) : visibilityMs(visibility) {}
      void enqueue(int id, string body) { ready.push({id, move(body)}); }
      optional<Msg> consume(long long now) {
          expire(now);
          if (ready.empty()) return nullopt;
          Msg m = ready.front(); ready.pop();
          inflight[m.id] = m;
          timeouts.push({now + visibilityMs, m.id});
          return m;
      }
      bool ack(int id) { return inflight.erase(id) > 0; }
  };
  ```
* **Code Explanation**: Consumed messages move to inflight and return to ready if not acknowledged before timeout.
* **Invariants**: A message is either ready, inflight, or acknowledged, never both ready and inflight.
* **Complexity**: Enqueue `O(1)`, consume timeout processing `O(log n)` per expired item, space `O(messages)`.
* **Optimizations**: Runtime: lazy timeout heap. Memory: compact ack/inflight metadata.
* **Edge Cases To Consider**: Ack before/after timeout, repeated consume, duplicate ids policy.
* **L7 Follow-ups**: Production brokers need durability, ordering semantics, dead-letter queues, and idempotent consumers.

## 4. Config Snapshot Manager

* **Pattern / Idea**: Immutable snapshots with atomic publish under lock.
* **Company Frequency Tags**: Public signal: `OpenAI: High (6m 66.7)`, `Apple: Medium (6m 54.0)`, `Snowflake: Medium (6m 49.9)`, `Oracle: Medium (6m 47.0)`, `Databricks: High (all 79.9)`; Domain fit: `CoreWeave: High`.
* **Question**: Implement a config snapshot manager that allows lock-light reads and atomic updates.
* **Test Cases**: [Test cases](./test_cases.md#4-config-snapshot-manager).
* **C++ Code**
  ```cpp
  class ConfigManager {
      mutable mutex mu;
      shared_ptr<const unordered_map<string, string>> current =
          make_shared<const unordered_map<string, string>>();
  public:
      optional<string> get(const string& key) const {
          shared_ptr<const unordered_map<string, string>> snap;
          {
              lock_guard<mutex> lk(mu);
              snap = current;
          }
          auto it = snap->find(key);
          if (it == snap->end()) return nullopt;
          return it->second;
      }

      void update(unordered_map<string, string> next) {
          auto snap = make_shared<const unordered_map<string, string>>(move(next));
          lock_guard<mutex> lk(mu);
          current = move(snap);
      }
  };
  ```
* **Code Explanation**: Readers copy a shared pointer to an immutable map, then read without holding the lock.
* **Invariants**: Every reader sees one complete config version.
* **Complexity**: Read `O(1)` average lookup, update `O(config size)`, space `O(active snapshots)`.
* **Optimizations**: Runtime: short lock duration for pointer copy. Memory: old snapshots release when readers finish.
* **Edge Cases To Consider**: Missing key, concurrent reads during update, large config, rapid updates.
* **L7 Follow-ups**: C++20 atomic shared pointers can remove the mutex for pointer publication.

## 5. Leaderboard

* **Pattern / Idea**: Hash map plus ordered set by score.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs; Domain fit: `Meta: High`, `Amazon/AWS: Medium`, `Microsoft: Medium`.
* **Question**: Implement a compact in-memory leaderboard with update score and query top `k`.
* **Test Cases**: [Test cases](./test_cases.md#5-leaderboard).
* **C++ Code**
  ```cpp
  class Leaderboard {
      struct Entry {
          int score;
          string user;
          bool operator<(const Entry& o) const {
              if (score != o.score) return score > o.score;
              return user < o.user;
          }
      };
      unordered_map<string, int> scores;
      set<Entry> ranking;
  public:
      void update(const string& user, int score) {
          if (scores.count(user)) ranking.erase({scores[user], user});
          scores[user] = score;
          ranking.insert({score, user});
      }
      vector<pair<string, int>> topK(int k) const {
          vector<pair<string, int>> ans;
          for (auto it = ranking.begin(); it != ranking.end() && k-- > 0; ++it)
              ans.push_back({it->user, it->score});
          return ans;
      }
  };
  ```
* **Code Explanation**: The set keeps users sorted by descending score and stable username tie-break.
* **Invariants**: Each user appears at most once in both `scores` and `ranking`.
* **Complexity**: Update `O(log n)`, top-K `O(k)`, space `O(n)`.
* **Optimizations**: Runtime: cache top pages for read-heavy leaderboards. Memory: store numeric user ids instead of strings.
* **Edge Cases To Consider**: Score update, ties, `k > n`, negative scores.
* **L7 Follow-ups**: Distributed leaderboards need sharding, approximate ranks, and delayed merge policies.

## 6. Sharded Key-Value Store Simulator

* **Pattern / Idea**: `sharding`, `key-value-store`: fixed modulo routing plus one owning map per shard. A single map is the baseline; separate shard maps expose ownership without introducing a distributed protocol.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Implement bounded put/get/erase operations with deterministic ownership for fixed shard membership.
* **Test Cases**: [Test cases](./test_cases.md#6-sharded-key-value-store-simulator).
* **API Contract / Failure Model**: A serialized, single-process simulator. Shard count and byte limits must be positive; the global entry capacity may be zero. Empty keys and values are allowed. Oversize keys/values throw `length_error`; invalid construction throws `invalid_argument`; invalid shard inspection throws `out_of_range`. `put` returns false only for a new key at capacity; overwrites remain allowed. `get` returns a value copy or `nullopt`; `erase` reports whether a key existed. Routing is stable for this object's lifetime using the implementation's string hash, with no cross-platform hash guarantee. There is no rebalance, replication, persistence, or availability protocol. Allocation failures propagate; recovery from allocation failure is outside this sample's contract.
* **C++ Code**
  ```cpp
  class ShardedKV {
      vector<unordered_map<string, string>> shards;
      size_t capacity, keyLimit, valueLimit;
      size_t entries = 0;

      void validateKey(const string& key) const {
          if (key.size() > keyLimit) throw length_error("key too large");
      }
  public:
      ShardedKV(size_t shardCount, size_t maxEntries,
                size_t maxKeyBytes, size_t maxValueBytes)
          : capacity(maxEntries), keyLimit(maxKeyBytes), valueLimit(maxValueBytes) {
          if (shardCount == 0 || keyLimit == 0 || valueLimit == 0)
              throw invalid_argument("positive shards and byte limits required");
          shards.resize(shardCount);
      }

      size_t owner(const string& key) const {
          validateKey(key);
          // Fixed membership keeps this routing decision stable for every operation.
          return hash<string>{}(key) % shards.size();
      }

      bool put(const string& key, const string& value) {
          const size_t index = owner(key);
          if (value.size() > valueLimit) throw length_error("value too large");
          auto& shard = shards[index];
          auto it = shard.find(key);
          if (it != shard.end()) {
              // An overwrite needs no new entry, even when global capacity is full.
              it->second = value;
              return true;
          }
          if (entries == capacity) return false;
          shard.emplace(key, value);
          // Count the key only after insertion succeeds.
          ++entries;
          return true;
      }

      optional<string> get(const string& key) const {
          const auto& shard = shards[owner(key)];
          auto it = shard.find(key);
          if (it == shard.end()) return nullopt;
          return it->second;
      }

      bool erase(const string& key) {
          if (shards[owner(key)].erase(key) == 0) return false;
          --entries;
          return true;
      }

      size_t size() const { return entries; }
      size_t shardSize(size_t index) const { return shards.at(index).size(); }
  };
  ```
* **Code Explanation**: Every operation recomputes the same shard index. New-key admission happens after checking for an overwrite, so a full store can still update an existing value. The global counter changes only after a successful insert or erase; inspections do not create entries.
* **Invariants**: A key appears in exactly its routing shard, at most once; the sum of shard sizes equals `size()` and never exceeds capacity. Stored strings respect their byte limits.
* **Complexity**: Time: average `O(1)` map operations plus `O(L)` key hashing/comparison and `O(V)` value copying; worst-case collision work `O(C L)` for capacity `C`. Space: `O(S + C(L + V) + sum(B_s))` for `S` shards, configured byte limits, and each shard's retained bucket count `B_s`, plus a copied get result. Erase need not shrink bucket arrays: churn that fills each shard to capacity in turn can retain `O(S C)` bucket storage despite at most C live entries. Map rehashes can make an individual insert linear even without adversarial collisions.
* **Optimizations**: Runtime: reserve per-shard maps only when the likely distribution is known; shard locks can permit independent concurrent access but need a separate global admission protocol. Memory: use a shared byte budget or compact numeric keys when per-entry bounds are too coarse; rebuild sparse shard maps at quiescent boundaries to release high-water bucket storage.
* **Edge Cases To Consider**: Zero capacity, empty strings, overwrite at capacity, nonexistent erase, byte-limit violations, hash skew, and exhausted memory.
* **L7 Follow-ups**: Add versioned routing and fenced migration before changing membership; explain how reads behave during copy/cutover. Discuss replication, idempotency, per-shard capacity metrics, and durable recovery separately from this local ownership model.

## 7. Rolling-Window Top-K Log Aggregator

* **Pattern / Idea**: `rolling-window`, `top-k`: aggregate equal-timestamp events into expiry buckets, maintain total message counts, and select with a bounded size-k heap. Sorting all active unique messages is the baseline; the heap retains only the best candidates.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Ingest bounded log messages and return exact top-k counts over a rolling time window with deterministic ties.
* **Test Cases**: [Test cases](./test_cases.md#7-rolling-window-top-k-log-aggregator).
* **API Contract / Failure Model**: Serialized calls use caller-supplied nonnegative, nondecreasing integer ticks across both methods; equal ticks are allowed. Retain accepted events with `now - timestamp < window`, so the left endpoint is excluded. Window and message-byte limit must be positive; capacity may be zero. Invalid time/configuration throws `invalid_argument`; oversize messages throw `length_error`. `ingest` first expires old buckets, then returns false if the active-event cap is full; a capacity rejection still advances time. `topK` also advances time, returns up to k pairs ordered by descending count then lexicographic message, and accepts k=0. This is processing-time order with no late-event support, persistence, or internal synchronization. Allocation failure is outside the recoverable failure model; discard the object if it occurs during mutation.
* **C++ Code**
  ```cpp
  class RollingLogTopK {
      struct Bucket {
          long long time;
          unordered_map<string, size_t> counts;
      };
      deque<Bucket> buckets;
      unordered_map<string, size_t> totals;
      long long window;
      optional<long long> lastTime;
      size_t capacity, messageLimit;
      size_t events = 0;

      void advance(long long now) {
          if (now < 0 || (lastTime && now < *lastTime))
              throw invalid_argument("time must be nonnegative and monotonic");
          lastTime = now;
          // Monotonic timestamps let us expire only from the front.
          while (!buckets.empty() && now - buckets.front().time >= window) {
              for (const auto& item : buckets.front().counts) {
                  auto it = totals.find(item.first);
                  // Remove this bucket's contribution, preserving newer duplicates.
                  it->second -= item.second;
                  events -= item.second;
                  if (it->second == 0) totals.erase(it);
              }
              buckets.pop_front();
          }
      }
  public:
      RollingLogTopK(long long windowTicks, size_t maxEvents, size_t maxMessageBytes)
          : window(windowTicks), capacity(maxEvents), messageLimit(maxMessageBytes) {
          if (window <= 0 || messageLimit == 0)
              throw invalid_argument("positive window and message limit required");
      }

      bool ingest(long long now, const string& message) {
          if (message.size() > messageLimit) throw length_error("message too large");
          advance(now);
          if (events == capacity) return false;
          if (buckets.empty() || buckets.back().time != now)
              buckets.push_back({now, {}});
          // Bucket counts support expiry; totals support ranking across buckets.
          ++buckets.back().counts[message];
          ++totals[message];
          ++events;
          return true;
      }

      vector<pair<string, size_t>> topK(long long now, size_t k) {
          advance(now);
          if (k == 0) return {};
          using Entry = pair<string, size_t>;
          const size_t limit = min(k, totals.size());
          auto better = [](const Entry& a, const Entry& b) {
              if (a.second != b.second) return a.second > b.second;
              return a.first < b.first;
          };
          // A "better" comparator puts the worst retained entry at the top.
          priority_queue<Entry, vector<Entry>, decltype(better)> best(better);
          for (const auto& item : totals) {
              Entry candidate{item.first, item.second};
              if (best.size() < limit) best.push(std::move(candidate));
              else if (better(candidate, best.top())) {
                  // Replace only the worst selected item; the heap never exceeds k.
                  best.pop();
                  best.push(std::move(candidate));
              }
          }
          vector<Entry> result;
          result.reserve(best.size());
          while (!best.empty()) {
              result.push_back(best.top());
              best.pop();
          }
          // Heap removal is worst-first; the API returns best-first with stable ties.
          sort(result.begin(), result.end(), better);
          return result;
      }

      size_t size() const { return events; }
  };
  ```
* **Code Explanation**: Monotonic timestamps make the bucket deque ordered by expiry. Expiration subtracts each bucket's exact counts and removes zero totals. The heap's top is the worst retained message by count and lexical tie-break; a better candidate replaces it. Only the selected entries are sorted for output, independent of hash iteration order. Zero k returns immediately after expiration. A duration comparison avoids adding the window to a timestamp near the integer limit.
* **Invariants**: After each time-advancing call, every bucket lies in the active window, every stored total is positive, the sum of totals equals `size()`, and accepted active events never exceed capacity. After each scanned message, the heap contains the best `min(k, scannedUnique)` entries, with the worst retained item at its top. Rejecting backward time leaves state unchanged.
* **Complexity**: Time: ingest amortized average `O(1)` excluding message bytes; a call expiring `E` bucket entries costs average `O(E)`. For `U` active unique messages and `r=min(k,U)`, query costs average `O(E + U log(r+1) + r log(r+1))`, conventionally `O(U log(min(k,U)+1) + k log k)` plus expiration when `1 <= k <= U`; heap extraction and final sorting touch only the retained r entries. With k=0 the query is `O(E)` and does not scan totals. Every expiry bucket entry is created and removed once, so expiration is amortized over accepted ingestion, though a single idle-jump call can expire all retained state. Hash collisions can make each map operation linear in its map size; string hashing/comparison/copy adds up to the message length per operation. Space: `O(C L)` retained state for event cap `C` and message-byte limit `L`; selection and output use `O(r)` entries (`O(r L)` bytes), never a copy of all U totals unless k requests them all. Equal-timestamp duplicates share a bucket count.
* **Optimizations**: Runtime: the bounded heap meets the top-k selection target; an incrementally maintained ranking helps frequent queries at the cost of update work. Memory: intern message ids or use time-bucket sketches when approximate counts and coarser expiry are acceptable.
* **Edge Cases To Consider**: Exact expiry boundary, equal timestamps, empty messages, empty window, k=0 or above unique count, full capacity, long idle jumps, backward time, and maximum representable timestamps.
* **L7 Follow-ups**: Separate event time from processing time; define watermarks and late-event budgets before distributing ingestion. Discuss dropped-event counters, shard-local top-k merge limits, cardinality attacks, retention, and backpressure.

## 8. Load Balancer With Three Policies

* **Pattern / Idea**: `load-balancing`, `strategy`: explicit policy selection over fixed backend indices, with a bounded live-ticket map for trustworthy load accounting.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Dispatch requests using round-robin, weighted round-robin, or least-loaded selection and release load only for matching outstanding assignments.
* **Test Cases**: [Test cases](./test_cases.md#8-load-balancer-with-three-policies).
* **API Contract / Failure Model**: Serialized simulator with a nonempty fixed backend list and strictly positive integer weights; invalid weights, empty membership, or invalid policy values throw `invalid_argument`. Capacity counts all outstanding requests and may be zero. `dispatch` returns `nullopt` at capacity. Weighted round-robin issues each backend's weight as a consecutive burst, then advances; it is not smooth weighted round-robin. Each policy has its own cursor; least-loaded ties choose the lowest backend index. `complete` returns false for unknown, duplicate, stale, or backend-mismatched tickets. Completion represents either success or terminal failure, both releasing one outstanding assignment. Tokens are instance-local, never reused, and throw `overflow_error` before exhaustion; they are not authentication secrets and must not cross instance lifetimes. Allocation failure is outside the recoverable failure model. There is no health detection, timeout, retry, networking, or internal thread safety; a lost completion consumes capacity until the caller resolves it.
* **C++ Code**
  ```cpp
  class LoadBalancer {
  public:
      enum class Mode { RoundRobin, WeightedRoundRobin, LeastLoaded };
      struct Ticket { uint64_t token; size_t backend; };
  private:
      vector<int> weights;
      vector<size_t> loads;
      unordered_map<uint64_t, size_t> live;
      size_t capacity;
      size_t rr = 0, weighted = 0;
      int remaining;
      uint64_t nextToken = 1;
  public:
      LoadBalancer(vector<int> backendWeights, size_t maxOutstanding)
          : weights(std::move(backendWeights)), capacity(maxOutstanding), remaining(0) {
          if (weights.empty()) throw invalid_argument("backends required");
          for (int weight : weights)
              if (weight <= 0) throw invalid_argument("weights must be positive");
          loads.resize(weights.size(), 0);
          remaining = weights.front();
      }

      optional<Ticket> dispatch(Mode mode) {
          if (mode != Mode::RoundRobin && mode != Mode::WeightedRoundRobin &&
              mode != Mode::LeastLoaded) throw invalid_argument("unknown policy");
          if (live.size() == capacity) return nullopt;
          if (nextToken == numeric_limits<uint64_t>::max())
              throw overflow_error("ticket tokens exhausted");
          size_t index = 0;
          if (mode == Mode::RoundRobin) index = rr;
          else if (mode == Mode::WeightedRoundRobin) index = weighted;
          else {
              // Strict comparison preserves the lowest-index tie winner.
              for (size_t i = 1; i < loads.size(); ++i)
                  if (loads[i] < loads[index]) index = i;
          }

          Ticket ticket{nextToken, index};
          // Record ownership before changing loads or advancing policy cursors.
          live.emplace(ticket.token, index);
          ++nextToken;
          ++loads[index];
          if (mode == Mode::RoundRobin) rr = (rr + 1) % weights.size();
          // Burst weighting spends this backend's quota before moving to the next.
          if (mode == Mode::WeightedRoundRobin && --remaining == 0) {
              weighted = (weighted + 1) % weights.size();
              remaining = weights[weighted];
          }
          return ticket;
      }

      bool complete(const Ticket& ticket) {
          auto it = live.find(ticket.token);
          // Duplicate or mismatched completions must not decrement another load.
          if (it == live.end() || it->second != ticket.backend) return false;
          --loads[it->second];
          live.erase(it);
          return true;
      }

      size_t load(size_t backend) const { return loads.at(backend); }
      size_t outstanding() const { return live.size(); }
  };
  ```
* **Code Explanation**: Selection precedes ticket insertion; cursors and loads change only after admission succeeds. Burst weighting uses a countdown, avoiding an expanded weight array or sums that could overflow. The ticket map binds each completion to its originally selected backend; a mismatched completion cannot underflow another backend's load.
* **Invariants**: The sum of loads equals the live-ticket count and stays at most capacity. Every ticket contributes exactly one load unit until one matching completion. Each weighted cycle issues exactly its configured weight per backend when that policy is selected; rejected dispatches do not advance cursors.
* **Complexity**: Time: average `O(1)` dispatch for round-robin/burst weighted round-robin and completion; average `O(B)` least-loaded dispatch for `B` backends. Live-map collisions or rehashes can add `O(C)` work for outstanding cap `C`. Constructor time is `O(B)`. Space: `O(B + C)`, with no history of completed tickets and no weight-expanded schedule.
* **Optimizations**: Runtime: a heap gives `O(log B)` least-loaded updates but needs indexed updates and deterministic ties; smooth weighted round-robin reduces bursts with extra selection work. Memory: a fixed ticket-slot pool can avoid hash allocation, but needs generations to reject stale completions.
* **Edge Cases To Consider**: Zero capacity, one backend, extreme positive weights, invalid modes, repeated or forged completions, equal loads, switching policies, token exhaustion, and missing completions.
* **L7 Follow-ups**: Add health states, draining, bounded timeouts, and attempt generations before retrying requests. Compare outstanding-count estimates to measured latency, heterogeneous service costs, and power-of-two choices; monitor skew and leaked assignments.

## 9. Backpressure Controller With Hysteresis

* **Pattern / Idea**: `backpressure`, `admission-control`: high/low watermark hysteresis over an externally owned bounded queue. A single threshold is simpler but can oscillate on every enqueue/dequeue.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Decide whether a caller should admit, delay, or reject work based on queue depth without accumulating delayed work inside the controller.
* **Test Cases**: [Test cases](./test_cases.md#9-backpressure-controller-with-hysteresis).
* **API Contract / Failure Model**: Configure `0 <= low < high <= capacity`, positive capacity, and positive retry-delay ticks; violations throw `invalid_argument`. `decide(depth, now)` observes depth before enqueue. Depth must not exceed capacity, and timestamps must be nonnegative and nondecreasing; invalid calls throw before state changes. Depth at capacity returns Reject, depth at/above high pauses admission, and a paused controller resumes only at/below low. Delay returns a relative retry hint, never an accepted reservation or an absolute deadline. The caller owns the bounded queue, serializes depth observation/decision/enqueue, enqueues exactly once only on Admit, and retries delayed work later subject to its own deadline/retry budget. This O(1)-state policy owns no queue, tokens, acknowledgments, threads, timer, or durability; concurrent unsynchronized callers cannot rely on it to enforce capacity.
* **C++ Code**
  ```cpp
  class BackpressureController {
  public:
      enum class Decision { Admit, Delay, Reject };
      struct Result { Decision decision; long long retryAfter; };
  private:
      size_t low, high, capacity;
      long long delay;
      optional<long long> lastTime;
      bool paused = false;
  public:
      BackpressureController(size_t lowWatermark, size_t highWatermark,
                             size_t maxDepth, long long retryDelay)
          : low(lowWatermark), high(highWatermark), capacity(maxDepth), delay(retryDelay) {
          if (capacity == 0 || low >= high || high > capacity || delay <= 0)
              throw invalid_argument("invalid watermarks, capacity, or delay");
      }

      Result decide(size_t depth, long long now) {
          if (depth > capacity || now < 0 || (lastTime && now < *lastTime))
              throw invalid_argument("invalid depth or nonmonotonic time");
          lastTime = now;
          // Keep the previous pause state between watermarks to avoid oscillation.
          if (depth >= high) paused = true;
          else if (depth <= low) paused = false;
          // A full queue rejects work rather than promising a delayed admission.
          if (depth == capacity) return {Decision::Reject, 0};
          // Delay is only a retry hint; the caller retains ownership of the work.
          if (paused) return {Decision::Delay, delay};
          return {Decision::Admit, 0};
      }
  };
  ```
* **Code Explanation**: Validation happens first. Queue depth changes the pause state only at the watermarks, preventing repeated toggles inside the middle band. The hard-capacity check takes precedence over Delay. Returning a duration avoids timestamp addition overflow and leaves scheduling policy with the caller.
* **Invariants**: Admit requires depth below high and below capacity. Once paused, admission remains paused throughout the middle band until the low watermark is reached. The caller's serialized enqueue protocol keeps queue depth at most capacity; delayed work consumes no controller storage.
* **Complexity**: Time: `O(1)` per decision and construction. Space: `O(1)` policy state; the external bounded queue and caller retry storage are additional, explicitly outside the controller.
* **Optimizations**: Runtime: add bounded jitter to caller retries; latency-aware watermarks can react earlier than queue depth alone. Memory: bound client retry queues and deadlines so Delay cannot move overload into an unbounded buffer elsewhere.
* **Edge Cases To Consider**: Exact watermark equality, high equal to capacity, smallest valid capacity, sustained overload, backward/equal time, caller-supplied invalid depth, and retry hints near the timestamp limit.
* **L7 Follow-ups**: Couple decision and queue admission atomically for multiple producers; add fairness, per-tenant budgets, adaptive concurrency limits, and rejection/delay metrics. State when downstream failure should fail closed and how recovery avoids synchronized retries.

## 10. Per-Customer Ordered Scheduler

* **Pattern / Idea**: `per-customer-ordering`, `scheduler`: one FIFO per customer, a round-robin ready-customer queue, and at most one in-flight task per customer. A global FIFO with one worker preserves order but prevents independent customers from progressing in parallel.
* **Company Frequency Tags**: Public signal: not assessed; Domain fit: `Amazon/AWS: High`, `Google: High`.
* **Question**: Assign tasks to fixed workers while preserving FIFO completion order per customer, including retries and stale acknowledgments.
* **Test Cases**: [Test cases](./test_cases.md#10-per-customer-ordered-scheduler).
* **API Contract / Failure Model**: Serialized in-memory coordinator; workers execute assignments externally and may finish in any order across customers. Worker count and customer-byte limit must be positive; outstanding capacity may be zero and includes queued plus running tasks. Empty customer names and repeated numeric task payloads are valid. `enqueue` returns false at capacity and throws `length_error` for oversize names. `take(worker)` returns `nullopt` if that worker is busy or no customer is ready. Invalid worker indices throw `out_of_range`. `finish(worker, token, success)` returns false for stale/duplicate/wrong-worker tokens. Success removes only the customer's front task; failure releases the worker and retries that same front task with a new token on its next dispatch. Tokens are unique within the instance and throw `overflow_error` before exhaustion; they must not be reused across coordinator restarts. There are no automatic leases, cancellation, persistence, internal locks, or exactly-once external effects. Failure acknowledgment requires the previous execution to have stopped before retry; lost acknowledgments otherwise leave work blocked. Allocation failure during mutation is outside the recoverable contract; discard the object if it occurs.
* **C++ Code**
  ```cpp
  class CustomerScheduler {
  public:
      struct Assignment { uint64_t token, task; string customer; };
  private:
      struct Customer {
          deque<uint64_t> tasks;
          bool running = false;
      };
      unordered_map<string, Customer> customers;
      deque<string> ready;
      vector<optional<Assignment>> workers;
      size_t capacity, customerLimit;
      size_t outstandingTasks = 0;
      uint64_t nextToken = 1;
  public:
      CustomerScheduler(size_t workerCount, size_t maxOutstanding, size_t maxCustomerBytes)
          : capacity(maxOutstanding), customerLimit(maxCustomerBytes) {
          if (workerCount == 0 || customerLimit == 0)
              throw invalid_argument("positive worker count and customer limit required");
          workers.resize(workerCount);
      }

      bool enqueue(const string& customer, uint64_t task) {
          if (customer.size() > customerLimit) throw length_error("customer too large");
          if (outstandingTasks == capacity) return false;
          auto [it, inserted] = customers.try_emplace(customer);
          it->second.tasks.push_back(task);
          // Existing customers are already ready or running; do not enqueue twice.
          if (inserted) ready.push_back(customer);
          ++outstandingTasks;
          return true;
      }

      optional<Assignment> take(size_t worker) {
          auto& slot = workers.at(worker);
          if (slot || ready.empty()) return nullopt;
          if (nextToken == numeric_limits<uint64_t>::max())
              throw overflow_error("assignment tokens exhausted");
          const string& name = ready.front();
          auto& customer = customers.at(name);
          // Leave the task at the FIFO front until a matching success completes it.
          Assignment assignment{nextToken, customer.tasks.front(), name};
          slot = assignment;
          ++nextToken;
          customer.running = true;
          ready.pop_front();
          return assignment;
      }

      bool finish(size_t worker, uint64_t token, bool success) {
          auto& slot = workers.at(worker);
          // A late acknowledgment cannot finish this worker's newer assignment.
          if (!slot || slot->token != token) return false;
          auto it = customers.find(slot->customer);
          auto& customer = it->second;
          // Failure retains the front task, so retries cannot be overtaken.
          if (success) {
              customer.tasks.pop_front();
              --outstandingTasks;
          }
          customer.running = false;
          if (customer.tasks.empty()) customers.erase(it);
          // Returning to the tail lets other ready customers make progress.
          else ready.push_back(slot->customer);
          slot.reset();
          return true;
      }

      size_t size() const { return outstandingTasks; }
      size_t customerCount() const { return customers.size(); }
  };
  ```
* **Code Explanation**: The running task stays at its customer's FIFO front until success, so enqueueing behind it cannot overtake it. A customer is added to the ready queue only when first created or when its current assignment finishes with work remaining. Finishing returns that customer to the tail, permitting other ready customers to make progress. Worker slots retain only current assignments; tokens prevent old acknowledgments from completing a newer attempt, even when a customer name is deleted and recreated.
* **Invariants**: Each nonempty customer is either ready exactly once or running on exactly one worker. Every occupied worker corresponds to that customer's front task. `size()` equals the sum of customer queue lengths, includes running tasks, and never exceeds capacity; success deletes empty customer state. Only a matching current token advances a customer's FIFO.
* **Complexity**: Time: enqueue, take, and finish average `O(1)` excluding bounded customer-string hashing/copies, with worst-case map collision work `O(A)` for `A` active customers; insert rehashes may also be linear. Space: `O(C + (A + P)L + P)` for outstanding cap `C`, workers `P`, and customer-byte limit `L`; `A <= C`. Completed tasks/customers leave no history, and retrying does not grow the queue.
* **Optimizations**: Runtime: use stable numeric customer ids to reduce string copies; parallelize independent customers with a synchronized coordinator or ownership partitions. Memory: fixed task/worker pools can reduce allocator overhead, retaining generation checks if slots are reused.
* **Edge Cases To Consider**: Zero capacity, one hot customer, more workers than ready customers, enqueue during execution, duplicate payload ids, failure/retry, stale and wrong-worker acknowledgments, recreated customers, and a worker that never finishes.
* **L7 Follow-ups**: Add durable task state, fenced worker leases, bounded retries/dead letters, and idempotent effects before recovering from worker crashes. FIFO safety alone does not guarantee liveness: a permanently failing front task blocks that customer, and slow customers need age/lag metrics and explicit failure policy.

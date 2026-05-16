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

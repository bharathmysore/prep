# Advanced Data Structures Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. LRU Cache

* **Pattern / Idea**: Hash map plus doubly linked list.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 100.0)`, `Oracle: High (6m 100.0)`, `Meta: High (6m 76.0)`, `Microsoft: High (6m 69.7)`, `NVIDIA: High (6m 66.0)`, `Snowflake: Medium (6m 49.9)`.
* **Question**: Implement an LRU cache with `get` and `put`.
* **Test Cases**: [Test cases](./test_cases.md#1-lru-cache).
* **C++ Code**
  ```cpp
  class LRUCache {
      int cap;
      list<pair<int, int>> items; // key, value; front is most recent
      unordered_map<int, list<pair<int, int>>::iterator> pos;
  public:
      explicit LRUCache(int capacity) : cap(capacity) {}

      int get(int key) {
          auto it = pos.find(key);
          if (it == pos.end()) return -1;
          items.splice(items.begin(), items, it->second);
          return it->second->second;
      }

      void put(int key, int value) {
          if (auto it = pos.find(key); it != pos.end()) {
              it->second->second = value;
              items.splice(items.begin(), items, it->second);
              return;
          }
          if (static_cast<int>(items.size()) == cap) {
              pos.erase(items.back().first);
              items.pop_back();
          }
          items.push_front({key, value});
          pos[key] = items.begin();
      }
  };
  ```
* **Code Explanation**: The list maintains recency order; the map jumps to nodes in `O(1)`.
* **Invariants**: Every map iterator points to the matching key in the list, and the list size never exceeds capacity.
* **Complexity**: Average time `O(1)` per operation, space `O(capacity)`.
* **Optimizations**: Runtime: `splice` avoids node allocation on access. Memory: store key in list node for eviction.
* **Edge Cases To Consider**: Capacity one, update existing key, eviction order, repeated gets.
* **L7 Follow-ups**: Add sharded locks for concurrent high-throughput cache access.

## 2. LFU Cache

* **Pattern / Idea**: Key metadata plus frequency buckets, each bucket ordered by recency.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 69.5)`.
* **Question**: Implement an LFU cache with LRU tie-breaking.
* **Test Cases**: [Test cases](./test_cases.md#2-lfu-cache).
* **C++ Code**
  ```cpp
  class LFUCache {
      struct Entry { int value, freq; list<int>::iterator it; };
      int cap, minFreq = 0;
      unordered_map<int, Entry> data;
      unordered_map<int, list<int>> buckets;

      void touch(int key) {
          auto& e = data[key];
          int f = e.freq;
          buckets[f].erase(e.it);
          if (buckets[f].empty() && minFreq == f) ++minFreq;
          ++e.freq;
          buckets[e.freq].push_front(key);
          e.it = buckets[e.freq].begin();
      }
  public:
      explicit LFUCache(int capacity) : cap(capacity) {}

      int get(int key) {
          if (!data.count(key)) return -1;
          touch(key);
          return data[key].value;
      }

      void put(int key, int value) {
          if (cap == 0) return;
          if (data.count(key)) {
              data[key].value = value;
              touch(key);
              return;
          }
          if (static_cast<int>(data.size()) == cap) {
              int evict = buckets[minFreq].back();
              buckets[minFreq].pop_back();
              data.erase(evict);
          }
          minFreq = 1;
          buckets[1].push_front(key);
          data[key] = {value, 1, buckets[1].begin()};
      }
  };
  ```
* **Code Explanation**: Frequency buckets provide `O(1)` movement between counts and LRU eviction within the minimum frequency bucket.
* **Invariants**: `minFreq` is the smallest frequency present; every key is in exactly one bucket.
* **Complexity**: Average time `O(1)` per operation, space `O(capacity)`.
* **Optimizations**: Runtime: update `minFreq` only when a bucket empties. Memory: metadata per key plus bucket nodes.
* **Edge Cases To Consider**: Capacity zero, ties by recency, update increments frequency, eviction after get.
* **L7 Follow-ups**: LFU can be vulnerable to stale hot keys; production caches often use aging/admission policies.

## 3. TTL Cache

* **Pattern / Idea**: Hash map for values plus expiry heap with version numbers.
* **Company Frequency Tags**: Public signal: `OpenAI: High (6m 66.7)`, `Apple: Medium (6m 54.0)`, `Snowflake: Medium (6m 49.9)`, `Oracle: Medium (6m 47.0)`, `Databricks: High (all 79.9)`; Domain fit: `Stripe: High`.
* **Question**: Implement an in-memory TTL cache with `put`, `get`, and opportunistic cleanup.
* **Test Cases**: [Test cases](./test_cases.md#3-ttl-cache).
* **C++ Code**
  ```cpp
  class TTLCache {
      struct Val { string value; long long expiry; int version; };
      struct Exp { long long expiry; string key; int version; bool operator>(const Exp& o) const { return expiry > o.expiry; } };
      long long ttl;
      int nextVersion = 0;
      unordered_map<string, Val> data;
      priority_queue<Exp, vector<Exp>, greater<Exp>> pq;

      void evict(long long now) {
          while (!pq.empty() && pq.top().expiry <= now) {
              auto e = pq.top(); pq.pop();
              auto it = data.find(e.key);
              if (it != data.end() && it->second.version == e.version) data.erase(it);
          }
      }
  public:
      explicit TTLCache(long long ttlMillis) : ttl(ttlMillis) {}
      void put(const string& key, const string& value, long long now) {
          evict(now);
          int version = ++nextVersion;
          data[key] = {value, now + ttl, version};
          pq.push({now + ttl, key, version});
      }
      optional<string> get(const string& key, long long now) {
          evict(now);
          auto it = data.find(key);
          if (it == data.end()) return nullopt;
          return it->second.value;
      }
  };
  ```
* **Code Explanation**: Heap entries are lazy; version numbers distinguish stale expirations from current values.
* **Invariants**: A returned value exists in `data` and has not expired at `now`.
* **Complexity**: Average get `O(1)` plus eviction, put `O(log n)`, space `O(n)` plus stale heap entries.
* **Optimizations**: Runtime: lazy cleanup. Memory: periodic heap rebuild can remove stale expiry records.
* **Edge Cases To Consider**: Overwrite before expiry, get after expiry, many overwrites, zero TTL.
* **L7 Follow-ups**: Use monotonic clocks; wall-clock jumps can break expiry semantics.

## 4. Fenwick Tree

* **Pattern / Idea**: Prefix sums with lowbit ranges.
* **Company Frequency Tags**: Public signal: `Google: Low (all 5.0)`.
* **Question**: Implement a range-sum structure with point updates.
* **Test Cases**: [Test cases](./test_cases.md#4-fenwick-tree).
* **C++ Code**
  ```cpp
  class Fenwick {
      vector<long long> bit;
  public:
      explicit Fenwick(int n) : bit(n + 1, 0) {}
      void add(int index, long long delta) {
          for (++index; index < static_cast<int>(bit.size()); index += index & -index)
              bit[index] += delta;
      }
      long long prefixSum(int index) const {
          long long ans = 0;
          for (++index; index > 0; index -= index & -index)
              ans += bit[index];
          return ans;
      }
      long long rangeSum(int l, int r) const {
          return prefixSum(r) - (l == 0 ? 0 : prefixSum(l - 1));
      }
  };
  ```
* **Code Explanation**: Each tree slot stores a power-of-two suffix of a prefix.
* **Invariants**: `bit[i]` summarizes the range `(i - lowbit(i), i]` in one-based indexing.
* **Complexity**: Update/query `O(log n)`, space `O(n)`.
* **Optimizations**: Runtime: tight iterative loops. Memory: one array.
* **Edge Cases To Consider**: Single element, negative deltas, range at boundaries, repeated updates.
* **L7 Follow-ups**: Segment trees are more flexible for non-invertible operations.

## 5. Interval Assignment Map

* **Pattern / Idea**: Ordered map of interval starts with coalescing.
* **Company Frequency Tags**: Public signal: `Databricks: Medium (all 37.2)`; Domain fit: `Stripe: Medium`, `Snowflake: Medium`.
* **Question**: Implement an interval assignment map that stores non-overlapping ranges compactly.
* **Test Cases**: [Test cases](./test_cases.md#5-interval-assignment-map).
* **C++ Code**
  ```cpp
  class IntervalMap {
      map<int, string> startValue; // value applies from key until next key
      string defaultValue;
  public:
      explicit IntervalMap(string def) : defaultValue(move(def)) {}

      string get(int x) const {
          auto it = startValue.upper_bound(x);
          if (it == startValue.begin()) return defaultValue;
          return prev(it)->second;
      }

      void assign(int l, int r, const string& value) {
          if (l >= r) return;
          string after = get(r);
          auto first = startValue.lower_bound(l);
          auto last = startValue.lower_bound(r);
          startValue.erase(first, last);
          if (get(l) != value) startValue[l] = value;
          if (value != after) startValue[r] = after;

          auto it = startValue.lower_bound(l);
          if (it != startValue.begin()) {
              auto p = prev(it);
              if (it != startValue.end() && p->second == it->second) startValue.erase(it);
          }
      }
  };
  ```
* **Code Explanation**: Boundaries mark value changes; assigning a range removes interior boundaries and restores the value after `r`.
* **Invariants**: Adjacent ranges with the same value should be coalesced.
* **Complexity**: Time `O(log n + changed intervals)`, space `O(number of boundaries)`.
* **Optimizations**: Runtime: erase range in one map operation. Memory: coalescing keeps representation compact.
* **Edge Cases To Consider**: Empty assignment, overwrite middle, assign default, adjacent equal ranges.
* **L7 Follow-ups**: This pattern appears in feature flags, ACLs, and memory maps.

## 6. Rolling Metrics Window

* **Pattern / Idea**: Fixed time buckets.
* **Company Frequency Tags**: Public signal: `Google: Low (6m 24.0)`; Domain fit: `Snowflake: High`, `Databricks: High`, `Stripe: Medium`, `OpenAI: Medium`.
* **Question**: Implement a rolling metrics window for counts and sums over the last five minutes.
* **Test Cases**: [Test cases](./test_cases.md#6-rolling-metrics-window).
* **C++ Code**
  ```cpp
  class RollingCounter {
      struct Bucket { long long start = -1; long long count = 0; };
      long long bucketMs, windowMs;
      vector<Bucket> buckets;
  public:
      RollingCounter(long long windowMillis, long long bucketMillis)
          : bucketMs(bucketMillis), windowMs(windowMillis),
            buckets((windowMillis + bucketMillis - 1) / bucketMillis) {}

      void add(long long now, long long delta = 1) {
          long long start = (now / bucketMs) * bucketMs;
          Bucket& b = buckets[(now / bucketMs) % buckets.size()];
          if (b.start != start) b = {start, 0};
          b.count += delta;
      }

      long long query(long long now) const {
          long long minStart = now - windowMs;
          long long total = 0;
          for (const Bucket& b : buckets)
              if (b.start > minStart) total += b.count;
          return total;
      }
  };
  ```
* **Code Explanation**: Each bucket is reused when time wraps around, resetting stale state.
* **Invariants**: A bucket contributes only if its start time is inside the active window.
* **Complexity**: Add `O(1)`, query `O(buckets)`, space `O(buckets)`.
* **Optimizations**: Runtime: maintain rolling total for `O(1)` query. Memory: coarser buckets reduce footprint at lower precision.
* **Edge Cases To Consider**: Bucket boundary, long idle gap, wraparound, high burst count.
* **L7 Follow-ups**: Distinguish event time from processing time and define late-event behavior.

# Hashing Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. First Non-Repeating Character In A Stream

* **Pattern / Idea**: Frequency table plus queue with lazy stale removal.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 69.3)`, `Google: Medium (6m 37.2)`.
* **Question**: Given a stream of characters, return the first non-repeating character after each update.
* **Test Cases**: [Test cases](./test_cases.md#1-first-non-repeating-character-in-a-stream).
* **C++ Code**
  ```cpp
  vector<char> firstUniqueAfterEachChar(const string& stream) {
      vector<int> freq(256, 0);
      queue<unsigned char> q;
      vector<char> ans;
      for (unsigned char c : stream) {
          ++freq[c];
          q.push(c);
          while (!q.empty() && freq[q.front()] > 1) q.pop();
          ans.push_back(q.empty() ? '#' : static_cast<char>(q.front()));
      }
      return ans;
  }
  ```
* **Code Explanation**: Every character is queued when first observed. Repeated characters are removed only when they reach the queue front.
* **Invariants**: The queue front, if present, is the earliest character with frequency one.
* **Complexity**: Amortized time `O(1)` per character, space `O(k)`.
* **Optimizations**: Runtime: lazy deletion avoids scanning all chars. Memory: fixed array for byte alphabet.
* **Edge Cases To Consider**: Empty stream, all duplicates, all unique, repeated first char.
* **L7 Follow-ups**: For distributed streams, partition by key and define ordering guarantees.

## 2. Group Anagrams

* **Pattern / Idea**: Hash by canonical signature.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 86.8)`, `NVIDIA: High (6m 84.8)`, `Amazon/AWS: High (6m 79.9)`, `Apple: High (6m 71.9)`, `Microsoft: High (6m 64.1)`, `Google: Medium (6m 49.2)`, `Meta: Medium (6m 43.7)`.
* **Question**: Given strings, group anagrams together.
* **Test Cases**: [Test cases](./test_cases.md#2-group-anagrams).
* **C++ Code**
  ```cpp
  vector<vector<string>> groupAnagrams(const vector<string>& words) {
      unordered_map<string, vector<string>> groups;
      for (const string& w : words) {
          array<int, 26> cnt{};
          for (char c : w) ++cnt[c - 'a'];
          string key;
          for (int x : cnt) {
              key += '#';
              key += to_string(x);
          }
          groups[key].push_back(w);
      }
      vector<vector<string>> ans;
      ans.reserve(groups.size());
      for (auto& [_, g] : groups) ans.push_back(move(g));
      return ans;
  }
  ```
* **Code Explanation**: Words with the same letter counts share a signature and therefore belong together.
* **Invariants**: Equal keys imply equal character multisets.
* **Complexity**: Time `O(total characters + groups * alphabet)`, space `O(total characters)`.
* **Optimizations**: Runtime: count signature beats sorting for bounded alphabets. Memory: reuse compact signatures.
* **Edge Cases To Consider**: Empty string, duplicates, single word, non-lowercase input if allowed.
* **L7 Follow-ups**: For Unicode, normalize strings and use a sparse count map.

## 3. Longest Consecutive Sequence

* **Pattern / Idea**: Hash set and start-of-run detection.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: High (6m 66.1)`, `Google: Medium (6m 58.6)`, `Microsoft: Medium (6m 50.0)`, `Apple: Medium (6m 47.7)`, `Oracle: Medium (6m 47.0)`, `Meta: Medium (6m 38.0)`.
* **Question**: Given an integer array, return the length of the longest consecutive sequence.
* **Test Cases**: [Test cases](./test_cases.md#3-longest-consecutive-sequence).
* **C++ Code**
  ```cpp
  int longestConsecutive(const vector<int>& nums) {
      unordered_set<int> s(nums.begin(), nums.end());
      int best = 0;
      for (int x : s) {
          if (s.count(x - 1)) continue;
          int len = 1;
          while (s.count(x + len)) ++len;
          best = max(best, len);
      }
      return best;
  }
  ```
* **Code Explanation**: Only values without a predecessor start runs, so every value is counted in at most one run.
* **Invariants**: A counted run always begins at its minimum value.
* **Complexity**: Average time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: skip non-starts. Memory: sort in place for `O(1)` extra space at `O(n log n)` time.
* **Edge Cases To Consider**: Duplicates, negative values, empty input, `INT_MIN` predecessor overflow.
* **L7 Follow-ups**: Discuss adversarial hash behavior and memory pressure for very large sets.

## 4. Randomized Set

* **Pattern / Idea**: Vector for random access, map for index lookup.
* **Company Frequency Tags**: Public signal: `Oracle: Medium (6m 47.0)`, `NVIDIA: Medium (all 59.6)`, `Snowflake: Medium (all 48.6)`.
* **Question**: Design a randomized set supporting insert, delete, and getRandom in average `O(1)`.
* **Test Cases**: [Test cases](./test_cases.md#4-randomized-set).
* **C++ Code**
  ```cpp
  class RandomizedSet {
      vector<int> values;
      unordered_map<int, int> index;
      mt19937 rng{random_device{}()};
  public:
      bool insert(int x) {
          if (index.count(x)) return false;
          index[x] = static_cast<int>(values.size());
          values.push_back(x);
          return true;
      }
      bool remove(int x) {
          auto it = index.find(x);
          if (it == index.end()) return false;
          int i = it->second, last = values.back();
          values[i] = last;
          index[last] = i;
          values.pop_back();
          index.erase(it);
          return true;
      }
      int getRandom() {
          uniform_int_distribution<int> dist(0, static_cast<int>(values.size()) - 1);
          return values[dist(rng)];
      }
  };
  ```
* **Code Explanation**: Removal swaps the deleted value with the last vector element, preserving dense indexing.
* **Invariants**: `index[x]` always points to `x` in `values`.
* **Complexity**: Average time `O(1)` per operation, space `O(n)`.
* **Optimizations**: Runtime: swap-delete. Memory: vector is compact; map is the main overhead.
* **Edge Cases To Consider**: Remove missing value, remove last value, repeated inserts, random from one element.
* **L7 Follow-ups**: Add locking or sharding if accessed concurrently.

## 5. TTL Deduplication Store

* **Pattern / Idea**: Hash set plus expiry queue.
* **Company Frequency Tags**: Public signal: `Google: Medium (all 42.2)`.
* **Question**: Given an event stream, suppress duplicate event ids within a retention window.
* **Test Cases**: [Test cases](./test_cases.md#5-ttl-deduplication-store).
* **C++ Code**
  ```cpp
  class Deduper {
      long long ttl;
      unordered_set<string> seen;
      queue<pair<long long, string>> expiry;

      void evict(long long now) {
          while (!expiry.empty() && expiry.front().first <= now) {
              seen.erase(expiry.front().second);
              expiry.pop();
          }
      }
  public:
      explicit Deduper(long long ttlMillis) : ttl(ttlMillis) {}

      bool firstSeen(const string& id, long long now) {
          evict(now);
          if (seen.count(id)) return false;
          seen.insert(id);
          expiry.push({now + ttl, id});
          return true;
      }
  };
  ```
* **Code Explanation**: An id is accepted only if it is absent after expiring old ids.
* **Invariants**: `seen` contains ids whose queued expiry time has not passed.
* **Complexity**: Amortized time `O(1)`, space `O(window)`.
* **Optimizations**: Runtime: lazy eviction. Memory: TTL bounds state; use compact ids or hashes if needed.
* **Edge Cases To Consider**: Same id before/after TTL, out-of-order timestamps, large burst.
* **L7 Follow-ups**: In distributed systems, idempotency requires durable storage and partition ownership.

## 6. Isomorphic Strings

* **Pattern / Idea**: Enforce one-to-one mapping.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (6m 36.0)`, `Google: Medium (6m 35.6)`, `Oracle: Medium (all 48.4)`.
* **Question**: Given two strings, determine whether they are isomorphic.
* **Test Cases**: [Test cases](./test_cases.md#6-isomorphic-strings).
* **C++ Code**
  ```cpp
  bool isIsomorphic(const string& a, const string& b) {
      if (a.size() != b.size()) return false;
      vector<int> ma(256, -1), mb(256, -1);
      for (int i = 0; i < static_cast<int>(a.size()); ++i) {
          unsigned char x = static_cast<unsigned char>(a[i]);
          unsigned char y = static_cast<unsigned char>(b[i]);
          if (ma[x] != mb[y]) return false;
          ma[x] = mb[y] = i;
      }
      return true;
  }
  ```
* **Code Explanation**: The two characters must have last appeared at the same previous index.
* **Invariants**: Equal last-seen positions encode a consistent bijection for processed prefixes.
* **Complexity**: Time `O(n)`, space `O(k)`.
* **Optimizations**: Runtime: single pass. Memory: fixed arrays for byte alphabets.
* **Edge Cases To Consider**: Different lengths, many-to-one mapping, repeated patterns, empty strings.
* **L7 Follow-ups**: For Unicode, map normalized code points rather than raw bytes.

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

## 7. Two Sum

* **Pattern / Idea**: `hash-map`, `complement-lookup`; store earlier values so each position can find its partner in one pass.
* **Company Frequency Tags**: Public signal: not scored in the current company-frequency index; no company-specific claim is made.
* **Question**: [LeetCode 1](https://leetcode.com/problems/two-sum/) asks for two distinct zero-based indices whose values add to `target`; exactly one pair exists.
* **Test Cases**: [Test cases](./test_cases.md#7-two-sum).
* **C++ Code**
  ```cpp
  vector<int> twoSum(const vector<int>& nums, int target) {
      unordered_map<int, int> firstIndex;
      firstIndex.reserve(nums.size());
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          const long long need = static_cast<long long>(target) - nums[i];
          if (need >= INT_MIN && need <= INT_MAX) {
              const auto it = firstIndex.find(static_cast<int>(need));
              if (it != firstIndex.end()) return {it->second, i};
          }
          firstIndex.emplace(nums[i], i);
      }
      return {};  // Outside LeetCode's exactly-one-solution contract.
  }
  ```
* **Code Explanation**: Before inserting the current value, look up the complement among earlier indices. This order prevents using the same element twice; `emplace` retains the first index for duplicates.
* **Invariants**: At the start of iteration `i`, the map contains only values from indices less than `i`, with one valid index per value.
* **Complexity**: Average time `O(n)` and space `O(n)`; a collision-heavy hash table can degrade lookup time toward `O(n²)` overall.
* **Optimizations**: Runtime: reserve the map once to reduce rehashing. Memory: sorting index-value pairs uses `O(n)` space as well, so the hash map is simpler for this API.
* **Edge Cases To Consider**: Equal values at different indices, negative values, complement arithmetic near integer limits, no match outside the stated contract.
* **L7 Follow-ups**: For a stream of targets, index the immutable input once; for adversarial keys, consider an ordered map with `O(n log n)` worst-case time.

## 8. Valid Anagram

* **Pattern / Idea**: `frequency-map`, `character-counting`; equal strings up to permutation have identical counts for every letter.
* **Company Frequency Tags**: Public signal: not scored in the current company-frequency index; no company-specific claim is made.
* **Question**: [LeetCode 242](https://leetcode.com/problems/valid-anagram/) asks whether two lowercase English strings contain the same letters with the same multiplicities.
* **Test Cases**: [Test cases](./test_cases.md#8-valid-anagram).
* **C++ Code**
  ```cpp
  bool isAnagram(const string& s, const string& t) {
      if (s.size() != t.size()) return false;
      array<int, 26> count{};
      for (size_t i = 0; i < s.size(); ++i) {
          ++count[s[i] - 'a'];
          --count[t[i] - 'a'];
      }
      return all_of(count.begin(), count.end(),
                    [](int value) { return value == 0; });
  }
  ```
* **Code Explanation**: Add counts from `s` and subtract matching positions from `t`. All zeroes at the end mean every letter occurs equally often.
* **Invariants**: After processing prefix length `i`, each slot is the count difference between those two prefixes.
* **Complexity**: Time `O(n + 26) = O(n)` and auxiliary space `O(26) = O(1)` for the fixed alphabet.
* **Optimizations**: Runtime: length rejection avoids scanning unequal lengths. Memory: the fixed array avoids hash-table allocation; sorting would use `O(n log n)` time.
* **Edge Cases To Consider**: Equal lengths with different counts, repeated letters, empty strings outside LeetCode's minimum length, Unicode input outside the lowercase contract.
* **L7 Follow-ups**: For Unicode, define normalization and count code points with a map; byte counts alone can misclassify equivalent text.

## 9. Top K Frequent Elements

* **Pattern / Idea**: `frequency-map`, `bucket-sort`, `top-k`; use the fact that each frequency lies between `1` and `n`.
* **Company Frequency Tags**: Public signal: not scored for this exact solution in the current company-frequency index; no company-specific claim is made.
* **Question**: [LeetCode 347](https://leetcode.com/problems/top-k-frequent-elements/) asks for the `k` most frequent distinct integers, in any order; the problem guarantees a unique answer.
* **Test Cases**: [Test cases](./test_cases.md#9-top-k-frequent-elements).
* **C++ Code**
  ```cpp
  vector<int> topKFrequent(const vector<int>& nums, int k) {
      unordered_map<int, int> frequency;
      frequency.reserve(nums.size());
      for (int x : nums) ++frequency[x];

      vector<vector<int>> buckets(nums.size() + 1);
      for (const auto& [value, count] : frequency) {
          buckets[count].push_back(value);
      }

      vector<int> answer;
      answer.reserve(k);
      for (size_t count = buckets.size(); count-- > 1 && answer.size() < static_cast<size_t>(k);) {
          for (int value : buckets[count]) answer.push_back(value);
      }
      return answer;
  }
  ```
* **Code Explanation**: Count each value, place it in the bucket matching its frequency, then scan buckets from highest frequency downward. The unique-answer guarantee means the last nonempty bucket cannot straddle the `k` boundary.
* **Invariants**: Every distinct value appears in exactly one bucket indexed by its frequency; before scanning frequency `f`, all selected values have frequency greater than `f`.
* **Complexity**: Average time `O(n)` and space `O(n)` for the hash table and buckets; adversarial hash collisions can make counting `O(n²)`.
* **Optimizations**: Runtime: buckets meet the better-than-`O(n log n)` follow-up. Memory: a size-`k` min-heap uses `O(u + k)` space with the frequency map and `O(n + u log k)` average time, where `u` is distinct values.
* **Edge Cases To Consider**: `k = 1`, `k = u`, negative values, one repeated value, ties below the cutoff, and the unique-answer assumption at the cutoff.
* **L7 Follow-ups**: With distributed shards, merge per-key counts before ranking; local top-`k` lists alone can miss a globally frequent key.

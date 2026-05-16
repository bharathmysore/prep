# Arrays And Strings Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`. These are representative high-frequency interview problems, written in the local L7 format.

## 1. Two Sum In A Sorted Array

* **Pattern / Idea**: Two pointers. Sorted input lets us move the smaller side up or the larger side down.
* **Company Frequency Tags**: Public signal: `Meta: Medium (6m 39.6)`, `Apple: Medium (6m 38.9)`, `Microsoft: Low (6m 22.6)`, `Oracle: Medium (all 33.6)`.
* **Question**: Given a sorted integer array and a target, return the two indices whose values sum to the target, or `{-1, -1}` if no pair exists.
* **Test Cases**: [Test cases](./test_cases.md#1-two-sum-in-a-sorted-array).
* **C++ Code**
  ```cpp
  vector<int> twoSumSorted(const vector<int>& a, int target) {
      int l = 0, r = static_cast<int>(a.size()) - 1;
      while (l < r) {
          long long sum = static_cast<long long>(a[l]) + a[r];
          if (sum == target) return {l, r};
          if (sum < target) ++l;
          else --r;
      }
      return {-1, -1};
  }
  ```
* **Code Explanation**: At each step, the current pair is the only pair using both endpoints. If it is too small, every pair with `r` and an index `<= l` is too small, so increment `l`; if too large, decrement `r`.
* **Invariants**: Any valid answer remains inside `[l, r]`.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: one pass, no hashing. Memory: no auxiliary map because sorted order is enough.
* **Edge Cases To Consider**: Empty, one element, negatives, duplicate values, no answer, overflow near `INT_MAX`.
* **L7 Follow-ups**: For unsorted input, choose hash map `O(n)` space or sort with index tracking `O(n log n)`.

## 2. Longest Substring Without Repeating Characters

* **Pattern / Idea**: Sliding window with last-seen positions.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 93.3)`, `Amazon/AWS: High (6m 82.5)`, `Microsoft: High (6m 72.4)`, `NVIDIA: High (6m 66.0)`, `Google: High (6m 65.1)`, `Apple: Medium (6m 58.9)`, `Meta: Medium (6m 57.0)`, `Snowflake: Medium (all 39.6)`.
* **Question**: Given a string, return the length of the longest substring without repeating characters.
* **Test Cases**: [Test cases](./test_cases.md#2-longest-substring-without-repeating-characters).
* **C++ Code**
  ```cpp
  int longestUniqueSubstring(const string& s) {
      vector<int> last(256, -1);
      int best = 0, left = 0;
      for (int right = 0; right < static_cast<int>(s.size()); ++right) {
          unsigned char c = static_cast<unsigned char>(s[right]);
          if (last[c] >= left) left = last[c] + 1;
          last[c] = right;
          best = max(best, right - left + 1);
      }
      return best;
  }
  ```
* **Code Explanation**: `left` jumps past the previous occurrence of a repeated character. The window never shrinks one step at a time when a direct jump is safe.
* **Invariants**: `s[left..right]` contains no duplicate byte characters.
* **Complexity**: Time `O(n)`, space `O(k)` where `k` is alphabet size.
* **Optimizations**: Runtime: direct last-seen jump. Memory: fixed array for byte input; use `unordered_map<char32_t,int>` for Unicode code points.
* **Edge Cases To Consider**: Empty string, all same chars, all unique chars, repeated char before current window.
* **L7 Follow-ups**: Clarify byte vs Unicode grapheme semantics in product code.

## 3. Minimum Window Substring

* **Pattern / Idea**: Sliding window with required frequency counts.
* **Company Frequency Tags**: Public signal: `Snowflake: High (6m 100.0)`, `Meta: High (6m 68.9)`, `Amazon/AWS: Medium (6m 56.1)`, `Microsoft: Medium (6m 50.0)`, `Apple: Medium (all 37.4)`, `Oracle: Medium (all 33.6)`.
* **Question**: Given a string `s` and a string `t`, return the minimum window in `s` that contains all characters from `t`.
* **Test Cases**: [Test cases](./test_cases.md#3-minimum-window-substring).
* **C++ Code**
  ```cpp
  string minWindow(string s, string t) {
      if (t.empty()) return "";
      vector<int> need(256, 0), have(256, 0);
      int required = 0;
      for (unsigned char c : t) {
          if (need[c]++ == 0) ++required;
      }

      int formed = 0, bestLen = INT_MAX, bestL = 0;
      for (int l = 0, r = 0; r < static_cast<int>(s.size()); ++r) {
          unsigned char cr = static_cast<unsigned char>(s[r]);
          if (++have[cr] == need[cr] && need[cr] > 0) ++formed;

          while (formed == required) {
              if (r - l + 1 < bestLen) {
                  bestLen = r - l + 1;
                  bestL = l;
              }
              unsigned char cl = static_cast<unsigned char>(s[l++]);
              if (have[cl]-- == need[cl] && need[cl] > 0) --formed;
          }
      }
      return bestLen == INT_MAX ? "" : s.substr(bestL, bestLen);
  }
  ```
* **Code Explanation**: Expand until all required characters are covered, then contract while preserving validity to find the smallest valid window ending at `r`.
* **Invariants**: `formed == required` iff the current window satisfies all required positive counts.
* **Complexity**: Time `O(n + m)`, space `O(k)`.
* **Optimizations**: Runtime: each pointer moves at most `n` times. Memory: fixed arrays when character domain is bounded.
* **Edge Cases To Consider**: `t` longer than `s`, duplicate required chars, no window, exact whole-string window.
* **L7 Follow-ups**: For streams, maintain a bounded deque of relevant characters instead of storing all input.

## 4. Count Subarrays With Sum K

* **Pattern / Idea**: Prefix sum plus hash frequency.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 66.3)`, `NVIDIA: High (6m 66.0)`, `Google: Medium (6m 58.2)`.
* **Question**: Given an integer array and a target `k`, return the number of subarrays with sum exactly `k`.
* **Test Cases**: [Test cases](./test_cases.md#4-count-subarrays-with-sum-k).
* **C++ Code**
  ```cpp
  long long countSubarraysWithSumK(const vector<int>& nums, long long k) {
      unordered_map<long long, long long> seen;
      seen.reserve(nums.size() * 2 + 1);
      seen[0] = 1;
      long long prefix = 0, ans = 0;
      for (int x : nums) {
          prefix += x;
          auto it = seen.find(prefix - k);
          if (it != seen.end()) ans += it->second;
          ++seen[prefix];
      }
      return ans;
  }
  ```
* **Code Explanation**: A subarray ending at the current index has sum `k` when an earlier prefix equals `currentPrefix - k`.
* **Invariants**: `seen` stores frequencies of all prefixes before the current position.
* **Complexity**: Average time `O(n)`, worst-case hash behavior can degrade; space `O(n)`.
* **Optimizations**: Runtime: reserve hash capacity. Memory: if all numbers are non-negative, sliding window may reduce space to `O(1)`.
* **Edge Cases To Consider**: Negative numbers, zeros, many repeated prefixes, large sums requiring `long long`.
* **L7 Follow-ups**: For untrusted keys, discuss hash hardening or ordered map fallback.

## 5. Merge Intervals

* **Pattern / Idea**: Sort by start, then maintain a canonical merged suffix.
* **Company Frequency Tags**: Public signal: `Meta: High (6m 81.2)`, `Amazon/AWS: High (6m 80.5)`, `Apple: High (6m 74.2)`, `Microsoft: High (6m 65.4)`, `Oracle: High (6m 64.0)`, `NVIDIA: High (all 67.4)`, `Snowflake: Medium (all 48.6)`, `Databricks: Medium (all 37.2)`.
* **Question**: Given a list of intervals, merge overlaps and return a canonical non-overlapping interval list.
* **Test Cases**: [Test cases](./test_cases.md#5-merge-intervals).
* **C++ Code**
  ```cpp
  vector<pair<int, int>> mergeIntervals(vector<pair<int, int>> intervals) {
      sort(intervals.begin(), intervals.end());
      vector<pair<int, int>> out;
      for (auto [s, e] : intervals) {
          if (out.empty() || s > out.back().second) {
              out.push_back({s, e});
          } else {
              out.back().second = max(out.back().second, e);
          }
      }
      return out;
  }
  ```
* **Code Explanation**: Once intervals are sorted, only the last emitted interval can overlap the next interval.
* **Invariants**: `out` is sorted, non-overlapping, and covers all processed intervals.
* **Complexity**: Time `O(n log n)`, space `O(n)` output.
* **Optimizations**: Runtime: if input is already sorted, skip sorting and run `O(n)`. Memory: reserve `out`.
* **Edge Cases To Consider**: Empty input, touching boundaries, nested intervals, duplicate intervals.
* **L7 Follow-ups**: Clarify whether `[1,2]` and `[2,3]` overlap for closed vs half-open intervals.

## 6. Product Of Array Except Self

* **Pattern / Idea**: Prefix products and suffix products without division.
* **Company Frequency Tags**: Public signal: `NVIDIA: High (6m 77.0)`, `Apple: Medium (6m 47.7)`, `Microsoft: Medium (6m 40.4)`, `Oracle: Medium (all 54.7)`.
* **Question**: Given an array, return the product of all elements except self without using division.
* **Test Cases**: [Test cases](./test_cases.md#6-product-of-array-except-self).
* **C++ Code**
  ```cpp
  vector<long long> productExceptSelf(const vector<int>& nums) {
      vector<long long> ans(nums.size(), 1);
      long long prefix = 1;
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          ans[i] = prefix;
          prefix *= nums[i];
      }
      long long suffix = 1;
      for (int i = static_cast<int>(nums.size()) - 1; i >= 0; --i) {
          ans[i] *= suffix;
          suffix *= nums[i];
      }
      return ans;
  }
  ```
* **Code Explanation**: First pass writes product of elements left of `i`; second pass multiplies by product of elements right of `i`.
* **Invariants**: Before second pass update, `ans[i]` equals left product and `suffix` equals product to the right.
* **Complexity**: Time `O(n)`, auxiliary space `O(1)` excluding output.
* **Optimizations**: Runtime: two linear passes. Memory: avoid separate prefix and suffix arrays.
* **Edge Cases To Consider**: One zero, multiple zeros, negative values, overflow constraints.
* **L7 Follow-ups**: In production, specify overflow behavior or use modular arithmetic / big integers.

## 7. Difference Array Range Updates

* **Pattern / Idea**: Store boundary deltas, then prefix-sum once.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given many range increment operations over an array, return the final array.
* **Test Cases**: [Test cases](./test_cases.md#7-difference-array-range-updates).
* **C++ Code**
  ```cpp
  vector<long long> applyRangeAdds(int n, const vector<tuple<int, int, int>>& updates) {
      vector<long long> diff(n + 1, 0);
      for (auto [l, r, delta] : updates) {
          diff[l] += delta;
          if (r + 1 < n) diff[r + 1] -= delta;
      }
      vector<long long> ans(n);
      long long cur = 0;
      for (int i = 0; i < n; ++i) {
          cur += diff[i];
          ans[i] = cur;
      }
      return ans;
  }
  ```
* **Code Explanation**: A range increment starts at `l` and stops after `r`; prefix accumulation materializes the final values.
* **Invariants**: `cur` equals the sum of all active range deltas at index `i`.
* **Complexity**: Time `O(n + q)`, space `O(n)`.
* **Optimizations**: Runtime: `O(1)` per update. Memory: coordinate compression for sparse huge ranges.
* **Edge Cases To Consider**: Empty updates, boundary at index `0`, update ending at `n - 1`, overlapping updates.
* **L7 Follow-ups**: For online queries, use Fenwick or segment tree instead.

## 8. Insert Interval

* **Pattern / Idea**: Linear scan through sorted non-overlapping intervals.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 47.7)`, `Meta: Low (6m 10.2)`, `Google: Medium (all 47.3)`, `Amazon/AWS: Medium (all 42.7)`, `Oracle: Medium (all 39.7)`, `Microsoft: Medium (all 37.7)`.
* **Question**: Given sorted non-overlapping intervals and a new interval, insert it and merge any overlaps.
* **Test Cases**: [Test cases](./test_cases.md#8-insert-interval).
* **C++ Code**
  ```cpp
  vector<pair<int, int>> insertInterval(vector<pair<int, int>> intervals, pair<int, int> add) {
      vector<pair<int, int>> out;
      int i = 0, n = static_cast<int>(intervals.size());
      while (i < n && intervals[i].second < add.first) out.push_back(intervals[i++]);
      while (i < n && intervals[i].first <= add.second) {
          add.first = min(add.first, intervals[i].first);
          add.second = max(add.second, intervals[i].second);
          ++i;
      }
      out.push_back(add);
      while (i < n) out.push_back(intervals[i++]);
      return out;
  }
  ```
* **Code Explanation**: Emit intervals strictly before the new interval, merge all overlaps into `add`, then emit the rest.
* **Invariants**: `out` stays sorted and non-overlapping.
* **Complexity**: Time `O(n)`, space `O(n)` output.
* **Optimizations**: Runtime: exploit pre-sorted input. Memory: reserve `intervals.size() + 1`.
* **Edge Cases To Consider**: Insert before all, after all, merge many, merge none, exact touching endpoints.
* **L7 Follow-ups**: For repeated inserts, maintain an ordered interval map.

## 9. Sort Colors

* **Pattern / Idea**: Three-way partition.
* **Company Frequency Tags**: Public signal: `Microsoft: High (6m 62.8)`, `Amazon/AWS: Medium (6m 55.1)`, `Meta: Medium (6m 49.9)`, `Oracle: Medium (6m 47.0)`, `Apple: Medium (all 47.8)`.
* **Question**: Sort an array containing only `0`, `1`, and `2` in-place using one pass.
* **Test Cases**: [Test cases](./test_cases.md#9-sort-colors).
* **C++ Code**
  ```cpp
  void sortColors(vector<int>& nums) {
      int low = 0, mid = 0, high = static_cast<int>(nums.size()) - 1;
      while (mid <= high) {
          if (nums[mid] == 0) swap(nums[low++], nums[mid++]);
          else if (nums[mid] == 1) ++mid;
          else swap(nums[mid], nums[high--]);
      }
  }
  ```
* **Code Explanation**: Maintain finished zero region, unknown middle region, and finished two region.
* **Invariants**: `[0, low)` are zeros, `[low, mid)` are ones, `(high, n)` are twos.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: one pass. Memory: no counting array needed for three symbols.
* **Edge Cases To Consider**: Empty, all same value, reverse sorted, alternating values.
* **L7 Follow-ups**: Generalize to many colors with counting sort or partition passes.

## 10. Rotate Array

* **Pattern / Idea**: Reverse three sections.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 71.9)`, `Microsoft: Medium (6m 56.5)`, `Meta: Medium (6m 39.6)`, `Oracle: Medium (all 44.5)`.
* **Question**: Rotate an array to the right by `k` positions in-place.
* **Test Cases**: [Test cases](./test_cases.md#10-rotate-array).
* **C++ Code**
  ```cpp
  void rotateRight(vector<int>& nums, int k) {
      int n = static_cast<int>(nums.size());
      if (n == 0) return;
      k %= n;
      reverse(nums.begin(), nums.end());
      reverse(nums.begin(), nums.begin() + k);
      reverse(nums.begin() + k, nums.end());
  }
  ```
* **Code Explanation**: Reversing the whole array moves the suffix to the front in reversed order; reversing each part restores local order.
* **Invariants**: After the final reverse, every element originally at `i` is at `(i + k) % n`.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: normalize `k`. Memory: in-place reversal avoids copy.
* **Edge Cases To Consider**: Empty, `k = 0`, `k > n`, one element, repeated values.
* **L7 Follow-ups**: For streaming rotation, use a ring-buffer view rather than moving data.

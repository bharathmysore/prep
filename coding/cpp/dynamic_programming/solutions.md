# Dynamic Programming Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. House Robber

* **Pattern / Idea**: DP with take/skip state.
* **Company Frequency Tags**: Public signal: `Databricks: High (6m 82.4)`, `Google: Medium (6m 52.1)`, `Microsoft: Medium (6m 50.0)`, `Apple: Medium (6m 38.9)`, `Meta: Low (6m 24.1)`, `NVIDIA: Medium (all 47.7)`.
* **Question**: Given non-negative house values, compute the maximum value you can rob without robbing adjacent houses.
* **Test Cases**: [Test cases](./test_cases.md#1-house-robber).
* **C++ Code**
  ```cpp
  long long rob(const vector<int>& nums) {
      long long skip = 0, take = 0;
      for (int x : nums) {
          long long nextTake = skip + x;
          skip = max(skip, take);
          take = nextTake;
      }
      return max(skip, take);
  }
  ```
* **Code Explanation**: `take` means rob current house; `skip` means do not rob current house.
* **Invariants**: After each house, states represent optimal totals for prefixes under their last action.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: one pass. Memory: two scalar states.
* **Edge Cases To Consider**: Empty, one house, all zeros, alternating high values.
* **L7 Follow-ups**: Circular houses require excluding first or last house.

## 2. Coin Change Minimum Coins

* **Pattern / Idea**: Unbounded amount DP.
* **Company Frequency Tags**: Public signal: `NVIDIA: Medium (all 38.4)`.
* **Question**: Given coin denominations and an amount, return the minimum number of coins needed.
* **Test Cases**: [Test cases](./test_cases.md#2-coin-change-minimum-coins).
* **C++ Code**
  ```cpp
  int coinChange(const vector<int>& coins, int amount) {
      const int INF = amount + 1;
      vector<int> dp(amount + 1, INF);
      dp[0] = 0;
      for (int x = 1; x <= amount; ++x)
          for (int c : coins)
              if (c <= x) dp[x] = min(dp[x], dp[x - c] + 1);
      return dp[amount] == INF ? -1 : dp[amount];
  }
  ```
* **Code Explanation**: Try each coin as the last coin used for amount `x`.
* **Invariants**: Before computing `dp[x]`, all smaller amounts are finalized.
* **Complexity**: Time `O(amount * coins)`, space `O(amount)`.
* **Optimizations**: Runtime: sort coins and break when `c > x`. Memory: one-dimensional table.
* **Edge Cases To Consider**: Amount zero, impossible amount, coin `1`, duplicate coins.
* **L7 Follow-ups**: For huge amounts, use graph shortest path or number-theory constraints.

## 3. 0/1 Knapsack

* **Pattern / Idea**: Capacity DP with descending update.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given weights and values, solve 0/1 knapsack.
* **Test Cases**: [Test cases](./test_cases.md#3-01-knapsack).
* **C++ Code**
  ```cpp
  int knapsack01(const vector<int>& weight, const vector<int>& value, int W) {
      vector<int> dp(W + 1, 0);
      for (int i = 0; i < static_cast<int>(weight.size()); ++i) {
          for (int cap = W; cap >= weight[i]; --cap) {
              dp[cap] = max(dp[cap], dp[cap - weight[i]] + value[i]);
          }
      }
      return dp[W];
  }
  ```
* **Code Explanation**: Descending capacity ensures each item is used at most once.
* **Invariants**: After item `i`, `dp[cap]` is optimal using processed items only.
* **Complexity**: Time `O(nW)`, space `O(W)`.
* **Optimizations**: Runtime: skip items heavier than `W`. Memory: rolling one-dimensional DP.
* **Edge Cases To Consider**: Zero capacity, no items, item too heavy, repeated weights.
* **L7 Follow-ups**: If `W` is huge, consider value-based DP or approximation.

## 4. Longest Increasing Subsequence

* **Pattern / Idea**: Tails array with binary search.
* **Company Frequency Tags**: Public signal: `Oracle: Medium (6m 56.9)`, `Google: Medium (6m 43.7)`.
* **Question**: Given a sequence, return the length of the longest increasing subsequence.
* **Test Cases**: [Test cases](./test_cases.md#4-longest-increasing-subsequence).
* **C++ Code**
  ```cpp
  int lengthOfLIS(const vector<int>& nums) {
      vector<int> tails;
      for (int x : nums) {
          auto it = lower_bound(tails.begin(), tails.end(), x);
          if (it == tails.end()) tails.push_back(x);
          else *it = x;
      }
      return tails.size();
  }
  ```
* **Code Explanation**: `tails[len]` is the smallest possible tail value of an increasing subsequence of length `len + 1`.
* **Invariants**: `tails` is sorted and each entry is minimal for its length.
* **Complexity**: Time `O(n log n)`, space `O(n)`.
* **Optimizations**: Runtime: binary search. Memory: omit predecessor tracking if only length is needed.
* **Edge Cases To Consider**: Duplicates, decreasing array, increasing array, empty input.
* **L7 Follow-ups**: Reconstructing the subsequence needs parent pointers and index tracking.

## 5. Longest Common Subsequence

* **Pattern / Idea**: Prefix DP with rolling rows.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given two strings, return the length of their longest common subsequence.
* **Test Cases**: [Test cases](./test_cases.md#5-longest-common-subsequence).
* **C++ Code**
  ```cpp
  int longestCommonSubsequence(const string& a, const string& b) {
      const string *s1 = &a, *s2 = &b;
      if (s2->size() > s1->size()) swap(s1, s2);
      vector<int> prev(s2->size() + 1), cur(s2->size() + 1);
      for (int i = 1; i <= static_cast<int>(s1->size()); ++i) {
          for (int j = 1; j <= static_cast<int>(s2->size()); ++j) {
              if ((*s1)[i - 1] == (*s2)[j - 1]) cur[j] = prev[j - 1] + 1;
              else cur[j] = max(prev[j], cur[j - 1]);
          }
          swap(prev, cur);
      }
      return prev[s2->size()];
  }
  ```
* **Code Explanation**: Each state chooses between matching current chars or skipping one side.
* **Invariants**: `prev[j]` stores LCS for previous row and prefix `j`.
* **Complexity**: Time `O(nm)`, space `O(min(n, m))`.
* **Optimizations**: Runtime: trim common prefixes/suffixes. Memory: rolling rows.
* **Edge Cases To Consider**: Empty string, no overlap, identical strings, repeated chars.
* **L7 Follow-ups**: Full diff reconstruction requires storing decisions or recomputing.

## 6. Edit Distance

* **Pattern / Idea**: Prefix transformation DP.
* **Company Frequency Tags**: Public signal: `Meta: Low (6m 15.8)`, `Microsoft: Medium (all 49.5)`, `Amazon/AWS: Medium (all 45.7)`, `Apple: Low (all 27.0)`.
* **Question**: Given two strings, return their edit distance.
* **Test Cases**: [Test cases](./test_cases.md#6-edit-distance).
* **C++ Code**
  ```cpp
  int editDistance(const string& a, const string& b) {
      vector<int> prev(b.size() + 1), cur(b.size() + 1);
      iota(prev.begin(), prev.end(), 0);
      for (int i = 1; i <= static_cast<int>(a.size()); ++i) {
          cur[0] = i;
          for (int j = 1; j <= static_cast<int>(b.size()); ++j) {
              if (a[i - 1] == b[j - 1]) cur[j] = prev[j - 1];
              else cur[j] = 1 + min({prev[j], cur[j - 1], prev[j - 1]});
          }
          swap(prev, cur);
      }
      return prev[b.size()];
  }
  ```
* **Code Explanation**: Insert, delete, or replace reduces the problem to smaller prefixes.
* **Invariants**: `prev` contains finalized distances for prefix `a[0:i-1]`.
* **Complexity**: Time `O(nm)`, space `O(m)`.
* **Optimizations**: Runtime: banded DP if max distance threshold is known. Memory: rolling rows.
* **Edge Cases To Consider**: Empty input, same strings, one-character replace, long unequal strings.
* **L7 Follow-ups**: Production fuzzy matching often uses thresholds and normalization.

## 7. Word Break

* **Pattern / Idea**: Prefix DP with dictionary lookup.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 78.3)`, `Meta: Medium (6m 48.0)`, `Microsoft: Medium (6m 47.2)`, `Snowflake: Medium (all 39.6)`.
* **Question**: Given a string and dictionary, determine whether the string can be segmented into words.
* **Test Cases**: [Test cases](./test_cases.md#7-word-break).
* **C++ Code**
  ```cpp
  bool wordBreak(const string& s, const vector<string>& dict) {
      unordered_set<string> words(dict.begin(), dict.end());
      int maxLen = 0;
      for (const string& w : dict) maxLen = max(maxLen, static_cast<int>(w.size()));
      vector<char> dp(s.size() + 1, false);
      dp[0] = true;
      for (int i = 1; i <= static_cast<int>(s.size()); ++i) {
          for (int len = 1; len <= maxLen && len <= i; ++len) {
              if (dp[i - len] && words.count(s.substr(i - len, len))) {
                  dp[i] = true;
                  break;
              }
          }
      }
      return dp[s.size()];
  }
  ```
* **Code Explanation**: A prefix is segmentable if it ends with a dictionary word and the prior prefix is segmentable.
* **Invariants**: `dp[i]` is final before being used by larger prefixes.
* **Complexity**: Time `O(n * maxWordLen * substringCost)`, space `O(n + dict)`.
* **Optimizations**: Runtime: trie avoids substring construction. Memory: store word lengths only for possible checks.
* **Edge Cases To Consider**: Empty string, repeated words, impossible suffix, very long dictionary words.
* **L7 Follow-ups**: Returning all segmentations can be exponential.

## 8. Minimum Path Sum

* **Pattern / Idea**: Grid DP with one row.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Medium (6m 42.3)`, `Microsoft: Medium (6m 36.0)`, `Google: Medium (6m 31.8)`, `Apple: Medium (all 34.5)`, `Meta: Low (all 22.3)`.
* **Question**: Given a grid of non-negative weights, find the minimum sum path from top-left to bottom-right moving only right or down.
* **Test Cases**: [Test cases](./test_cases.md#8-minimum-path-sum).
* **C++ Code**
  ```cpp
  int minPathSum(const vector<vector<int>>& grid) {
      int rows = grid.size(), cols = grid[0].size();
      vector<int> dp(cols, INT_MAX / 4);
      dp[0] = 0;
      for (int r = 0; r < rows; ++r) {
          for (int c = 0; c < cols; ++c) {
              if (c == 0) dp[c] += grid[r][c];
              else dp[c] = min(dp[c], dp[c - 1]) + grid[r][c];
          }
      }
      return dp.back();
  }
  ```
* **Code Explanation**: Each cell can only be reached from top or left.
* **Invariants**: After processing cell `(r,c)`, `dp[c]` is min cost to that cell.
* **Complexity**: Time `O(rows * cols)`, space `O(cols)`.
* **Optimizations**: Runtime: one pass. Memory: mutate grid if allowed.
* **Edge Cases To Consider**: One row, one column, large values, empty grid contract.
* **L7 Follow-ups**: With obstacles, initialize unreachable states carefully.

## 9. Decode Ways

* **Pattern / Idea**: One-dimensional string DP.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Medium (6m 35.3)`, `Microsoft: Medium (all 45.5)`, `Oracle: Medium (all 44.5)`, `Meta: Medium (all 39.8)`, `Apple: Medium (all 34.5)`.
* **Question**: Given a digit string where `1` to `26` map to letters, count valid decodings.
* **Test Cases**: [Test cases](./test_cases.md#9-decode-ways).
* **C++ Code**
  ```cpp
  int numDecodings(const string& s) {
      if (s.empty() || s[0] == '0') return 0;
      int prev2 = 1, prev1 = 1;
      for (int i = 1; i < static_cast<int>(s.size()); ++i) {
          int cur = 0;
          if (s[i] != '0') cur += prev1;
          int two = (s[i - 1] - '0') * 10 + (s[i] - '0');
          if (two >= 10 && two <= 26) cur += prev2;
          prev2 = prev1;
          prev1 = cur;
      }
      return prev1;
  }
  ```
* **Code Explanation**: A position can be decoded as a single digit or paired with the previous digit if valid.
* **Invariants**: `prev1` and `prev2` are ways for the two previous prefix lengths.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: no substring parse. Memory: scalar compression.
* **Edge Cases To Consider**: Leading zero, `10`, `100`, `226`, long input.
* **L7 Follow-ups**: Wildcard variants require modular arithmetic and more states.

## 10. Count Palindromic Substrings

* **Pattern / Idea**: Expand around centers.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 64.0)`.
* **Question**: Given a string, count all palindromic substrings.
* **Test Cases**: [Test cases](./test_cases.md#10-count-palindromic-substrings).
* **C++ Code**
  ```cpp
  int countPalindromicSubstrings(const string& s) {
      int ans = 0, n = s.size();
      auto expand = [&](int l, int r) {
          int cnt = 0;
          while (l >= 0 && r < n && s[l--] == s[r++]) ++cnt;
          return cnt;
      };
      for (int i = 0; i < n; ++i) {
          ans += expand(i, i);
          ans += expand(i, i + 1);
      }
      return ans;
  }
  ```
* **Code Explanation**: Every palindrome has a unique odd or even center.
* **Invariants**: Expansion maintains `s[l+1..r-1]` as a palindrome.
* **Complexity**: Time `O(n^2)`, space `O(1)`.
* **Optimizations**: Runtime: Manacher's algorithm gives `O(n)` but is harder to explain. Memory: center expansion uses no DP table.
* **Edge Cases To Consider**: Empty, all same chars, no repeated chars, even palindromes.
* **L7 Follow-ups**: Choose explainable `O(n^2)` unless constraints require Manacher.

# Backtracking Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Generate Subsets

* **Pattern / Idea**: Include/exclude recursion.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Medium (6m 56.1)`, `Meta: Medium (6m 49.9)`, `Microsoft: Medium (all 52.2)`, `Apple: Medium (all 37.4)`.
* **Question**: Generate all subsets of a list of distinct integers.
* **Test Cases**: [Test cases](./test_cases.md#1-generate-subsets).
* **C++ Code**
  ```cpp
  vector<vector<int>> subsets(vector<int> nums) {
      vector<vector<int>> ans;
      vector<int> path;
      function<void(int)> dfs = [&](int i) {
          if (i == static_cast<int>(nums.size())) {
              ans.push_back(path);
              return;
          }
          dfs(i + 1);
          path.push_back(nums[i]);
          dfs(i + 1);
          path.pop_back();
      };
      dfs(0);
      return ans;
  }
  ```
* **Code Explanation**: Each element has two choices: absent or present.
* **Invariants**: `path` represents decisions for indices before `i`.
* **Complexity**: Time `O(n * 2^n)`, space `O(n)` recursion plus output.
* **Optimizations**: Runtime: bitmask iteration avoids recursion overhead. Memory: reuse `path`.
* **Edge Cases To Consider**: Empty input, one value, duplicate policy.
* **L7 Follow-ups**: Output-size lower bound dominates.

## 2. Unique Permutations

* **Pattern / Idea**: Sort and skip duplicate choices.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (all 43.6)`, `Amazon/AWS: Low (all 28.1)`, `Apple: Low (all 21.9)`, `Meta: Low (all 18.4)`.
* **Question**: Generate all unique permutations of a list that may contain duplicates.
* **Test Cases**: [Test cases](./test_cases.md#2-unique-permutations).
* **C++ Code**
  ```cpp
  vector<vector<int>> permuteUnique(vector<int> nums) {
      sort(nums.begin(), nums.end());
      vector<vector<int>> ans;
      vector<int> path;
      vector<char> used(nums.size(), false);
      function<void()> dfs = [&]() {
          if (path.size() == nums.size()) {
              ans.push_back(path);
              return;
          }
          for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
              if (used[i]) continue;
              if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) continue;
              used[i] = true;
              path.push_back(nums[i]);
              dfs();
              path.pop_back();
              used[i] = false;
          }
      };
      dfs();
      return ans;
  }
  ```
* **Code Explanation**: Equal values are chosen in a canonical order at each recursion depth.
* **Invariants**: `path` contains each used index exactly once and no duplicate prefix choice is explored.
* **Complexity**: Time `O(n * uniquePermutations)`, space `O(n)` plus output.
* **Optimizations**: Runtime: frequency-map recursion can reduce duplicate iteration. Memory: reuse `path`.
* **Edge Cases To Consider**: All duplicates, no duplicates, empty input.
* **L7 Follow-ups**: Parallelizing requires partitioning prefixes without overlap.

## 3. Combination Sum

* **Pattern / Idea**: Sorted candidates and remaining-target pruning.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (6m 56.5)`, `Amazon/AWS: Medium (6m 49.0)`, `Google: Medium (6m 41.4)`, `Oracle: Medium (all 54.7)`, `Apple: Medium (all 47.8)`, `Meta: Medium (all 39.8)`.
* **Question**: Given candidates and a target, return all combinations that sum to target.
* **Test Cases**: [Test cases](./test_cases.md#3-combination-sum).
* **C++ Code**
  ```cpp
  vector<vector<int>> combinationSum(vector<int> cand, int target) {
      sort(cand.begin(), cand.end());
      vector<vector<int>> ans;
      vector<int> path;
      function<void(int, int)> dfs = [&](int start, int rem) {
          if (rem == 0) { ans.push_back(path); return; }
          for (int i = start; i < static_cast<int>(cand.size()) && cand[i] <= rem; ++i) {
              path.push_back(cand[i]);
              dfs(i, rem - cand[i]);
              path.pop_back();
          }
      };
      dfs(0, target);
      return ans;
  }
  ```
* **Code Explanation**: Passing `i` instead of `i + 1` allows reuse of the same candidate.
* **Invariants**: `path` is nondecreasing and sums to `target - rem`.
* **Complexity**: Exponential in output size, space `O(depth)` plus output.
* **Optimizations**: Runtime: sort and break when candidate exceeds remaining. Memory: path reused.
* **Edge Cases To Consider**: No solution, target zero, candidate one, duplicate candidate contract.
* **L7 Follow-ups**: If each candidate can be used once, recurse with `i + 1` and skip duplicates.

## 4. N Queens

* **Pattern / Idea**: Row-by-row placement with column and diagonal sets.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (6m 59.9)`, `Oracle: Medium (6m 56.9)`, `Amazon/AWS: Medium (6m 49.0)`, `Snowflake: Medium (all 39.6)`, `Apple: Medium (all 37.4)`, `Meta: Medium (all 33.6)`.
* **Question**: Solve the N queens problem.
* **Test Cases**: [Test cases](./test_cases.md#4-n-queens).
* **C++ Code**
  ```cpp
  vector<vector<string>> solveNQueens(int n) {
      vector<vector<string>> ans;
      vector<string> board(n, string(n, '.'));
      vector<char> col(n), diag1(2 * n), diag2(2 * n);
      function<void(int)> dfs = [&](int r) {
          if (r == n) { ans.push_back(board); return; }
          for (int c = 0; c < n; ++c) {
              int d1 = r - c + n, d2 = r + c;
              if (col[c] || diag1[d1] || diag2[d2]) continue;
              col[c] = diag1[d1] = diag2[d2] = true;
              board[r][c] = 'Q';
              dfs(r + 1);
              board[r][c] = '.';
              col[c] = diag1[d1] = diag2[d2] = false;
          }
      };
      dfs(0);
      return ans;
  }
  ```
* **Code Explanation**: Each row gets exactly one queen; constraints reject attacked columns and diagonals.
* **Invariants**: Rows before `r` contain non-attacking queens.
* **Complexity**: Roughly `O(n!)`, space `O(n)` plus output.
* **Optimizations**: Runtime: bitmasks improve constants. Memory: store column positions, render boards at output.
* **Edge Cases To Consider**: `n = 1`, `n = 2`, `n = 3`, known counts for small `n`.
* **L7 Follow-ups**: Split first-row choices to parallelize safely.

## 5. Sudoku Solver

* **Pattern / Idea**: Constraint backtracking with bitmasks.
* **Company Frequency Tags**: Public signal: `Google: Medium (6m 41.4)`, `Amazon/AWS: Medium (6m 40.2)`, `Microsoft: Medium (6m 36.0)`, `Oracle: Medium (all 39.7)`, `Meta: Medium (all 30.6)`, `Apple: Low (all 27.0)`.
* **Question**: Solve a Sudoku board.
* **Test Cases**: [Test cases](./test_cases.md#5-sudoku-solver).
* **C++ Code**
  ```cpp
  bool solveSudoku(vector<vector<char>>& b) {
      int row[9]{}, col[9]{}, box[9]{};
      vector<pair<int, int>> empty;
      for (int r = 0; r < 9; ++r) for (int c = 0; c < 9; ++c) {
          if (b[r][c] == '.') empty.push_back({r, c});
          else {
              int bit = 1 << (b[r][c] - '1'), k = (r / 3) * 3 + c / 3;
              row[r] |= bit; col[c] |= bit; box[k] |= bit;
          }
      }
      function<bool(int)> dfs = [&](int idx) {
          if (idx == static_cast<int>(empty.size())) return true;
          int best = idx, bestMask = 0, bestCnt = 10;
          for (int i = idx; i < static_cast<int>(empty.size()); ++i) {
              auto [r, c] = empty[i];
              int k = (r / 3) * 3 + c / 3;
              int mask = (~(row[r] | col[c] | box[k])) & 0x1FF;
              int cnt = __builtin_popcount(mask);
              if (cnt < bestCnt) { best = i; bestMask = mask; bestCnt = cnt; }
          }
          if (bestCnt == 0) return false;
          swap(empty[idx], empty[best]);
          auto [r, c] = empty[idx];
          int k = (r / 3) * 3 + c / 3;
          for (int mask = bestMask; mask; mask &= mask - 1) {
              int bit = mask & -mask, d = __builtin_ctz(bit);
              b[r][c] = char('1' + d);
              row[r] |= bit; col[c] |= bit; box[k] |= bit;
              if (dfs(idx + 1)) return true;
              row[r] ^= bit; col[c] ^= bit; box[k] ^= bit;
          }
          b[r][c] = '.';
          swap(empty[idx], empty[best]);
          return false;
      };
      return dfs(0);
  }
  ```
* **Code Explanation**: Always fill the most constrained empty cell to reduce branching.
* **Invariants**: Row, column, and box bitmasks match the current board.
* **Complexity**: Exponential worst case, space `O(1)` board plus recursion.
* **Optimizations**: Runtime: minimum-candidate heuristic. Memory: bitmasks instead of sets.
* **Edge Cases To Consider**: Already solved, unsolvable, one empty cell, invalid initial board policy.
* **L7 Follow-ups**: Deterministic heuristics make tests and debugging easier.

## 6. Palindrome Partitioning

* **Pattern / Idea**: Precompute palindrome table, then backtrack cuts.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Medium (6m 47.5)`, `Meta: Low (6m 24.1)`.
* **Question**: Partition a string into all lists of palindromic substrings.
* **Test Cases**: [Test cases](./test_cases.md#6-palindrome-partitioning).
* **C++ Code**
  ```cpp
  vector<vector<string>> partitionPalindromes(const string& s) {
      int n = s.size();
      vector<vector<char>> pal(n, vector<char>(n, false));
      for (int len = 1; len <= n; ++len)
          for (int l = 0; l + len <= n; ++l) {
              int r = l + len - 1;
              pal[l][r] = s[l] == s[r] && (len <= 2 || pal[l + 1][r - 1]);
          }
      vector<vector<string>> ans;
      vector<string> path;
      function<void(int)> dfs = [&](int start) {
          if (start == n) { ans.push_back(path); return; }
          for (int end = start; end < n; ++end) if (pal[start][end]) {
              path.push_back(s.substr(start, end - start + 1));
              dfs(end + 1);
              path.pop_back();
          }
      };
      dfs(0);
      return ans;
  }
  ```
* **Code Explanation**: The DP table makes each palindrome check `O(1)` during DFS.
* **Invariants**: `path` partitions exactly `s[0:start]` into palindromes.
* **Complexity**: Time `O(n^2 + outputSize * n)`, space `O(n^2)` plus output.
* **Optimizations**: Runtime: precompute palindromes. Memory: expand on demand if memory is tight.
* **Edge Cases To Consider**: Empty, all same chars, no multi-char palindromes.
* **L7 Follow-ups**: Returning only min cuts is a different DP problem.

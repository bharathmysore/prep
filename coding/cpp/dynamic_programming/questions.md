# Dynamic Programming Coding Questions

Solve each question in C++ by naming the state, recurrence, transition order, invariant, and memory compression opportunity.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Given non-negative house values, compute the maximum value you can rob without robbing adjacent houses.
   - Expected pattern: take/skip DP with two scalar states.
   - Pattern tags: `take-skip-dp`, `constant-space`.
   - Solution: [House Robber](./solutions.md#1-house-robber).
   - Complexity target: time `O(n)`, space `O(1)`.

2. Given coin denominations and an amount, return the minimum number of coins needed.
   - Expected pattern: one-dimensional unbounded DP.
   - Pattern tags: `unbounded-knapsack`, `dp`.
   - Solution: [Coin Change Minimum Coins](./solutions.md#2-coin-change-minimum-coins).
   - Complexity target: time `O(amount * coins)`, space `O(amount)`.

3. Given weights and values, solve 0/1 knapsack.
   - Expected pattern: capacity DP with descending updates.
   - Pattern tags: `zero-one-knapsack`, `dp`.
   - Solution: [0/1 Knapsack](./solutions.md#3-01-knapsack).
   - Complexity target: time `O(nW)`, space `O(W)`.

4. Given a sequence, return the length of the longest increasing subsequence.
   - Expected pattern: tails array with binary search.
   - Pattern tags: `lis`, `binary-search-dp`.
   - Solution: [Longest Increasing Subsequence](./solutions.md#4-longest-increasing-subsequence).
   - Complexity target: time `O(n log n)`, space `O(n)`.

5. Given two strings, return the length of their longest common subsequence.
   - Expected pattern: two-dimensional DP with rolling rows.
   - Pattern tags: `two-dimensional-dp`, `rolling-array`.
   - Solution: [Longest Common Subsequence](./solutions.md#5-longest-common-subsequence).
   - Complexity target: time `O(nm)`, space `O(min(n, m))`.

6. Given two strings, return their edit distance.
   - Expected pattern: prefix DP.
   - Pattern tags: `two-dimensional-dp`, `edit-distance`.
   - Solution: [Edit Distance](./solutions.md#6-edit-distance).
   - Complexity target: time `O(nm)`, space `O(min(n, m))`.

<a id="7-word-break"></a>
7. Given a string and dictionary, determine whether the string can be segmented into words.
   - Expected pattern: prefix DP plus dictionary lookup or trie.
   - Pattern tags: `prefix-dp`, `dictionary`.
   - Solution: [Word Break](./solutions.md#7-word-break).
   - Complexity target: time `O(n * maxWordLen)`, space `O(n)`.

8. Given a grid of non-negative weights, find the minimum sum path from top-left to bottom-right moving only right or down.
   - Expected pattern: grid DP with one rolling row.
   - Pattern tags: `grid-dp`, `rolling-array`.
   - Solution: [Minimum Path Sum](./solutions.md#8-minimum-path-sum).
   - Complexity target: time `O(rows * cols)`, space `O(cols)`.

9. Given a digit string where `1` to `26` map to letters, count valid decodings.
   - Expected pattern: one-dimensional string DP.
   - Pattern tags: `prefix-dp`, `string-dp`.
   - Solution: [Decode Ways](./solutions.md#9-decode-ways).
   - Complexity target: time `O(n)`, space `O(1)`.

10. Given a string, count all palindromic substrings.
   - Expected pattern: expand around each possible center.
   - Pattern tags: `palindrome`, `center-expansion`.
   - Solution: [Count Palindromic Substrings](./solutions.md#10-count-palindromic-substrings).
   - Complexity target: time `O(n^2)`, space `O(1)`.

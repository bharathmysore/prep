# Dynamic Programming Coding Patterns

Dynamic programming is state design plus transition order. In L7 interviews, first define the state, then the recurrence, then the invariant that each state is final when used.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Climbing stairs | Count ways with local recurrence | `dp[i]` depends only on smaller finalized states | Time `O(n)`, space `O(1)` | Runtime: iterative recurrence. Memory: keep last two values. |
| House robber | Choose non-adjacent items | Best through `i` is max of taking or skipping `i` | Time `O(n)`, space `O(1)` | Runtime: one pass. Memory: two scalar states. |
| Coin change min coins | Minimize count over unbounded choices | `dp[x]` is best known coins for amount `x` | Time `O(amount * coins)`, space `O(amount)` | Runtime: prune coins larger than amount. Memory: one-dimensional table. |
| 0/1 knapsack | Choose each item at most once | Descending capacity prevents reusing same item | Time `O(nW)`, space `O(W)` | Runtime: skip impossible capacities. Memory: compress rows. |
| Longest increasing subsequence | Need longest ordered subsequence | `tails[len]` is minimum possible tail for length `len` | Time `O(n log n)`, space `O(n)` | Runtime: binary search tails. Memory: omit parent pointers unless reconstruction required. |
| Longest common subsequence | Two sequences with align/skip choices | `dp[i][j]` is optimal for prefixes | Time `O(nm)`, space `O(min(n, m))` | Runtime: iterate smaller dimension inside. Memory: rolling rows. |
| Edit distance | Transform one string to another | `dp[i][j]` is min edits for prefixes | Time `O(nm)`, space `O(min(n, m))` | Runtime: trim common prefixes/suffixes. Memory: rolling rows. |
| Word break | Segment string using dictionary | `dp[i]` true means prefix `s[0:i]` can be segmented | Time `O(n * maxWordLen)`, space `O(n)` | Runtime: bound word lengths. Memory: trie can reduce substring creation. |
| Interval DP burst balloons | Choices split interval into independent subproblems | Shorter intervals are finalized before longer intervals | Time `O(n^3)`, space `O(n^2)` | Runtime: skip zero or impossible pivots. Memory: triangular storage if needed. |
| DP on DAG | Dependencies form acyclic graph | Topological order finalizes predecessors before successors | Time `O(V + E)`, space `O(V)` | Runtime: topological traversal. Memory: store only needed state per node. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `take-skip-dp`, `constant-space` | take/skip DP with two scalar states | [questions](./questions.md): Q1 |
| `unbounded-knapsack`, `dp` | one-dimensional unbounded DP | [questions](./questions.md): Q2 |
| `zero-one-knapsack`, `dp` | capacity DP with descending updates | [questions](./questions.md): Q3 |
| `lis`, `binary-search-dp` | tails array with binary search | [questions](./questions.md): Q4 |
| `two-dimensional-dp`, `rolling-array` | two-dimensional DP with rolling rows | [questions](./questions.md): Q5 |
| `two-dimensional-dp`, `edit-distance` | prefix DP | [questions](./questions.md): Q6 |
| `prefix-dp`, `dictionary` | prefix DP plus dictionary lookup or trie | [questions](./questions.md): Q7 |
| `grid-dp`, `rolling-array` | grid DP with one rolling row | [questions](./questions.md): Q8 |
| `prefix-dp`, `string-dp` | one-dimensional string DP | [questions](./questions.md): Q9 |
| `palindrome`, `center-expansion` | expand around each possible center | [questions](./questions.md): Q10 |

## L7 Follow-Ups

- How do you prove the state captures all history needed?
- What is the transition order, and why is it safe?
- Can memory be compressed without corrupting dependencies?
- When does DP become too large and need approximation, pruning, or graph reformulation?

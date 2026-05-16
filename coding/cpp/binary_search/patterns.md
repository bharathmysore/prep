# Binary Search Coding Patterns

Binary search is about proving monotonicity. For L7 interviews, state the predicate and the search invariant before writing code.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Search in sorted array | Direct sorted lookup | Target, if present, remains inside `[lo, hi]` | Time `O(log n)`, space `O(1)` | Runtime: overflow-safe midpoint. Memory: iterative loop. |
| Lower bound / first true | Need first index satisfying predicate | All indices before `lo` are false, at or after `hi` may be true | Time `O(log n)`, space `O(1)` | Runtime: half-open interval reduces edge bugs. Memory: no recursion. |
| Search rotated sorted array | Sorted array with pivot | At least one half is sorted each step | Time `O(log n)`, space `O(1)` | Runtime: choose sorted half. Memory: no pivot prepass needed. |
| Find minimum in rotated array | Pivot search | Minimum lies in unsorted half or at boundary | Time `O(log n)`, space `O(1)` | Runtime: compare mid with right. Memory: constant. |
| Koko eating bananas | Min feasible speed | Predicate "can finish by H" is monotonic | Time `O(n log maxPile)`, space `O(1)` | Runtime: early stop when hours exceed limit. Memory: no extra arrays. |
| Capacity to ship packages | Min feasible capacity | Predicate "can ship in D days" is monotonic | Time `O(n log sum)`, space `O(1)` | Runtime: lower bound max package. Memory: scan only. |
| Median of two sorted arrays | Need partition by rank | Left partition size fixed and all left values <= right values | Time `O(log min(n, m))`, space `O(1)` | Runtime: binary search smaller array. Memory: no merge. |
| Kth smallest in sorted matrix | Matrix sorted by rows and columns | Count `<= mid` is monotonic in value | Time `O(n log valueRange)`, space `O(1)` | Runtime: staircase count. Memory: avoid heap for large k. |
| Split array largest sum | Minimize maximum partition sum | Feasible partition count decreases as limit increases | Time `O(n log sum)`, space `O(1)` | Runtime: greedy feasibility. Memory: no DP table needed. |
| Peak element | Local comparison guarantees a peak direction | A peak exists in the half that rises from mid | Time `O(log n)`, space `O(1)` | Runtime: compare `mid` and `mid + 1`. Memory: constant. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `binary-search-partition` | binary search partition | [questions](./questions.md): Q1 |
| `rotated-binary-search` | binary search with one sorted half | [questions](./questions.md): Q2 |
| `answer-space-binary-search`, `monotonic-predicate` | binary search on answer | [questions](./questions.md): Q3 |
| `answer-space-binary-search`, `monotonic-predicate` | binary search on feasible capacity | [questions](./questions.md): Q4 |
| `value-space-binary-search`, `monotonic-count` | value-space binary search plus monotonic count | [questions](./questions.md): Q5 |
| `answer-space-binary-search`, `greedy-feasibility` | answer-space binary search with greedy feasibility | [questions](./questions.md): Q6 |

## L7 Follow-Ups

- What exact condition makes the predicate monotonic?
- Which boundary convention prevents infinite loops?
- How do you avoid overflow in midpoints and feasibility sums?
- When does answer-space search hide a greedy proof?

# Binary Search Coding Questions

Solve each question in C++ by stating the monotonic predicate and loop invariant before coding.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

<a id="1-median-of-two-sorted-arrays"></a>
1. Given two sorted arrays, find their median without merging.
   - Expected pattern: binary search partition.
   - Pattern tags: `binary-search-partition`.
   - Solution: [Median Of Two Sorted Arrays](./solutions.md#1-median-of-two-sorted-arrays).
   - Complexity target: time `O(log min(n, m))`, space `O(1)`.

2. Search for a target in a rotated sorted array.
   - Expected pattern: binary search with one sorted half.
   - Pattern tags: `rotated-binary-search`.
   - Solution: [Search Rotated Sorted Array](./solutions.md#2-search-rotated-sorted-array).
   - Complexity target: time `O(log n)`, space `O(1)`.

3. Given piles of bananas and hours `h`, find the minimum eating speed.
   - Expected pattern: binary search on answer.
   - Pattern tags: `answer-space-binary-search`, `monotonic-predicate`.
   - Solution: [Koko Eating Bananas](./solutions.md#3-koko-eating-bananas).
   - Complexity target: time `O(n log maxPile)`, space `O(1)`.

4. Given package weights and `D` days, find the minimum ship capacity.
   - Expected pattern: binary search on feasible capacity.
   - Pattern tags: `answer-space-binary-search`, `monotonic-predicate`.
   - Solution: [Ship Packages Within D Days](./solutions.md#4-ship-packages-within-d-days).
   - Complexity target: time `O(n log sumWeights)`, space `O(1)`.

5. Given a matrix sorted by rows and columns, find the kth smallest value.
   - Expected pattern: value-space binary search plus monotonic count.
   - Pattern tags: `value-space-binary-search`, `monotonic-count`.
   - Solution: [Kth Smallest In Sorted Matrix](./solutions.md#5-kth-smallest-in-sorted-matrix).
   - Complexity target: time `O(n log valueRange)` for square matrix, space `O(1)`.

6. Split an array into `m` non-empty subarrays minimizing the largest subarray sum.
   - Expected pattern: answer-space binary search with greedy feasibility.
   - Pattern tags: `answer-space-binary-search`, `greedy-feasibility`.
   - Solution: [Split Array Largest Sum](./solutions.md#6-split-array-largest-sum).
   - Complexity target: time `O(n log sum)`, space `O(1)`.

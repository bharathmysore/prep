# Arrays And Strings Coding Questions

Solve each question in C++ with explanation, invariants, time and space complexity, runtime optimizations, memory optimizations, tests, and L7 follow-ups.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Given a sorted integer array and a target, return the two indices whose values sum to the target, or `{-1, -1}` if no pair exists.
   - Expected pattern: two pointers over sorted input.
   - Pattern tags: `two-pointers`, `sorted-array`.
   - Solution: [Two Sum In A Sorted Array](./solutions.md#1-two-sum-in-a-sorted-array).
   - Complexity target: time `O(n)`, space `O(1)`.

2. Given a string, return the length of the longest substring without repeating characters.
   - Expected pattern: sliding window with last-seen positions.
   - Pattern tags: `sliding-window`, `last-seen-index`.
   - Solution: [Longest Substring Without Repeating Characters](./solutions.md#2-longest-substring-without-repeating-characters).
   - Complexity target: time `O(n)`, space `O(k)` for alphabet size.

3. Given a string `s` and a string `t`, return the minimum window in `s` that contains all characters from `t`.
   - Expected pattern: sliding window with required counts.
   - Pattern tags: `sliding-window`, `frequency-counts`.
   - Solution: [Minimum Window Substring](./solutions.md#3-minimum-window-substring).
   - Complexity target: time `O(n)`, space `O(k)`.

<a id="4-count-subarrays-with-sum-k"></a>
4. Given an integer array and a target `k`, return the number of subarrays with sum exactly `k`.
   - Expected pattern: prefix sums plus frequency map.
   - Pattern tags: `prefix-sum`, `hash-map`.
   - Solution: [Count Subarrays With Sum K](./solutions.md#4-count-subarrays-with-sum-k).
   - Complexity target: time `O(n)` average, space `O(n)`.

<a id="5-merge-intervals"></a>
5. Given a list of intervals, merge overlaps and return a canonical non-overlapping interval list.
   - Expected pattern: sorting plus linear merge.
   - Pattern tags: `intervals`, `sorting`.
   - Solution: [Merge Intervals](./solutions.md#5-merge-intervals).
   - Complexity target: time `O(n log n)`, space `O(n)` output.

6. Given an array, return the product of all elements except self without using division.
   - Expected pattern: prefix and suffix products.
   - Pattern tags: `prefix-suffix`, `array-product`.
   - Solution: [Product Of Array Except Self](./solutions.md#6-product-of-array-except-self).
   - Complexity target: time `O(n)`, auxiliary space `O(1)` excluding output.

7. Given many range increment operations over an array, return the final array.
   - Expected pattern: difference array.
   - Pattern tags: `difference-array`, `range-updates`.
   - Solution: [Difference Array Range Updates](./solutions.md#7-difference-array-range-updates).
   - Complexity target: time `O(n + q)`, space `O(n)`.

8. Given sorted non-overlapping intervals and a new interval, insert it and merge any overlaps.
   - Expected pattern: linear scan exploiting sorted non-overlap.
   - Pattern tags: `intervals`, `linear-merge`.
   - Solution: [Insert Interval](./solutions.md#8-insert-interval).
   - Complexity target: time `O(n)`, space `O(n)` output.

9. Sort an array containing only `0`, `1`, and `2` in-place using one pass.
   - Expected pattern: Dutch national flag three-way partition.
   - Pattern tags: `three-way-partition`, `in-place`.
   - Solution: [Sort Colors](./solutions.md#9-sort-colors).
   - Complexity target: time `O(n)`, space `O(1)`.

10. Rotate an array to the right by `k` positions in-place.
   - Expected pattern: reverse sections after reducing `k` modulo `n`.
   - Pattern tags: `array-rotation`, `in-place`.
   - Solution: [Rotate Array](./solutions.md#10-rotate-array).
   - Complexity target: time `O(n)`, space `O(1)`.

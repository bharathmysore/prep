# Arrays Strings Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Two Sum In A Sorted Array

* **Question**: Given a sorted integer array and a target, return the two indices whose values sum to the target, or `{-1, -1}` if no pair exists.
* **Solution**: [Two Sum In A Sorted Array](./solutions.md#1-two-sum-in-a-sorted-array).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Basic pair | `a=[2,7,11,15]`, `target=9` | Return `[0,1]`. |
| Negative values | `a=[-3,-1,0,2,4]`, `target=1` | Return `[0,4]`. |
| No match | `a=[1,2,3]`, `target=7` | Return `[-1,-1]`. |

## 2. Longest Substring Without Repeating Characters

* **Question**: Given a string, return the length of the longest substring without repeating characters.
* **Solution**: [Longest Substring Without Repeating Characters](./solutions.md#2-longest-substring-without-repeating-characters).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Repeated window | `s="abcabcbb"` | Return `3` for `abc`. |
| All same | `s="bbbbb"` | Return `1`. |
| Empty | `s=""` | Return `0`. |

## 3. Minimum Window Substring

* **Question**: Given a string `s` and a string `t`, return the minimum window in `s` that contains all characters from `t`.
* **Solution**: [Minimum Window Substring](./solutions.md#3-minimum-window-substring).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `s="ADOBECODEBANC"`, `t="ABC"` | Return `"BANC"`. |
| Duplicate needs | `s="AAABBC"`, `t="AABC"` | Return `"AABBC"`. |
| No window | `s="abc"`, `t="z"` | Return empty string. |

## 4. Count Subarrays With Sum K

* **Question**: Given an integer array and a target `k`, return the number of subarrays with sum exactly `k`.
* **Solution**: [Count Subarrays With Sum K](./solutions.md#4-count-subarrays-with-sum-k).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Mixed signs | `a=[1,1,1]`, `k=2` | Return `2`. |
| Zeros | `a=[0,0,0]`, `k=0` | Return `6`. |
| Negative values | `a=[1,-1,1]`, `k=1` | Return `3`. |

## 5. Merge Intervals

* **Question**: Given a list of intervals, merge overlaps and return a canonical non-overlapping interval list.
* **Solution**: [Merge Intervals](./solutions.md#5-merge-intervals).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Overlap chain | `[[1,3],[2,6],[8,10],[15,18]]` | Return `[[1,6],[8,10],[15,18]]`. |
| Touching intervals | `[[1,4],[4,5]]` | Return `[[1,5]]` if closed intervals merge. |
| Already disjoint | `[[1,2],[3,4]]` | Return unchanged. |

## 6. Product Of Array Except Self

* **Question**: Given an array, return the product of all elements except self without using division.
* **Solution**: [Product Of Array Except Self](./solutions.md#6-product-of-array-except-self).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| No zeros | `[1,2,3,4]` | Return `[24,12,8,6]`. |
| One zero | `[1,2,0,4]` | Return `[0,0,8,0]`. |
| Two zeros | `[0,2,0]` | Return `[0,0,0]`. |

## 7. Difference Array Range Updates

* **Question**: Given many range increment operations over an array, return the final array.
* **Solution**: [Difference Array Range Updates](./solutions.md#7-difference-array-range-updates).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Two updates | `n=5`, updates `[(1,3,+2),(2,4,+1)]` | Return `[0,2,3,3,1]` with zero-based inclusive ranges. |
| Full range | `n=3`, update `[(0,2,+5)]` | Return `[5,5,5]`. |
| No updates | `n=4`, updates `[]` | Return `[0,0,0,0]`. |

## 8. Insert Interval

* **Question**: Given sorted non-overlapping intervals and a new interval, insert it and merge any overlaps.
* **Solution**: [Insert Interval](./solutions.md#8-insert-interval).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Middle merge | intervals `[[1,3],[6,9]]`, new `[2,5]` | Return `[[1,5],[6,9]]`. |
| Merge many | `[[1,2],[3,5],[6,7],[8,10],[12,16]]`, new `[4,8]` | Return `[[1,2],[3,10],[12,16]]`. |
| Append | `[[1,2]]`, new `[3,4]` | Return `[[1,2],[3,4]]`. |

## 9. Sort Colors

* **Question**: Sort an array containing only `0`, `1`, and `2` in-place using one pass.
* **Solution**: [Sort Colors](./solutions.md#9-sort-colors).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Mixed | `[2,0,2,1,1,0]` | Array becomes `[0,0,1,1,2,2]`. |
| Already sorted | `[0,0,1,2]` | Unchanged. |
| Single color | `[1,1,1]` | Unchanged. |

## 10. Rotate Array

* **Question**: Rotate an array to the right by `k` positions in-place.
* **Solution**: [Rotate Array](./solutions.md#10-rotate-array).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Normal | `[1,2,3,4,5,6,7]`, `k=3` | Array becomes `[5,6,7,1,2,3,4]`. |
| K greater than n | `[1,2]`, `k=3` | Array becomes `[2,1]`. |
| Empty or zero k | `[]` or `k=0` | No change. |

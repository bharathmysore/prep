# Binary Search Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Median Of Two Sorted Arrays

* **Question**: Given two sorted arrays, find their median without merging.
* **Solution**: [Median Of Two Sorted Arrays](./solutions.md#1-median-of-two-sorted-arrays).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[1,3]` and `[2]` | Return `2.0`. |
| Even total | `[1,2]` and `[3,4]` | Return `2.5`. |
| One empty | `[]` and `[1]` | Return `1.0`. |

## 2. Search Rotated Sorted Array

* **Question**: Search for a target in a rotated sorted array.
* **Solution**: [Search Rotated Sorted Array](./solutions.md#2-search-rotated-sorted-array).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Found | nums `[4,5,6,7,0,1,2]`, target 0 | Return index `4`. |
| Missing | same nums, target 3 | Return `-1`. |
| Not rotated | `[1,2,3]`, target 2 | Return index `1`. |

## 3. Koko Eating Bananas

* **Question**: Given piles of bananas and hours `h`, find the minimum eating speed.
* **Solution**: [Koko Eating Bananas](./solutions.md#3-koko-eating-bananas).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | piles `[3,6,7,11]`, h=8 | Return `4`. |
| Tight hours | piles `[30,11,23,4,20]`, h=5 | Return `30`. |
| Single pile | `[9]`, h=3 | Return `3`. |

## 4. Ship Packages Within D Days

* **Question**: Given package weights and `D` days, find the minimum ship capacity.
* **Solution**: [Ship Packages Within D Days](./solutions.md#4-ship-packages-within-d-days).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | weights `[1,2,3,4,5,6,7,8,9,10]`, days 5 | Return `15`. |
| One day | days 1 | Return sum of weights. |
| Many days | days >= n | Return max weight. |

## 5. Kth Smallest In Sorted Matrix

* **Question**: Given a matrix sorted by rows and columns, find the kth smallest value.
* **Solution**: [Kth Smallest In Sorted Matrix](./solutions.md#5-kth-smallest-in-sorted-matrix).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | matrix `[[1,5,9],[10,11,13],[12,13,15]]`, k=8 | Return `13`. |
| Duplicates | matrix with repeated values | Counts duplicates as separate elements. |
| k=1 | any matrix | Return smallest element. |

## 6. Split Array Largest Sum

* **Question**: Split an array into `m` non-empty subarrays minimizing the largest subarray sum.
* **Solution**: [Split Array Largest Sum](./solutions.md#6-split-array-largest-sum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | nums `[7,2,5,10,8]`, m=2 | Return `18`. |
| m=1 | single partition | Return total sum. |
| m=n | each element alone | Return max element. |

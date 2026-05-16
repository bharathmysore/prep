# Arrays And Strings Coding Patterns

Use these as pattern drills for L7 C++ interviews. For each problem, practice a clean C++ solution, explain the invariant, give time and space complexity, and split runtime vs memory optimizations.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Two Sum in a sorted array | Sorted input, pair target | Left side is too small, right side is too large | Time `O(n)`, space `O(1)` | Runtime: one pass with two pointers. Memory: avoid hash table when sorted. |
| Longest substring without repeating characters | Longest contiguous window with uniqueness | Window `[l, r]` contains no duplicate characters | Time `O(n)`, space `O(k)` alphabet | Runtime: jump `l` using last-seen index. Memory: fixed array for ASCII where valid. |
| Minimum window substring | Smallest window covering required counts | Window is valid when all required counts are satisfied | Time `O(n)`, space `O(k)` | Runtime: contract only after valid. Memory: fixed count arrays when alphabet is bounded. |
| Subarray sum equals K | Count ranges with target sum | Previous prefix sums encode all starts for current end | Time `O(n)` average, space `O(n)` | Runtime: one pass hash counts. Memory: use integer counts and reserve map capacity. |
| Merge intervals | Overlapping sorted ranges | Current merged interval covers all overlapping intervals seen | Time `O(n log n)`, space `O(n)` output | Runtime: sort by start once. Memory: merge into output vector in place. |
| Insert interval | Sorted non-overlapping intervals plus one new interval | Emit intervals before, merge overlaps, emit intervals after | Time `O(n)`, space `O(n)` output | Runtime: exploit existing sorted order. Memory: reserve result size. |
| Product of array except self | Need all products except current index | Prefix captures left product, suffix captures right product | Time `O(n)`, space `O(1)` auxiliary | Runtime: two linear passes. Memory: store result only, no prefix/suffix arrays. |
| Rotate array or matrix | Cyclic movement without extra copy | Each position receives exactly one predecessor value | Time `O(n)` array or `O(n^2)` matrix, space `O(1)` | Runtime: reverse sections or layer rotation. Memory: swap in place. |
| Difference array range updates | Many range increments, final array requested | Difference prefix reconstructs true value at each index | Time `O(n + q)`, space `O(n)` | Runtime: `O(1)` per update. Memory: compress coordinates for sparse ranges. |
| Sort colors / Dutch national flag | Small finite set of values | Regions before `low`, between, and after `high` are classified | Time `O(n)`, space `O(1)` | Runtime: single pass partition. Memory: no counting array needed for three values. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `two-pointers`, `sorted-array` | two pointers over sorted input | [questions](./questions.md): Q1 |
| `sliding-window`, `last-seen-index` | sliding window with last-seen positions | [questions](./questions.md): Q2 |
| `sliding-window`, `frequency-counts` | sliding window with required counts | [questions](./questions.md): Q3 |
| `prefix-sum`, `hash-map` | prefix sums plus frequency map | [questions](./questions.md): Q4 |
| `intervals`, `sorting` | sorting plus linear merge | [questions](./questions.md): Q5 |
| `prefix-suffix`, `array-product` | prefix and suffix products | [questions](./questions.md): Q6 |
| `difference-array`, `range-updates` | difference array | [questions](./questions.md): Q7 |
| `intervals`, `linear-merge` | linear interval insertion and merge | [questions](./questions.md): Q8 |
| `three-way-partition`, `in-place` | Dutch national flag partition | [questions](./questions.md): Q9 |
| `array-rotation`, `in-place` | in-place array rotation by reversal | [questions](./questions.md): Q10 |

## L7 Follow-Ups

- How do constraints change if the input is streaming rather than in memory?
- What changes if characters are Unicode instead of ASCII?
- How do you avoid integer overflow in prefix sums or product problems?
- Which patterns parallelize cleanly, and which are sequential because the invariant depends on order?

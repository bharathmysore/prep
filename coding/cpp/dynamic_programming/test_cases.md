# Dynamic Programming Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. House Robber

* **Question**: Given non-negative house values, compute the maximum value you can rob without robbing adjacent houses.
* **Solution**: [House Robber](./solutions.md#1-house-robber).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[1,2,3,1]` | Return `4`. |
| Choose non-adjacent peaks | `[2,7,9,3,1]` | Return `12`. |
| Empty | `[]` | Return `0`. |

## 2. Coin Change Minimum Coins

* **Question**: Given coin denominations and an amount, return the minimum number of coins needed.
* **Solution**: [Coin Change Minimum Coins](./solutions.md#2-coin-change-minimum-coins).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | coins `[1,2,5]`, amount `11` | Return `3`. |
| Impossible | coins `[2]`, amount `3` | Return `-1`. |
| Zero amount | amount `0` | Return `0`. |

## 3. 0/1 Knapsack

* **Question**: Given weights and values, solve 0/1 knapsack.
* **Solution**: [0/1 Knapsack](./solutions.md#3-01-knapsack).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Basic | weights `[1,3,4]`, values `[15,20,30]`, capacity `4` | Return `35`. |
| No capacity | capacity `0` | Return `0`. |
| Item too heavy | all weights greater than capacity | Return `0`. |

## 4. Longest Increasing Subsequence

* **Question**: Given a sequence, return the length of the longest increasing subsequence.
* **Solution**: [Longest Increasing Subsequence](./solutions.md#4-longest-increasing-subsequence).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[10,9,2,5,3,7,101,18]` | Return `4`. |
| Strictly decreasing | `[5,4,3]` | Return `1`. |
| Duplicates | `[2,2,2]` | Return `1`. |

## 5. Longest Common Subsequence

* **Question**: Given two strings, return the length of their longest common subsequence.
* **Solution**: [Longest Common Subsequence](./solutions.md#5-longest-common-subsequence).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `text1="abcde"`, `text2="ace"` | Return `3`. |
| No common chars | `"abc"`, `"def"` | Return `0`. |
| One empty | `"abc"`, `""` | Return `0`. |

## 6. Edit Distance

* **Question**: Given two strings, return their edit distance.
* **Solution**: [Edit Distance](./solutions.md#6-edit-distance).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `horse` -> `ros` | Return `3`. |
| Same string | `abc` -> `abc` | Return `0`. |
| Insert all | `""` -> `abc` | Return `3`. |

## 7. Word Break

* **Question**: Given a string and dictionary, determine whether the string can be segmented into words.
* **Solution**: [Word Break](./solutions.md#7-word-break).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| True | `s="leetcode"`, dict `{leet,code}` | Return `true`. |
| False | `s="catsandog"`, dict `{cats,dog,sand,and,cat}` | Return `false`. |
| Overlapping words | `s="aaaaaaa"`, dict `{aaaa,aaa}` | Return `true`. |

## 8. Minimum Path Sum

* **Question**: Given a grid of non-negative weights, find the minimum sum path from top-left to bottom-right moving only right or down.
* **Solution**: [Minimum Path Sum](./solutions.md#8-minimum-path-sum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | grid `[[1,3,1],[1,5,1],[4,2,1]]` | Return `7`. |
| Single cell | grid `[[5]]` | Return `5`. |
| One row | grid `[[1,2,3]]` | Return `6`. |

## 9. Decode Ways

* **Question**: Given a digit string where `1` to `26` map to letters, count valid decodings.
* **Solution**: [Decode Ways](./solutions.md#9-decode-ways).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `"226"` | Return `3`. |
| Leading zero | `"06"` | Return `0`. |
| Zero pair | `"10"` | Return `1`. |

## 10. Count Palindromic Substrings

* **Question**: Given a string, count all palindromic substrings.
* **Solution**: [Count Palindromic Substrings](./solutions.md#10-count-palindromic-substrings).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `"aaa"` | Return `6`. |
| No multi-char | `"abc"` | Return `3`. |
| Empty | `""` | Return `0`. |

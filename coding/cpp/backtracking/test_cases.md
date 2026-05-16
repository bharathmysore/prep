# Backtracking Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Generate Subsets

* **Question**: Generate all subsets of a list of distinct integers.
* **Solution**: [Generate Subsets](./solutions.md#1-generate-subsets).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Three items | `[1,2,3]` | Return `8` subsets including empty and full set. |
| Empty | `[]` | Return `[[]]`. |
| Single | `[7]` | Return `[[],[7]]`. |

## 2. Unique Permutations

* **Question**: Generate all unique permutations of a list that may contain duplicates.
* **Solution**: [Unique Permutations](./solutions.md#2-unique-permutations).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Duplicates | `[1,1,2]` | Return `[[1,1,2],[1,2,1],[2,1,1]]` in any order. |
| All same | `[1,1]` | Return one permutation. |
| No duplicates | `[1,2,3]` | Return `6` permutations. |

## 3. Combination Sum

* **Question**: Given candidates and a target, return all combinations that sum to target.
* **Solution**: [Combination Sum](./solutions.md#3-combination-sum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | candidates `[2,3,6,7]`, target `7` | Return `[[2,2,3],[7]]`. |
| No solution | candidates `[5]`, target `3` | Return empty list. |
| Reusable candidate | candidates `[2]`, target `4` | Return `[[2,2]]`. |

## 4. N Queens

* **Question**: Solve the N queens problem.
* **Solution**: [N Queens](./solutions.md#4-n-queens).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| n=4 | `n=4` | Return `2` valid boards. |
| n=1 | `n=1` | Return one board with one queen. |
| n=2 | `n=2` | Return no boards. |

## 5. Sudoku Solver

* **Question**: Solve a Sudoku board.
* **Solution**: [Sudoku Solver](./solutions.md#5-sudoku-solver).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Valid puzzle | Standard partially-filled valid Sudoku board | Board is filled so every row, column, and box has digits 1-9. |
| Already solved | A completed valid board | Board remains valid and unchanged. |
| Contradiction handling | Board with duplicate fixed digit in a row | Solver should reject or report no solution if validation is included. |

## 6. Palindrome Partitioning

* **Question**: Partition a string into all lists of palindromic substrings.
* **Solution**: [Palindrome Partitioning](./solutions.md#6-palindrome-partitioning).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `"aab"` | Return `["a","a","b"]` and `["aa","b"]`. |
| Single char | `"x"` | Return `[["x"]]`. |
| All same | `"aaa"` | Return all palindromic cuts. |

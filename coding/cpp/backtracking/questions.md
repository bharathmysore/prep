# Backtracking Coding Questions

Solve each question in C++ and explicitly discuss pruning, duplicate handling, recursion depth, and output-size complexity.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Generate all subsets of a list of distinct integers.
   - Expected pattern: include/exclude recursion.
   - Pattern tags: `subsets`, `include-exclude`.
   - Solution: [Generate Subsets](./solutions.md#1-generate-subsets).
   - Complexity target: time `O(n * 2^n)`, space `O(n)` recursion plus output.

2. Generate all unique permutations of a list that may contain duplicates.
   - Expected pattern: sorting plus duplicate skip or frequency map.
   - Pattern tags: `permutations`, `duplicate-skipping`.
   - Solution: [Unique Permutations](./solutions.md#2-unique-permutations).
   - Complexity target: time `O(n * uniquePermutations)`, space `O(n)` recursion plus output.

3. Given candidates and a target, return all combinations that sum to target.
   - Expected pattern: sorted candidates plus remaining-target pruning.
   - Pattern tags: `combination-sum`, `pruning`.
   - Solution: [Combination Sum](./solutions.md#3-combination-sum).
   - Complexity target: exponential, space `O(depth)` plus output.

4. Solve the N queens problem.
   - Expected pattern: row-by-row placement with column and diagonal constraints.
   - Pattern tags: `constraint-backtracking`, `n-queens`.
   - Solution: [N Queens](./solutions.md#4-n-queens).
   - Complexity target: roughly `O(n!)`, space `O(n)` plus output.

5. Solve a Sudoku board.
   - Expected pattern: constraint propagation plus backtracking.
   - Pattern tags: `constraint-backtracking`, `bitmask`.
   - Solution: [Sudoku Solver](./solutions.md#5-sudoku-solver).
   - Complexity target: exponential worst case, space `O(1)` board plus recursion.

6. Partition a string into all lists of palindromic substrings.
   - Expected pattern: backtracking plus palindrome precomputation.
   - Pattern tags: `palindrome-partitioning`, `precompute-dp`.
   - Solution: [Palindrome Partitioning](./solutions.md#6-palindrome-partitioning).
   - Complexity target: time `O(n * 2^n)`, space `O(n^2)` optional palindrome table.

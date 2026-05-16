# Backtracking Coding Patterns

Backtracking explores a search tree. L7 answers should explain pruning, duplicate handling, recursion depth, and when the search is exponential by nature.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Generate subsets | Need all combinations of inclusion/exclusion | Current path represents decisions for processed positions | Time `O(n * 2^n)`, space `O(n)` recursion plus output | Runtime: iterative bitmask alternative. Memory: path reused across recursion. |
| Generate permutations | Need all orderings | Path contains each used element at most once | Time `O(n * n!)`, space `O(n)` recursion plus output | Runtime: swap in place. Memory: avoid copied vectors per call. |
| Combination sum | Choose values reaching target | Remaining target never negative in valid branch | Exponential, space `O(depth)` plus output | Runtime: sort and break when value exceeds remaining. Memory: reuse path. |
| N queens | Place non-attacking queens | Used columns and diagonals contain prior queens only | Time roughly `O(n!)`, space `O(n)` plus output | Runtime: sets or bitmasks for attacks. Memory: bitmasks replace hash sets. |
| Word search | Path through grid spelling word | Visited cells are unique in current path | Time `O(rows * cols * 4^L)`, space `O(L)` | Runtime: frequency precheck and rare-end reversal. Memory: mark grid in place. |
| Palindrome partitioning | Split string into valid palindromes | Path partitions prefix exactly | Time `O(n * 2^n)`, space `O(n^2)` optional palindrome table | Runtime: precompute palindromes. Memory: compute palindrome on demand if memory tight. |
| Sudoku solver | Constraint satisfaction | Board remains valid after every placement | Exponential, space `O(1)` board plus recursion | Runtime: choose cell with fewest candidates. Memory: row/col/box bitmasks. |
| Restore IP addresses | Partition string into 4 valid octets | Current path contains valid octets and consumes prefix | Time `O(1)` bounded by fixed partitions, space `O(1)` plus output | Runtime: prune by remaining length. Memory: avoid substring copies until output. |
| Unique permutations with duplicates | Duplicates in permutations | Equal values are used in one canonical order at each depth | Time `O(n * uniquePermutations)`, space `O(n)` | Runtime: sort and skip duplicates. Memory: used vector or frequency map. |
| Search with branch and bound | Optimization over combinatorial choices | Current bound is an optimistic limit on branch quality | Problem-dependent exponential, space `O(depth)` | Runtime: strong upper/lower bounds. Memory: iterative stack if recursion depth is high. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `subsets`, `include-exclude` | include/exclude recursion | [questions](./questions.md): Q1 |
| `permutations`, `duplicate-skipping` | sorting plus duplicate skip or frequency map | [questions](./questions.md): Q2 |
| `combination-sum`, `pruning` | sorted candidates plus remaining-target pruning | [questions](./questions.md): Q3 |
| `constraint-backtracking`, `n-queens` | row-by-row placement with column and diagonal constraints | [questions](./questions.md): Q4 |
| `constraint-backtracking`, `bitmask` | constraint propagation plus backtracking | [questions](./questions.md): Q5 |
| `palindrome-partitioning`, `precompute-dp` | backtracking plus palindrome precomputation | [questions](./questions.md): Q6 |

## L7 Follow-Ups

- What pruning rule is sound, and why cannot it drop an optimal answer?
- How do you avoid duplicate outputs?
- What is the worst-case search size?
- How would you parallelize the search tree without duplicating work?

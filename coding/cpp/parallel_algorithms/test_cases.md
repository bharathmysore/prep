# Parallel Algorithms Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Parallel Map

* **Question**: Implement parallel map over a vector with a fixed number of workers.
* **Solution**: [Parallel Map](./solutions.md#1-parallel-map).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Order preservation | input `[1,2,3]`, square function, workers 2 | Return `[1,4,9]` in input order. |
| Empty input | `[]` | Return empty output without starting unnecessary work. |
| Exception policy | mapper throws for one item | Error is propagated or captured according to API. |

## 2. Parallel Reduce

* **Question**: Implement parallel reduce for an associative operation.
* **Solution**: [Parallel Reduce](./solutions.md#2-parallel-reduce).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Sum | input `[1,2,3,4]`, identity 0 | Return `10`. |
| Empty | empty input | Return identity. |
| Associativity requirement | non-associative combine | Document that result may differ by partitioning. |

## 3. Parallel Prefix Sum

* **Question**: Implement parallel prefix sum.
* **Solution**: [Parallel Prefix Sum](./solutions.md#3-parallel-prefix-sum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[1,2,3,4]` | Return `[1,3,6,10]`. |
| Single element | `[5]` | Return `[5]`. |
| Large input | many values split across workers | Matches sequential prefix exactly. |

## 4. Parallel Top K

* **Question**: Given a very large log split into chunks, compute top `k` error codes in parallel.
* **Solution**: [Parallel Top K](./solutions.md#4-parallel-top-k).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Basic | input `[5,1,9,3,7]`, k=3 | Return top values `[9,7,5]`. |
| k larger than n | k > input size | Return all values sorted by rank. |
| Skewed partitions | largest values concentrated in one partition | Global merge still returns correct top k. |

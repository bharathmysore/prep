# Graphs Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Course Schedule Order

* **Question**: Given prerequisite pairs, return a valid course order or report that none exists.
* **Solution**: [Course Schedule Order](./solutions.md#1-course-schedule-order).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| DAG | numCourses 2, prerequisites `[[1,0]]` | Return an order such as `[0,1]`. |
| Cycle | `[[0,1],[1,0]]` | Return empty order. |
| Disconnected | courses with no prerequisites | All courses appear exactly once. |

## 2. Connected Components

* **Question**: Given an undirected graph, return the number of connected components.
* **Solution**: [Connected Components](./solutions.md#2-connected-components).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Two components | n=5, edges `[[0,1],[1,2],[3,4]]` | Return `2`. |
| No edges | n=3, edges `[]` | Return `3`. |
| Fully connected | chain over all nodes | Return `1`. |

## 3. Dijkstra Shortest Paths

* **Question**: Given a weighted directed graph with non-negative weights, return shortest paths from a source.
* **Solution**: [Dijkstra Shortest Paths](./solutions.md#3-dijkstra-shortest-paths).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Weighted graph | edges A-B 4, A-C 1, C-B 2 | Distance A->B is `3`. |
| Unreachable | node disconnected from source | Distance remains infinity/sentinel. |
| Zero-weight edge | include weight 0 edge | Still computes shortest paths with non-negative weights. |

## 4. Evaluate Division

* **Question**: Given equations like `a / b = 2.0`, answer division queries.
* **Solution**: [Evaluate Division](./solutions.md#4-evaluate-division).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | equations `a/b=2`, `b/c=3`, query `a/c` | Return `6.0`. |
| Reverse query | query `c/a` | Return `1/6`. |
| Unknown variable | query `a/x` | Return `-1.0`. |

## 5. Accounts Merge

* **Question**: Given accounts with shared email addresses, merge accounts belonging to the same person.
* **Solution**: [Accounts Merge](./solutions.md#5-accounts-merge).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Shared email | John accounts share `john@mail.com` | Merged account contains union of emails sorted. |
| Same name no shared email | Two accounts same name but no shared email | Remain separate. |
| Transitive merge | A shares with B, B shares with C | All three merge. |

## 6. Alien Dictionary

* **Question**: Given words sorted in an alien language, infer a valid character order.
* **Solution**: [Alien Dictionary](./solutions.md#6-alien-dictionary).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | words `wrt, wrf, er, ett, rftt` | One valid order is `wertf`. |
| Invalid prefix | `abc`, `ab` | Return empty order. |
| Cycle | constraints create cycle | Return empty order. |

## 7. Bipartite Graph

* **Question**: Given an undirected graph, determine whether its vertices can be colored with two colors without same-color adjacent vertices.
* **Solution**: [Bipartite Graph](./solutions.md#7-bipartite-graph).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Even cycle | 0-1-2-3-0 | Return true. |
| Odd cycle | 0-1-2-0 | Return false. |
| Disconnected | one bipartite component plus isolated node | Return true. |

## 8. Shortest Path In Binary Matrix

* **Question**: Given a binary grid, find the shortest 8-direction path from top-left to bottom-right through zero cells.
* **Solution**: [Shortest Path In Binary Matrix](./solutions.md#8-shortest-path-in-binary-matrix).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Small path | grid `[[0,1],[1,0]]` | Return `2`. |
| Blocked start | start cell is 1 | Return `-1`. |
| Single open cell | grid `[[0]]` | Return `1`. |

# Graphs Coding Questions

Solve each question in C++ after first defining vertices, edges, direction, weights, and graph representation.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

<a id="1-course-schedule-order"></a>
1. Given prerequisite pairs, return a valid course order or report that none exists.
   - Expected pattern: topological sort.
   - Pattern tags: `topological-sort`, `kahn`.
   - Solution: [Course Schedule Order](./solutions.md#1-course-schedule-order).
   - Complexity target: time `O(V + E)`, space `O(V + E)`.

2. Given an undirected graph, return the number of connected components.
   - Expected pattern: DFS, BFS, or DSU.
   - Pattern tags: `connected-components`, `graph-traversal`.
   - Solution: [Connected Components](./solutions.md#2-connected-components).
   - Complexity target: time `O(V + E)`, space `O(V)`.

3. Given a weighted directed graph with non-negative weights, return shortest paths from a source.
   - Expected pattern: Dijkstra with heap.
   - Pattern tags: `dijkstra`, `shortest-path`.
   - Solution: [Dijkstra Shortest Paths](./solutions.md#3-dijkstra-shortest-paths).
   - Complexity target: time `O((V + E) log V)`, space `O(V + E)`.

4. Given equations like `a / b = 2.0`, answer division queries.
   - Expected pattern: weighted graph traversal or weighted DSU.
   - Pattern tags: `weighted-graph`, `graph-traversal`.
   - Solution: [Evaluate Division](./solutions.md#4-evaluate-division).
   - Complexity target: build `O(E)`, query `O(V + E)` by BFS or near `O(alpha(V))` with weighted DSU.

5. Given accounts with shared email addresses, merge accounts belonging to the same person.
   - Expected pattern: DSU or graph connected components.
   - Pattern tags: `dsu`, `connected-components`.
   - Solution: [Accounts Merge](./solutions.md#5-accounts-merge).
   - Complexity target: time `O(n alpha(n))` plus output sorting, space `O(n)`.

6. Given words sorted in an alien language, infer a valid character order.
   - Expected pattern: graph construction plus topological sort.
   - Pattern tags: `topological-sort`, `graph-modeling`.
   - Solution: [Alien Dictionary](./solutions.md#6-alien-dictionary).
   - Complexity target: time `O(total chars + alphabet + edges)`, space `O(alphabet + edges)`.

7. Given an undirected graph, determine whether its vertices can be colored with two colors without same-color adjacent vertices.
   - Expected pattern: BFS or DFS two-coloring across every component.
   - Pattern tags: `bipartite`, `graph-coloring`.
   - Solution: [Bipartite Graph](./solutions.md#7-bipartite-graph).
   - Complexity target: time `O(V + E)`, space `O(V)`.

8. Given a binary grid, find the shortest 8-direction path from top-left to bottom-right through zero cells.
   - Expected pattern: BFS on an unweighted grid graph.
   - Pattern tags: `grid-bfs`, `shortest-path`.
   - Solution: [Shortest Path In Binary Matrix](./solutions.md#8-shortest-path-in-binary-matrix).
   - Complexity target: time `O(rows * cols)`, space `O(rows * cols)`.

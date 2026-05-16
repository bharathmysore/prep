# Graphs Coding Patterns

Graph questions are L7 favorites because the hard part is often modeling. First define vertices, edges, direction, weights, and failure or cycle behavior.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Number of connected components | Undirected reachability | Each DFS/BFS marks exactly one component | Time `O(V + E)`, space `O(V)` | Runtime: adjacency list. Memory: bitset visited for dense integer ids. |
| Course schedule | Dependencies and possible cycles | Zero-indegree queue contains currently schedulable nodes | Time `O(V + E)`, space `O(V + E)` | Runtime: Kahn topological sort. Memory: store indegree plus adjacency. |
| Dijkstra shortest path | Non-negative weighted edges | Popped heap node has final shortest distance | Time `O((V + E) log V)`, space `O(V + E)` | Runtime: lazy heap deletion. Memory: compact adjacency vectors. |
| Bellman-Ford | Negative edges or need negative-cycle detection | After `i` rounds, shortest paths with at most `i` edges are known | Time `O(VE)`, space `O(V)` | Runtime: stop if no relaxation. Memory: one distance array unless path length isolation is needed. |
| Minimum spanning tree | Connect all nodes with minimum undirected edge cost | Chosen edges never create a cycle | Time `O(E log E)` Kruskal, space `O(V)` DSU | Runtime: sort once. Memory: DSU arrays. |
| Accounts merge / components by shared key | Entities connected by shared attributes | DSU parent represents merged identity group | Time `O(n alpha(n))`, space `O(n)` | Runtime: union by rank and path compression. Memory: map only external keys to ids. |
| Bipartite graph check | Need two-color partition | Every edge connects opposite colors | Time `O(V + E)`, space `O(V)` | Runtime: BFS each component. Memory: color array with three states. |
| Shortest path in grid | Uniform edge cost grid | BFS visits cells in nondecreasing distance | Time `O(rows * cols)`, space `O(rows * cols)` | Runtime: mutate grid for visited if allowed. Memory: encode coordinates as ints. |
| A* search | Shortest path with admissible heuristic | Priority is `g + h`, and `h` never overestimates | Time depends on heuristic, worst `O(E log V)`, space `O(V)` | Runtime: stronger admissible heuristic reduces explored states. Memory: store parent only if path reconstruction is needed. |
| Alien dictionary | Infer character order from sorted words | First differing char gives a precedence edge | Time `O(total chars + alphabet + edges)`, space `O(alphabet + edges)` | Runtime: validate prefix invalid case early. Memory: dedupe edges. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `topological-sort`, `kahn` | topological sort | [questions](./questions.md): Q1 |
| `connected-components`, `graph-traversal` | DFS, BFS, or DSU | [questions](./questions.md): Q2 |
| `dijkstra`, `shortest-path` | Dijkstra with heap | [questions](./questions.md): Q3 |
| `weighted-graph`, `graph-traversal` | weighted graph traversal or weighted DSU | [questions](./questions.md): Q4 |
| `dsu`, `connected-components` | DSU or graph connected components | [questions](./questions.md): Q5 |
| `topological-sort`, `graph-modeling` | graph construction plus topological sort | [questions](./questions.md): Q6 |
| `bipartite`, `graph-coloring` | BFS or DFS two-coloring | [questions](./questions.md): Q7 |
| `grid-bfs`, `shortest-path` | BFS on an unweighted grid graph | [questions](./questions.md): Q8 |

## L7 Follow-Ups

- How do you choose between DFS, BFS, DSU, and topological sort?
- What graph representation works best for sparse vs dense graphs?
- How do you handle very large graphs that do not fit in memory?
- Which graph algorithms parallelize, and where do races appear?

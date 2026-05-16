# Graphs Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Course Schedule Order

* **Pattern / Idea**: Kahn topological sort.
* **Company Frequency Tags**: Public signal: `Snowflake: High (6m 94.3)`, `Meta: Medium (6m 59.8)`, `Apple: Medium (6m 58.9)`, `Microsoft: Medium (6m 47.2)`, `Oracle: Medium (6m 47.0)`, `NVIDIA: Medium (all 47.7)`, `Databricks: Medium (all 37.2)`.
* **Question**: Given prerequisite pairs, return a valid course order or report that none exists.
* **Test Cases**: [Test cases](./test_cases.md#1-course-schedule-order).
* **C++ Code**
  ```cpp
  vector<int> courseOrder(int n, const vector<pair<int, int>>& prereq) {
      vector<vector<int>> g(n);
      vector<int> indeg(n, 0);
      for (auto [course, pre] : prereq) {
          g[pre].push_back(course);
          ++indeg[course];
      }
      queue<int> q;
      for (int i = 0; i < n; ++i) if (indeg[i] == 0) q.push(i);
      vector<int> order;
      while (!q.empty()) {
          int u = q.front(); q.pop();
          order.push_back(u);
          for (int v : g[u]) if (--indeg[v] == 0) q.push(v);
      }
      return order.size() == static_cast<size_t>(n) ? order : vector<int>{};
  }
  ```
* **Code Explanation**: Nodes with zero remaining prerequisites are schedulable; removing them unlocks dependents.
* **Invariants**: Queue contains exactly nodes with current indegree zero that have not been emitted.
* **Complexity**: Time `O(V + E)`, space `O(V + E)`.
* **Optimizations**: Runtime: adjacency list. Memory: store edges once.
* **Edge Cases To Consider**: Cycle, disconnected graph, no prerequisites, self dependency.
* **L7 Follow-ups**: For incremental dependency changes, recomputing full order may be too expensive.

## 2. Connected Components

* **Pattern / Idea**: BFS/DFS over undirected graph.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given an undirected graph, return the number of connected components.
* **Test Cases**: [Test cases](./test_cases.md#2-connected-components).
* **C++ Code**
  ```cpp
  int connectedComponents(int n, const vector<pair<int, int>>& edges) {
      vector<vector<int>> g(n);
      for (auto [a, b] : edges) {
          g[a].push_back(b);
          g[b].push_back(a);
      }
      vector<char> seen(n, false);
      int comps = 0;
      for (int i = 0; i < n; ++i) {
          if (seen[i]) continue;
          ++comps;
          queue<int> q;
          q.push(i);
          seen[i] = true;
          while (!q.empty()) {
              int u = q.front(); q.pop();
              for (int v : g[u]) if (!seen[v]) {
                  seen[v] = true;
                  q.push(v);
              }
          }
      }
      return comps;
  }
  ```
* **Code Explanation**: Each BFS marks one maximal reachable component.
* **Invariants**: All nodes marked during a BFS belong to the same component.
* **Complexity**: Time `O(V + E)`, space `O(V + E)`.
* **Optimizations**: Runtime: DSU is good for streaming edges. Memory: compact adjacency for dense integer ids.
* **Edge Cases To Consider**: Isolated nodes, duplicate edges, all connected, no nodes.
* **L7 Follow-ups**: For huge graphs, process partitions and merge component labels.

## 3. Dijkstra Shortest Paths

* **Pattern / Idea**: Greedy shortest path for non-negative weights.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given a weighted directed graph with non-negative weights, return shortest paths from a source.
* **Test Cases**: [Test cases](./test_cases.md#3-dijkstra-shortest-paths).
* **C++ Code**
  ```cpp
  vector<long long> dijkstra(int n, vector<vector<pair<int, int>>> g, int src) {
      const long long INF = LLONG_MAX / 4;
      vector<long long> dist(n, INF);
      priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;
      dist[src] = 0;
      pq.push({0, src});
      while (!pq.empty()) {
          auto [d, u] = pq.top(); pq.pop();
          if (d != dist[u]) continue;
          for (auto [v, w] : g[u]) {
              if (d + w < dist[v]) {
                  dist[v] = d + w;
                  pq.push({dist[v], v});
              }
          }
      }
      return dist;
  }
  ```
* **Code Explanation**: The smallest unstale heap entry has final shortest distance because all edges are non-negative.
* **Invariants**: `dist[u]` is the best known distance; popped non-stale node is finalized.
* **Complexity**: Time `O((V + E) log V)`, space `O(V + E)`.
* **Optimizations**: Runtime: lazy deletion avoids decrease-key. Memory: adjacency vectors reduce overhead.
* **Edge Cases To Consider**: Disconnected nodes, zero-weight edges, large weights, negative edge rejection.
* **L7 Follow-ups**: Use Bellman-Ford or Johnson's algorithm if negative edges appear.

## 4. Evaluate Division

* **Pattern / Idea**: Weighted graph traversal.
* **Company Frequency Tags**: Public signal: `Google: Medium (6m 31.8)`, `Stripe: High (all 68.2)`, `Snowflake: Medium (all 39.6)`.
* **Question**: Given equations like `a / b = 2.0`, answer division queries.
* **Test Cases**: [Test cases](./test_cases.md#4-evaluate-division).
* **C++ Code**
  ```cpp
  vector<double> calcEquation(vector<tuple<string, string, double>> eq,
                              vector<pair<string, string>> queries) {
      unordered_map<string, vector<pair<string, double>>> g;
      for (auto& [a, b, v] : eq) {
          g[a].push_back({b, v});
          g[b].push_back({a, 1.0 / v});
      }
      vector<double> ans;
      for (auto& [src, dst] : queries) {
          if (!g.count(src) || !g.count(dst)) {
              ans.push_back(-1.0);
              continue;
          }
          queue<pair<string, double>> q;
          unordered_set<string> seen{src};
          q.push({src, 1.0});
          double found = -1.0;
          while (!q.empty() && found < 0) {
              auto [u, val] = q.front(); q.pop();
              if (u == dst) { found = val; break; }
              for (auto& [v, w] : g[u]) if (!seen.count(v)) {
                  seen.insert(v);
                  q.push({v, val * w});
              }
          }
          ans.push_back(found);
      }
      return ans;
  }
  ```
* **Code Explanation**: Edge weights encode ratios; multiplying weights along a path yields the query ratio.
* **Invariants**: Queue value equals ratio from source to queued node.
* **Complexity**: Build `O(E)`, query `O(V + E)`, space `O(V + E)`.
* **Optimizations**: Runtime: weighted DSU speeds repeated queries. Memory: intern strings to integer ids.
* **Edge Cases To Consider**: Unknown variable, self query, disconnected components, reciprocal query.
* **L7 Follow-ups**: Floating-point error accumulates over long paths.

## 5. Accounts Merge

* **Pattern / Idea**: DSU by shared email.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given accounts with shared email addresses, merge accounts belonging to the same person.
* **Test Cases**: [Test cases](./test_cases.md#5-accounts-merge).
* **C++ Code**
  ```cpp
  struct DSU {
      vector<int> p, sz;
      explicit DSU(int n) : p(n), sz(n, 1) { iota(p.begin(), p.end(), 0); }
      int find(int x) { return p[x] == x ? x : p[x] = find(p[x]); }
      void unite(int a, int b) {
          a = find(a); b = find(b);
          if (a == b) return;
          if (sz[a] < sz[b]) swap(a, b);
          p[b] = a; sz[a] += sz[b];
      }
  };

  vector<vector<string>> accountsMerge(vector<vector<string>> accounts) {
      DSU dsu(accounts.size());
      unordered_map<string, int> owner;
      for (int i = 0; i < static_cast<int>(accounts.size()); ++i) {
          for (int j = 1; j < static_cast<int>(accounts[i].size()); ++j) {
              string& email = accounts[i][j];
              if (!owner.count(email)) owner[email] = i;
              else dsu.unite(i, owner[email]);
          }
      }
      unordered_map<int, vector<string>> grouped;
      for (auto& [email, idx] : owner) grouped[dsu.find(idx)].push_back(email);
      vector<vector<string>> ans;
      for (auto& [idx, emails] : grouped) {
          sort(emails.begin(), emails.end());
          vector<string> row{accounts[idx][0]};
          row.insert(row.end(), emails.begin(), emails.end());
          ans.push_back(move(row));
      }
      return ans;
  }
  ```
* **Code Explanation**: Accounts sharing any email are unioned into the same component.
* **Invariants**: All emails in a DSU component belong to the same merged account.
* **Complexity**: Near `O(total emails * alpha(n))` plus sorting output; space `O(total emails)`.
* **Optimizations**: Runtime: union by size and path compression. Memory: map only emails to account ids.
* **Edge Cases To Consider**: Duplicate email in one account, same name not enough to merge, multiple components.
* **L7 Follow-ups**: Real identity merge requires auditability and undo.

## 6. Alien Dictionary

* **Pattern / Idea**: Build precedence graph from first differing char.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given words sorted in an alien language, infer a valid character order.
* **Test Cases**: [Test cases](./test_cases.md#6-alien-dictionary).
* **C++ Code**
  ```cpp
  string alienOrder(vector<string> words) {
      unordered_map<char, unordered_set<char>> g;
      unordered_map<char, int> indeg;
      for (const string& w : words) for (char c : w) indeg.try_emplace(c, 0);
      for (int i = 1; i < static_cast<int>(words.size()); ++i) {
          string& a = words[i - 1];
          string& b = words[i];
          int j = 0;
          while (j < static_cast<int>(min(a.size(), b.size())) && a[j] == b[j]) ++j;
          if (j == static_cast<int>(min(a.size(), b.size()))) {
              if (a.size() > b.size()) return "";
              continue;
          }
          if (!g[a[j]].count(b[j])) {
              g[a[j]].insert(b[j]);
              ++indeg[b[j]];
          }
      }
      queue<char> q;
      for (auto [c, d] : indeg) if (d == 0) q.push(c);
      string ans;
      while (!q.empty()) {
          char u = q.front(); q.pop();
          ans.push_back(u);
          for (char v : g[u]) if (--indeg[v] == 0) q.push(v);
      }
      return ans.size() == indeg.size() ? ans : "";
  }
  ```
* **Code Explanation**: Only the first differing character between adjacent sorted words creates a guaranteed order edge.
* **Invariants**: Indegree counts remaining prerequisites for each character.
* **Complexity**: Time `O(total chars + edges)`, space `O(alphabet + edges)`.
* **Optimizations**: Runtime: validate prefix case early. Memory: dedupe edges.
* **Edge Cases To Consider**: Prefix invalid, cycle, disconnected chars, one word.
* **L7 Follow-ups**: Multiple valid orders are acceptable unless deterministic output is required.

## 7. Bipartite Graph

* **Pattern / Idea**: Two-color BFS.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given an undirected graph, determine whether its vertices can be colored with two colors without same-color adjacent vertices.
* **Test Cases**: [Test cases](./test_cases.md#7-bipartite-graph).
* **C++ Code**
  ```cpp
  bool isBipartite(const vector<vector<int>>& g) {
      vector<int> color(g.size(), -1);
      for (int i = 0; i < static_cast<int>(g.size()); ++i) {
          if (color[i] != -1) continue;
          queue<int> q;
          q.push(i);
          color[i] = 0;
          while (!q.empty()) {
              int u = q.front(); q.pop();
              for (int v : g[u]) {
                  if (color[v] == -1) {
                      color[v] = color[u] ^ 1;
                      q.push(v);
                  } else if (color[v] == color[u]) {
                      return false;
                  }
              }
          }
      }
      return true;
  }
  ```
* **Code Explanation**: Adjacent nodes must have opposite colors; a same-color edge proves an odd cycle.
* **Invariants**: Colored edges processed so far connect opposite colors.
* **Complexity**: Time `O(V + E)`, space `O(V)`.
* **Optimizations**: Runtime: BFS or DFS equivalent. Memory: `int8_t` color array for large graphs.
* **Edge Cases To Consider**: Disconnected graph, self-loop, odd cycle, even cycle.
* **L7 Follow-ups**: For dynamic graphs, incremental bipartiteness needs DSU with parity.

## 8. Shortest Path In Binary Matrix

* **Pattern / Idea**: BFS on unweighted grid.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given a binary grid, find the shortest 8-direction path from top-left to bottom-right through zero cells.
* **Test Cases**: [Test cases](./test_cases.md#8-shortest-path-in-binary-matrix).
* **C++ Code**
  ```cpp
  int shortestPathBinaryMatrix(vector<vector<int>> grid) {
      int n = grid.size();
      if (n == 0 || grid[0][0] || grid[n - 1][n - 1]) return -1;
      queue<pair<int, int>> q;
      q.push({0, 0});
      grid[0][0] = 1;
      int dist = 1;
      int dirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
      while (!q.empty()) {
          int sz = q.size();
          while (sz--) {
              auto [r, c] = q.front(); q.pop();
              if (r == n - 1 && c == n - 1) return dist;
              for (auto& d : dirs) {
                  int nr = r + d[0], nc = c + d[1];
                  if (nr >= 0 && nr < n && nc >= 0 && nc < n && grid[nr][nc] == 0) {
                      grid[nr][nc] = 1;
                      q.push({nr, nc});
                  }
              }
          }
          ++dist;
      }
      return -1;
  }
  ```
* **Code Explanation**: BFS explores cells in increasing path length.
* **Invariants**: All cells in the current queue layer have distance `dist`.
* **Complexity**: Time `O(n^2)`, space `O(n^2)` queue worst case.
* **Optimizations**: Runtime: mark visited when enqueuing. Memory: mutate grid for visited if allowed.
* **Edge Cases To Consider**: Blocked start/end, one cell, no path, diagonal-only path.
* **L7 Follow-ups**: For weighted grids, switch to Dijkstra or A*.

# Trees And Tries Coding Patterns

Trees test recursive invariants and boundary conditions. Tries test prefix compression, memory tradeoffs, and search pruning.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Inorder traversal | Need sorted order from BST or traversal sequence | Stack path contains ancestors whose left side is done | Time `O(n)`, space `O(h)` | Runtime: iterative avoids recursion limits. Memory: Morris traversal is `O(1)` but mutates temporarily. |
| Validate BST | Need global ordering constraints | Each node value lies within inherited `(low, high)` bounds | Time `O(n)`, space `O(h)` | Runtime: fail early. Memory: iterative stack if recursion depth is risky. |
| Lowest common ancestor | Need deepest shared ancestor | Return non-null when subtree contains a target | Time `O(n)`, space `O(h)` | Runtime: BST case can use ordering for `O(h)`. Memory: parent map trades space for iterative flow. |
| Serialize and deserialize binary tree | Need stable external representation | Null markers preserve shape, not only values | Time `O(n)`, space `O(n)` | Runtime: preorder with index. Memory: avoid substring parsing by tokenizing once. |
| Kth smallest in BST | Need rank in sorted tree | Inorder count equals number of visited smaller nodes | Time `O(h + k)`, space `O(h)` | Runtime: stop at `k`. Memory: augment subtree sizes for repeated queries. |
| Path sum variants | Need root-to-leaf or any downward sum | Prefix sum map counts ancestors with matching difference | Time `O(n)`, space `O(h)` average for path-prefix | Runtime: backtrack counts exactly. Memory: map state bounded by depth. |
| Build tree from traversals | Reconstruct tree from preorder/inorder or postorder/inorder | Root partitions inorder into left and right subtrees | Time `O(n)`, space `O(n)` | Runtime: map value to inorder index. Memory: pass ranges, not sliced vectors. |
| Trie insert/search/prefix | Many prefix lookups | Each node represents one prefix | Time `O(length)`, space `O(total chars * alphabet)` | Runtime: array child table for small alphabet. Memory: unordered children for sparse alphabets. |
| Autocomplete top K | Prefix plus ranked suggestions | Trie node can cache best suggestions for its prefix | Query time `O(prefix + k)`, space higher by cached lists | Runtime: precompute top K at insert. Memory: cap cached lists. |
| Word search with trie | Grid search for dictionary words | DFS path maps to a trie prefix and visited cells are unique | Time `O(cells * branching^word length)`, pruned by trie, space `O(dictionary chars)` | Runtime: prune dead trie branches. Memory: mark found words in trie to dedupe. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `bst-bounds`, `tree-dfs` | recursive bounds or inorder monotonicity | [questions](./questions.md): Q1 |
| `lca`, `postorder-dfs` | postorder subtree containment | [questions](./questions.md): Q2 |
| `tree-serialization`, `preorder-dfs` | traversal with null markers | [questions](./questions.md): Q3 |
| `bst-inorder`, `rank-query` | inorder traversal with early stop | [questions](./questions.md): Q4 |
| `tree-prefix-sum`, `hash-map` | prefix sums on the root-to-current path | [questions](./questions.md): Q5 |
| `tree-reconstruction`, `preorder-inorder` | root partitions inorder range | [questions](./questions.md): Q6 |
| `trie`, `prefix-tree` | prefix tree nodes with terminal markers | [questions](./questions.md): Q7 |
| `trie`, `grid-dfs`, `backtracking` | trie plus DFS backtracking | [questions](./questions.md): Q8 |

## L7 Follow-Ups

- How do you avoid recursion depth failures for skewed trees?
- When is a compressed trie or radix tree worth the complexity?
- How do you support deletes in a trie without breaking shared prefixes?
- What metadata would you add for repeated rank, prefix, or autocomplete queries?

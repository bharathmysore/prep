# Trees And Tries Coding Questions

Solve each question in C++ and state recursion depth, iterative alternatives, and prefix-memory tradeoffs.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Validate whether a binary tree is a binary search tree.
   - Expected pattern: recursive bounds or inorder monotonicity.
   - Pattern tags: `bst-bounds`, `tree-dfs`.
   - Solution: [Validate Binary Search Tree](./solutions.md#1-validate-binary-search-tree).
   - Complexity target: time `O(n)`, space `O(h)`.

2. Return the lowest common ancestor of two nodes in a binary tree.
   - Expected pattern: postorder subtree containment.
   - Pattern tags: `lca`, `postorder-dfs`.
   - Solution: [Lowest Common Ancestor In Binary Tree](./solutions.md#2-lowest-common-ancestor-in-binary-tree).
   - Complexity target: time `O(n)`, space `O(h)`.

3. Serialize and deserialize a binary tree.
   - Expected pattern: traversal with null markers.
   - Pattern tags: `tree-serialization`, `preorder-dfs`.
   - Solution: [Serialize And Deserialize Binary Tree](./solutions.md#3-serialize-and-deserialize-binary-tree).
   - Complexity target: time `O(n)`, space `O(n)`.

4. Return the kth smallest value in a BST.
   - Expected pattern: inorder traversal with early stop.
   - Pattern tags: `bst-inorder`, `rank-query`.
   - Solution: [Kth Smallest In BST](./solutions.md#4-kth-smallest-in-bst).
   - Complexity target: time `O(h + k)`, space `O(h)`.

5. Given a binary tree and target sum, count downward paths whose values add to the target.
   - Expected pattern: prefix sums on the root-to-current path.
   - Pattern tags: `tree-prefix-sum`, `hash-map`.
   - Solution: [Path Sum III](./solutions.md#5-path-sum-iii).
   - Complexity target: time `O(n)` average, space `O(h)` average path state.

6. Given preorder and inorder traversals with unique values, reconstruct the binary tree.
   - Expected pattern: root partitions inorder range.
   - Pattern tags: `tree-reconstruction`, `preorder-inorder`.
   - Solution: [Build Tree From Preorder And Inorder](./solutions.md#6-build-tree-from-preorder-and-inorder).
   - Complexity target: time `O(n)`, space `O(n)`.

7. Implement a trie with insert, search, and prefix search.
   - Expected pattern: prefix tree nodes with terminal markers.
   - Pattern tags: `trie`, `prefix-tree`.
   - Solution: [Trie Insert Search Prefix](./solutions.md#7-trie-insert-search-prefix).
   - Complexity target: time `O(length)` per operation, space `O(total chars)`.

8. Given a board and dictionary, return all dictionary words that can be formed by adjacent cells.
   - Expected pattern: trie plus DFS backtracking.
   - Pattern tags: `trie`, `grid-dfs`, `backtracking`.
   - Solution: [Word Search II](./solutions.md#8-word-search-ii).
   - Complexity target: worst-case exponential in word length, pruned by trie; space `O(dictionary chars + recursion depth)`.

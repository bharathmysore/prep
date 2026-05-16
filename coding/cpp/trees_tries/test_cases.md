# Trees Tries Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Validate Binary Search Tree

* **Question**: Validate whether a binary tree is a binary search tree.
* **Solution**: [Validate Binary Search Tree](./solutions.md#1-validate-binary-search-tree).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Valid | tree `2,1,3` | Return `true`. |
| Invalid descendant | tree `5,1,4,null,null,3,6` | Return `false`. |
| Duplicate | duplicate equal to parent | Return `false` if BST requires strict ordering. |

## 2. Lowest Common Ancestor In Binary Tree

* **Question**: Return the lowest common ancestor of two nodes in a binary tree.
* **Solution**: [Lowest Common Ancestor In Binary Tree](./solutions.md#2-lowest-common-ancestor-in-binary-tree).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Split nodes | root 3; nodes 5 and 1 | Return 3. |
| Ancestor is answer | nodes 5 and 4 where 4 is under 5 | Return 5. |
| Missing node policy | one node absent | Return found node or null according to stated contract. |

## 3. Serialize And Deserialize Binary Tree

* **Question**: Serialize and deserialize a binary tree.
* **Solution**: [Serialize And Deserialize Binary Tree](./solutions.md#3-serialize-and-deserialize-binary-tree).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Round trip | tree `1,2,3,null,null,4,5` | Deserialize(serialize(tree)) preserves structure and values. |
| Empty tree | `null` | Round trip returns null. |
| Negative values | tree with negative values | Values are preserved. |

## 4. Kth Smallest In BST

* **Question**: Return the kth smallest value in a BST.
* **Solution**: [Kth Smallest In BST](./solutions.md#4-kth-smallest-in-bst).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | BST `[3,1,4,null,2]`, `k=1` | Return `1`. |
| Largest k | same tree, `k=n` | Return largest value. |
| Invalid k | `k=0` or `k>n` | Reject or return sentinel per API contract. |

## 5. Path Sum III

* **Question**: Given a binary tree and target sum, count downward paths whose values add to the target.
* **Solution**: [Path Sum III](./solutions.md#5-path-sum-iii).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | tree `[10,5,-3,3,2,null,11,3,-2,null,1]`, target 8 | Return `3`. |
| Negative values | tree with negative and positive values | Count paths using prefix sums, not monotonic pruning. |
| Empty tree | `null` | Return `0`. |

## 6. Build Tree From Preorder And Inorder

* **Question**: Given preorder and inorder traversals with unique values, reconstruct the binary tree.
* **Solution**: [Build Tree From Preorder And Inorder](./solutions.md#6-build-tree-from-preorder-and-inorder).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | preorder `[3,9,20,15,7]`, inorder `[9,3,15,20,7]` | Reconstruct the expected tree rooted at 3. |
| Single node | preorder `[1]`, inorder `[1]` | Return single-node tree. |
| Invalid traversals | mismatched sets | Reject or fail fast if validation is included. |

## 7. Trie Insert Search Prefix

* **Question**: Implement a trie with insert, search, and prefix search.
* **Solution**: [Trie Insert Search Prefix](./solutions.md#7-trie-insert-search-prefix).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Insert/search | insert `apple`; search `apple` | Return true. |
| Prefix only | search `app` before inserting `app` | `search` false, `startsWith` true. |
| Shared prefix | insert `app` then `apple` | Both words searchable. |

## 8. Word Search II

* **Question**: Given a board and dictionary, return all dictionary words that can be formed by adjacent cells.
* **Solution**: [Word Search II](./solutions.md#8-word-search-ii).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | board with words `{oath,pea,eat,rain}` | Return `{oath,eat}`. |
| Duplicate path prevention | word requiring same cell twice | Do not return that word. |
| Prefix pruning | many words sharing prefix | Returns only completed words present on board. |

# Trees And Tries Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

```cpp
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int v = 0) : val(v), left(nullptr), right(nullptr) {}
};
```

## 1. Validate Binary Search Tree

* **Pattern / Idea**: Recursive bounds.
* **Company Frequency Tags**: Public signal: `Oracle: Medium (6m 47.0)`, `Amazon/AWS: Medium (6m 45.9)`, `Microsoft: Medium (6m 40.4)`, `Meta: Low (6m 27.2)`, `NVIDIA: Medium (all 54.4)`, `Apple: Medium (all 46.1)`.
* **Question**: Validate whether a binary tree is a binary search tree.
* **Test Cases**: [Test cases](./test_cases.md#1-validate-binary-search-tree).
* **C++ Code**
  ```cpp
  bool validBstDfs(TreeNode* node, long long lo, long long hi) {
      if (!node) return true;
      if (node->val <= lo || node->val >= hi) return false;
      return validBstDfs(node->left, lo, node->val) &&
             validBstDfs(node->right, node->val, hi);
  }

  bool isValidBST(TreeNode* root) {
      return validBstDfs(root, LLONG_MIN, LLONG_MAX);
  }
  ```
* **Code Explanation**: Each subtree inherits the valid value range imposed by all ancestors.
* **Invariants**: Every node must be strictly inside its inherited `(lo, hi)` interval.
* **Complexity**: Time `O(n)`, space `O(h)` recursion.
* **Optimizations**: Runtime: fail early. Memory: iterative inorder avoids recursion for skewed trees.
* **Edge Cases To Consider**: Duplicate values, `INT_MIN`, `INT_MAX`, violation deep in subtree.
* **L7 Follow-ups**: Clarify duplicate policy and recursion depth limits.

## 2. Lowest Common Ancestor In Binary Tree

* **Pattern / Idea**: Postorder subtree containment.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 47.7)`, `Microsoft: Medium (6m 36.0)`, `Oracle: Medium (all 54.7)`.
* **Question**: Return the lowest common ancestor of two nodes in a binary tree.
* **Test Cases**: [Test cases](./test_cases.md#2-lowest-common-ancestor-in-binary-tree).
* **C++ Code**
  ```cpp
  TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
      if (!root || root == p || root == q) return root;
      TreeNode* left = lowestCommonAncestor(root->left, p, q);
      TreeNode* right = lowestCommonAncestor(root->right, p, q);
      if (left && right) return root;
      return left ? left : right;
  }
  ```
* **Code Explanation**: If both children contain one target each, the current node is the split point.
* **Invariants**: Return value is null, one found target, or the LCA for targets found in that subtree.
* **Complexity**: Time `O(n)`, space `O(h)`.
* **Optimizations**: Runtime: BST version can use ordering for `O(h)`. Memory: parent pointers avoid recursion if available.
* **Edge Cases To Consider**: One target ancestor of other, missing target contract, same node, skewed tree.
* **L7 Follow-ups**: Decide behavior when either target may be absent.

## 3. Serialize And Deserialize Binary Tree

* **Pattern / Idea**: Preorder traversal with null markers.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 54.0)`, `NVIDIA: Medium (all 47.7)`.
* **Question**: Serialize and deserialize a binary tree.
* **Test Cases**: [Test cases](./test_cases.md#3-serialize-and-deserialize-binary-tree).
* **C++ Code**
  ```cpp
  void serializeDfs(TreeNode* node, vector<string>& out) {
      if (!node) {
          out.push_back("#");
          return;
      }
      out.push_back(to_string(node->val));
      serializeDfs(node->left, out);
      serializeDfs(node->right, out);
  }

  string serialize(TreeNode* root) {
      vector<string> parts;
      serializeDfs(root, parts);
      string s;
      for (const string& p : parts) {
          if (!s.empty()) s += ',';
          s += p;
      }
      return s;
  }

  TreeNode* deserializeDfs(const vector<string>& parts, int& i) {
      if (parts[i] == "#") {
          ++i;
          return nullptr;
      }
      TreeNode* node = new TreeNode(stoi(parts[i++]));
      node->left = deserializeDfs(parts, i);
      node->right = deserializeDfs(parts, i);
      return node;
  }

  TreeNode* deserialize(const string& data) {
      vector<string> parts;
      string token;
      stringstream ss(data);
      while (getline(ss, token, ',')) parts.push_back(token);
      int i = 0;
      return deserializeDfs(parts, i);
  }
  ```
* **Code Explanation**: Null markers preserve shape, so preorder is unambiguous.
* **Invariants**: Deserializer consumes exactly one serialized subtree per recursive call.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: parse tokens once. Memory: stream parsing can avoid storing all tokens.
* **Edge Cases To Consider**: Empty tree, negative values, duplicate values, skewed tree.
* **L7 Follow-ups**: Define versioning and compatibility for persisted formats.

## 4. Kth Smallest In BST

* **Pattern / Idea**: Inorder traversal.
* **Company Frequency Tags**: Public signal: `Oracle: Medium (6m 47.0)`, `Google: Medium (all 40.2)`.
* **Question**: Return the kth smallest value in a BST.
* **Test Cases**: [Test cases](./test_cases.md#4-kth-smallest-in-bst).
* **C++ Code**
  ```cpp
  int kthSmallest(TreeNode* root, int k) {
      vector<TreeNode*> st;
      while (root || !st.empty()) {
          while (root) {
              st.push_back(root);
              root = root->left;
          }
          root = st.back();
          st.pop_back();
          if (--k == 0) return root->val;
          root = root->right;
      }
      throw invalid_argument("k is out of range");
  }
  ```
* **Code Explanation**: Inorder traversal visits BST nodes in sorted order and stops at the kth visit.
* **Invariants**: Stack contains the path to the next unvisited smallest node.
* **Complexity**: Time `O(h + k)`, space `O(h)`.
* **Optimizations**: Runtime: augment nodes with subtree sizes for repeated rank queries. Memory: Morris traversal is `O(1)` but mutates temporarily.
* **Edge Cases To Consider**: `k = 1`, `k = n`, invalid `k`, skewed BST.
* **L7 Follow-ups**: Discuss maintaining rank under inserts/deletes.

## 5. Path Sum III

* **Pattern / Idea**: Prefix sums on root-to-current path.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given a binary tree and target sum, count downward paths whose values add to the target.
* **Test Cases**: [Test cases](./test_cases.md#5-path-sum-iii).
* **C++ Code**
  ```cpp
  long long pathSumDfs(TreeNode* node, long long cur, long long target,
                       unordered_map<long long, int>& count) {
      if (!node) return 0;
      cur += node->val;
      long long ans = count[cur - target];
      ++count[cur];
      ans += pathSumDfs(node->left, cur, target, count);
      ans += pathSumDfs(node->right, cur, target, count);
      if (--count[cur] == 0) count.erase(cur);
      return ans;
  }

  long long pathSum(TreeNode* root, long long target) {
      unordered_map<long long, int> count{{0, 1}};
      return pathSumDfs(root, 0, target, count);
  }
  ```
* **Code Explanation**: Two equal-difference prefix sums delimit a downward path with the target sum.
* **Invariants**: `count` contains prefix sums only along the current recursion path.
* **Complexity**: Average time `O(n)`, space `O(h)`.
* **Optimizations**: Runtime: one DFS. Memory: erase zero counts to bound map by depth.
* **Edge Cases To Consider**: Negative values, zero target, duplicate prefix sums, skewed tree.
* **L7 Follow-ups**: For very deep trees, convert to iterative DFS with enter/exit frames.

## 6. Build Tree From Preorder And Inorder

* **Pattern / Idea**: Root partitions inorder range.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Medium (6m 40.2)`, `Snowflake: Medium (all 39.6)`, `Meta: Medium (all 33.6)`, `Apple: Low (all 21.9)`.
* **Question**: Given preorder and inorder traversals with unique values, reconstruct the binary tree.
* **Test Cases**: [Test cases](./test_cases.md#6-build-tree-from-preorder-and-inorder).
* **C++ Code**
  ```cpp
  TreeNode* buildDfs(const vector<int>& pre, int& pi, int lo, int hi,
                     const unordered_map<int, int>& pos) {
      if (lo > hi) return nullptr;
      int rootVal = pre[pi++];
      int mid = pos.at(rootVal);
      TreeNode* root = new TreeNode(rootVal);
      root->left = buildDfs(pre, pi, lo, mid - 1, pos);
      root->right = buildDfs(pre, pi, mid + 1, hi, pos);
      return root;
  }

  TreeNode* buildTree(vector<int> preorder, vector<int> inorder) {
      unordered_map<int, int> pos;
      for (int i = 0; i < static_cast<int>(inorder.size()); ++i) pos[inorder[i]] = i;
      int pi = 0;
      return buildDfs(preorder, pi, 0, static_cast<int>(inorder.size()) - 1, pos);
  }
  ```
* **Code Explanation**: Preorder gives root order; inorder gives left/right subtree boundaries.
* **Invariants**: `pi` points to the next root for the current inorder range.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: value-to-index map avoids repeated scans. Memory: pass index ranges, not sliced vectors.
* **Edge Cases To Consider**: Empty arrays, one node, skewed tree, duplicate values invalid.
* **L7 Follow-ups**: Define behavior when values are not unique.

## 7. Trie Insert Search Prefix

* **Pattern / Idea**: Prefix tree with terminal markers.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 58.9)`, `Snowflake: Medium (6m 49.9)`, `Oracle: Medium (6m 47.0)`, `NVIDIA: Medium (all 47.7)`.
* **Question**: Implement a trie with insert, search, and prefix search.
* **Test Cases**: [Test cases](./test_cases.md#7-trie-insert-search-prefix).
* **C++ Code**
  ```cpp
  class Trie {
      struct Node {
          array<int, 26> next;
          bool end = false;
          Node() { next.fill(-1); }
      };
      vector<Node> nodes{Node()};
  public:
      void insert(const string& word) {
          int cur = 0;
          for (char c : word) {
              int x = c - 'a';
              if (nodes[cur].next[x] == -1) {
                  nodes[cur].next[x] = static_cast<int>(nodes.size());
                  nodes.push_back(Node());
              }
              cur = nodes[cur].next[x];
          }
          nodes[cur].end = true;
      }
      bool search(const string& word) const {
          int cur = 0;
          for (char c : word) {
              int x = c - 'a';
              if (nodes[cur].next[x] == -1) return false;
              cur = nodes[cur].next[x];
          }
          return nodes[cur].end;
      }
      bool startsWith(const string& prefix) const {
          int cur = 0;
          for (char c : prefix) {
              int x = c - 'a';
              if (nodes[cur].next[x] == -1) return false;
              cur = nodes[cur].next[x];
          }
          return true;
      }
  };
  ```
* **Code Explanation**: Each node represents a prefix, and terminal flags distinguish whole words from prefixes.
* **Invariants**: Following characters from root reaches the node for that prefix if it exists.
* **Complexity**: Time `O(length)` per operation, space `O(total chars * alphabet)` worst case.
* **Optimizations**: Runtime: array children are fast for dense lowercase alphabets. Memory: sparse maps or compressed tries save space.
* **Edge Cases To Consider**: Empty string policy, prefix that is not a word, duplicate insert.
* **L7 Follow-ups**: Add delete with reference counts to safely prune nodes.

## 8. Word Search II

* **Pattern / Idea**: Trie-guided DFS with visited cells.
* **Company Frequency Tags**: Public signal: `Snowflake: High (6m 95.8)`, `Meta: Low (6m 15.8)`, `Google: Low (6m 10.5)`.
* **Question**: Given a board and dictionary, return all dictionary words that can be formed by adjacent cells.
* **Test Cases**: [Test cases](./test_cases.md#8-word-search-ii).
* **C++ Code**
  ```cpp
  struct WordNode {
      array<WordNode*, 26> next{};
      string word;
  };

  void addWord(WordNode* root, const string& w) {
      for (char c : w) {
          int i = c - 'a';
          if (!root->next[i]) root->next[i] = new WordNode();
          root = root->next[i];
      }
      root->word = w;
  }

  void searchBoard(vector<vector<char>>& board, int r, int c, WordNode* node,
                   vector<string>& ans) {
      char ch = board[r][c];
      if (ch == '#') return;
      WordNode* next = node->next[ch - 'a'];
      if (!next) return;
      if (!next->word.empty()) {
          ans.push_back(next->word);
          next->word.clear();
      }
      board[r][c] = '#';
      static int dr[4] = {1, -1, 0, 0};
      static int dc[4] = {0, 0, 1, -1};
      for (int k = 0; k < 4; ++k) {
          int nr = r + dr[k], nc = c + dc[k];
          if (nr >= 0 && nr < static_cast<int>(board.size()) &&
              nc >= 0 && nc < static_cast<int>(board[0].size())) {
              searchBoard(board, nr, nc, next, ans);
          }
      }
      board[r][c] = ch;
  }

  vector<string> findWords(vector<vector<char>> board, vector<string> words) {
      WordNode root;
      for (const string& w : words) addWord(&root, w);
      vector<string> ans;
      for (int r = 0; r < static_cast<int>(board.size()); ++r)
          for (int c = 0; c < static_cast<int>(board[0].size()); ++c)
              searchBoard(board, r, c, &root, ans);
      return ans;
  }
  ```
* **Code Explanation**: DFS stops immediately when the current board path is not a dictionary prefix.
* **Invariants**: Marked cells are exactly the current path and cannot be reused.
* **Complexity**: Worst-case exponential per start, pruned by trie; space `O(dictionary chars + word length)`.
* **Optimizations**: Runtime: trie pruning and clearing found words prevent duplicate output. Memory: store word at terminal node instead of reconstructing path.
* **Edge Cases To Consider**: Empty board, duplicate words, repeated letters, word requiring cell reuse.
* **L7 Follow-ups**: Discuss memory ownership of trie nodes in production C++.

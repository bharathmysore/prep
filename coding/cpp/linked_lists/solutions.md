# Linked Lists Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

```cpp
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v = 0, ListNode* n = nullptr) : val(v), next(n) {}
};
```

## 1. Reverse Linked List

* **Pattern / Idea**: Iterative pointer reversal.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (6m 59.9)`, `Apple: Medium (6m 58.9)`, `Oracle: Medium (6m 56.9)`, `Google: Medium (6m 47.5)`, `Meta: Medium (6m 32.3)`, `NVIDIA: Medium (all 47.7)`.
* **Question**: Reverse a singly linked list.
* **Test Cases**: [Test cases](./test_cases.md#1-reverse-linked-list).
* **C++ Code**
  ```cpp
  ListNode* reverseList(ListNode* head) {
      ListNode* prev = nullptr;
      while (head) {
          ListNode* next = head->next;
          head->next = prev;
          prev = head;
          head = next;
      }
      return prev;
  }
  ```
* **Code Explanation**: Move one node at a time from the unreversed suffix to the front of the reversed prefix.
* **Invariants**: `prev` is a fully reversed prefix; `head` points to the remaining suffix.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: one pass. Memory: iterative version avoids recursion stack.
* **Edge Cases To Consider**: Empty, one node, two nodes, long list.
* **L7 Follow-ups**: Discuss ownership if nodes are `unique_ptr` rather than raw interview pointers.

## 2. Reverse Nodes In K-Group

* **Pattern / Idea**: Check group length, reverse group, reconnect.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Medium (6m 50.3)`, `Microsoft: Medium (6m 44.1)`, `Google: Medium (6m 42.5)`, `Snowflake: High (all 64.2)`, `Meta: Medium (all 36.2)`, `Apple: Medium (all 34.5)`.
* **Question**: Reverse nodes in groups of `k`.
* **Test Cases**: [Test cases](./test_cases.md#2-reverse-nodes-in-k-group).
* **C++ Code**
  ```cpp
  ListNode* reverseKGroup(ListNode* head, int k) {
      ListNode dummy(0, head);
      ListNode* groupPrev = &dummy;
      while (true) {
          ListNode* kth = groupPrev;
          for (int i = 0; i < k && kth; ++i) kth = kth->next;
          if (!kth) break;
          ListNode* groupNext = kth->next;

          ListNode* prev = groupNext;
          ListNode* cur = groupPrev->next;
          while (cur != groupNext) {
              ListNode* next = cur->next;
              cur->next = prev;
              prev = cur;
              cur = next;
          }
          ListNode* oldHead = groupPrev->next;
          groupPrev->next = kth;
          groupPrev = oldHead;
      }
      return dummy.next;
  }
  ```
* **Code Explanation**: Only complete groups are reversed. `groupNext` anchors the tail connection during reversal.
* **Invariants**: Nodes before `groupPrev` are already finalized; nodes from current group are reversed onto `groupNext`.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: each node rewired once. Memory: no auxiliary list.
* **Edge Cases To Consider**: `k = 1`, length multiple of `k`, leftover nodes, empty list.
* **L7 Follow-ups**: Explain why no node becomes unreachable during pointer rewiring.

## 3. Merge Two Sorted Lists

* **Pattern / Idea**: Two-pointer merge.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: High (6m 60.5)`, `Snowflake: Medium (6m 59.1)`, `Google: Medium (6m 58.6)`, `Microsoft: Medium (6m 58.2)`, `Meta: Medium (6m 46.0)`, `Apple: Medium (6m 38.9)`, `Oracle: High (all 61.6)`.
* **Question**: Merge two sorted linked lists.
* **Test Cases**: [Test cases](./test_cases.md#3-merge-two-sorted-lists).
* **C++ Code**
  ```cpp
  ListNode* mergeTwoLists(ListNode* a, ListNode* b) {
      ListNode dummy;
      ListNode* tail = &dummy;
      while (a && b) {
          if (a->val <= b->val) {
              tail->next = a;
              a = a->next;
          } else {
              tail->next = b;
              b = b->next;
          }
          tail = tail->next;
      }
      tail->next = a ? a : b;
      return dummy.next;
  }
  ```
* **Code Explanation**: Repeatedly append the smaller current node, preserving sorted order.
* **Invariants**: `dummy.next..tail` is sorted and contains the smallest consumed nodes.
* **Complexity**: Time `O(n + m)`, auxiliary space `O(1)`.
* **Optimizations**: Runtime: no allocation. Memory: relink existing nodes.
* **Edge Cases To Consider**: One empty list, duplicates, negative values, unequal lengths.
* **L7 Follow-ups**: Discuss stable merge behavior when values tie.

## 4. Merge K Sorted Lists

* **Pattern / Idea**: Min-heap of current list heads.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 77.9)`, `Amazon/AWS: High (6m 77.3)`, `Meta: High (6m 74.7)`, `NVIDIA: High (6m 66.0)`, `Microsoft: Medium (6m 56.5)`, `Snowflake: Medium (6m 49.9)`, `Apple: Medium (6m 47.7)`, `Google: Medium (6m 44.7)`.
* **Question**: Merge `k` sorted linked lists.
* **Test Cases**: [Test cases](./test_cases.md#4-merge-k-sorted-lists).
* **C++ Code**
  ```cpp
  ListNode* mergeKLists(vector<ListNode*> lists) {
      auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
      priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);
      for (ListNode* node : lists) if (node) pq.push(node);

      ListNode dummy;
      ListNode* tail = &dummy;
      while (!pq.empty()) {
          ListNode* node = pq.top();
          pq.pop();
          if (node->next) pq.push(node->next);
          tail->next = node;
          tail = tail->next;
      }
      tail->next = nullptr;
      return dummy.next;
  }
  ```
* **Code Explanation**: The heap exposes the smallest available head across all lists.
* **Invariants**: Heap contains at most one current candidate from each list.
* **Complexity**: Time `O(N log k)`, space `O(k)`.
* **Optimizations**: Runtime: divide-and-conquer merge has same asymptotic complexity and often better constants. Memory: relink nodes.
* **Edge Cases To Consider**: Empty vector, all empty lists, one list, many duplicate values.
* **L7 Follow-ups**: For external streams, heap size bounds memory to active streams.

## 5. Detect Cycle Entry

* **Pattern / Idea**: Floyd slow/fast pointers.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: Low (6m 24.7)`.
* **Question**: Detect whether a linked list has a cycle and return the cycle entry if it exists.
* **Test Cases**: [Test cases](./test_cases.md#5-detect-cycle-entry).
* **C++ Code**
  ```cpp
  ListNode* cycleEntry(ListNode* head) {
      ListNode *slow = head, *fast = head;
      while (fast && fast->next) {
          slow = slow->next;
          fast = fast->next->next;
          if (slow == fast) {
              ListNode* p = head;
              while (p != slow) {
                  p = p->next;
                  slow = slow->next;
              }
              return p;
          }
      }
      return nullptr;
  }
  ```
* **Code Explanation**: After meeting inside the cycle, moving one pointer from head and one from meeting point converges at the entry.
* **Invariants**: Fast advances twice as quickly; after reset, both pointers have equal distance to cycle entry.
* **Complexity**: Time `O(n)`, space `O(1)`.
* **Optimizations**: Runtime: no hash lookups. Memory: avoids visited set.
* **Edge Cases To Consider**: No cycle, cycle at head, single-node cycle, long tail.
* **L7 Follow-ups**: Explain proof using tail length and cycle length modulo arithmetic.

## 6. Copy List With Random Pointer

* **Pattern / Idea**: Interleave clones to avoid a map.
* **Company Frequency Tags**: Public signal: `Meta: High (6m 78.8)`, `Amazon/AWS: High (6m 71.8)`, `Snowflake: High (6m 65.6)`, `Microsoft: Medium (6m 30.3)`, `Google: Low (6m 27.0)`, `NVIDIA: High (all 63.8)`, `Oracle: Medium (all 44.5)`.
* **Question**: Deep-copy a linked list where each node has a `random` pointer.
* **Test Cases**: [Test cases](./test_cases.md#6-copy-list-with-random-pointer).
* **C++ Code**
  ```cpp
  struct RandomNode {
      int val;
      RandomNode* next;
      RandomNode* random;
      RandomNode(int v) : val(v), next(nullptr), random(nullptr) {}
  };

  RandomNode* copyRandomList(RandomNode* head) {
      for (RandomNode* cur = head; cur; cur = cur->next->next) {
          auto* clone = new RandomNode(cur->val);
          clone->next = cur->next;
          cur->next = clone;
      }
      for (RandomNode* cur = head; cur; cur = cur->next->next) {
          if (cur->random) cur->next->random = cur->random->next;
      }
      RandomNode dummy(0);
      RandomNode* tail = &dummy;
      for (RandomNode* cur = head; cur;) {
          RandomNode* clone = cur->next;
          cur->next = clone->next;
          cur = cur->next;
          tail->next = clone;
          tail = clone;
      }
      return dummy.next;
  }
  ```
* **Code Explanation**: Each original node's clone is temporarily stored immediately after it, making `random` clone lookup `original->random->next`.
* **Invariants**: During the second pass, every original node is followed by its clone.
* **Complexity**: Time `O(n)`, auxiliary space `O(1)` excluding cloned nodes.
* **Optimizations**: Runtime: three linear passes. Memory: avoids hash map but temporarily mutates input.
* **Edge Cases To Consider**: Null randoms, self-random, random to previous/next, empty list.
* **L7 Follow-ups**: If input cannot be mutated, use an `unordered_map<RandomNode*, RandomNode*>`.

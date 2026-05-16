# Linked Lists Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Reverse Linked List

* **Question**: Reverse a singly linked list.
* **Solution**: [Reverse Linked List](./solutions.md#1-reverse-linked-list).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Three nodes | `1->2->3` | Return `3->2->1`. |
| Single node | `1` | Return `1`. |
| Empty | `null` | Return `null`. |

## 2. Reverse Nodes In K-Group

* **Question**: Reverse nodes in groups of `k`.
* **Solution**: [Reverse Nodes In K-Group](./solutions.md#2-reverse-nodes-in-k-group).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `1->2->3->4->5`, k=2 | Return `2->1->4->3->5`. |
| Incomplete tail | same list, k=3 | Return `3->2->1->4->5`. |
| k=1 | any list | Unchanged. |

## 3. Merge Two Sorted Lists

* **Question**: Merge two sorted linked lists.
* **Solution**: [Merge Two Sorted Lists](./solutions.md#3-merge-two-sorted-lists).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `1->2->4` and `1->3->4` | Return `1->1->2->3->4->4`. |
| One empty | one list null | Return the other list. |
| Duplicates | lists with equal values | All duplicates preserved. |

## 4. Merge K Sorted Lists

* **Question**: Merge `k` sorted linked lists.
* **Solution**: [Merge K Sorted Lists](./solutions.md#4-merge-k-sorted-lists).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | lists `[1,4,5]`, `[1,3,4]`, `[2,6]` | Return merged sorted list. |
| All empty | `[]` or only null lists | Return null. |
| One list | single list | Return it unchanged. |

## 5. Detect Cycle Entry

* **Question**: Detect whether a linked list has a cycle and return the cycle entry if it exists.
* **Solution**: [Detect Cycle Entry](./solutions.md#5-detect-cycle-entry).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Cycle at middle | tail points to node with value 2 | Return that node. |
| No cycle | linear list | Return null. |
| Cycle at head | tail points to head | Return head. |

## 6. Copy List With Random Pointer

* **Question**: Deep-copy a linked list where each node has a `random` pointer.
* **Solution**: [Copy List With Random Pointer](./solutions.md#6-copy-list-with-random-pointer).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Mixed randoms | A->B->C with randoms C,A,null | Deep copy has same value/random topology, no original nodes. |
| Self random | node random points to itself | Copied node random points to copied node. |
| Empty | null | Return null. |

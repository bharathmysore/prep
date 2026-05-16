# Linked Lists Coding Questions

Solve each question in C++ and be precise about pointer invariants and ownership assumptions.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Reverse a singly linked list.
   - Expected pattern: iterative pointer rewiring.
   - Pattern tags: `pointer-reversal`.
   - Solution: [Reverse Linked List](./solutions.md#1-reverse-linked-list).
   - Complexity target: time `O(n)`, space `O(1)`.

2. Reverse nodes in groups of `k`.
   - Expected pattern: group boundary check plus in-place reversal.
   - Pattern tags: `group-reversal`, `pointer-reversal`.
   - Solution: [Reverse Nodes In K-Group](./solutions.md#2-reverse-nodes-in-k-group).
   - Complexity target: time `O(n)`, space `O(1)`.

3. Merge two sorted linked lists.
   - Expected pattern: two-pointer merge with dummy head.
   - Pattern tags: `two-pointer-merge`.
   - Solution: [Merge Two Sorted Lists](./solutions.md#3-merge-two-sorted-lists).
   - Complexity target: time `O(n + m)`, auxiliary space `O(1)`.

4. Merge `k` sorted linked lists.
   - Expected pattern: min-heap over current list heads.
   - Pattern tags: `heap`, `k-way-merge`.
   - Solution: [Merge K Sorted Lists](./solutions.md#4-merge-k-sorted-lists).
   - Complexity target: time `O(N log k)`, space `O(k)`.

5. Detect whether a linked list has a cycle and return the cycle entry if it exists.
   - Expected pattern: Floyd slow/fast pointers.
   - Pattern tags: `floyd-cycle-detection`.
   - Solution: [Detect Cycle Entry](./solutions.md#5-detect-cycle-entry).
   - Complexity target: time `O(n)`, space `O(1)`.

6. Deep-copy a linked list where each node has a `random` pointer.
   - Expected pattern: hash map or interleaved clone nodes.
   - Pattern tags: `deep-copy`, `random-pointer`, `interleaving`.
   - Solution: [Copy List With Random Pointer](./solutions.md#6-copy-list-with-random-pointer).
   - Complexity target: time `O(n)`, space `O(n)` with map or auxiliary `O(1)` with interleaving.

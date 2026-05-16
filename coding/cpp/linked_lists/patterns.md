# Linked Lists Coding Patterns

Linked-list problems test pointer discipline, ownership clarity, and edge-case handling. In C++, use raw pointers only when the interview signature gives them; otherwise prefer clear ownership types.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Reverse linked list | Need reverse all next pointers | `prev` is reversed prefix, `curr` starts unreversed suffix | Time `O(n)`, space `O(1)` | Runtime: iterative pointer rewiring. Memory: avoid recursion stack. |
| Reverse nodes in k-group | Reverse fixed-size chunks | Completed groups are reversed and connected to remaining list | Time `O(n)`, space `O(1)` | Runtime: precheck group length. Memory: reverse in place. |
| Merge two sorted lists | Two sorted streams | Output tail is sorted and contains smallest consumed nodes | Time `O(n + m)`, space `O(1)` auxiliary | Runtime: dummy head simplifies branches. Memory: relink nodes. |
| Merge k sorted lists | Many sorted streams | Heap contains current smallest head from each list | Time `O(N log k)`, space `O(k)` | Runtime: heap over list heads. Memory: relink existing nodes. |
| Detect cycle | Need find loop in pointer chain | Fast pointer moves twice as fast as slow pointer | Time `O(n)`, space `O(1)` | Runtime: Floyd cycle detection. Memory: avoid visited set unless easier to justify. |
| Find cycle start | Need entry node after detecting cycle | Reset pointer aligns distances to entry | Time `O(n)`, space `O(1)` | Runtime: two-phase Floyd. Memory: no extra state. |
| Intersection of two lists | Shared tail by pointer identity | Equalized distances mean simultaneous traversal meets at intersection | Time `O(n + m)`, space `O(1)` | Runtime: pointer switching avoids length pass. Memory: no set needed. |
| Copy list with random pointer | Deep copy with cross links | Each original maps to exactly one clone | Time `O(n)`, space `O(n)` or `O(1)` auxiliary | Runtime: interleave clone nodes. Memory: interleaving avoids map but mutates temporarily. |
| Reorder list | First, last, second, second-last order | Second half reversed, then merged alternately | Time `O(n)`, space `O(1)` | Runtime: split with slow/fast. Memory: in-place reverse. |
| Sort linked list | Need `O(n log n)` stable-ish sort | Each merge combines two sorted sublists | Time `O(n log n)`, space `O(log n)` recursive or `O(1)` iterative | Runtime: bottom-up merge avoids recursion. Memory: iterative merge sort. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `pointer-reversal` | iterative pointer rewiring | [questions](./questions.md): Q1 |
| `group-reversal`, `pointer-reversal` | group boundary check plus in-place reversal | [questions](./questions.md): Q2 |
| `two-pointer-merge` | two-pointer merge with dummy head | [questions](./questions.md): Q3 |
| `heap`, `k-way-merge` | min-heap over current list heads | [questions](./questions.md): Q4 |
| `floyd-cycle-detection` | Floyd slow/fast pointers | [questions](./questions.md): Q5 |
| `deep-copy`, `random-pointer`, `interleaving` | hash map or interleaved clone nodes | [questions](./questions.md): Q6 |

## L7 Follow-Ups

- How do you prove pointer rewiring never loses the rest of the list?
- What changes if nodes are owned by `unique_ptr`?
- How do you test one-node, two-node, cycle, and shared-tail cases?
- Which linked-list problems are better solved by changing the data structure?

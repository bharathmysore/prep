# Stacks And Queues Coding Patterns

Stacks encode unresolved prior work. Queues encode breadth-first or arrival-order processing. For L7 prep, be ready to explain amortized behavior and bounded-memory variants.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Valid parentheses | Nested open/close structure | Stack contains unmatched open tokens | Time `O(n)`, space `O(n)` | Runtime: direct matching table. Memory: early return on impossible length. |
| Min stack | Need push/pop/top/min all fast | Auxiliary state tracks minimum for each stack depth | Time `O(1)` per op, space `O(n)` | Runtime: store min with each value. Memory: compressed min stack stores only changes. |
| Next greater element | Need nearest future greater value | Monotonic stack holds unresolved decreasing candidates | Time `O(n)`, space `O(n)` | Runtime: pop each element once. Memory: store indices, not pairs, when input array is available. |
| Largest rectangle in histogram | Need max area over contiguous bars | Stack has increasing bar heights with unresolved right boundary | Time `O(n)`, space `O(n)` | Runtime: sentinel height flushes stack. Memory: reuse input indices. |
| Sliding window maximum | Max over moving fixed window | Deque stores indices in decreasing value order | Time `O(n)`, space `O(k)` | Runtime: each index enters and leaves once. Memory: store indices only. |
| Basic calculator | Expression with precedence or parentheses | Operator stack preserves deferred operations | Time `O(n)`, space `O(n)` | Runtime: parse numbers in one scan. Memory: collapse operations eagerly when precedence allows. |
| BFS level order traversal | Shortest unweighted layers | Queue contains exactly current frontier then next frontier | Time `O(V + E)`, space `O(width)` | Runtime: process by level size. Memory: avoid storing all levels if streaming output is fine. |
| Queue using two stacks | FIFO using LIFO primitives | Output stack has oldest elements when non-empty | Amortized time `O(1)`, space `O(n)` | Runtime: transfer only when output stack is empty. Memory: no duplicate storage after transfer. |
| Circular buffer | Fixed-capacity FIFO | Size, head, and tail agree on occupied slots | Time `O(1)` per op, space `O(capacity)` | Runtime: modulo or branch wrap. Memory: fixed array avoids allocation churn. |
| Producer-consumer queue model | Multiple producers and consumers | Items enqueued before shutdown are eventually consumed | Time `O(1)` per enqueue/dequeue under lock, space `O(capacity)` | Runtime: notify one vs all carefully. Memory: bounded queue enforces backpressure. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `stack`, `matching` | stack of unmatched opens | [questions](./questions.md): Q1 |
| `auxiliary-stack`, `min-tracking` | value stack plus min stack or encoded minimum | [questions](./questions.md): Q2 |
| `monotonic-stack` | monotonic stack | [questions](./questions.md): Q3 |
| `monotonic-stack`, `histogram` | monotonic increasing stack | [questions](./questions.md): Q4 |
| `monotonic-deque`, `sliding-window` | monotonic deque | [questions](./questions.md): Q5 |
| `two-stacks`, `amortized-queue` | input stack plus output stack | [questions](./questions.md): Q6 |

## L7 Follow-Ups

- Which stack or queue operations are amortized rather than worst-case?
- How would you make the queue bounded and thread-safe in C++?
- What invariants prevent lost wakeups with condition variables?
- Where do monotonic queues fail if the window is not fixed size?

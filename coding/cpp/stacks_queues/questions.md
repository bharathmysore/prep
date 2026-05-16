# Stacks And Queues Coding Questions

Solve each question in C++ and explain amortized behavior where it applies.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Validate whether a string of brackets is balanced.
   - Expected pattern: stack of unmatched opens.
   - Pattern tags: `stack`, `matching`.
   - Solution: [Valid Parentheses](./solutions.md#1-valid-parentheses).
   - Complexity target: time `O(n)`, space `O(n)`.

<a id="2-min-stack"></a>
2. Implement a stack that supports `push`, `pop`, `top`, and `getMin`.
   - Expected pattern: value stack plus min stack or encoded minimum.
   - Pattern tags: `auxiliary-stack`, `min-tracking`.
   - Solution: [Min Stack](./solutions.md#2-min-stack).
   - Complexity target: time `O(1)` per operation, space `O(n)`.

3. Given an array, return the next greater element for every index.
   - Expected pattern: monotonic stack.
   - Pattern tags: `monotonic-stack`.
   - Solution: [Next Greater Element](./solutions.md#3-next-greater-element).
   - Complexity target: time `O(n)`, space `O(n)`.

4. Given histogram bar heights, return the largest rectangle area.
   - Expected pattern: monotonic increasing stack.
   - Pattern tags: `monotonic-stack`, `histogram`.
   - Solution: [Largest Rectangle In Histogram](./solutions.md#4-largest-rectangle-in-histogram).
   - Complexity target: time `O(n)`, space `O(n)`.

5. Given an array and window size `k`, return the maximum value in every sliding window.
   - Expected pattern: monotonic deque.
   - Pattern tags: `monotonic-deque`, `sliding-window`.
   - Solution: [Sliding Window Maximum](./solutions.md#5-sliding-window-maximum).
   - Complexity target: time `O(n)`, space `O(k)`.

6. Implement a queue using two stacks and explain amortized cost.
   - Expected pattern: input stack plus output stack.
   - Pattern tags: `two-stacks`, `amortized-queue`.
   - Solution: [Queue Using Two Stacks](./solutions.md#6-queue-using-two-stacks).
   - Complexity target: amortized time `O(1)` per operation, space `O(n)`.

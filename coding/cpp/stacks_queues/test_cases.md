# Stacks Queues Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Valid Parentheses

* **Question**: Validate whether a string of brackets is balanced.
* **Solution**: [Valid Parentheses](./solutions.md#1-valid-parentheses).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Balanced | `()[]{}` | Return true. |
| Wrong order | `(]` | Return false. |
| Unclosed | `((` | Return false. |

## 2. Min Stack

* **Question**: Implement a stack that supports `push`, `pop`, `top`, and `getMin`.
* **Solution**: [Min Stack](./solutions.md#2-min-stack).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Min changes | push -2,0,-3; getMin; pop; top; getMin | Returns -3, then 0, then -2. |
| Duplicate min | push 1,1; pop one | Min remains 1. |
| Empty policy | top/getMin on empty | Reject or return sentinel per API contract. |

## 3. Next Greater Element

* **Question**: Given an array, return the next greater element for every index.
* **Solution**: [Next Greater Element](./solutions.md#3-next-greater-element).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[2,1,2,4,3]` | Return `[4,2,4,-1,-1]` for next greater to right. |
| Descending | `[5,4,3]` | Return all `-1`. |
| Duplicates | `[2,2,3]` | Return `[3,3,-1]`. |

## 4. Largest Rectangle In Histogram

* **Question**: Given histogram bar heights, return the largest rectangle area.
* **Solution**: [Largest Rectangle In Histogram](./solutions.md#4-largest-rectangle-in-histogram).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[2,1,5,6,2,3]` | Return `10`. |
| Increasing | `[1,2,3]` | Return `4`. |
| Empty | `[]` | Return `0`. |

## 5. Sliding Window Maximum

* **Question**: Given an array and window size `k`, return the maximum value in every sliding window.
* **Solution**: [Sliding Window Maximum](./solutions.md#5-sliding-window-maximum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | nums `[1,3,-1,-3,5,3,6,7]`, k=3 | Return `[3,3,5,5,6,7]`. |
| k=1 | any array, `k=1` | Return original array. |
| All equal | `[2,2,2]`, k=2 | Return `[2,2]`. |

## 6. Queue Using Two Stacks

* **Question**: Implement a queue using two stacks and explain amortized cost.
* **Solution**: [Queue Using Two Stacks](./solutions.md#6-queue-using-two-stacks).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| FIFO | push 1, push 2, pop | Pop returns 1. |
| Interleaved | push 1, pop, push 2, peek | Peek returns 2. |
| Empty pop | pop empty | Reject or return sentinel per API contract. |

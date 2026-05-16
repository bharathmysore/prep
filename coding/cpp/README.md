# C++ L7 Coding Prep

Use this directory for C++ coding interview preparation. Each category has:

- `patterns.md`: pattern catalog with recognition signals, invariants, complexity targets, and optimization notes.
- `questions.md`: representative interview-style coding prompts for that category, tagged with mirrored pattern tags.
- `solutions.md`: implemented C++ solutions using the L7 answer format.
- `test_cases.md`: concrete test cases linked from solved entries, kept outside the solution file.
- `company_frequency.md`: company-specific focus index that links target companies back to solved questions.
- `top_20.md`: short-list practice index for top questions across categories and companies.

Pattern tags are bidirectional: every question includes `Pattern tags:`, and every category's `patterns.md` includes a `Pattern Tag Map` that points those tags back to question numbers.
Company frequency tags are also maintained on each solved entry. Public scores come from public company-tag datasets where available; L7 domain-fit tags are separate relevance signals for systems, concurrency, parallel, and distributed-systems questions.

## Reference Context

This set is a curated representative top-100 list, not an official question bank. It is informed by common public prep lists and pattern roadmaps, including [NeetCode 150](https://neetcode.io/practice/practice/neetcode150), [Grind 75 / Blind 75](https://www.techinterviewhandbook.org/grind75/faq), and [LeetCode Top Interview 150](https://leetcode-top-interview-150.github.io/), plus L7-focused additions for concurrency, parallel algorithms, distributed-systems algorithms, and systems-style coding. Company focus tags use public sources such as the [company-wise LeetCode CSV repository](https://github.com/liquidslr/interview-company-wise-problems), [Interview Browser's company page](https://interviewbrowser.com/leetcode-questions), and public guidance on using company tags for targeted practice.

## Company Focus

- [C++ Top 20 Coding Question Focus](./top_20.md)
- [C++ Company Frequency Focus Index](./company_frequency.md)
- [Company And Position Specific Prep](../../company_positions/README.md)

Use the top-20 focus index when time is short. Use the company focus index after a category pass: pick a target company, review the `High` public signals first, then fill gaps with `Medium` public signals and L7 domain-fit questions.

## Focused Areas

- [Rate Limiters](../../focused_areas/rate_limiters.md)

## Categories

| Category | Patterns | Questions | Solutions | Test Cases |
| --- | --- | --- | --- | --- |
| Advanced data structures | [patterns](./advanced_data_structures/patterns.md) | [questions](./advanced_data_structures/questions.md) | [solutions](./advanced_data_structures/solutions.md) | [test cases](./advanced_data_structures/test_cases.md) |
| Arrays and strings | [patterns](./arrays_strings/patterns.md) | [questions](./arrays_strings/questions.md) | [solutions](./arrays_strings/solutions.md) | [test cases](./arrays_strings/test_cases.md) |
| Backtracking | [patterns](./backtracking/patterns.md) | [questions](./backtracking/questions.md) | [solutions](./backtracking/solutions.md) | [test cases](./backtracking/test_cases.md) |
| Binary search | [patterns](./binary_search/patterns.md) | [questions](./binary_search/questions.md) | [solutions](./binary_search/solutions.md) | [test cases](./binary_search/test_cases.md) |
| Concurrency | [patterns](./concurrency/patterns.md) | [questions](./concurrency/questions.md) | [solutions](./concurrency/solutions.md) | [test cases](./concurrency/test_cases.md) |
| Distributed systems algorithms | [patterns](./distributed_systems_algorithms/patterns.md) | [questions](./distributed_systems_algorithms/questions.md) | [solutions](./distributed_systems_algorithms/solutions.md) | [test cases](./distributed_systems_algorithms/test_cases.md) |
| Dynamic programming | [patterns](./dynamic_programming/patterns.md) | [questions](./dynamic_programming/questions.md) | [solutions](./dynamic_programming/solutions.md) | [test cases](./dynamic_programming/test_cases.md) |
| Graphs | [patterns](./graphs/patterns.md) | [questions](./graphs/questions.md) | [solutions](./graphs/solutions.md) | [test cases](./graphs/test_cases.md) |
| Hashing | [patterns](./hashing/patterns.md) | [questions](./hashing/questions.md) | [solutions](./hashing/solutions.md) | [test cases](./hashing/test_cases.md) |
| Heaps and ordered structures | [patterns](./heaps_ordered_structures/patterns.md) | [questions](./heaps_ordered_structures/questions.md) | [solutions](./heaps_ordered_structures/solutions.md) | [test cases](./heaps_ordered_structures/test_cases.md) |
| Linked lists | [patterns](./linked_lists/patterns.md) | [questions](./linked_lists/questions.md) | [solutions](./linked_lists/solutions.md) | [test cases](./linked_lists/test_cases.md) |
| Parallel algorithms | [patterns](./parallel_algorithms/patterns.md) | [questions](./parallel_algorithms/questions.md) | [solutions](./parallel_algorithms/solutions.md) | [test cases](./parallel_algorithms/test_cases.md) |
| Stacks and queues | [patterns](./stacks_queues/patterns.md) | [questions](./stacks_queues/questions.md) | [solutions](./stacks_queues/solutions.md) | [test cases](./stacks_queues/test_cases.md) |
| Systems-style coding | [patterns](./systems_style/patterns.md) | [questions](./systems_style/questions.md) | [solutions](./systems_style/solutions.md) | [test cases](./systems_style/test_cases.md) |
| Trees and tries | [patterns](./trees_tries/patterns.md) | [questions](./trees_tries/questions.md) | [solutions](./trees_tries/solutions.md) | [test cases](./trees_tries/test_cases.md) |

## Practice Rule

For every solved question, include a short question explanation, company frequency tags, a link to external test cases, C++ code, explanation, invariants, time and space complexity, runtime optimizations, memory optimizations, edge cases to consider, and L7 follow-ups about scale, concurrency, failure, observability, and tradeoffs.

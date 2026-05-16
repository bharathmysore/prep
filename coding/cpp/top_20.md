# C++ Top 20 Coding Question Focus

Use this file for short-list practice when there is not enough time to sweep every category. It is derived from the current C++ solution catalog and company frequency tags.

## Methodology

- **Top 20 Across Categories** ranks solved questions by strongest public company signal, preferring recent six-month scores over all-time fallback scores.
- **Top 20 Across Tracked Companies** ranks solved questions by breadth across tracked companies, then by high-frequency public signals.
- **Company-Specific Top 20** ranks public company signals first; domain-fit signals fill gaps for L7 systems, concurrency, parallel, and distributed-systems coverage.
- If a company has fewer than 20 currently tagged questions, list every available tagged question rather than inventing weak matches.
- Public scores are preparation signals from public company-tag data, not official interview probabilities. Domain-fit entries are relevance signals, not frequency measurements.

## Top 20 Across Categories

| Rank | Question | Category | Best signal | Why practice it |
| --- | --- | --- | --- | --- |
| 1 | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | 6m max 100.0 (Snowflake 100.0) | Broad cross-company pattern |
| 2 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m max 100.0 (Apple 100.0, Oracle 100.0) | Very broad high-frequency signal |
| 3 | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | 6m max 100.0 (Databricks 100.0) | L7 systems signal |
| 4 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | 6m max 100.0 (Databricks 100.0) | L7 systems signal |
| 5 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | 6m max 100.0 (Databricks 100.0) | L7 systems signal |
| 6 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m max 95.8 (NVIDIA 95.8) | Very broad high-frequency signal |
| 7 | [Word Search II](./trees_tries/solutions.md#8-word-search-ii) | trees tries | 6m max 95.8 (Snowflake 95.8) | Very high recent signal |
| 8 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 6m max 94.3 (Snowflake 94.3) | Broad cross-company pattern |
| 9 | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | 6m max 93.6 (Meta 93.6) | Very high recent signal |
| 10 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | 6m max 93.6 (Meta 93.6) | L7 systems signal |
| 11 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m max 93.3 (Oracle 93.3) | Very broad high-frequency signal |
| 12 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m max 86.8 (Oracle 86.8) | Very broad high-frequency signal |
| 13 | [Sliding Window Maximum](./stacks_queues/solutions.md#5-sliding-window-maximum) | stacks queues | 6m max 84.2 (Oracle 84.2) | Core interview pattern |
| 14 | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6m max 82.4 (Databricks 82.4) | Broad cross-company pattern |
| 15 | [Top K Frequent Words](./heaps_ordered_structures/solutions.md#6-top-k-frequent-words) | heaps ordered structures | 6m max 81.7 (Apple 81.7) | Core interview pattern |
| 16 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 6m max 81.2 (Meta 81.2) | Very broad high-frequency signal |
| 17 | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 6m max 78.8 (Meta 78.8) | Very broad high-frequency signal |
| 18 | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | 6m max 78.3 (Apple 78.3) | Core interview pattern |
| 19 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m max 77.9 (Oracle 77.9) | Very broad high-frequency signal |
| 20 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m max 77.9 (Oracle 77.9) | Very broad high-frequency signal |

## Top 20 Across Tracked Companies

| Rank | Question | Category | Company coverage | Best signal | Why practice it |
| --- | --- | --- | --- | --- | --- |
| 1 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 8: Amazon/AWS, Apple, Databricks, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 81.2 (Meta 81.2) | Very broad high-frequency signal |
| 2 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 8: Amazon/AWS, Apple, Google, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 93.3 (Oracle 93.3) | Very broad high-frequency signal |
| 3 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 8: Amazon/AWS, Apple, Google, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 77.9 (Oracle 77.9) | Very broad high-frequency signal |
| 4 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 8: Amazon/AWS, Apple, Google, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 77.9 (Oracle 77.9) | Very broad high-frequency signal |
| 5 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 7: Amazon/AWS, Apple, Google, Meta, Microsoft, NVIDIA, Oracle | 6m max 86.8 (Oracle 86.8) | Very broad high-frequency signal |
| 6 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 7: Amazon/AWS, Apple, Google, Meta, Microsoft, NVIDIA, Oracle | 6m max 73.6 (Amazon/AWS 73.6) | Very broad high-frequency signal |
| 7 | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 7: Amazon/AWS, Google, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 78.8 (Meta 78.8) | Very broad high-frequency signal |
| 8 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 7: Amazon/AWS, Apple, Google, Meta, Microsoft, NVIDIA, Oracle | 6m max 95.8 (NVIDIA 95.8) | Very broad high-frequency signal |
| 9 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 7: Amazon/AWS, Apple, Google, Meta, Microsoft, Oracle, Snowflake | 6m max 60.5 (Amazon/AWS 60.5) | Broad cross-company pattern |
| 10 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 7: Apple, Databricks, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 94.3 (Snowflake 94.3) | Broad cross-company pattern |
| 11 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6: Apple, Meta, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 100.0 (Apple 100.0, Oracle 100.0) | Very broad high-frequency signal |
| 12 | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | 6: Amazon/AWS, Apple, Google, Meta, Microsoft, Oracle | 6m max 68.5 (Google 68.5) | Very broad high-frequency signal |
| 13 | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | 6: Amazon/AWS, Apple, Meta, Microsoft, Oracle, Snowflake | 6m max 100.0 (Snowflake 100.0) | Broad cross-company pattern |
| 14 | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | 6: Apple, Google, Microsoft, NVIDIA, Oracle, Snowflake | 6m max 69.5 (Oracle 69.5) | Broad cross-company pattern |
| 15 | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6: Amazon/AWS, Apple, Google, Meta, Microsoft, Oracle | 6m max 66.1 (Amazon/AWS 66.1) | Broad cross-company pattern |
| 16 | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | 6: Amazon/AWS, Apple, Google, Meta, Microsoft, Snowflake | 6m max 50.3 (Amazon/AWS 50.3) | Broad cross-company pattern |
| 17 | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6: Apple, Databricks, Google, Meta, Microsoft, NVIDIA | 6m max 82.4 (Databricks 82.4) | Broad cross-company pattern |
| 18 | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | 6: Apple, Google, Meta, Microsoft, NVIDIA, Oracle | 6m max 59.9 (Microsoft 59.9) | Broad cross-company pattern |
| 19 | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | 6: Amazon/AWS, Apple, Meta, Microsoft, Oracle, Snowflake | 6m max 59.9 (Microsoft 59.9) | Broad cross-company pattern |
| 20 | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | 6: Amazon/AWS, Apple, Google, Meta, Microsoft, Oracle | 6m max 56.5 (Microsoft 56.5) | Broad cross-company pattern |

## Company-Specific Top 20

### Amazon/AWS

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public High (6m 82.5) |
| 2 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public High (6m 80.5) |
| 3 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public High (6m 79.9) |
| 4 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public High (6m 77.3) |
| 5 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public High (6m 77.3) |
| 6 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public High (6m 73.6) |
| 7 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | Public High (6m 73.1) |
| 8 | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | Public High (6m 71.8) |
| 9 | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | Public High (6m 66.1) |
| 10 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | Public High (6m 60.5) |
| 11 | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | Public High (all 75.3) |
| 12 | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | Public Medium (6m 56.1) |
| 13 | [Generate Subsets](./backtracking/solutions.md#1-generate-subsets) | backtracking | Public Medium (6m 56.1) |
| 14 | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | Public Medium (6m 55.1) |
| 15 | [Largest Rectangle In Histogram](./stacks_queues/solutions.md#4-largest-rectangle-in-histogram) | stacks queues | Public Medium (6m 52.8) |
| 16 | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | Public Medium (6m 50.3) |
| 17 | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | Public Medium (6m 49.0) |
| 18 | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | Public Medium (6m 49.0) |
| 19 | [Palindrome Partitioning](./backtracking/solutions.md#6-palindrome-partitioning) | backtracking | Public Medium (6m 47.5) |
| 20 | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | Public Medium (6m 45.9) |

### Google

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | Public High (6m 68.5) |
| 2 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public High (6m 65.1) |
| 3 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public High (6m 60.8) |
| 4 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | Public Medium (6m 58.6) |
| 5 | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | Public Medium (6m 58.6) |
| 6 | [Count Subarrays With Sum K](./arrays_strings/solutions.md#4-count-subarrays-with-sum-k) | arrays strings | Public Medium (6m 58.2) |
| 7 | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | Public Medium (6m 52.1) |
| 8 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public Medium (6m 49.2) |
| 9 | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | Public Medium (6m 47.5) |
| 10 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | Public Medium (6m 45.7) |
| 11 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public Medium (6m 44.7) |
| 12 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public Medium (6m 44.7) |
| 13 | [Longest Increasing Subsequence](./dynamic_programming/solutions.md#4-longest-increasing-subsequence) | dynamic programming | Public Medium (6m 43.7) |
| 14 | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | Public Medium (6m 42.5) |
| 15 | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | Public Medium (6m 41.4) |
| 16 | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | Public Medium (6m 41.4) |
| 17 | [First Non-Repeating Character In A Stream](./hashing/solutions.md#1-first-non-repeating-character-in-a-stream) | hashing | Public Medium (6m 37.2) |
| 18 | [Isomorphic Strings](./hashing/solutions.md#6-isomorphic-strings) | hashing | Public Medium (6m 35.6) |
| 19 | [Minimum Path Sum](./dynamic_programming/solutions.md#8-minimum-path-sum) | dynamic programming | Public Medium (6m 31.8) |
| 20 | [Evaluate Division](./graphs/solutions.md#4-evaluate-division) | graphs | Public Medium (6m 31.8) |

### Meta

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | Public High (6m 93.6) |
| 2 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Public High (6m 93.6) |
| 3 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public High (6m 81.2) |
| 4 | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | Public High (6m 78.8) |
| 5 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | Public High (6m 76.0) |
| 6 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public High (6m 74.7) |
| 7 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public High (6m 74.7) |
| 8 | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | Public High (6m 68.9) |
| 9 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public High (6m 66.2) |
| 10 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | Public Medium (6m 59.8) |
| 11 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public Medium (6m 57.0) |
| 12 | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | Public Medium (6m 53.8) |
| 13 | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | Public Medium (6m 49.9) |
| 14 | [Generate Subsets](./backtracking/solutions.md#1-generate-subsets) | backtracking | Public Medium (6m 49.9) |
| 15 | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | Public Medium (6m 48.0) |
| 16 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | Public Medium (6m 46.0) |
| 17 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | Public Medium (6m 44.9) |
| 18 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public Medium (6m 43.7) |
| 19 | [Two Sum In A Sorted Array](./arrays_strings/solutions.md#1-two-sum-in-a-sorted-array) | arrays strings | Public Medium (6m 39.6) |
| 20 | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | Public Medium (6m 39.6) |

### Microsoft

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public High (6m 72.4) |
| 2 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | Public High (6m 69.7) |
| 3 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public High (6m 65.4) |
| 4 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public High (6m 64.1) |
| 5 | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | Public High (6m 64.1) |
| 6 | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | Public High (6m 62.8) |
| 7 | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | Public Medium (6m 59.9) |
| 8 | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | Public Medium (6m 59.9) |
| 9 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | Public Medium (6m 58.2) |
| 10 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public Medium (6m 56.5) |
| 11 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public Medium (6m 56.5) |
| 12 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public Medium (6m 56.5) |
| 13 | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | Public Medium (6m 56.5) |
| 14 | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | Public Medium (6m 56.5) |
| 15 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | Public Medium (6m 54.5) |
| 16 | [Largest Rectangle In Histogram](./stacks_queues/solutions.md#4-largest-rectangle-in-histogram) | stacks queues | Public Medium (6m 54.5) |
| 17 | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | Public Medium (6m 52.4) |
| 18 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Public Medium (6m 52.4) |
| 19 | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | Public Medium (6m 52.4) |
| 20 | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | Public Medium (6m 50.0) |

### Apple

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | Public High (6m 100.0) |
| 2 | [Top K Frequent Words](./heaps_ordered_structures/solutions.md#6-top-k-frequent-words) | heaps ordered structures | Public High (6m 81.7) |
| 3 | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | Public High (6m 78.3) |
| 4 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public High (6m 74.2) |
| 5 | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | Public High (6m 74.2) |
| 6 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public High (6m 71.9) |
| 7 | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | Public High (6m 71.9) |
| 8 | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | Public High (6m 69.3) |
| 9 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Public High (6m 69.3) |
| 10 | [First Non-Repeating Character In A Stream](./hashing/solutions.md#1-first-non-repeating-character-in-a-stream) | hashing | Public High (6m 69.3) |
| 11 | [Count Subarrays With Sum K](./arrays_strings/solutions.md#4-count-subarrays-with-sum-k) | arrays strings | Public High (6m 66.3) |
| 12 | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | Public High (6m 62.9) |
| 13 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Public High (6m 62.9) |
| 14 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | Public High (6m 62.9) |
| 15 | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | Public High (all 75.8) |
| 16 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public Medium (6m 58.9) |
| 17 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | Public Medium (6m 58.9) |
| 18 | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | Public Medium (6m 58.9) |
| 19 | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | Public Medium (6m 58.9) |
| 20 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public Medium (6m 54.0) |

### NVIDIA

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | Public High (6m 95.8) |
| 2 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public High (6m 84.8) |
| 3 | [Product Of Array Except Self](./arrays_strings/solutions.md#6-product-of-array-except-self) | arrays strings | Public High (6m 77.0) |
| 4 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public High (6m 66.0) |
| 5 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public High (6m 66.0) |
| 6 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public High (6m 66.0) |
| 7 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public High (6m 66.0) |
| 8 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | Public High (6m 66.0) |
| 9 | [Count Subarrays With Sum K](./arrays_strings/solutions.md#4-count-subarrays-with-sum-k) | arrays strings | Public High (6m 66.0) |
| 10 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public High (all 67.4) |
| 11 | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | Public High (all 63.8) |
| 12 | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | Public High (all 63.8) |
| 13 | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | Public Medium (all 59.6) |
| 14 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Public Medium (all 59.6) |
| 15 | [Randomized Set](./hashing/solutions.md#4-randomized-set) | hashing | Public Medium (all 59.6) |
| 16 | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | Public Medium (all 54.4) |
| 17 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | Public Medium (all 47.7) |
| 18 | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | Public Medium (all 47.7) |
| 19 | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | Public Medium (all 47.7) |
| 20 | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | Public Medium (all 47.7) |

### Oracle

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | Public High (6m 100.0) |
| 2 | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | Public High (6m 93.3) |
| 3 | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | Public High (6m 86.8) |
| 4 | [Sliding Window Maximum](./stacks_queues/solutions.md#5-sliding-window-maximum) | stacks queues | Public High (6m 84.2) |
| 5 | [Top K Frequent Words](./heaps_ordered_structures/solutions.md#6-top-k-frequent-words) | heaps ordered structures | Public High (6m 81.2) |
| 6 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public High (6m 77.9) |
| 7 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public High (6m 77.9) |
| 8 | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | Public High (6m 69.5) |
| 9 | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | Public High (6m 69.5) |
| 10 | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | Public High (6m 69.5) |
| 11 | [LFU Cache](./advanced_data_structures/solutions.md#2-lfu-cache) | advanced data structures | Public High (6m 69.5) |
| 12 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public High (6m 64.0) |
| 13 | [Count Palindromic Substrings](./dynamic_programming/solutions.md#10-count-palindromic-substrings) | dynamic programming | Public High (6m 64.0) |
| 14 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | Public High (all 61.6) |
| 15 | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | Public Medium (6m 56.9) |
| 16 | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | Public Medium (6m 56.9) |
| 17 | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | Public Medium (6m 56.9) |
| 18 | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | Public Medium (6m 56.9) |
| 19 | [Longest Increasing Subsequence](./dynamic_programming/solutions.md#4-longest-increasing-subsequence) | dynamic programming | Public Medium (6m 56.9) |
| 20 | [Koko Eating Bananas](./binary_search/solutions.md#3-koko-eating-bananas) | binary search | Public Medium (6m 56.9) |

### Databricks

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | Public High (6m 100.0) |
| 2 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Public High (6m 100.0) |
| 3 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | Public High (6m 100.0) |
| 4 | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | Public High (6m 82.4) |
| 5 | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | Public High (all 79.9) |
| 6 | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | Public High (all 79.9) |
| 7 | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | Public High (all 79.9) |
| 8 | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | Public Medium (all 54.1) |
| 9 | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | Public Medium (all 54.1) |
| 10 | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | Public Medium (all 54.1) |
| 11 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public Medium (all 37.2) |
| 12 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | Public Medium (all 37.2) |
| 13 | [Interval Assignment Map](./advanced_data_structures/solutions.md#5-interval-assignment-map) | advanced data structures | Public Medium (all 37.2) |
| 14 | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | Domain High |
| 15 | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | Domain High |
| 16 | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | Domain High |
| 17 | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | Domain High |
| 18 | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | Domain High |
| 19 | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | Domain High |
| 20 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Domain High |

### Snowflake

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | Public High (6m 100.0) |
| 2 | [Word Search II](./trees_tries/solutions.md#8-word-search-ii) | trees tries | Public High (6m 95.8) |
| 3 | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | Public High (6m 94.3) |
| 4 | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | Public High (6m 65.6) |
| 5 | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | Public High (all 64.2) |
| 6 | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | Public Medium (6m 59.1) |
| 7 | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | Public Medium (6m 59.1) |
| 8 | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | Public Medium (6m 59.1) |
| 9 | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | Public Medium (6m 49.9) |
| 10 | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | Public Medium (6m 49.9) |
| 11 | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | Public Medium (6m 49.9) |
| 12 | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | Public Medium (6m 49.9) |
| 13 | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | Public Medium (6m 49.9) |
| 14 | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | Public Medium (6m 49.9) |
| 15 | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | Public Medium (6m 49.9) |
| 16 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Public Medium (6m 49.9) |
| 17 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | Public Medium (6m 49.9) |
| 18 | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | Public Medium (all 55.0) |
| 19 | [Sliding Window Median](./heaps_ordered_structures/solutions.md#7-sliding-window-median) | heaps ordered structures | Public Medium (all 55.0) |
| 20 | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | Public Medium (all 48.6) |

### Stripe

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Evaluate Division](./graphs/solutions.md#4-evaluate-division) | graphs | Public High (all 68.2) |
| 2 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Domain High |
| 3 | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | Domain High |
| 4 | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | Domain High |
| 5 | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | Domain High |
| 6 | [Message Broker With Visibility Timeout](./systems_style/solutions.md#3-message-broker-with-visibility-timeout) | systems style | Domain High |
| 7 | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | Domain High |
| 8 | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | Domain Medium |
| 9 | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | Domain Medium |
| 10 | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | Domain Medium |
| 11 | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | Domain Medium |
| 12 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | Domain Medium |
| 13 | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | Domain Medium |
| 14 | [Interval Assignment Map](./advanced_data_structures/solutions.md#5-interval-assignment-map) | advanced data structures | Domain Medium |

### OpenAI

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | Public High (6m 66.7) |
| 2 | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | Public High (6m 66.7) |
| 3 | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | Public High (6m 66.7) |
| 4 | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | Public High (all 67.5) |
| 5 | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | Public High (all 67.5) |
| 6 | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | Public High (all 67.5) |
| 7 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Domain High |
| 8 | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | Domain High |
| 9 | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | Domain High |
| 10 | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | Domain High |
| 11 | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | Domain Medium |
| 12 | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | Domain Medium |
| 13 | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | Domain Medium |
| 14 | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | Domain Medium |
| 15 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | Domain Medium |
| 16 | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | Domain Medium |
| 17 | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | Domain Medium |
| 18 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Domain Medium |
| 19 | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | Domain Medium |
| 20 | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | Domain Medium |

### Anthropic

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Domain High |
| 2 | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | Domain High |
| 3 | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | Domain High |
| 4 | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | Domain High |
| 5 | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | Domain High |
| 6 | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | Domain High |
| 7 | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | Domain High |

### CoreWeave

| Rank | Question | Category | Signal |
| --- | --- | --- | --- |
| 1 | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | Domain High |
| 2 | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | Domain High |
| 3 | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | Domain High |
| 4 | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | Domain High |
| 5 | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | Domain High |
| 6 | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | Domain High |
| 7 | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | Domain High |
| 8 | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | Domain High |
| 9 | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | Domain High |
| 10 | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | Domain High |
| 11 | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | Domain High |
| 12 | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | Domain High |
| 13 | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | Domain High |
| 14 | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | Domain High |
| 15 | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | Domain High |
| 16 | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | Domain High |
| 17 | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | Domain Medium |
| 18 | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | Domain Medium |

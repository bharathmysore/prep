# C++ Company Frequency Focus Index

Use this index to bias final-stage practice toward a target company while still drilling the underlying pattern. These tags are preparation signals, not official or confidential interview banks.

## Methodology

- Public signal comes from public LeetCode company-wise CSVs in the [liquidslr/interview-company-wise-problems](https://github.com/liquidslr/interview-company-wise-problems) repository.
- The six-month CSV is preferred. The all-time CSV is used only when a solution-equivalent or close pattern-equivalent problem was not present in the six-month data.
- Public frequency buckets: `High >= 60`, `Medium 30-59.9`, `Low < 30`. The number shown is the public source frequency score, not a probability of being asked.
- Domain-fit tags cover L7 systems, concurrency, parallel, and distributed-systems coding questions where public LeetCode-style company data is sparse. Treat these as company-relevance signals, not frequency measurements.
- Company-tag reliability is best used for pattern focus, not memorization; public prep guidance commonly notes that exact company tags are less reliable than repeated patterns, especially for smaller or less-tagged companies.

## Source Context

- [Public company-wise LeetCode CSV repository](https://github.com/liquidslr/interview-company-wise-problems)
- [Interview Browser company-wise LeetCode page](https://interviewbrowser.com/leetcode-questions)
- [LeetCode company tags strategy note](https://leetcopilot.dev/blog/how-to-use-leetcode-premium-company-tags-for-targeted-interview-prep)

## Company Sections

### Amazon/AWS

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 82.5; source title: Longest Substring Without Repeating Characters |
| High | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 6m 80.5; source title: Merge Intervals |
| High | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 79.9; source title: Group Anagrams |
| High | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 77.3; source title: Merge k Sorted Lists |
| High | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 77.3; source title: Merge k Sorted Lists |
| High | Public | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | all 75.3; source title: Median of Two Sorted Arrays |
| High | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 73.6; source title: Valid Parentheses |
| High | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 73.1; source title: Search in Rotated Sorted Array |
| High | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 6m 71.8; source title: Copy List with Random Pointer |
| High | Public | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6m 66.1; source title: Longest Consecutive Sequence |
| High | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 6m 60.5; source title: Merge Two Sorted Lists |
| High | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Message Broker With Visibility Timeout](./systems_style/solutions.md#3-message-broker-with-visibility-timeout) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | 6m 56.1; source title: Minimum Window Substring |
| Medium | Public | [Generate Subsets](./backtracking/solutions.md#1-generate-subsets) | backtracking | 6m 56.1; source title: Subsets |
| Medium | Public | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | 6m 55.1; source title: Sort Colors |
| Medium | Public | [Largest Rectangle In Histogram](./stacks_queues/solutions.md#4-largest-rectangle-in-histogram) | stacks queues | 6m 52.8; source title: Largest Rectangle in Histogram |
| Medium | Public | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | 6m 50.3; source title: Reverse Nodes in k-Group |
| Medium | Public | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | 6m 49.0; source title: Combination Sum |
| Medium | Public | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | 6m 49.0; source title: N-Queens |
| Medium | Public | [Palindrome Partitioning](./backtracking/solutions.md#6-palindrome-partitioning) | backtracking | 6m 47.5; source title: Palindrome Partitioning |
| Medium | Public | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | 6m 45.9; source title: Validate Binary Search Tree |
| Medium | Public | [Edit Distance](./dynamic_programming/solutions.md#6-edit-distance) | dynamic programming | all 45.7; source title: Edit Distance |
| Medium | Public | [Insert Interval](./arrays_strings/solutions.md#8-insert-interval) | arrays strings | all 42.7; source title: Insert Interval |
| Medium | Public | [Minimum Path Sum](./dynamic_programming/solutions.md#8-minimum-path-sum) | dynamic programming | 6m 42.3; source title: Minimum Path Sum |
| Medium | Public | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | 6m 40.2; source title: Sudoku Solver |
| Medium | Public | [Build Tree From Preorder And Inorder](./trees_tries/solutions.md#6-build-tree-from-preorder-and-inorder) | trees tries | 6m 40.2; source title: Construct Binary Tree from Preorder and Inorder Traversal |
| Medium | Public | [Decode Ways](./dynamic_programming/solutions.md#9-decode-ways) | dynamic programming | 6m 35.3; source title: Decode Ways |
| Medium | Domain | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Leaderboard](./systems_style/solutions.md#5-leaderboard) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Low | Public | [Unique Permutations](./backtracking/solutions.md#2-unique-permutations) | backtracking | all 28.1; source title: Permutations II |
| Low | Public | [Detect Cycle Entry](./linked_lists/solutions.md#5-detect-cycle-entry) | linked lists | 6m 24.7; source title: Linked List Cycle II |

### Google

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | 6m 68.5; source title: Median of Two Sorted Arrays |
| High | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 65.1; source title: Longest Substring Without Repeating Characters |
| High | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 60.8; source title: Valid Parentheses |
| High | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6m 58.6; source title: Longest Consecutive Sequence |
| Medium | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 6m 58.6; source title: Merge Two Sorted Lists |
| Medium | Public | [Count Subarrays With Sum K](./arrays_strings/solutions.md#4-count-subarrays-with-sum-k) | arrays strings | 6m 58.2; source title: Subarray Sum Equals K |
| Medium | Public | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | all 55.4; source title: Find Median from Data Stream |
| Medium | Public | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6m 52.1; source title: House Robber |
| Medium | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 49.2; source title: Group Anagrams |
| Medium | Public | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | 6m 47.5; source title: Reverse Linked List |
| Medium | Public | [Insert Interval](./arrays_strings/solutions.md#8-insert-interval) | arrays strings | all 47.3; source title: Insert Interval |
| Medium | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 45.7; source title: Search in Rotated Sorted Array |
| Medium | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 44.7; source title: Merge k Sorted Lists |
| Medium | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 44.7; source title: Merge k Sorted Lists |
| Medium | Public | [Longest Increasing Subsequence](./dynamic_programming/solutions.md#4-longest-increasing-subsequence) | dynamic programming | 6m 43.7; source title: Longest Increasing Subsequence |
| Medium | Public | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | 6m 42.5; source title: Reverse Nodes in k-Group |
| Medium | Public | [TTL Deduplication Store](./hashing/solutions.md#5-ttl-deduplication-store) | hashing | all 42.2; source title: Contains Duplicate II |
| Medium | Public | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | 6m 41.4; source title: Combination Sum |
| Medium | Public | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | 6m 41.4; source title: Sudoku Solver |
| Medium | Public | [Kth Smallest In BST](./trees_tries/solutions.md#4-kth-smallest-in-bst) | trees tries | all 40.2; source title: Kth Smallest Element in a BST |
| Medium | Public | [First Non-Repeating Character In A Stream](./hashing/solutions.md#1-first-non-repeating-character-in-a-stream) | hashing | 6m 37.2; source title: First Unique Character in a String |
| Medium | Public | [Isomorphic Strings](./hashing/solutions.md#6-isomorphic-strings) | hashing | 6m 35.6; source title: Isomorphic Strings |
| Medium | Public | [Minimum Path Sum](./dynamic_programming/solutions.md#8-minimum-path-sum) | dynamic programming | 6m 31.8; source title: Minimum Path Sum |
| Medium | Public | [Evaluate Division](./graphs/solutions.md#4-evaluate-division) | graphs | 6m 31.8; source title: Evaluate Division |
| Medium | Domain | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Low | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 6m 27.0; source title: Copy List with Random Pointer |
| Low | Public | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | 6m 24.0; source title: Moving Average from Data Stream |
| Low | Public | [Word Search II](./trees_tries/solutions.md#8-word-search-ii) | trees tries | 6m 10.5; source title: Word Search II |
| Low | Public | [Fenwick Tree](./advanced_data_structures/solutions.md#4-fenwick-tree) | advanced data structures | all 5.0; source title: Range Sum Query - Mutable |
| Low | Public | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | all 5.0; source title: Range Sum Query - Mutable |

### Meta

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | 6m 93.6; source title: Kth Largest Element in an Array |
| High | Public | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | 6m 93.6; source title: Kth Largest Element in an Array |
| High | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 6m 81.2; source title: Merge Intervals |
| High | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 6m 78.8; source title: Copy List with Random Pointer |
| High | Public | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m 76.0; source title: LRU Cache |
| High | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 74.7; source title: Merge k Sorted Lists |
| High | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 74.7; source title: Merge k Sorted Lists |
| High | Public | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | 6m 68.9; source title: Minimum Window Substring |
| High | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 66.2; source title: Valid Parentheses |
| High | Domain | [Leaderboard](./systems_style/solutions.md#5-leaderboard) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 6m 59.8; source title: Course Schedule |
| Medium | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 57.0; source title: Longest Substring Without Repeating Characters |
| Medium | Public | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | 6m 53.8; source title: Median of Two Sorted Arrays |
| Medium | Public | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | 6m 49.9; source title: Sort Colors |
| Medium | Public | [Generate Subsets](./backtracking/solutions.md#1-generate-subsets) | backtracking | 6m 49.9; source title: Subsets |
| Medium | Public | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | 6m 48.0; source title: Word Break |
| Medium | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 6m 46.0; source title: Merge Two Sorted Lists |
| Medium | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 44.9; source title: Search in Rotated Sorted Array |
| Medium | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 43.7; source title: Group Anagrams |
| Medium | Public | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | all 39.8; source title: Combination Sum |
| Medium | Public | [Decode Ways](./dynamic_programming/solutions.md#9-decode-ways) | dynamic programming | all 39.8; source title: Decode Ways |
| Medium | Public | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | 6m 39.6; source title: Rotate Array |
| Medium | Public | [Two Sum In A Sorted Array](./arrays_strings/solutions.md#1-two-sum-in-a-sorted-array) | arrays strings | 6m 39.6; source title: Two Sum II - Input Array Is Sorted |
| Medium | Public | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6m 38.0; source title: Longest Consecutive Sequence |
| Medium | Public | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | all 36.2; source title: Reverse Nodes in k-Group |
| Medium | Public | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | all 33.6; source title: N-Queens |
| Medium | Public | [Build Tree From Preorder And Inorder](./trees_tries/solutions.md#6-build-tree-from-preorder-and-inorder) | trees tries | all 33.6; source title: Construct Binary Tree from Preorder and Inorder Traversal |
| Medium | Public | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | 6m 32.3; source title: Reverse Linked List |
| Medium | Public | [Largest Rectangle In Histogram](./stacks_queues/solutions.md#4-largest-rectangle-in-histogram) | stacks queues | 6m 32.3; source title: Largest Rectangle in Histogram |
| Medium | Public | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | all 30.6; source title: Sudoku Solver |
| Low | Public | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | 6m 27.2; source title: Validate Binary Search Tree |
| Low | Public | [Palindrome Partitioning](./backtracking/solutions.md#6-palindrome-partitioning) | backtracking | 6m 24.1; source title: Palindrome Partitioning |
| Low | Public | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6m 24.1; source title: House Robber |
| Low | Public | [Minimum Path Sum](./dynamic_programming/solutions.md#8-minimum-path-sum) | dynamic programming | all 22.3; source title: Minimum Path Sum |
| Low | Public | [Unique Permutations](./backtracking/solutions.md#2-unique-permutations) | backtracking | all 18.4; source title: Permutations II |
| Low | Public | [Edit Distance](./dynamic_programming/solutions.md#6-edit-distance) | dynamic programming | 6m 15.8; source title: Edit Distance |
| Low | Public | [Word Search II](./trees_tries/solutions.md#8-word-search-ii) | trees tries | 6m 15.8; source title: Word Search II |
| Low | Public | [Insert Interval](./arrays_strings/solutions.md#8-insert-interval) | arrays strings | 6m 10.2; source title: Insert Interval |

### Microsoft

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 72.4; source title: Longest Substring Without Repeating Characters |
| High | Public | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m 69.7; source title: LRU Cache |
| High | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 6m 65.4; source title: Merge Intervals |
| High | Public | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | 6m 64.1; source title: Median of Two Sorted Arrays |
| High | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 64.1; source title: Group Anagrams |
| High | Public | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | 6m 62.8; source title: Sort Colors |
| High | Domain | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | 6m 59.9; source title: N-Queens |
| Medium | Public | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | 6m 59.9; source title: Reverse Linked List |
| Medium | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 6m 58.2; source title: Merge Two Sorted Lists |
| Medium | Public | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | 6m 56.5; source title: Rotate Array |
| Medium | Public | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | 6m 56.5; source title: Combination Sum |
| Medium | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 56.5; source title: Merge k Sorted Lists |
| Medium | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 56.5; source title: Merge k Sorted Lists |
| Medium | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 56.5; source title: Valid Parentheses |
| Medium | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 54.5; source title: Search in Rotated Sorted Array |
| Medium | Public | [Largest Rectangle In Histogram](./stacks_queues/solutions.md#4-largest-rectangle-in-histogram) | stacks queues | 6m 54.5; source title: Largest Rectangle in Histogram |
| Medium | Public | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | 6m 52.4; source title: Kth Largest Element in an Array |
| Medium | Public | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | 6m 52.4; source title: Kth Largest Element in an Array |
| Medium | Public | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | 6m 52.4; source title: Min Stack |
| Medium | Public | [Generate Subsets](./backtracking/solutions.md#1-generate-subsets) | backtracking | all 52.2; source title: Subsets |
| Medium | Public | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | 6m 50.0; source title: Minimum Window Substring |
| Medium | Public | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6m 50.0; source title: House Robber |
| Medium | Public | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6m 50.0; source title: Longest Consecutive Sequence |
| Medium | Public | [Edit Distance](./dynamic_programming/solutions.md#6-edit-distance) | dynamic programming | all 49.5; source title: Edit Distance |
| Medium | Public | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | 6m 47.2; source title: Word Break |
| Medium | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 6m 47.2; source title: Course Schedule |
| Medium | Public | [Decode Ways](./dynamic_programming/solutions.md#9-decode-ways) | dynamic programming | all 45.5; source title: Decode Ways |
| Medium | Public | [Meeting Rooms II](./heaps_ordered_structures/solutions.md#4-meeting-rooms-ii) | heaps ordered structures | 6m 44.1; source title: Meeting Rooms II |
| Medium | Public | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | 6m 44.1; source title: Reverse Nodes in k-Group |
| Medium | Public | [Unique Permutations](./backtracking/solutions.md#2-unique-permutations) | backtracking | all 43.6; source title: Permutations II |
| Medium | Public | [Product Of Array Except Self](./arrays_strings/solutions.md#6-product-of-array-except-self) | arrays strings | 6m 40.4; source title: Product of Array Except Self |
| Medium | Public | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | 6m 40.4; source title: Validate Binary Search Tree |
| Medium | Public | [Insert Interval](./arrays_strings/solutions.md#8-insert-interval) | arrays strings | all 37.7; source title: Insert Interval |
| Medium | Public | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | 6m 36.0; source title: Sudoku Solver |
| Medium | Public | [Minimum Path Sum](./dynamic_programming/solutions.md#8-minimum-path-sum) | dynamic programming | 6m 36.0; source title: Minimum Path Sum |
| Medium | Public | [Isomorphic Strings](./hashing/solutions.md#6-isomorphic-strings) | hashing | 6m 36.0; source title: Isomorphic Strings |
| Medium | Public | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | 6m 36.0; source title: Find Median from Data Stream |
| Medium | Public | [Lowest Common Ancestor In Binary Tree](./trees_tries/solutions.md#2-lowest-common-ancestor-in-binary-tree) | trees tries | 6m 36.0; source title: Lowest Common Ancestor of a Binary Tree |
| Medium | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 6m 30.3; source title: Copy List with Random Pointer |
| Medium | Domain | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Leaderboard](./systems_style/solutions.md#5-leaderboard) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Message Broker With Visibility Timeout](./systems_style/solutions.md#3-message-broker-with-visibility-timeout) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Low | Public | [Two Sum In A Sorted Array](./arrays_strings/solutions.md#1-two-sum-in-a-sorted-array) | arrays strings | 6m 22.6; source title: Two Sum II - Input Array Is Sorted |

### Apple

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m 100.0; source title: LRU Cache |
| High | Public | [Top K Frequent Words](./heaps_ordered_structures/solutions.md#6-top-k-frequent-words) | heaps ordered structures | 6m 81.7; source title: Top K Frequent Elements (pattern-equivalent) |
| High | Public | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | 6m 78.3; source title: Word Break |
| High | Public | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | all 75.8; source title: Median of Two Sorted Arrays |
| High | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 6m 74.2; source title: Merge Intervals |
| High | Public | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | 6m 74.2; source title: Min Stack |
| High | Public | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | 6m 71.9; source title: Rotate Array |
| High | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 71.9; source title: Group Anagrams |
| High | Public | [First Non-Repeating Character In A Stream](./hashing/solutions.md#1-first-non-repeating-character-in-a-stream) | hashing | 6m 69.3; source title: First Unique Character in a String |
| High | Public | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | 6m 69.3; source title: Kth Largest Element in an Array |
| High | Public | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | 6m 69.3; source title: Kth Largest Element in an Array |
| High | Public | [Count Subarrays With Sum K](./arrays_strings/solutions.md#4-count-subarrays-with-sum-k) | arrays strings | 6m 66.3; source title: Subarray Sum Equals K |
| High | Public | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | 6m 62.9; source title: Design Hit Counter |
| High | Public | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | 6m 62.9; source title: Design Hit Counter |
| High | Public | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | 6m 62.9; source title: Design Hit Counter |
| Medium | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 58.9; source title: Longest Substring Without Repeating Characters |
| Medium | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 6m 58.9; source title: Course Schedule II |
| Medium | Public | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | 6m 58.9; source title: Reverse Linked List |
| Medium | Public | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | 6m 58.9; source title: Implement Trie (Prefix Tree) |
| Medium | Public | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | 6m 54.0; source title: Time Based Key-Value Store |
| Medium | Public | [Kth Smallest In Sorted Matrix](./binary_search/solutions.md#5-kth-smallest-in-sorted-matrix) | binary search | 6m 54.0; source title: Kth Smallest Element in a Sorted Matrix |
| Medium | Public | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | 6m 54.0; source title: Time Based Key-Value Store |
| Medium | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 54.0; source title: Valid Parentheses |
| Medium | Public | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | 6m 54.0; source title: Time Based Key-Value Store |
| Medium | Public | [Serialize And Deserialize Binary Tree](./trees_tries/solutions.md#3-serialize-and-deserialize-binary-tree) | trees tries | 6m 54.0; source title: Serialize and Deserialize Binary Tree |
| Medium | Public | [Largest Rectangle In Histogram](./stacks_queues/solutions.md#4-largest-rectangle-in-histogram) | stacks queues | all 52.1; source title: Largest Rectangle in Histogram |
| Medium | Public | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | all 47.8; source title: Sort Colors |
| Medium | Public | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | all 47.8; source title: Combination Sum |
| Medium | Public | [Insert Interval](./arrays_strings/solutions.md#8-insert-interval) | arrays strings | 6m 47.7; source title: Insert Interval |
| Medium | Public | [Product Of Array Except Self](./arrays_strings/solutions.md#6-product-of-array-except-self) | arrays strings | 6m 47.7; source title: Product of Array Except Self |
| Medium | Public | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6m 47.7; source title: Longest Consecutive Sequence |
| Medium | Public | [Meeting Rooms II](./heaps_ordered_structures/solutions.md#4-meeting-rooms-ii) | heaps ordered structures | 6m 47.7; source title: Meeting Rooms II |
| Medium | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 47.7; source title: Merge k Sorted Lists |
| Medium | Public | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | 6m 47.7; source title: Find Median from Data Stream |
| Medium | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 47.7; source title: Merge k Sorted Lists |
| Medium | Public | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | 6m 47.7; source title: Task Scheduler |
| Medium | Public | [Lowest Common Ancestor In Binary Tree](./trees_tries/solutions.md#2-lowest-common-ancestor-in-binary-tree) | trees tries | 6m 47.7; source title: Lowest Common Ancestor of a Binary Tree |
| Medium | Public | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | all 46.1; source title: Validate Binary Search Tree |
| Medium | Public | [Two Sum In A Sorted Array](./arrays_strings/solutions.md#1-two-sum-in-a-sorted-array) | arrays strings | 6m 38.9; source title: Two Sum II - Input Array Is Sorted |
| Medium | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 38.9; source title: Search in Rotated Sorted Array |
| Medium | Public | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6m 38.9; source title: House Robber |
| Medium | Public | [Sliding Window Median](./heaps_ordered_structures/solutions.md#7-sliding-window-median) | heaps ordered structures | 6m 38.9; source title: Sliding Window Median |
| Medium | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 6m 38.9; source title: Merge Two Sorted Lists |
| Medium | Public | [Queue Using Two Stacks](./stacks_queues/solutions.md#6-queue-using-two-stacks) | stacks queues | 6m 38.9; source title: Implement Queue using Stacks |
| Medium | Public | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | all 37.4; source title: Minimum Window Substring |
| Medium | Public | [Generate Subsets](./backtracking/solutions.md#1-generate-subsets) | backtracking | all 37.4; source title: Subsets |
| Medium | Public | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | all 37.4; source title: N-Queens |
| Medium | Public | [Decode Ways](./dynamic_programming/solutions.md#9-decode-ways) | dynamic programming | all 34.5; source title: Decode Ways |
| Medium | Public | [Minimum Path Sum](./dynamic_programming/solutions.md#8-minimum-path-sum) | dynamic programming | all 34.5; source title: Minimum Path Sum |
| Medium | Public | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | all 34.5; source title: Reverse Nodes in k-Group |
| Low | Public | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | all 27.0; source title: Sudoku Solver |
| Low | Public | [Edit Distance](./dynamic_programming/solutions.md#6-edit-distance) | dynamic programming | all 27.0; source title: Edit Distance |
| Low | Public | [Unique Permutations](./backtracking/solutions.md#2-unique-permutations) | backtracking | all 21.9; source title: Permutations II |
| Low | Public | [Build Tree From Preorder And Inorder](./trees_tries/solutions.md#6-build-tree-from-preorder-and-inorder) | trees tries | all 21.9; source title: Construct Binary Tree from Preorder and Inorder Traversal |

### NVIDIA

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 95.8; source title: Search in Rotated Sorted Array |
| High | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 84.8; source title: Group Anagrams |
| High | Public | [Product Of Array Except Self](./arrays_strings/solutions.md#6-product-of-array-except-self) | arrays strings | 6m 77.0; source title: Product of Array Except Self |
| High | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | all 67.4; source title: Merge Intervals |
| High | Public | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m 66.0; source title: LRU Cache |
| High | Public | [Count Subarrays With Sum K](./arrays_strings/solutions.md#4-count-subarrays-with-sum-k) | arrays strings | 6m 66.0; source title: Subarray Sum Equals K |
| High | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 66.0; source title: Longest Substring Without Repeating Characters |
| High | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 66.0; source title: Merge k Sorted Lists |
| High | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 66.0; source title: Merge k Sorted Lists |
| High | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 66.0; source title: Valid Parentheses |
| High | Public | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | all 63.8; source title: Find Median from Data Stream |
| High | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | all 63.8; source title: Copy List with Random Pointer |
| High | Domain | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [Randomized Set](./hashing/solutions.md#4-randomized-set) | hashing | all 59.6; source title: Insert Delete GetRandom O(1) |
| Medium | Public | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | all 59.6; source title: Kth Largest Element in an Array |
| Medium | Public | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | all 59.6; source title: Kth Largest Element in an Array |
| Medium | Public | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | all 54.4; source title: Validate Binary Search Tree |
| Medium | Public | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | all 47.7; source title: House Robber |
| Medium | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | all 47.7; source title: Course Schedule II |
| Medium | Public | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | all 47.7; source title: Reverse Linked List |
| Medium | Public | [Serialize And Deserialize Binary Tree](./trees_tries/solutions.md#3-serialize-and-deserialize-binary-tree) | trees tries | all 47.7; source title: Serialize and Deserialize Binary Tree |
| Medium | Public | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | all 47.7; source title: Implement Trie (Prefix Tree) |
| Medium | Public | [Coin Change Minimum Coins](./dynamic_programming/solutions.md#2-coin-change-minimum-coins) | dynamic programming | all 38.4; source title: Coin Change |
| Medium | Public | [Meeting Rooms II](./heaps_ordered_structures/solutions.md#4-meeting-rooms-ii) | heaps ordered structures | all 38.4; source title: Meeting Rooms II |
| Medium | Public | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | all 38.4; source title: Min Stack |
| Medium | Domain | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |

### Oracle

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m 100.0; source title: LRU Cache |
| High | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | 6m 93.3; source title: Longest Substring Without Repeating Characters |
| High | Public | [Group Anagrams](./hashing/solutions.md#2-group-anagrams) | hashing | 6m 86.8; source title: Group Anagrams |
| High | Public | [Sliding Window Maximum](./stacks_queues/solutions.md#5-sliding-window-maximum) | stacks queues | 6m 84.2; source title: Sliding Window Maximum |
| High | Public | [Top K Frequent Words](./heaps_ordered_structures/solutions.md#6-top-k-frequent-words) | heaps ordered structures | 6m 81.2; source title: Top K Frequent Elements (pattern-equivalent) |
| High | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 77.9; source title: Merge k Sorted Lists |
| High | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 77.9; source title: Merge k Sorted Lists |
| High | Public | [LFU Cache](./advanced_data_structures/solutions.md#2-lfu-cache) | advanced data structures | 6m 69.5; source title: LFU Cache |
| High | Public | [Search Rotated Sorted Array](./binary_search/solutions.md#2-search-rotated-sorted-array) | binary search | 6m 69.5; source title: Search in Rotated Sorted Array |
| High | Public | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | 6m 69.5; source title: Find Median from Data Stream |
| High | Public | [Valid Parentheses](./stacks_queues/solutions.md#1-valid-parentheses) | stacks queues | 6m 69.5; source title: Valid Parentheses |
| High | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | 6m 64.0; source title: Merge Intervals |
| High | Public | [Count Palindromic Substrings](./dynamic_programming/solutions.md#10-count-palindromic-substrings) | dynamic programming | 6m 64.0; source title: Palindromic Substrings |
| High | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | all 61.6; source title: Merge Two Sorted Lists |
| Medium | Public | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | 6m 56.9; source title: N-Queens |
| Medium | Public | [Koko Eating Bananas](./binary_search/solutions.md#3-koko-eating-bananas) | binary search | 6m 56.9; source title: Koko Eating Bananas |
| Medium | Public | [Longest Increasing Subsequence](./dynamic_programming/solutions.md#4-longest-increasing-subsequence) | dynamic programming | 6m 56.9; source title: Longest Increasing Subsequence |
| Medium | Public | [Reverse Linked List](./linked_lists/solutions.md#1-reverse-linked-list) | linked lists | 6m 56.9; source title: Reverse Linked List |
| Medium | Public | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | 6m 56.9; source title: Task Scheduler |
| Medium | Public | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | 6m 56.9; source title: Task Scheduler |
| Medium | Public | [Product Of Array Except Self](./arrays_strings/solutions.md#6-product-of-array-except-self) | arrays strings | all 54.7; source title: Product of Array Except Self |
| Medium | Public | [Combination Sum](./backtracking/solutions.md#3-combination-sum) | backtracking | all 54.7; source title: Combination Sum |
| Medium | Public | [Lowest Common Ancestor In Binary Tree](./trees_tries/solutions.md#2-lowest-common-ancestor-in-binary-tree) | trees tries | all 54.7; source title: Lowest Common Ancestor of a Binary Tree |
| Medium | Public | [Median Of Two Sorted Arrays](./binary_search/solutions.md#1-median-of-two-sorted-arrays) | binary search | all 51.8; source title: Median of Two Sorted Arrays |
| Medium | Public | [Isomorphic Strings](./hashing/solutions.md#6-isomorphic-strings) | hashing | all 48.4; source title: Isomorphic Strings |
| Medium | Public | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | 6m 47.0; source title: Time Based Key-Value Store |
| Medium | Public | [Sort Colors](./arrays_strings/solutions.md#9-sort-colors) | arrays strings | 6m 47.0; source title: Sort Colors |
| Medium | Public | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | 6m 47.0; source title: Time Based Key-Value Store |
| Medium | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 6m 47.0; source title: Course Schedule II |
| Medium | Public | [Longest Consecutive Sequence](./hashing/solutions.md#3-longest-consecutive-sequence) | hashing | 6m 47.0; source title: Longest Consecutive Sequence |
| Medium | Public | [Randomized Set](./hashing/solutions.md#4-randomized-set) | hashing | 6m 47.0; source title: Insert Delete GetRandom O(1) |
| Medium | Public | [Kth Largest Element](./heaps_ordered_structures/solutions.md#1-kth-largest-element) | heaps ordered structures | 6m 47.0; source title: Kth Largest Element in an Array |
| Medium | Public | [Meeting Rooms II](./heaps_ordered_structures/solutions.md#4-meeting-rooms-ii) | heaps ordered structures | 6m 47.0; source title: Meeting Rooms II |
| Medium | Public | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | 6m 47.0; source title: Kth Largest Element in an Array |
| Medium | Public | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | 6m 47.0; source title: Min Stack |
| Medium | Public | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | 6m 47.0; source title: Time Based Key-Value Store |
| Medium | Public | [Kth Smallest In BST](./trees_tries/solutions.md#4-kth-smallest-in-bst) | trees tries | 6m 47.0; source title: Kth Smallest Element in a BST |
| Medium | Public | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | 6m 47.0; source title: Implement Trie (Prefix Tree) |
| Medium | Public | [Validate Binary Search Tree](./trees_tries/solutions.md#1-validate-binary-search-tree) | trees tries | 6m 47.0; source title: Validate Binary Search Tree |
| Medium | Public | [Rotate Array](./arrays_strings/solutions.md#10-rotate-array) | arrays strings | all 44.5; source title: Rotate Array |
| Medium | Public | [Decode Ways](./dynamic_programming/solutions.md#9-decode-ways) | dynamic programming | all 44.5; source title: Decode Ways |
| Medium | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | all 44.5; source title: Copy List with Random Pointer |
| Medium | Public | [Insert Interval](./arrays_strings/solutions.md#8-insert-interval) | arrays strings | all 39.7; source title: Insert Interval |
| Medium | Public | [Sudoku Solver](./backtracking/solutions.md#5-sudoku-solver) | backtracking | all 39.7; source title: Sudoku Solver |
| Medium | Public | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | all 33.6; source title: Minimum Window Substring |
| Medium | Public | [Two Sum In A Sorted Array](./arrays_strings/solutions.md#1-two-sum-in-a-sorted-array) | arrays strings | all 33.6; source title: Two Sum II - Input Array Is Sorted |
| Medium | Public | [Queue Using Two Stacks](./stacks_queues/solutions.md#6-queue-using-two-stacks) | stacks queues | all 33.6; source title: Implement Queue using Stacks |

### Databricks

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | 6m 100.0; source title: Design Hit Counter |
| High | Public | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | 6m 100.0; source title: Design Hit Counter |
| High | Public | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | 6m 100.0; source title: Design Hit Counter |
| High | Public | [House Robber](./dynamic_programming/solutions.md#1-house-robber) | dynamic programming | 6m 82.4; source title: House Robber |
| High | Public | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | all 79.9; source title: Time Based Key-Value Store |
| High | Public | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | all 79.9; source title: Time Based Key-Value Store |
| High | Public | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | all 79.9; source title: Time Based Key-Value Store |
| High | Domain | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | all 54.1; source title: Web Crawler Multithreaded |
| Medium | Public | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | all 54.1; source title: Web Crawler Multithreaded |
| Medium | Public | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | all 54.1; source title: Web Crawler Multithreaded |
| Medium | Public | [Interval Assignment Map](./advanced_data_structures/solutions.md#5-interval-assignment-map) | advanced data structures | all 37.2; source title: Data Stream as Disjoint Intervals |
| Medium | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | all 37.2; source title: Merge Intervals |
| Medium | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | all 37.2; source title: Course Schedule II |

### Snowflake

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Minimum Window Substring](./arrays_strings/solutions.md#3-minimum-window-substring) | arrays strings | 6m 100.0; source title: Minimum Window Substring |
| High | Public | [Word Search II](./trees_tries/solutions.md#8-word-search-ii) | trees tries | 6m 95.8; source title: Word Search II |
| High | Public | [Course Schedule Order](./graphs/solutions.md#1-course-schedule-order) | graphs | 6m 94.3; source title: Course Schedule II |
| High | Public | [Copy List With Random Pointer](./linked_lists/solutions.md#6-copy-list-with-random-pointer) | linked lists | 6m 65.6; source title: Copy List with Random Pointer |
| High | Public | [Reverse Nodes In K-Group](./linked_lists/solutions.md#2-reverse-nodes-in-k-group) | linked lists | all 64.2; source title: Reverse Nodes in k-Group |
| High | Domain | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Public | [Merge Two Sorted Lists](./linked_lists/solutions.md#3-merge-two-sorted-lists) | linked lists | 6m 59.1; source title: Merge Two Sorted Lists |
| Medium | Public | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | 6m 59.1; source title: Task Scheduler |
| Medium | Public | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | 6m 59.1; source title: Task Scheduler |
| Medium | Public | [Sliding Window Median](./heaps_ordered_structures/solutions.md#7-sliding-window-median) | heaps ordered structures | all 55.0; source title: Sliding Window Median |
| Medium | Public | [Min Stack](./stacks_queues/solutions.md#2-min-stack) | stacks queues | all 55.0; source title: Min Stack |
| Medium | Public | [LRU Cache](./advanced_data_structures/solutions.md#1-lru-cache) | advanced data structures | 6m 49.9; source title: LRU Cache |
| Medium | Public | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | 6m 49.9; source title: Time Based Key-Value Store |
| Medium | Public | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | 6m 49.9; source title: Design Hit Counter |
| Medium | Public | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | 6m 49.9; source title: Time Based Key-Value Store |
| Medium | Public | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | 6m 49.9; source title: Design Hit Counter |
| Medium | Public | [Merge K Sorted Streams](./heaps_ordered_structures/solutions.md#3-merge-k-sorted-streams) | heaps ordered structures | 6m 49.9; source title: Merge k Sorted Lists |
| Medium | Public | [Merge K Sorted Lists](./linked_lists/solutions.md#4-merge-k-sorted-lists) | linked lists | 6m 49.9; source title: Merge k Sorted Lists |
| Medium | Public | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | 6m 49.9; source title: Time Based Key-Value Store |
| Medium | Public | [Trie Insert Search Prefix](./trees_tries/solutions.md#7-trie-insert-search-prefix) | trees tries | 6m 49.9; source title: Implement Trie (Prefix Tree) |
| Medium | Public | [Merge Intervals](./arrays_strings/solutions.md#5-merge-intervals) | arrays strings | all 48.6; source title: Merge Intervals |
| Medium | Public | [Randomized Set](./hashing/solutions.md#4-randomized-set) | hashing | all 48.6; source title: Insert Delete GetRandom O(1) |
| Medium | Public | [Streaming Median](./heaps_ordered_structures/solutions.md#2-streaming-median) | heaps ordered structures | all 48.6; source title: Find Median from Data Stream |
| Medium | Public | [Longest Substring Without Repeating Characters](./arrays_strings/solutions.md#2-longest-substring-without-repeating-characters) | arrays strings | all 39.6; source title: Longest Substring Without Repeating Characters |
| Medium | Public | [N Queens](./backtracking/solutions.md#4-n-queens) | backtracking | all 39.6; source title: N-Queens |
| Medium | Public | [Word Break](./dynamic_programming/solutions.md#7-word-break) | dynamic programming | all 39.6; source title: Word Break |
| Medium | Public | [Evaluate Division](./graphs/solutions.md#4-evaluate-division) | graphs | all 39.6; source title: Evaluate Division |
| Medium | Public | [Top K Frequent Words](./heaps_ordered_structures/solutions.md#6-top-k-frequent-words) | heaps ordered structures | all 39.6; source title: Top K Frequent Elements (pattern-equivalent) |
| Medium | Public | [Meeting Rooms II](./heaps_ordered_structures/solutions.md#4-meeting-rooms-ii) | heaps ordered structures | all 39.6; source title: Meeting Rooms II |
| Medium | Public | [Build Tree From Preorder And Inorder](./trees_tries/solutions.md#6-build-tree-from-preorder-and-inorder) | trees tries | all 39.6; source title: Construct Binary Tree from Preorder and Inorder Traversal |
| Medium | Domain | [Interval Assignment Map](./advanced_data_structures/solutions.md#5-interval-assignment-map) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |

### Stripe

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Evaluate Division](./graphs/solutions.md#4-evaluate-division) | graphs | all 68.2; source title: Evaluate Division |
| High | Domain | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Message Broker With Visibility Timeout](./systems_style/solutions.md#3-message-broker-with-visibility-timeout) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Interval Assignment Map](./advanced_data_structures/solutions.md#5-interval-assignment-map) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |

### OpenAI

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Public | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | all 67.5; source title: Web Crawler Multithreaded |
| High | Public | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | all 67.5; source title: Web Crawler Multithreaded |
| High | Public | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | all 67.5; source title: Web Crawler Multithreaded |
| High | Public | [TTL Cache](./advanced_data_structures/solutions.md#3-ttl-cache) | advanced data structures | 6m 66.7; source title: Time Based Key-Value Store |
| High | Public | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | 6m 66.7; source title: Time Based Key-Value Store |
| High | Public | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | 6m 66.7; source title: Time Based Key-Value Store |
| High | Domain | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Rolling Metrics Window](./advanced_data_structures/solutions.md#6-rolling-metrics-window) | advanced data structures | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Retry Queue With Backoff](./systems_style/solutions.md#2-retry-queue-with-backoff) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |

### Anthropic

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Domain | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |

### CoreWeave

| Level | Signal | Solution | Category | Evidence |
| --- | --- | --- | --- | --- |
| High | Domain | [Bounded Blocking Queue](./concurrency/solutions.md#1-bounded-blocking-queue) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Concurrent Token Bucket](./concurrency/solutions.md#3-concurrent-token-bucket) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Deadlock-Free Account Transfer](./concurrency/solutions.md#7-deadlock-free-account-transfer) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Readers-Writer Cache](./concurrency/solutions.md#5-readers-writer-cache) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Reusable Barrier](./concurrency/solutions.md#6-reusable-barrier) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Single-Flight Duplicate Suppression](./concurrency/solutions.md#4-single-flight-duplicate-suppression) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Thread Pool](./concurrency/solutions.md#2-thread-pool) | concurrency | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Consistent Hashing Ring](./distributed_systems_algorithms/solutions.md#1-consistent-hashing-ring) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Heartbeat Failure Detector](./distributed_systems_algorithms/solutions.md#4-heartbeat-failure-detector) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Rendezvous Hashing](./distributed_systems_algorithms/solutions.md#2-rendezvous-hashing) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Map](./parallel_algorithms/solutions.md#1-parallel-map) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Prefix Sum](./parallel_algorithms/solutions.md#3-parallel-prefix-sum) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Reduce](./parallel_algorithms/solutions.md#2-parallel-reduce) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Parallel Top K](./parallel_algorithms/solutions.md#4-parallel-top-k) | parallel algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Config Snapshot Manager](./systems_style/solutions.md#4-config-snapshot-manager) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| High | Domain | [Delayed Job Scheduler](./systems_style/solutions.md#1-delayed-job-scheduler) | systems style | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Quorum Read/Write Simulator](./distributed_systems_algorithms/solutions.md#3-quorum-readwrite-simulator) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |
| Medium | Domain | [Vector Clock Comparison](./distributed_systems_algorithms/solutions.md#5-vector-clock-comparison) | distributed systems algorithms | L7 domain fit; no exact public frequency in reviewed CSVs |

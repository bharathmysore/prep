# Sources, License, and Selection Method

## Selection

The ranking uses the public [leetcode-api metadata snapshot](https://github.com/noworneverev/leetcode-api/tree/main/data) read on 2026-10-07. It included 4,073 problems. This collection filters to free problems in its `Algorithms` category (3,121 candidates), sorts by descending `likes`, breaks ties by ascending numeric LeetCode problem ID, and takes the first 1,000. The snapshot is fixed for this collection; current likes and rankings may differ. Likes are a popularity signal, not a measured interview frequency.

Source revisions at collection time: [ranking data `cb0c097e8212`](https://github.com/noworneverev/leetcode-api/commit/cb0c097e8212), [Doocs guide `82523c8e09d9`](https://github.com/doocs/leetcode/commit/82523c8e09d9), and [kamyu104 C++ `e2b2693fb490`](https://github.com/kamyu104/LeetCode-Solutions/commit/e2b2693fb490).

## Teaching Material

- The linked LeetCode page is the authority for each problem's current prompt and constraints. The local **Problem** line is a short excerpt for navigation, not a replacement for that page.
- The **Solution idea** is an excerpt adapted from the corresponding English page in [Doocs LeetCode](https://github.com/doocs/leetcode). The exact guide page is linked from each entry that uses Doocs C++ code; for other entries, the matching guide path is `https://github.com/doocs/leetcode/blob/main/solution/.../README_EN.md`. Doocs publishes this material under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). The notes here are shortened and reformatted and are shared under the same license.
- Of the 1,000 **C++ solution** blocks, 728 come from the linked Doocs guides under CC BY-SA 4.0. The other 272 come from [kamyu104/LeetCode-Solutions](https://github.com/kamyu104/LeetCode-Solutions), under the [MIT license](./LICENSE_KAMYU.md). Each entry links its exact code source.
- Time and space complexity for the MIT C++ blocks come from their source comments. For Doocs C++ blocks they come from the guide's analysis, with remaining gaps filled by direct analysis of the included code. Auxiliary space conventions vary across the sources; read the code when output storage or recursion depth matters.

## Verification Limits

Every entry has a C++ code block and explicit time and space complexity fields. These imported snippets use LeetCode's surrounding type definitions and interfaces. They were checked structurally for coverage, but were not individually compiled or submitted to LeetCode. A guide's solution idea and a separately sourced C++ implementation may use different valid algorithms; use the linked source when studying implementation details.

Some snippets use C++20 library features such as `contains`; use a C++20 compiler where needed. The LeetCode runner supplies types such as `ListNode` and `TreeNode` for relevant problems.

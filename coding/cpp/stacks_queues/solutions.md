# Stacks And Queues Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Valid Parentheses

* **Pattern / Idea**: Stack of unmatched opening brackets.
* **Company Frequency Tags**: Public signal: `Amazon/AWS: High (6m 73.6)`, `Oracle: High (6m 69.5)`, `Meta: High (6m 66.2)`, `NVIDIA: High (6m 66.0)`, `Google: High (6m 60.8)`, `Microsoft: Medium (6m 56.5)`, `Apple: Medium (6m 54.0)`.
* **Question**: Validate whether a string of brackets is balanced.
* **Test Cases**: [Test cases](./test_cases.md#1-valid-parentheses).
* **C++ Code**
  ```cpp
  bool validParentheses(const string& s) {
      unordered_map<char, char> close{{')', '('}, {']', '['}, {'}', '{'}};
      vector<char> st;
      for (char c : s) {
          if (c == '(' || c == '[' || c == '{') st.push_back(c);
          else if (close.count(c)) {
              if (st.empty() || st.back() != close[c]) return false;
              st.pop_back();
          }
      }
      return st.empty();
  }
  ```
* **Code Explanation**: A closing bracket must match the most recent unmatched opening bracket.
* **Invariants**: `st` contains exactly the unmatched opening brackets for the processed prefix.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: use a switch instead of map for tiny constants. Memory: reserve stack size if needed.
* **Edge Cases To Consider**: Empty, starts with close, nested, interleaved mismatch, non-bracket chars policy.
* **L7 Follow-ups**: Extend to parser tokens with line/column diagnostics.

## 2. Min Stack

* **Pattern / Idea**: Store current minimum with each stack depth.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 74.2)`, `Microsoft: Medium (6m 52.4)`, `Oracle: Medium (6m 47.0)`, `Snowflake: Medium (all 55.0)`, `NVIDIA: Medium (all 38.4)`.
* **Question**: Implement a stack that supports `push`, `pop`, `top`, and `getMin`.
* **Test Cases**: [Test cases](./test_cases.md#2-min-stack).
* **C++ Code**
  ```cpp
  class MinStack {
      vector<pair<int, int>> st;
  public:
      void push(int x) {
          int mn = st.empty() ? x : min(x, st.back().second);
          st.push_back({x, mn});
      }
      void pop() { st.pop_back(); }
      int top() const { return st.back().first; }
      int getMin() const { return st.back().second; }
      bool empty() const { return st.empty(); }
  };
  ```
* **Code Explanation**: Each entry remembers the minimum for the prefix ending at that entry.
* **Invariants**: `st.back().second` is the minimum of all values currently in the stack.
* **Complexity**: Time `O(1)` per operation, space `O(n)`.
* **Optimizations**: Runtime: no scanning. Memory: compressed auxiliary min stack stores only min changes.
* **Edge Cases To Consider**: Duplicate minimums, pop minimum, negative values, empty operation contract.
* **L7 Follow-ups**: Make API exception-safe or return optional values.

## 3. Next Greater Element

* **Pattern / Idea**: Monotonic decreasing stack of unresolved indices.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given an array, return the next greater element for every index.
* **Test Cases**: [Test cases](./test_cases.md#3-next-greater-element).
* **C++ Code**
  ```cpp
  vector<int> nextGreater(const vector<int>& nums) {
      vector<int> ans(nums.size(), -1), st;
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          while (!st.empty() && nums[i] > nums[st.back()]) {
              ans[st.back()] = nums[i];
              st.pop_back();
          }
          st.push_back(i);
      }
      return ans;
  }
  ```
* **Code Explanation**: When a larger value arrives, it resolves all smaller values waiting on the stack.
* **Invariants**: Values at stacked indices are monotonically decreasing from bottom to top.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: every index is pushed and popped once. Memory: store indices only.
* **Edge Cases To Consider**: Decreasing input, increasing input, duplicates, empty input.
* **L7 Follow-ups**: Circular version scans `2n` positions with modulo.

## 4. Largest Rectangle In Histogram

* **Pattern / Idea**: Monotonic increasing stack.
* **Company Frequency Tags**: Public signal: `Microsoft: Medium (6m 54.5)`, `Amazon/AWS: Medium (6m 52.8)`, `Meta: Medium (6m 32.3)`, `Apple: Medium (all 52.1)`.
* **Question**: Given histogram bar heights, return the largest rectangle area.
* **Test Cases**: [Test cases](./test_cases.md#4-largest-rectangle-in-histogram).
* **C++ Code**
  ```cpp
  long long largestRectangleArea(const vector<int>& h) {
      vector<int> st;
      long long best = 0;
      for (int i = 0; i <= static_cast<int>(h.size()); ++i) {
          int cur = (i == static_cast<int>(h.size())) ? 0 : h[i];
          while (!st.empty() && cur < h[st.back()]) {
              int height = h[st.back()];
              st.pop_back();
              int left = st.empty() ? -1 : st.back();
              best = max(best, 1LL * height * (i - left - 1));
          }
          st.push_back(i);
      }
      return best;
  }
  ```
* **Code Explanation**: Popping a bar discovers its first smaller boundary on both sides.
* **Invariants**: Stack heights are nondecreasing.
* **Complexity**: Time `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: sentinel zero flushes all bars. Memory: stack of indices.
* **Edge Cases To Consider**: Empty, all equal, increasing, decreasing, large area overflow.
* **L7 Follow-ups**: For online histograms, exact answer may require retaining unresolved bars.

## 5. Sliding Window Maximum

* **Pattern / Idea**: Monotonic deque.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 84.2)`.
* **Question**: Given an array and window size `k`, return the maximum value in every sliding window.
* **Test Cases**: [Test cases](./test_cases.md#5-sliding-window-maximum).
* **C++ Code**
  ```cpp
  vector<int> maxSlidingWindow(const vector<int>& nums, int k) {
      deque<int> dq;
      vector<int> ans;
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          while (!dq.empty() && dq.front() <= i - k) dq.pop_front();
          while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
          dq.push_back(i);
          if (i >= k - 1) ans.push_back(nums[dq.front()]);
      }
      return ans;
  }
  ```
* **Code Explanation**: The deque front is always the largest active window element.
* **Invariants**: Deque indices are in increasing order and values are in decreasing order.
* **Complexity**: Time `O(n)`, space `O(k)`.
* **Optimizations**: Runtime: each index is inserted and removed once. Memory: store indices, not values.
* **Edge Cases To Consider**: `k = 1`, `k = n`, duplicates, negative values.
* **L7 Follow-ups**: For distributed windows, define event-time vs processing-time semantics.

## 6. Queue Using Two Stacks

* **Pattern / Idea**: Amortized transfer from input stack to output stack.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 38.9)`, `Oracle: Medium (all 33.6)`.
* **Question**: Implement a queue using two stacks and explain amortized cost.
* **Test Cases**: [Test cases](./test_cases.md#6-queue-using-two-stacks).
* **C++ Code**
  ```cpp
  class TwoStackQueue {
      vector<int> in, out;
      void refill() {
          if (!out.empty()) return;
          while (!in.empty()) {
              out.push_back(in.back());
              in.pop_back();
          }
      }
  public:
      void push(int x) { in.push_back(x); }
      int pop() {
          refill();
          int x = out.back();
          out.pop_back();
          return x;
      }
      int front() {
          refill();
          return out.back();
      }
      bool empty() const { return in.empty() && out.empty(); }
  };
  ```
* **Code Explanation**: Reversing `in` into `out` restores FIFO order when needed.
* **Invariants**: If `out` is non-empty, its back is the queue front; otherwise queue order is reverse of `in`.
* **Complexity**: Amortized time `O(1)` per operation, worst transfer `O(n)`, space `O(n)`.
* **Optimizations**: Runtime: transfer only when `out` is empty. Memory: no duplicate storage after transfer.
* **Edge Cases To Consider**: Alternating push/pop, popping after bulk push, empty operation contract.
* **L7 Follow-ups**: For real concurrency, use a proper synchronized queue instead.

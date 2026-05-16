# Heaps And Ordered Structures Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Kth Largest Element

* **Pattern / Idea**: Size-`k` min-heap.
* **Company Frequency Tags**: Public signal: `Meta: High (6m 93.6)`, `Apple: High (6m 69.3)`, `Microsoft: Medium (6m 52.4)`, `Oracle: Medium (6m 47.0)`, `NVIDIA: Medium (all 59.6)`.
* **Question**: Given an array, return the kth largest element.
* **Test Cases**: [Test cases](./test_cases.md#1-kth-largest-element).
* **C++ Code**
  ```cpp
  int kthLargest(const vector<int>& nums, int k) {
      priority_queue<int, vector<int>, greater<int>> pq;
      for (int x : nums) {
          pq.push(x);
          if (static_cast<int>(pq.size()) > k) pq.pop();
      }
      return pq.top();
  }
  ```
* **Code Explanation**: The heap keeps only the largest `k` elements seen; the smallest among them is the kth largest overall.
* **Invariants**: Heap size is at most `k` and contains the best `k` candidates from the processed prefix.
* **Complexity**: Time `O(n log k)`, space `O(k)`.
* **Optimizations**: Runtime: quickselect gives average `O(n)` when mutation is allowed. Memory: heap bounds retained candidates.
* **Edge Cases To Consider**: Duplicates, `k = 1`, `k = n`, negative values.
* **L7 Follow-ups**: For streams, heap is stable and online; quickselect is not.

## 2. Streaming Median

* **Pattern / Idea**: Two heaps split lower and upper halves.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 69.5)`, `Apple: Medium (6m 47.7)`, `Microsoft: Medium (6m 36.0)`, `NVIDIA: High (all 63.8)`, `Google: Medium (all 55.4)`, `Snowflake: Medium (all 48.6)`.
* **Question**: Implement a streaming median data structure.
* **Test Cases**: [Test cases](./test_cases.md#2-streaming-median).
* **C++ Code**
  ```cpp
  class MedianFinder {
      priority_queue<int> low;
      priority_queue<int, vector<int>, greater<int>> high;
  public:
      void addNum(int x) {
          if (low.empty() || x <= low.top()) low.push(x);
          else high.push(x);
          if (low.size() > high.size() + 1) {
              high.push(low.top()); low.pop();
          } else if (high.size() > low.size()) {
              low.push(high.top()); high.pop();
          }
      }
      double findMedian() const {
          if (low.size() == high.size()) return (low.top() / 2.0) + (high.top() / 2.0);
          return low.top();
      }
  };
  ```
* **Code Explanation**: `low` stores the smaller half, `high` stores the larger half, and sizes differ by at most one.
* **Invariants**: Every value in `low` is `<=` every value in `high`; `low.size() >= high.size()`.
* **Complexity**: Add `O(log n)`, median `O(1)`, space `O(n)`.
* **Optimizations**: Runtime: rebalance by one move. Memory: approximate quantiles if exact median is too expensive.
* **Edge Cases To Consider**: Odd/even counts, duplicates, negative values, integer overflow in average.
* **L7 Follow-ups**: Distributed median needs sketches or coordinated partitioning.

## 3. Merge K Sorted Streams

* **Pattern / Idea**: Min-heap of stream heads.
* **Company Frequency Tags**: Public signal: `Oracle: High (6m 77.9)`, `Amazon/AWS: High (6m 77.3)`, `Meta: High (6m 74.7)`, `NVIDIA: High (6m 66.0)`, `Microsoft: Medium (6m 56.5)`, `Snowflake: Medium (6m 49.9)`, `Apple: Medium (6m 47.7)`, `Google: Medium (6m 44.7)`.
* **Question**: Given `k` sorted arrays or lists, merge them into sorted order.
* **Test Cases**: [Test cases](./test_cases.md#3-merge-k-sorted-streams).
* **C++ Code**
  ```cpp
  vector<int> mergeKSorted(const vector<vector<int>>& lists) {
      using Item = tuple<int, int, int>; // value, list index, element index
      priority_queue<Item, vector<Item>, greater<Item>> pq;
      for (int i = 0; i < static_cast<int>(lists.size()); ++i)
          if (!lists[i].empty()) pq.push({lists[i][0], i, 0});
      vector<int> out;
      while (!pq.empty()) {
          auto [value, li, ei] = pq.top(); pq.pop();
          out.push_back(value);
          if (ei + 1 < static_cast<int>(lists[li].size()))
              pq.push({lists[li][ei + 1], li, ei + 1});
      }
      return out;
  }
  ```
* **Code Explanation**: Pop the smallest current head and replace it with the next value from the same stream.
* **Invariants**: Heap contains the smallest unconsumed value from each non-empty stream.
* **Complexity**: Time `O(N log k)`, space `O(k)`.
* **Optimizations**: Runtime: pairwise merge can improve locality. Memory: output streaming avoids storing all results.
* **Edge Cases To Consider**: Empty list set, empty sublists, duplicates, one stream.
* **L7 Follow-ups**: For external files, heap size remains bounded by number of files.

## 4. Meeting Rooms II

* **Pattern / Idea**: Min-heap of active meeting end times.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 47.7)`, `Oracle: Medium (6m 47.0)`, `Microsoft: Medium (6m 44.1)`, `Snowflake: Medium (all 39.6)`, `NVIDIA: Medium (all 38.4)`.
* **Question**: Given meeting intervals, return the minimum number of rooms required.
* **Test Cases**: [Test cases](./test_cases.md#4-meeting-rooms-ii).
* **C++ Code**
  ```cpp
  int minMeetingRooms(vector<pair<int, int>> meetings) {
      sort(meetings.begin(), meetings.end());
      priority_queue<int, vector<int>, greater<int>> ends;
      int best = 0;
      for (auto [s, e] : meetings) {
          while (!ends.empty() && ends.top() <= s) ends.pop();
          ends.push(e);
          best = max(best, static_cast<int>(ends.size()));
      }
      return best;
  }
  ```
* **Code Explanation**: Active meetings are exactly those whose end time is greater than the next start.
* **Invariants**: Heap contains end times for meetings currently using rooms.
* **Complexity**: Time `O(n log n)`, space `O(n)`.
* **Optimizations**: Runtime: two sorted arrays can reduce heap overhead. Memory: heap size is peak concurrency.
* **Edge Cases To Consider**: Touching intervals, all overlap, none overlap, empty input.
* **L7 Follow-ups**: For calendar systems, specify time zone and half-open interval semantics.

## 5. Calendar Booking Without Overlap

* **Pattern / Idea**: Ordered map predecessor/successor checks.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Implement a calendar booking API that rejects overlapping intervals.
* **Test Cases**: [Test cases](./test_cases.md#5-calendar-booking-without-overlap).
* **C++ Code**
  ```cpp
  class MyCalendar {
      map<int, int> byStart;
  public:
      bool book(int start, int end) {
          auto next = byStart.lower_bound(start);
          if (next != byStart.end() && next->first < end) return false;
          if (next != byStart.begin()) {
              auto prev = std::prev(next);
              if (prev->second > start) return false;
          }
          byStart[start] = end;
          return true;
      }
  };
  ```
* **Code Explanation**: In a sorted non-overlapping set, only the adjacent intervals can conflict.
* **Invariants**: Stored intervals are sorted and non-overlapping.
* **Complexity**: Time `O(log n)` per booking, space `O(n)`.
* **Optimizations**: Runtime: adjacency check only. Memory: store accepted intervals only.
* **Edge Cases To Consider**: Touching intervals, nested interval rejection, duplicate start, empty calendar.
* **L7 Follow-ups**: For recurring events, expand lazily or store recurrence rules separately.

## 6. Top K Frequent Words

* **Pattern / Idea**: Count then maintain worst retained candidate.
* **Company Frequency Tags**: Public signal: `Apple: High (6m 81.7)`, `Oracle: High (6m 81.2)`, `Snowflake: Medium (all 39.6)`.
* **Question**: Given a stream of words, return the top `k` most frequent words with lexical tie-breaking.
* **Test Cases**: [Test cases](./test_cases.md#6-top-k-frequent-words).
* **C++ Code**
  ```cpp
  vector<string> topKFrequentWords(const vector<string>& words, int k) {
      unordered_map<string, int> freq;
      for (const string& w : words) ++freq[w];
      auto worse = [&](const string& a, const string& b) {
          if (freq[a] != freq[b]) return freq[a] > freq[b];
          return a < b;
      };
      priority_queue<string, vector<string>, decltype(worse)> pq(worse);
      for (auto& [w, _] : freq) {
          pq.push(w);
          if (static_cast<int>(pq.size()) > k) pq.pop();
      }
      vector<string> ans;
      while (!pq.empty()) { ans.push_back(pq.top()); pq.pop(); }
      reverse(ans.begin(), ans.end());
      return ans;
  }
  ```
* **Code Explanation**: The heap keeps the best `k` words while the comparator exposes the worst retained word at the top.
* **Invariants**: Heap contains at most `k` best candidates among processed unique words.
* **Complexity**: Time `O(n + u log k)`, space `O(u + k)`.
* **Optimizations**: Runtime: bucket by frequency if max frequency is small. Memory: stream approximate heavy hitters for huge `u`.
* **Edge Cases To Consider**: Lexical ties, `k > unique`, duplicate-heavy input.
* **L7 Follow-ups**: For distributed top-K, merge local candidates and account for skew.

## 7. Sliding Window Median

* **Pattern / Idea**: Two ordered multisets for active window halves.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 38.9)`, `Snowflake: Medium (all 55.0)`.
* **Question**: Given a stream of numbers and window size `k`, return the median for each sliding window.
* **Test Cases**: [Test cases](./test_cases.md#7-sliding-window-median).
* **C++ Code**
  ```cpp
  class WindowMedian {
      multiset<int> low, high;
      void rebalance() {
          while (low.size() > high.size() + 1) {
              high.insert(*low.rbegin());
              low.erase(prev(low.end()));
          }
          while (high.size() > low.size()) {
              low.insert(*high.begin());
              high.erase(high.begin());
          }
      }
  public:
      void add(int x) {
          if (low.empty() || x <= *low.rbegin()) low.insert(x);
          else high.insert(x);
          rebalance();
      }
      void remove(int x) {
          auto it = low.find(x);
          if (it != low.end()) low.erase(it);
          else high.erase(high.find(x));
          rebalance();
      }
      double median() const {
          if (low.size() == high.size()) return (*low.rbegin() / 2.0) + (*high.begin() / 2.0);
          return *low.rbegin();
      }
  };

  vector<double> slidingWindowMedian(const vector<int>& nums, int k) {
      WindowMedian wm;
      vector<double> ans;
      for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
          wm.add(nums[i]);
          if (i >= k) wm.remove(nums[i - k]);
          if (i >= k - 1) ans.push_back(wm.median());
      }
      return ans;
  }
  ```
* **Code Explanation**: Ordered multisets support insert, delete, and median lookup for exactly the current window.
* **Invariants**: `low` contains lower half, `high` upper half, and sizes differ by at most one.
* **Complexity**: Time `O(n log k)`, space `O(k)`.
* **Optimizations**: Runtime: lazy heaps can reduce constants but complicate cleanup. Memory: multisets store only active window.
* **Edge Cases To Consider**: Duplicates, even `k`, `k = 1`, negative values.
* **L7 Follow-ups**: Approximate quantile sketches reduce memory for massive windows.

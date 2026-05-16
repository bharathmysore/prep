# Binary Search Top Interview Solutions

Snippets assume C++17 standard headers and `using namespace std;`.

## 1. Median Of Two Sorted Arrays

* **Pattern / Idea**: Binary search smaller partition.
* **Company Frequency Tags**: Public signal: `Google: High (6m 68.5)`, `Microsoft: High (6m 64.1)`, `Meta: Medium (6m 53.8)`, `Apple: High (all 75.8)`, `Amazon/AWS: High (all 75.3)`, `Oracle: Medium (all 51.8)`.
* **Question**: Given two sorted arrays, find their median without merging.
* **Test Cases**: [Test cases](./test_cases.md#1-median-of-two-sorted-arrays).
* **C++ Code**
  ```cpp
  double medianTwoSorted(const vector<int>& a, const vector<int>& b) {
      if (a.size() > b.size()) return medianTwoSorted(b, a);
      int n = a.size(), m = b.size(), totalLeft = (n + m + 1) / 2;
      int lo = 0, hi = n;
      while (lo <= hi) {
          int i = lo + (hi - lo) / 2;
          int j = totalLeft - i;
          int aL = i == 0 ? INT_MIN : a[i - 1];
          int aR = i == n ? INT_MAX : a[i];
          int bL = j == 0 ? INT_MIN : b[j - 1];
          int bR = j == m ? INT_MAX : b[j];
          if (aL <= bR && bL <= aR) {
              if ((n + m) % 2) return max(aL, bL);
              return (max(aL, bL) / 2.0) + (min(aR, bR) / 2.0);
          }
          if (aL > bR) hi = i - 1;
          else lo = i + 1;
      }
      throw invalid_argument("inputs not sorted");
  }
  ```
* **Code Explanation**: Find a partition where everything on the left is `<=` everything on the right.
* **Invariants**: The correct partition remains in `[lo, hi]`.
* **Complexity**: Time `O(log min(n, m))`, space `O(1)`.
* **Optimizations**: Runtime: search smaller array. Memory: no merge.
* **Edge Cases To Consider**: One empty array, odd/even total, duplicates, extreme integers.
* **L7 Follow-ups**: For streaming medians, use heaps instead.

## 2. Search Rotated Sorted Array

* **Pattern / Idea**: Binary search with one sorted half.
* **Company Frequency Tags**: Public signal: `NVIDIA: High (6m 95.8)`, `Amazon/AWS: High (6m 73.1)`, `Oracle: High (6m 69.5)`, `Microsoft: Medium (6m 54.5)`, `Google: Medium (6m 45.7)`, `Meta: Medium (6m 44.9)`, `Apple: Medium (6m 38.9)`.
* **Question**: Search for a target in a rotated sorted array.
* **Test Cases**: [Test cases](./test_cases.md#2-search-rotated-sorted-array).
* **C++ Code**
  ```cpp
  int searchRotated(const vector<int>& nums, int target) {
      int lo = 0, hi = static_cast<int>(nums.size()) - 1;
      while (lo <= hi) {
          int mid = lo + (hi - lo) / 2;
          if (nums[mid] == target) return mid;
          if (nums[lo] <= nums[mid]) {
              if (nums[lo] <= target && target < nums[mid]) hi = mid - 1;
              else lo = mid + 1;
          } else {
              if (nums[mid] < target && target <= nums[hi]) lo = mid + 1;
              else hi = mid - 1;
          }
      }
      return -1;
  }
  ```
* **Code Explanation**: At least one side of a rotated sorted array is sorted; use that side to discard half.
* **Invariants**: Target, if present, remains in `[lo, hi]`.
* **Complexity**: Time `O(log n)`, space `O(1)`.
* **Optimizations**: Runtime: no pivot prepass. Memory: iterative loop.
* **Edge Cases To Consider**: Not rotated, rotated by one, one element, missing target.
* **L7 Follow-ups**: With duplicates, worst-case can degrade to `O(n)`.

## 3. Koko Eating Bananas

* **Pattern / Idea**: Binary search minimum feasible speed.
* **Company Frequency Tags**: Public signal: `Oracle: Medium (6m 56.9)`.
* **Question**: Given piles of bananas and hours `h`, find the minimum eating speed.
* **Test Cases**: [Test cases](./test_cases.md#3-koko-eating-bananas).
* **C++ Code**
  ```cpp
  int minEatingSpeed(const vector<int>& piles, int h) {
      int lo = 1, hi = *max_element(piles.begin(), piles.end());
      auto ok = [&](int speed) {
          long long hours = 0;
          for (int p : piles) {
              hours += (p + speed - 1LL) / speed;
              if (hours > h) return false;
          }
          return true;
      };
      while (lo < hi) {
          int mid = lo + (hi - lo) / 2;
          if (ok(mid)) hi = mid;
          else lo = mid + 1;
      }
      return lo;
  }
  ```
* **Code Explanation**: If a speed works, any higher speed works; find first true.
* **Invariants**: Answer is always in `[lo, hi]`.
* **Complexity**: Time `O(n log maxPile)`, space `O(1)`.
* **Optimizations**: Runtime: early stop in feasibility. Memory: no auxiliary state.
* **Edge Cases To Consider**: One pile, `h == piles.size()`, large pile values.
* **L7 Follow-ups**: State monotonic predicate before coding answer-space search.

## 4. Ship Packages Within D Days

* **Pattern / Idea**: Binary search capacity with greedy feasibility.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Given package weights and `D` days, find the minimum ship capacity.
* **Test Cases**: [Test cases](./test_cases.md#4-ship-packages-within-d-days).
* **C++ Code**
  ```cpp
  int shipWithinDays(const vector<int>& weights, int days) {
      int lo = *max_element(weights.begin(), weights.end());
      int hi = accumulate(weights.begin(), weights.end(), 0);
      auto ok = [&](int cap) {
          int used = 1, cur = 0;
          for (int w : weights) {
              if (cur + w > cap) { ++used; cur = 0; }
              cur += w;
          }
          return used <= days;
      };
      while (lo < hi) {
          int mid = lo + (hi - lo) / 2;
          if (ok(mid)) hi = mid;
          else lo = mid + 1;
      }
      return lo;
  }
  ```
* **Code Explanation**: Greedily filling days minimizes days used for a fixed capacity.
* **Invariants**: Capacities below `lo` are infeasible; `hi` is feasible.
* **Complexity**: Time `O(n log sumWeights)`, space `O(1)`.
* **Optimizations**: Runtime: lower bound is max weight. Memory: one scan per predicate.
* **Edge Cases To Consider**: `days = 1`, `days = n`, large sums, one package.
* **L7 Follow-ups**: Explain why greedy feasibility is valid.

## 5. Kth Smallest In Sorted Matrix

* **Pattern / Idea**: Value-space binary search.
* **Company Frequency Tags**: Public signal: `Apple: Medium (6m 54.0)`.
* **Question**: Given a matrix sorted by rows and columns, find the kth smallest value.
* **Test Cases**: [Test cases](./test_cases.md#5-kth-smallest-in-sorted-matrix).
* **C++ Code**
  ```cpp
  int kthSmallestMatrix(const vector<vector<int>>& matrix, int k) {
      int n = matrix.size();
      int lo = matrix[0][0], hi = matrix[n - 1][n - 1];
      auto countLE = [&](int x) {
          int r = n - 1, c = 0, cnt = 0;
          while (r >= 0 && c < n) {
              if (matrix[r][c] <= x) {
                  cnt += r + 1;
                  ++c;
              } else {
                  --r;
              }
          }
          return cnt;
      };
      while (lo < hi) {
          int mid = lo + (hi - lo) / 2;
          if (countLE(mid) >= k) hi = mid;
          else lo = mid + 1;
      }
      return lo;
  }
  ```
* **Code Explanation**: Count of values `<= x` is monotonic in `x`; staircase count is linear per probe.
* **Invariants**: The kth smallest value remains in `[lo, hi]`.
* **Complexity**: Time `O(n log valueRange)`, space `O(1)`.
* **Optimizations**: Runtime: heap can be better for small `k`. Memory: value search avoids heap.
* **Edge Cases To Consider**: Duplicates, `k = 1`, `k = n^2`, negative values.
* **L7 Follow-ups**: Distinguish value range complexity from index range complexity.

## 6. Split Array Largest Sum

* **Pattern / Idea**: Minimize maximum partition sum via binary search.
* **Company Frequency Tags**: Public signal: none in reviewed public CSVs.
* **Question**: Split an array into `m` non-empty subarrays minimizing the largest subarray sum.
* **Test Cases**: [Test cases](./test_cases.md#6-split-array-largest-sum).
* **C++ Code**
  ```cpp
  long long splitArrayLargestSum(const vector<int>& nums, int m) {
      long long lo = *max_element(nums.begin(), nums.end());
      long long hi = accumulate(nums.begin(), nums.end(), 0LL);
      auto ok = [&](long long limit) {
          int parts = 1;
          long long cur = 0;
          for (int x : nums) {
              if (cur + x > limit) { ++parts; cur = 0; }
              cur += x;
          }
          return parts <= m;
      };
      while (lo < hi) {
          long long mid = lo + (hi - lo) / 2;
          if (ok(mid)) hi = mid;
          else lo = mid + 1;
      }
      return lo;
  }
  ```
* **Code Explanation**: For a fixed max sum, greedily creating partitions minimizes the number of partitions.
* **Invariants**: `lo` excludes impossible low limits; `hi` is always feasible.
* **Complexity**: Time `O(n log sum)`, space `O(1)`.
* **Optimizations**: Runtime: greedy predicate avoids `O(nm)` DP. Memory: no DP table.
* **Edge Cases To Consider**: `m = 1`, `m = n`, large values, already balanced arrays.
* **L7 Follow-ups**: If negative numbers are allowed, the greedy monotonic argument changes.

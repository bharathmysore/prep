# Heaps And Ordered Structures Coding Questions

Solve each question in C++ and explain heap ordering, stale entries, or ordered-map predecessor/successor logic.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Given an array, return the kth largest element.
   - Expected pattern: min-heap of size `k` or quickselect.
   - Pattern tags: `heap`, `top-k`.
   - Solution: [Kth Largest Element](./solutions.md#1-kth-largest-element).
   - Complexity target: heap time `O(n log k)`, space `O(k)`; quickselect average time `O(n)`, space `O(1)`.

2. Implement a streaming median data structure.
   - Expected pattern: max-heap for lower half and min-heap for upper half.
   - Pattern tags: `two-heaps`, `streaming-median`.
   - Solution: [Streaming Median](./solutions.md#2-streaming-median).
   - Complexity target: update `O(log n)`, median `O(1)`, space `O(n)`.

3. Given `k` sorted arrays or lists, merge them into sorted order.
   - Expected pattern: min-heap of stream heads.
   - Pattern tags: `heap`, `k-way-merge`.
   - Solution: [Merge K Sorted Streams](./solutions.md#3-merge-k-sorted-streams).
   - Complexity target: time `O(N log k)`, space `O(k)`.

4. Given meeting intervals, return the minimum number of rooms required.
   - Expected pattern: sort by start and min-heap of end times.
   - Pattern tags: `min-heap`, `sweep-line`.
   - Solution: [Meeting Rooms II](./solutions.md#4-meeting-rooms-ii).
   - Complexity target: time `O(n log n)`, space `O(n)`.

5. Implement a calendar booking API that rejects overlapping intervals.
   - Expected pattern: ordered map with predecessor and successor checks.
   - Pattern tags: `ordered-map`, `intervals`.
   - Solution: [Calendar Booking Without Overlap](./solutions.md#5-calendar-booking-without-overlap).
   - Complexity target: time `O(log n)` per booking, space `O(n)`.

<a id="6-top-k-frequent-words"></a>
6. Given a stream of words, return the top `k` most frequent words with lexical tie-breaking.
   - Expected pattern: hash counts plus custom heap or ordered set.
   - Pattern tags: `heap`, `top-k`, `custom-comparator`.
   - Solution: [Top K Frequent Words](./solutions.md#6-top-k-frequent-words).
   - Complexity target: time `O(n log k)` after counting, space `O(unique)`.

7. Given a stream of numbers and window size `k`, return the median for each sliding window.
   - Expected pattern: two ordered multisets or two heaps with lazy deletion.
   - Pattern tags: `sliding-window-median`, `ordered-multiset`.
   - Solution: [Sliding Window Median](./solutions.md#7-sliding-window-median).
   - Complexity target: time `O(n log k)`, space `O(k)`.

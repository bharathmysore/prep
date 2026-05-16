# Heaps Ordered Structures Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Kth Largest Element

* **Question**: Given an array, return the kth largest element.
* **Solution**: [Kth Largest Element](./solutions.md#1-kth-largest-element).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[3,2,1,5,6,4]`, k=2 | Return `5`. |
| Duplicates | `[3,2,3,1,2,4,5,5,6]`, k=4 | Return `4`. |
| k=1 | any array | Return max element. |

## 2. Streaming Median

* **Question**: Implement a streaming median data structure.
* **Solution**: [Streaming Median](./solutions.md#2-streaming-median).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Odd/even stream | add 1,2,3 | Medians are `1`, `1.5`, `2`. |
| Negative values | add `-1,-2,-3` | Median updates correctly. |
| Duplicate values | add repeated numbers | Median handles equal values. |

## 3. Merge K Sorted Streams

* **Question**: Given `k` sorted arrays or lists, merge them into sorted order.
* **Solution**: [Merge K Sorted Streams](./solutions.md#3-merge-k-sorted-streams).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | lists `[1,4,5]`, `[1,3,4]`, `[2,6]` | Return `[1,1,2,3,4,4,5,6]`. |
| Empty lists | some streams empty | Ignored without failure. |
| One stream | single sorted stream | Return it unchanged. |

## 4. Meeting Rooms II

* **Question**: Given meeting intervals, return the minimum number of rooms required.
* **Solution**: [Meeting Rooms II](./solutions.md#4-meeting-rooms-ii).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Overlap | intervals `[[0,30],[5,10],[15,20]]` | Return `2`. |
| No overlap | `[[7,10],[2,4]]` | Return `1`. |
| Same start | multiple meetings start together | Rooms equal concurrent starts. |

## 5. Calendar Booking Without Overlap

* **Question**: Implement a calendar booking API that rejects overlapping intervals.
* **Solution**: [Calendar Booking Without Overlap](./solutions.md#5-calendar-booking-without-overlap).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Accept disjoint | book `[10,20)`, then `[20,30)` | Both accepted. |
| Reject overlap | book `[10,20)`, then `[15,25)` | Second rejected. |
| Nested overlap | existing `[10,30)`, new `[15,20)` | Rejected. |

## 6. Top K Frequent Words

* **Question**: Given a stream of words, return the top `k` most frequent words with lexical tie-breaking.
* **Solution**: [Top K Frequent Words](./solutions.md#6-top-k-frequent-words).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | words `i,love,leetcode,i,love,coding`, k=2 | Return `[i,love]`. |
| Lex tie | `the,day,is,sunny,the,the,the,sunny,is,is`, k=4 | Return `[the,is,sunny,day]`. |
| k exceeds unique | k larger than unique words | Return all unique words in rank order. |

## 7. Sliding Window Median

* **Question**: Given a stream of numbers and window size `k`, return the median for each sliding window.
* **Solution**: [Sliding Window Median](./solutions.md#7-sliding-window-median).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | nums `[1,3,-1,-3,5,3,6,7]`, k=3 | Return `[1,-1,-1,3,5,6]`. |
| Even k | `[1,2,3,4]`, k=2 | Return `[1.5,2.5,3.5]`. |
| Duplicates | window with repeated values | Median removes only one outgoing instance. |

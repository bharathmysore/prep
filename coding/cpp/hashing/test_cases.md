# Hashing Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. First Non-Repeating Character In A Stream

* **Question**: Given a stream of characters, return the first non-repeating character after each update.
* **Solution**: [First Non-Repeating Character In A Stream](./solutions.md#1-first-non-repeating-character-in-a-stream).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Stream | `aabc` | First unique sequence can be `a, -1, b, b`. |
| All repeats | `aabb` | Eventually no unique character. |
| Late unique | repeated prefix then `z` | Returns `z` when it becomes first unique. |

## 2. Group Anagrams

* **Question**: Given strings, group anagrams together.
* **Solution**: [Group Anagrams](./solutions.md#2-group-anagrams).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `eat,tea,tan,ate,nat,bat` | Groups are `{eat,tea,ate}`, `{tan,nat}`, `{bat}`. |
| Empty string | `[""]` | One group containing empty string. |
| Case policy | `Eat` and `tea` | Grouping follows stated case-sensitivity policy. |

## 3. Longest Consecutive Sequence

* **Question**: Given an integer array, return the length of the longest consecutive sequence.
* **Solution**: [Longest Consecutive Sequence](./solutions.md#3-longest-consecutive-sequence).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Canonical | `[100,4,200,1,3,2]` | Return `4`. |
| Duplicates | `[1,2,2,3]` | Return `3`. |
| Empty | `[]` | Return `0`. |

## 4. Randomized Set

* **Question**: Design a randomized set supporting insert, delete, and getRandom in average `O(1)`.
* **Solution**: [Randomized Set](./solutions.md#4-randomized-set).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Insert/remove/get | insert 1, remove 2, insert 2, getRandom | Operations return true/false correctly; random returns 1 or 2. |
| Remove existing | remove 1 after insert | Element is gone and index map stays valid. |
| Duplicate insert | insert same value twice | Second insert returns false. |

## 5. TTL Deduplication Store

* **Question**: Given an event stream, suppress duplicate event ids within a retention window.
* **Solution**: [TTL Deduplication Store](./solutions.md#5-ttl-deduplication-store).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| First event | key K at time 10 | Accepted and stored. |
| Duplicate inside TTL | same key before expiry | Rejected as duplicate. |
| After TTL | same key after expiry | Accepted again and old entry cleaned. |

## 6. Isomorphic Strings

* **Question**: Given two strings, determine whether they are isomorphic.
* **Solution**: [Isomorphic Strings](./solutions.md#6-isomorphic-strings).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| True | `egg`, `add` | Return true. |
| False | `foo`, `bar` | Return false. |
| Two-to-one conflict | `ab`, `aa` | Return false. |

## 7. Two Sum

* **Question**: Return distinct zero-based indices whose values sum to the target.
* **Solution**: [Two Sum](./solutions.md#7-two-sum).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Basic | `nums=[2,7,11,15]`, `target=9` | `[0,1]` |
| Duplicate values | `nums=[3,3]`, `target=6` | `[0,1]`, using different indices |
| Unsorted | `nums=[3,2,4]`, `target=6` | `[1,2]` |
| Signed values | `nums=[-3,4,3,90]`, `target=0` | `[0,2]` |

## 8. Valid Anagram

* **Question**: Decide whether two lowercase strings have the same character multiset.
* **Solution**: [Valid Anagram](./solutions.md#8-valid-anagram).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Reordered | `s="anagram"`, `t="nagaram"` | `true` |
| Equal length, wrong counts | `s="rat"`, `t="car"` | `false` |
| Repeated letters | `s="aabb"`, `t="abab"` | `true` |
| Different lengths | `s="a"`, `t="aa"` | `false` |

## 9. Top K Frequent Elements

* **Question**: Return the `k` values with greatest frequency in any order.
* **Solution**: [Top K Frequent Elements](./solutions.md#9-top-k-frequent-elements).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Basic | `nums=[1,1,1,2,2,3]`, `k=2` | `{1,2}` in any order |
| One value | `nums=[1]`, `k=1` | `[1]` |
| All distinct | `nums=[4,2,7]`, `k=3` | `{4,2,7}` in any order |
| Negative values | `nums=[-1,-1,2,2,2,3]`, `k=1` | `[2]` |

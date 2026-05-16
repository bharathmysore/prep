# Hashing Coding Questions

Solve each question in C++ with average and worst-case behavior called out where relevant.

Pattern tags use kebab-case backtick labels and are mirrored in [patterns](./patterns.md).

## Questions

1. Given a stream of characters, return the first non-repeating character after each update.
   - Expected pattern: frequency map plus queue with lazy stale removal.
   - Pattern tags: `frequency-map`, `queue`, `lazy-deletion`.
   - Solution: [First Non-Repeating Character In A Stream](./solutions.md#1-first-non-repeating-character-in-a-stream).
   - Complexity target: amortized time `O(1)` per character, space `O(k)`.

<a id="2-group-anagrams"></a>
2. Given strings, group anagrams together.
   - Expected pattern: normalized hashable signature.
   - Pattern tags: `signature-hashing`, `grouping`.
   - Solution: [Group Anagrams](./solutions.md#2-group-anagrams).
   - Complexity target: time `O(total chars)` for bounded alphabet signatures, space `O(total chars)`.

3. Given an integer array, return the length of the longest consecutive sequence.
   - Expected pattern: hash set and start-of-run detection.
   - Pattern tags: `hash-set`, `start-of-run`.
   - Solution: [Longest Consecutive Sequence](./solutions.md#3-longest-consecutive-sequence).
   - Complexity target: time `O(n)` average, space `O(n)`.

4. Design a randomized set supporting insert, delete, and getRandom in average `O(1)`.
   - Expected pattern: vector plus index hash map.
   - Pattern tags: `vector-index-map`, `randomized-structure`.
   - Solution: [Randomized Set](./solutions.md#4-randomized-set).
   - Complexity target: average time `O(1)`, space `O(n)`.

5. Given an event stream, suppress duplicate event ids within a retention window.
   - Expected pattern: hash set plus expiry queue.
   - Pattern tags: `deduplication`, `ttl-window`.
   - Solution: [TTL Deduplication Store](./solutions.md#5-ttl-deduplication-store).
   - Complexity target: average time `O(1)` per event, space `O(window)`.

6. Given two strings, determine whether they are isomorphic.
   - Expected pattern: bidirectional character mapping.
   - Pattern tags: `bidirectional-map`, `bijection`.
   - Solution: [Isomorphic Strings](./solutions.md#6-isomorphic-strings).
   - Complexity target: time `O(n)`, space `O(k)`.

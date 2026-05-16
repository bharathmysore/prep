# Hashing Coding Patterns

Use these problems to practice choosing between hash maps, ordered maps, sorting, and specialized arrays. In L7 interviews, always discuss average vs worst-case behavior and adversarial inputs.

Pattern tags are mirrored from [questions](./questions.md); keep both directions in sync when adding or changing prompts.

## Pattern Problems

| Pattern problem | Recognition signal | Core invariant | Target complexity | Optimization notes |
| --- | --- | --- | --- | --- |
| Group anagrams | Group strings by normalized signature | Equal signatures imply same character multiset | Time `O(total chars)`, space `O(total chars)` | Runtime: count signature for bounded alphabet. Memory: avoid sorting each string when alphabet is small. |
| Top K frequent elements | Need most common keys | Heap or bucket stores best candidates seen so far | Time `O(n log k)` or `O(n)`, space `O(n)` | Runtime: bucket sort when frequency range is bounded by `n`. Memory: min-heap keeps only `k` candidates. |
| Longest consecutive sequence | Unordered values, longest run | Start only at numbers with no predecessor | Time `O(n)` average, space `O(n)` | Runtime: scan starts only once. Memory: use unordered set with reserved capacity. |
| Count pairs with target sum | Count pairs rather than return one | Prior frequencies represent all valid left endpoints | Time `O(n)` average, space `O(n)` | Runtime: one pass. Memory: if value range is small, replace map with vector counts. |
| First unique character in stream | Online uniqueness | Queue front is unique after stale entries are removed | Amortized time `O(1)`, space `O(k)` | Runtime: lazy deletion. Memory: fixed frequency array for bounded alphabet. |
| Randomized set insert/delete/getRandom | Need `O(1)` updates and random access | Map index always points to the key's vector slot | Average time `O(1)`, space `O(n)` | Runtime: swap-delete. Memory: store compact vector plus index map. |
| Memoized recursion | Overlapping subproblems | Cache value is final for each state once written | Time `O(states * transition)`, space `O(states)` | Runtime: encode state compactly. Memory: use arrays when state bounds are dense. |
| Hash-based deduplication | Need suppress duplicate events | Seen set contains all ids inside retention window | Time `O(n)` average, space `O(window)` | Runtime: combine hash set with expiry queue. Memory: TTL eviction bounds state. |
| Subarray with equal zero and one | Binary condition can be converted to prefix balance | Same balance at two indices means neutral segment | Time `O(n)`, space `O(n)` | Runtime: store earliest balance only. Memory: array offset if balance range is bounded. |
| Detect isomorphic strings | Need one-to-one mapping | Forward and reverse maps agree for all visited chars | Time `O(n)`, space `O(k)` | Runtime: fixed arrays for chars. Memory: store last-seen positions instead of maps. |

## Pattern Tag Map

| Pattern tags | Pattern coverage | Related questions |
| --- | --- | --- |
| `frequency-map`, `queue`, `lazy-deletion` | frequency map plus queue with lazy stale removal | [questions](./questions.md): Q1 |
| `signature-hashing`, `grouping` | normalized hashable signature | [questions](./questions.md): Q2 |
| `hash-set`, `start-of-run` | hash set and start-of-run detection | [questions](./questions.md): Q3 |
| `vector-index-map`, `randomized-structure` | vector plus index hash map | [questions](./questions.md): Q4 |
| `deduplication`, `ttl-window` | hash set plus expiry queue | [questions](./questions.md): Q5 |
| `bidirectional-map`, `bijection` | bidirectional character mapping | [questions](./questions.md): Q6 |

## L7 Follow-Ups

- What happens under collision-heavy or adversarial keys?
- When would sorting be preferable to hashing despite worse asymptotic complexity?
- How do you bound memory for streaming deduplication?
- What observability would you add around hash-table growth or hot keys?

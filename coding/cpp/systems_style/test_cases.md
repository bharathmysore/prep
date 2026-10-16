# Systems Style Test Cases

Concrete test cases live here so solution explanations stay focused on approach, invariants, complexity, and tradeoffs.

## 1. Delayed Job Scheduler

* **Question**: Implement a delayed job scheduler that runs jobs at or after their due time.
* **Solution**: [Delayed Job Scheduler](./solutions.md#1-delayed-job-scheduler).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Due order | schedule jobs at t+10 and t+5 | t+5 job runs first. |
| Same deadline | multiple jobs same due time | All run once; tie order follows contract. |
| Cancel/shutdown | shutdown with pending jobs | Workers wake and pending jobs are handled according to shutdown policy. |

## 2. Retry Queue With Backoff

* **Question**: Implement a retry queue that schedules failed jobs using bounded exponential backoff and retry limits.
* **Solution**: [Retry Queue With Backoff](./solutions.md#2-retry-queue-with-backoff).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Retry then success | job fails twice then succeeds | Attempts scheduled with increasing delays; stops after success. |
| Max attempts | job always fails | Stops after retry budget and reports dead-letter/failure. |
| Jitter bounds | many jobs fail together | Next attempt times are spread within configured jitter bounds. |

## 3. Message Broker With Visibility Timeout

* **Question**: Implement a message broker with enqueue, consume, ack, and visibility timeout.
* **Solution**: [Message Broker With Visibility Timeout](./solutions.md#3-message-broker-with-visibility-timeout).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Ack success | consumer receives and acks before timeout | Message is removed and not redelivered. |
| Timeout redelivery | consumer receives but does not ack | Message becomes visible again after timeout. |
| Duplicate ack | ack same receipt twice | Second ack is ignored or rejected idempotently. |

## 4. Config Snapshot Manager

* **Question**: Implement a config snapshot manager that allows lock-light reads and atomic updates.
* **Solution**: [Config Snapshot Manager](./solutions.md#4-config-snapshot-manager).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Versioned publish | publish v1 then v2 | Readers can fetch latest v2 and older v1 if retained. |
| Atomic snapshot | reader observes during publish | Reader sees complete old or complete new snapshot. |
| Retention | many versions published | Old versions are compacted according to retention policy. |

## 5. Leaderboard

* **Question**: Implement a compact in-memory leaderboard with update score and query top `k`.
* **Solution**: [Leaderboard](./solutions.md#5-leaderboard).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Top K | scores A=10, B=20, C=15; top 2 | Return B then C. |
| Score update | A changes from 10 to 30 | A moves to rank 1. |
| Tie policy | two users same score | Order follows deterministic tie-breaker. |

## 6. Sharded Key-Value Store Simulator

* **Question**: Implement bounded put/get/erase operations with deterministic ownership for fixed shard membership.
* **Solution**: [Sharded Key-Value Store Simulator](./solutions.md#6-sharded-key-value-store-simulator).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid construction | shards=0, or key/value byte limit=0 | `invalid_argument`. |
| Empty and missing | 3 shards, capacity=2, byte limits=8; get missing; put empty key/value | Missing gives `nullopt`; empty strings round-trip; size=1. |
| Full overwrite | capacity=1; put a=x, overwrite a=y, put b=z | First two succeed, third returns false; get a=y and size=1. |
| Reclaim capacity | erase a twice, then put b=z | Erases return true then false; insertion succeeds. |
| Byte validation | key limit=2, value limit=3; use key abc or value wxyz | `length_error`; existing entries and size remain unchanged. |
| Ownership | Insert many keys, inspect every shard | `owner(key)` stays stable, sum of shard sizes equals global size; single-shard store routes everything to 0. |
| Inspection boundary | Inspect shard index equal to shard count | `out_of_range`. |
| Zero capacity | put empty key/value at capacity=0 | false; lookups/erases remain empty. |
| Reference model | Fixed-seed inserts, overwrites, gets, and erases with capacity=64 | Results match a capacity-bounded reference map throughout; no ownership or size drift. |

## 7. Rolling-Window Top-K Log Aggregator

* **Question**: Ingest bounded log messages and return exact top-k counts over a rolling time window with deterministic ties.
* **Solution**: [Rolling-Window Top-K Log Aggregator](./solutions.md#7-rolling-window-top-k-log-aggregator).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid construction | window=0 or -1; message byte limit=0 | `invalid_argument`. |
| Ranking and ties | window=10; ingest b,a,b,a,c at t=0; query k=9 | a:2, b:2, c:1, in that order. |
| Shared counts across buckets | ingest x twice at t=0, x at t=1, y at t=9; query t=10 | t=0 bucket expires; x:1 then y:1. |
| Exact boundary | window=10; event at t=1; query t=10 then t=11 | Retained at t=10; absent at t=11. |
| Capacity and time | capacity=1, window=10; ingest a at t=0, reject b at t=1, then call t=0 | Reject advances clock; backward query throws; insertion at t=10 succeeds after expiry. |
| k boundaries | k=0 and k above active unique count | Empty result for zero; all active uniques for large k; zero query still expires events. |
| Heap replacement / cutoff ties | Ingest z:5, b:3, a:3, y:1, x:2 at t=0; query k=1,2,3 | z alone; z,a; z,a,b. Lower count loses first, lexically larger message loses equal-count cutoff. |
| Many equal counts | Ingest j through a once each at the same timestamp; query k=4 | a,b,c,d regardless of hash traversal order. |
| Invalid calls | Negative/backward time, oversize message | Documented exception; failed validation does not advance time or alter counts. |
| Zero capacity / empty message | capacity=0; separately, ingest empty message with positive capacity | Zero-capacity ingestion returns false; empty message is a valid ranked key. |
| Integer limit | window=LLONG_MAX; ingest at 0, query LLONG_MAX, then ingest/query at LLONG_MAX | Old event expires exactly; new event remains with no signed overflow. |
| Reference model | Fixed-seed events and queries, cap=17, window=11 | Match exact event-deque counts, lexical ties, capacity rejections, and active size after every step. |

## 8. Load Balancer With Three Policies

* **Question**: Dispatch requests using three policies and accept only matching current completion tickets.
* **Solution**: [Load Balancer With Three Policies](./solutions.md#8-load-balancer-with-three-policies).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid configuration/policy | Empty weights, nonpositive weight, or cast policy=99 | `invalid_argument`; invalid policy also throws at capacity=0. |
| Round-robin | weights=[2,1,3], six RR dispatches | backend indices 0,1,2,0,1,2. |
| Burst weighting | weights=[2,1,3], two complete weighted cycles | 0,0,1,2,2,2 repeated; exact configured counts. |
| Least-loaded tie | Three empty backends; three dispatches; complete backend 1; dispatch again | 0,1,2 then 1; equal-load ties always choose lowest index. |
| Capacity / cursor | cap=1; RR dispatch, rejected dispatch, completion, RR dispatch | Rejection does not advance cursor; accepted indices are 0 then 1. |
| Stale/mismatched completion | Reuse old token or change a live ticket's backend | false; current loads unchanged; matching ticket completes once only. |
| Zero cap / one backend | cap=0; separately weights=[1] | No admission at zero; every policy selects backend 0 otherwise. |
| Extreme weight | weights=[INT_MAX,1]; several weighted dispatches | First backend continues its burst without expanded memory or arithmetic overflow. |
| Inspection boundary | load(index=backend count) | `out_of_range`. |
| Mixed-policy model | Fixed-seed dispatch/completion operations at cap=7 | All policy choices match independent counters; sum of loads equals outstanding count and never exceeds 7. |

## 9. Backpressure Controller With Hysteresis

* **Question**: Return Admit, Delay, or Reject for a caller-owned bounded queue with hysteresis and bounded policy state.
* **Solution**: [Backpressure Controller With Hysteresis](./solutions.md#9-backpressure-controller-with-hysteresis).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid parameters | cap=0; low>=high; high>cap; retry delay<=0 | `invalid_argument`. |
| Hysteresis | low=2, high=4, cap=6, delay=5; depths 3,4,3,2 at t=0,1,2,3 | Admit, Delay(5), Delay(5), Admit. |
| Hard rejection | Same parameters; depth=6 then depth=5 | Reject(0) then Delay(5). |
| Boundary recovery | Reach low exactly after pausing | Admission resumes; reaching high exactly pauses. |
| Invalid depth/time | depth=7, negative timestamp, or backward timestamp | `invalid_argument`; no pause-state or time mutation. |
| Equal / maximum time | Repeated calls at LLONG_MAX, retryDelay=LLONG_MAX | Equal timestamps allowed; relative hint returned without deadline overflow. |
| Smallest queue | low=0, high=1, cap=1; depths 0,1,0 | Admit, Reject, Admit. |
| Caller protocol | Repeated serialized observe/decide/enqueue with deterministic drains | Enqueue only on Admit; queue stays bounded; delayed/rejected calls add no work. |

## 10. Per-Customer Ordered Scheduler

* **Question**: Preserve per-customer FIFO ordering across workers, failures, and stale acknowledgments.
* **Solution**: [Per-Customer Ordered Scheduler](./solutions.md#10-per-customer-ordered-scheduler).

| Case | Input / Scenario | Expected |
| --- | --- | --- |
| Invalid construction | worker count=0 or customer byte limit=0 | `invalid_argument`. |
| Cross-customer progress | Enqueue A:1,A:2,B:3; take workers 0 and 1 | A:1 and B:3 run concurrently; A:2 cannot run until A:1 succeeds. |
| Busy / no ready work | Call take on busy worker or idle worker while only A is running | `nullopt`; no queue or token changes. |
| FIFO during execution | Enqueue A:4 while A:1 runs | A's success order remains 1,2,4. |
| Failure retry | Fail A:1; take again | A:1 retried with a distinct token; no extra queued task or capacity usage. |
| Stale / wrong-worker acknowledgment | Finish with previous attempt token or another worker's token | false; no task removed or worker released. |
| Capacity includes running | cap=2; enqueue two tasks, dispatch one, attempt a third enqueue | Third enqueue returns false until successful completion frees a slot; failure does not free capacity. |
| Round-robin fairness | Ready A,B; fail A, then dispatch next idle worker | B is selected before A's retry. |
| Duplicate task ids | Enqueue A:7 twice | Both tasks execute FIFO with distinct tokens; no payload-based deduplication. |
| Empty/recreated customer | Complete empty-name customer's last task; recreate name; use old token | Old state reclaimed; stale token rejected for new assignment. |
| Invalid index / name | worker index=worker count; customer name exceeds configured byte limit | `out_of_range` / `length_error`; no task-state changes. |
| Zero capacity | Valid workers with cap=0 | Enqueue returns false; take returns `nullopt`. |
| Repeated lifecycle | Thousands of independent customer create/dispatch/complete cycles | Size and customer count return to zero; tokens increase; no retained completion history. |
| Interleaving model | Fixed-seed arrivals, worker dispatches, retry/success acknowledgments | Every dispatch matches its customer's FIFO head; at most one active worker per customer; bounded size and exact cleanup. |

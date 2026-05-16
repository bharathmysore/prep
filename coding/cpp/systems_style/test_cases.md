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

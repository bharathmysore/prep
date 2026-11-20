# Starter sample test cases

Assertions are explicit runtime checks, so NDEBUG does not disable them. Every executable returns a nonzero status on a failed check.

## Block device

| Scenario | Expected result |
| --- | --- |
| New 8 × 4096-byte sparse image | First block reads as 4096 zero bytes |
| Write block 0 with 0x41 and block 7 with 0x5a; sync, close, reopen | Exact contents preserved; block 1 remains zero |
| Read/write block 8 | Reject out-of-range ID |
| Write two bytes as a full block | Reject wrong transfer length |
| Zero block count or size | Reject geometry |
| UINT64_MAX blocks × 4096 bytes | Reject arithmetic/offset overflow |
| Open image using seven-block geometry | Reject size mismatch |
| Truncate owned test image to one block while open | Reading block 7 reports unexpected EOF |
| Open the truncated image using eight-block geometry | Reject size mismatch |

Actual short writes, EINTR, sync failure, device power loss and corrupted data are not injected by this sample; add callbacks/fault models in E1/E3.

## Journal model

| Transaction cut | Expected state after completed recovery |
| --- | --- |
| 0: before any persistence | x=0, y=0 |
| 1: payload only | x=0, y=0 |
| 2: commit persisted | x=10, y=20 |
| 3: first home field persisted | x=10, y=20 |
| 4: both home fields persisted | x=10, y=20 |
| 5: journal cleared | x=10, y=20 |

For each of these six snapshots, try four recovery cut positions (0–3), complete recovery, then replay again: 24 histories. Some cut numbers are unreachable when recovery has less work; those cases simply complete. Also test a transaction without interruption and reject a committed journal with no payload.

These are persistence-model histories, not process-kill or power-failure tests.

## Striping

| Scenario | Expected result |
| --- | --- |
| offset=3, length=14, stripe=4, targets=3 | Five fragments; logical offsets 3,4,8,12,16 |
| Fourth fragment in that example | target=0, target_offset=4, length=4 |
| Fifth fragment | target=1, target_offset=4, length=1 |
| Stripes 1/4/7, targets 1/2/3, offsets 0–31, lengths 0–39 | 11,520 cases: continuous coverage, positive bounded fragments, valid targets, inverse reconstruction |
| Zero stripe or zero targets | Reject geometry |
| offset=UINT64_MAX, length=1 | Reject exclusive-endpoint overflow |
| offset=UINT64_MAX, length=0 | Empty result |

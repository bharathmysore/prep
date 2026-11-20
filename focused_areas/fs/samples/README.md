# Runnable C++17 starter samples

## Run

Requirements: C++17 compiler, make, POSIX file APIs; macOS or Linux. No external libraries.

```sh
cd /Users/bmysoren/prep/focused_areas/fs/samples
make test
```

Build outputs default to `/tmp/fs-learning-build`; override with `make BUILD=/tmp/my-fs-build test`. The block sample creates a unique temporary regular file and removes only that file on exit. It accepts no user file/device paths. No mount or elevated privilege is needed.

## Samples and explanations

### Block device

**Question:** transfer exactly one bounded block to/from a regular-file image and verify sync/reopen behavior.

**Pattern / tags:** checked offset arithmetic, exact I/O loops and RAII; `block-io`, `bounds-checking`, `raii`.

**Code:** [block_device.cpp](block_device.cpp). Geometry is validated against off_t and image size. pread/pwrite loops handle interruptions and short transfers. The owner disables copying and closes the descriptor. Explicit sync reports errors; destructor close is not the durability API. Newly created holes read as zeros; an unexpected EOF is an error.

**Invariants:** valid block IDs map inside the image; writes require a full block; no operation silently accepts EOF. Single caller, no external concurrent image changes in supported use. Deliberate test truncation checks failure handling.

**Complexity:** O(B) transfer work and buffer space for block size B; O(1) device metadata. A short-I/O loop may issue multiple syscalls.

**Optimizations:** batch contiguous transfers for runtime; reuse buffers for memory. Follow up with explicit image headers, allocation, checksums and injected I/O callbacks.

**Limits:** no allocation map, format header, crash/power-loss emulator, directory-sync protocol or hardware persistence proof. Tests exercise actual reopen and EOF paths but do not force EINTR/short-write branches.

### Journal model

**Question:** recover either the old state or a committed pair of metadata values after interrupted writes and interrupted replay.

**Pattern / tags:** redo log, commit-before-home ordering and idempotent replay; `write-ahead-log`, `recovery`, `idempotency`.

**Code:** [journal_model.cpp](journal_model.cpp). A durable payload precedes the durable commit marker. Home fields are copied only after commit; replay completes them after a crash. Clearing the journal follows complete home persistence.

**Invariants:** commit at transaction boundary 2 makes the new pair recoverable; cuts before commit retain the old pair; repeated completed replay is harmless. A committed marker without payload is rejected. Single outstanding transaction; no network or clocks.

**Complexity:** O(1) time/state in this fixed-size model. A variable journal generalizes to O(J) replay work and representation-dependent memory.

**Optimizations:** group commit can reduce sync frequency; streaming replay can bound memory. Both need new ordering and space-reuse tests.

**Limits:** state lives in C++ objects. Each assignment is modeled as atomic, ordered, immediately durable. Journal clearing is one modeled atomic event. No real disk serialization, flush implementation, checksum framing, torn writes, write reordering, log reuse or application data protocol is implemented. Extending the model is an exercise, not a production guarantee.

### Stripe range mapper

**Question:** split a byte range into target-local fragments for a fixed immutable stripe layout.

**Pattern / tags:** quotient/remainder mapping; `striping`, `range-mapping`.

**Code:** [striping.cpp](striping.cpp). For stripe size S and N targets: stripe index = offset/S; target = index%N; target offset = (index/N)×S + offset%S. Split at each stripe boundary.

**Invariants:** fragments cover the requested range exactly once, remain within a stripe, and invert to their logical offsets. Geometry is positive and the exclusive range endpoint must fit uint64_t.

**Complexity:** O(F) time and result space for F fragments; O(1) mapping per fragment. Huge ranges with tiny stripes can create huge vectors; add an iterator or fragment budget for untrusted inputs.

**Optimizations:** coalesce target operations and bound concurrency for runtime; stream fragment generation to reduce memory.

**Limits:** no storage, RPC, metadata owner, locking, layout migration or failure recovery. This mapping is a teaching layout, not a complete implementation of any vendor's placement rules.

## Test cases and learner work

Concrete inputs and expected results are in [test_cases.md](test_cases.md). Full milestone implementations remain in [exercises.md](../exercises.md).

Company frequency: no public frequency claim; domain relevance is filesystem and distributed-storage engineering. These samples are original educational code.

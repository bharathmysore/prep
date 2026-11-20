# LMDB: memory-mapped copy-on-write B+tree

[Contents](../README.md) · [Comparisons](../comparison.md) · [Learning path](../learning-path.md)

Evidence scope: `/Users/bmysoren/tpt/lmdb`, HEAD `ba47717cc87753f86f24135c17bfaaa82d5daf53`, read on 2026-10-27. The actual implementation is **`libraries/liblmdb/`**, not a root-level `src/` directory. The local header identifies version macros 0.9.90 ([version](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/lmdb.h:235)); do not silently substitute documentation for another LMDB release. Production embedded transactional engine; static source review only.

## Fundamentals

A B+tree routes ordered keys through branch pages to leaves. Memory mapping lets the operating system supply pages and caching. Copy-on-write gives a writer new pages while old readers keep a stable tree. A transaction commits by publishing new metadata after the data pages are made safe. A reader table identifies the oldest still-visible version so freed pages can eventually be reused.

The header explicitly describes mapped reads, copy-on-write, one active writer, concurrent readers, and free-page reuse ([overview](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/lmdb.h:6)). This replaces the WAL/SST/compaction architecture used by LevelDB; an application may still need backups, space planning, and stale-reader handling.

## Read, write, commit and reclaim

- **Read:** begin a read transaction, take its version/root, navigate the B+tree and return mapped data. Tree descent is in `mdb_page_search` ([search](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:7580)). Returned pointers have transaction/lifetime restrictions; mapping does not make them independently owned copies.
- **Write:** acquire the single-writer transaction and copy/touch pages before changing active data ([page touch](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:3028)). Splits/rebalancing and freed-page bookkeeping update the tree without overwriting pages still visible to readers.
- **Commit:** finalize transaction pages and publish metadata ([commit](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:4652)). Pages 0 and 1 alternate metadata publication by transaction ID ([layout](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1368)); open selects metadata by transaction ID ([selection](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:5026)). This is root publication, not log replay.
- **Recovery:** select the committed tree; no LSM-style WAL replay is required by the architecture. This assumes the configured persistence ordering and platform contract; a corrupt filesystem or unsafe flags are outside a blanket crash-safety claim.
- **Delete:** `mdb_del` routes into cursor deletion/tree modification ([delete](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:10636)). Freed pages become reusable once no older reader needs them. The oldest-reader logic is documented in the reader table discussion ([reader reclamation](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:802)). Reuse is different from shrinking the file.

## Page structures, concurrency and cache behavior

The tree's persistent units are pages, not per-key log records. Page flags distinguish branch, leaf, overflow and metadata pages ([page types](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1075)). A page contains an index of node offsets and free-space bounds; nodes contain key size, flags and key/data payload. Large data can point to overflow pages ([node layout](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1230)). A database descriptor tracks tree depth, page counts and root page number ([descriptor](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1339)). This makes a root publication sufficient to describe a whole committed tree, provided all reachable pages obey the persistence ordering.

The metadata stores descriptors for the free-space database and main database ([metadata](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:1370)). Free-space tracking is part of the transactional state, rather than an unrelated filesystem scavenger. Otherwise a crash could make a live page appear free or leak pages indefinitely. Copy-on-write also applies along the path toward the root: changing one leaf may require new ancestors so an older root continues to reach only the older version.

Readers use a transaction ID in the reader table; the table's oldest-reader information tells the writer which retired pages remain unsafe to reuse. The reader-table discussion explains that readers take a mutex to obtain a slot, then do not lock every data access ([reader table](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/mdb.c:802)). “Lock-free reads” should be interpreted in this data-path sense, not as a promise that opening every transaction requires no coordination or that stale processes never need cleanup.

The OS page cache is the main data cache. A mapped pointer does not imply a resident page: touching a cold page can cause a fault and storage I/O. Sequential cursor traversal and random lookups can therefore have very different locality even though neither explicitly calls `read` for each record. A larger map reservation is also different from a larger resident working set; the header explicitly cautions about that distinction ([address space](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/lmdb.h:83)).

## Worked history: an old reader pins old pages

Assume default synchronous durability and an environment containing `a=old` at transaction N.

1. Reader R begins and records snapshot N. Its root leads to the leaf holding `a=old`.
2. Writer W starts transaction N+1 and changes `a` to `new`. W touches/copies the relevant leaf and ancestors instead of rewriting pages visible through R's root. Another writer must wait; R can continue reading.
3. W commits the new pages and publishes the new metadata/root. A new reader sees `a=new`; R still sees `a=old`. The consistent view comes from the root and transaction lifetime, not from copying the whole database for each reader.
4. Writer D deletes `a` in N+2, potentially rebalancing its new tree. New readers see absence. Pages belonging to earlier versions cannot all become reusable while R still needs them.
5. Additional writes arrive while R stays open. They may allocate more pages because the oldest reader delays reuse. This is how a read-heavy application can cause file growth without directly blocking the writer mutex.
6. R ends its transaction. A later writer can reclaim pages whose retirement is now older than every active reader. Reclaiming pages supplies space for subsequent updates; it does not imply the filesystem file shrinks immediately.
7. A restart uses the published committed metadata rather than reconstructing these writes from a WAL. If the application had chosen unsafe sync flags, the last valid-looking tree on storage might not reflect the same durability assumptions.

The learning objective is to separate three lifetimes: the application's mapped-value pointer, its read transaction, and the underlying physical page's eligibility for reuse.

## Durability and cost model

`MDB_NOMETASYNC` can sacrifice the last transaction on system crash while preserving integrity. `MDB_NOSYNC` can lose transactions or corrupt the database, with narrower integrity guarantees only under specified filesystem/order/mapping conditions. `MDB_WRITEMAP` changes stray-write exposure and flush behavior ([flags](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/lmdb.h:631)). ACID claims must therefore name the flags and failure model.

A single writer bounds write parallelism; transaction batching amortizes sync but increases writer occupancy. Random page faults can dominate cold reads. Long-lived readers prevent freed-page reuse and can grow the file, even though they do not lock out writers ([caveat](/Users/bmysoren/tpt/lmdb/libraries/liblmdb/lmdb.h:106)). Map sizing/address space, active transaction lifetime and reader cleanup are operational concerns.

## Exercises and evolution hypotheses

Draw two roots before/after a transaction and mark which pages an older reader still needs. Trace deletion while that reader remains open. Compare the ordering requirements of data-page flush and metadata publication with a WAL commit.

Conditional forecast: heavy concurrent writes push applications toward batching, partitioning into independent environments, or another engine; large read-mostly datasets favor the simple mapped-read path when the OS working set fits the access pattern. Not established: performance relative to LSMs on your hardware, behavior of local crypto/remote-page extensions, or corruption detection coverage. Those need separate study.

# Placement, file metadata, and object storage

[Contents](../README.md) · Previous: [Distribution](03-distribution-and-transactions.md)

File and object systems turn names into many physical pieces, place those pieces across failure domains, and keep that mapping valid during mutation and repair. The metadata path can determine performance even when bulk data bypasses metadata servers.

## 1. Naming and indirection

A file path resolves through directories to an inode or equivalent metadata record, then through extents or chunk references to bytes. An object name resolves through bucket/key metadata to payload pieces. A large file can contain many chunks; many small objects can be packed into one volume. Logical and physical granularity need not match.

Every level of indirection buys flexibility but creates metadata that must be cached, persisted, replicated, garbage collected and recovered. A million tiny files can cost more metadata operations than one very large sequential object with the same total bytes.

## 2. Placement units are different architectural choices

| System | Units to distinguish | Why the distinction matters |
|---|---|---|
| Ceph | Object, placement group, pool, OSD, CRUSH failure domain | Objects map through PGs to acting sets; map changes and peering govern placement/recovery |
| SeaweedFS | File ID/needle, volume, volume server, filer entry/chunks | Master tracks volume placement; filer provides the namespace above blob storage |
| CubeFS | Inode/dentry, metapartition, data partition, extent; blobstore code modes | Namespace partitions and payload partitions scale and recover differently |
| TiKV | User key, Region, peer, Store | Ordered range movement and replicated ownership are coupled to Region lifecycle |
| OceanBase | Table partition, tablet, log stream, server/zone | Several tablets can share a log stream; log and transaction boundaries require separate tracing |

These are conceptual mappings, not interchangeable names. See each dossier before assigning an exact acknowledgment or failure guarantee.

## 3. Deterministic placement versus lookup

A deterministic placement function computes candidate destinations from an object identity and a topology map. This avoids consulting a central per-object directory for every I/O, but requires map distribution and careful transitions. Ceph's CRUSH belongs here; placement groups add a manageable unit for coordination and recovery.

A directory/lookup approach resolves an identifier through maintained metadata. Caching and coarse placement units can keep lookup state small. SeaweedFS's volume indirection means the master need not keep the complete file namespace. The filer stores a richer mapping from paths to metadata and chunks.

The right comparison includes map/lookup freshness, change handling, cache misses, and hotspot control. Neither “no central metadata server” nor “O(1) lookup” alone proves infinite scalability.

## 4. Replication and erasure coding

With `r` full copies, ideal payload overhead is `r` times. With `k` data fragments and `m` parity fragments in a suitable erasure code, ideal payload overhead is `(k+m)/k`. A `6+3` example yields `1.5×` payload capacity, before metadata, padding, versions, free-space reserve and implementation overhead.

Reconstructing a missing fragment requires enough surviving fragments and resource availability. The code's mathematical failure tolerance does not prove tolerance of a rack or zone loss: placement must separate the relevant fragments. Small updates can require read-modify-write or other coding work; large sequential immutable objects are a different workload.

Ceph documents [erasure coding](/Users/bmysoren/tpt/ceph/doc/rados/operations/erasure-code.rst:1). CubeFS and SeaweedFS also have coding paths, but their placement and update designs must be studied separately. Never borrow a Ceph quorum rule for a SeaweedFS replication path.

## 5. The write has two kinds of state

A large-file write often entails allocation, payload writes, and namespace publication. Possible interruption points include allocated-but-unwritten chunks, persisted-but-unreferenced payload, and published metadata whose referenced data is not sufficiently durable. Systems need ordering, cleanup, retry and reconciliation rules for these states.

For each system trace:

1. Who allocates identifiers and destinations?
2. Who stores and replicates payload?
3. Who publishes the mapping from name/offset to payload?
4. Which step permits the client reply?
5. How does recovery distinguish abandoned work from live data?

An object PUT may replace a whole logical object even though the system internally writes several pieces. POSIX overwrite, append, rename and fsync have distinct semantics. Do not infer one from another.

## 6. Delete, snapshots and garbage collection

A namespace delete can first make an object unreachable. Physical chunks may be reclaimed later, after snapshot/version references and concurrent readers are accounted for. Packed volumes may need compaction or vacuum. Erasure-coded layouts may require different deletion bookkeeping from replicated volumes.

Reference tracking is itself distributed state. A GC that relies on an incomplete metadata view can delete live data. A GC that never trusts absence can leak storage indefinitely. The key proof is when lack of a reference becomes authoritative enough to reclaim bytes.

## 7. Repair is the hidden capacity requirement

After a failure, surviving nodes must read, transmit, and write replacement data while continuing user traffic. An approximate lower bound is `bytes needing repair / effective repair bandwidth`, where effective bandwidth is limited by the narrowest disk, network, CPU and scheduling constraint. Erasure reconstruction and contention can make it much slower.

Capacity planning therefore needs free space and performance reserve. Filling every device to nominal capacity may leave no safe repair destination. Increasing failure tolerance also changes normal write cost, recovery fan-in, and correlated-risk exposure.

## 8. Metadata is a data plane too

Calling something a control plane does not mean it is absent from request latency. Ceph MONs distribute authority/maps; CephFS MDSs serve filesystem metadata operations; RGW has object metadata/index work. SeaweedFS masters and filers have different roles. CubeFS masters, metanodes and datanodes have different paths.

A workload can be bottlenecked on directory operations, listing, index updates or allocation before it saturates payload disks. Measure data bytes and metadata operations separately.

## 9. Protocol compatibility is a layer

S3, FUSE, NFS-like interfaces, block devices and SQL can expose the same underlying storage through different semantics and caches. Test operations that matter: concurrent overwrite, rename, listing after mutation, multipart completion, crash after acknowledgment and snapshot restore. Feature presence is not proof of identical behavior across all gateways.

## Check your understanding

For a 1 GiB file split into chunks, draw namespace metadata, chunk placement and repair units in CephFS, SeaweedFS and CubeFS. Then repeat for ten million 1 KiB objects. Predict which resources change first, and state what evidence would falsify the prediction.

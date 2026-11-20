# Queries, indexes, and resource control

[Contents](../README.md)

Storage layouts become useful applications through data models and query execution. These upper layers feed work back into every lower layer: an index creates extra writes, a scan competes for cache, and a transaction can retain versions long after the application stops touching them.

## 1. Primary and secondary indexes

A primary key identifies a record; a secondary index maps an alternate search key to primary identity or stored fields. In an ordered KV substrate, a secondary index can be represented as keys such as `(index prefix, indexed value, primary key)`. A query becomes a range scan followed by record fetches, unless the index contains enough fields to cover the query.

Updating a record may require deleting an old index entry and inserting a new one. If these changes are not atomic with the record update, readers can find missing or stale entries. FoundationDB layers can implement this logic above transactional KV; OceanBase implements SQL/index behavior in the database. An ordered engine's WriteBatch supplies a useful local primitive, but does not by itself validate concurrent application decisions.

## 2. Row and column layouts

A row layout places attributes of a record together, often helping point access and whole-record updates. A column layout groups values from an attribute, often helping scans that read a few columns, compression and vectorized execution. Mixed layouts can serve both but add representation and maintenance costs.

Do not confuse a RocksDB **column family**, which partitions engine state/configuration, with a relational column store. Names can obscure completely different abstractions. OceanBase's [column-store source](/Users/bmysoren/tpt/oceanbase/src/storage/column_store/ob_co_merge_dag.cpp:464) supplies an entry point; existence of that path alone does not establish every supported deployment mode or measured benefit.

## 3. Planning and execution

A SQL planner chooses access paths, join order and execution strategies using schema, statistics and cost estimates. The executor performs scans, filters, joins, sorting and aggregation; distributed execution adds data exchange and participant coordination. Poor cardinality estimates can turn a selective-looking query into a large scan or network shuffle. A storage engine can be functioning correctly while the query plan dominates latency.

Pushing filters or aggregation toward storage can reduce network traffic but consumes storage-node CPU and can interfere with latency-sensitive operations. TiKV coprocessor paths illustrate why a storage server is sometimes also a compute server; read its dossier before treating CPU as available exclusively for I/O.

## 4. Memory data structures and specialized indexes

Hash tables favor exact lookup; ordered trees/skiplists favor ranges and ranking; radix-like structures exploit shared prefixes; compact contiguous encodings reduce pointer overhead for small collections. Representation thresholds trade memory against mutation and search cost. Redis is the main local study case for exposing these choices directly through commands.

Approximate nearest-neighbor vector indexes trade exactness, memory and latency. HNSW uses a graph to navigate candidate neighbors. Quantization reduces representation cost but can change recall. Filtering after a search and integrating filters during search can have very different work/quality effects. Redis's [local vector-set guide](/Users/bmysoren/tpt/redis/modules/vector-sets/README.md:1) explicitly exposes filtering and quantization choices. Do not compare vector throughput without fixing dimension, recall target, index size and filter selectivity.

## 5. Compression and checksums in the path

Compression trades CPU for fewer stored/transferred bytes; block size affects random-read amplification, index size and compression opportunity. A good ratio on cold data can be a bad choice on a CPU-bound hot write path. Checksums consume CPU too, while enabling detection that redundancy or backup can act upon. Encode/decode cost belongs in the budget rather than being treated as zero-cost plumbing.

## 6. Multi-tenancy and admission control

A tenant boundary can carry identity, authorization, namespace, quotas and resource budgets. Logical namespace separation does not prove CPU, cache or I/O isolation. OceanBase's units/pools, FoundationDB's admission mechanisms, and RocksDB's caches/thread pools operate at different scopes.

Admission control rejects, delays or rate-limits work before shared queues become unbounded. Scheduling divides resources between foreground reads/writes and maintenance/recovery. Priority alone cannot satisfy every latency target at overload; reserved capacity, fairness and explicit rejection policies matter.

A useful model distinguishes demand, admitted load, service capacity, queue length and completion latency. A system can improve p99 by rejecting more work, so report goodput and errors alongside latency.

## 7. Security and identity are architectural boundaries

Authentication establishes the caller; authorization constrains its operations; encryption protects selected links or stored bytes; audit trails preserve evidence of actions. Each additional gateway and metadata backend introduces a trust boundary. Tenant isolation must be enforced where data is accessed, including caches and background workers. These general principles do not constitute a security audit of the checkouts.

## 8. Table formats and object storage

An analytical table can be a set of immutable data files with metadata naming a consistent snapshot. A catalog commit publishes a new metadata state, while background compaction and snapshot expiration maintain efficiency. The payload store's object durability and the catalog's atomic update are separate contracts.

The local SeaweedFS README describes [table/catalog interfaces and maintenance](/Users/bmysoren/tpt/seaweedfs/README.md:224). Treat these as documented capabilities to investigate through code/tests, not proof that a blob API automatically supplies SQL transactions. Catalog concurrency, file GC and the query engine each need their own correctness argument.

## Check your understanding

Take one logical row update that changes two secondary indexes. Count the logical mutations, identify the atomicity boundary, and predict the extra engine/log/replica work. Then turn the workload into a column scan or filtered vector search and explain which resources become central.

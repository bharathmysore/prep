# Primary-source reading list

## How to use sources

Start with local filesystem fundamentals, then compare distributed architectures, then test application-level implications. Historical papers explain design reasoning; current manuals govern deployed behavior. Product recommendations in this pack are conditional engineering judgments. Initial recommendation sources were researched in the discussion on October 27, 2026; additional foundational links below are reference entry points rather than a claim that every page was rechecked on that date.

## Fundamentals and implementation

- [OSTEP free textbook](https://pages.cs.wisc.edu/~remzi/OSTEP/): persistence, devices, RAID, filesystem implementation, journaling, log-structured filesystems, integrity, NFS and AFS. Explain each chapter's assumptions before coding.
- [OSTEP exercises](https://github.com/remzi-arpacidusseau/ostep-homework): simulations accompanying the textbook; useful before implementing an image-backed filesystem.
- [Linux VFS documentation](https://docs.kernel.org/filesystems/vfs.html): inode/dentry/file abstractions and filesystem interfaces.
- [ext4 documentation](https://docs.kernel.org/filesystems/ext4/index.html): layout and journal design.
- [XFS documentation](https://docs.kernel.org/filesystems/xfs/index.html): filesystem design and implementation entry point.
- [OpenZFS documentation](https://openzfs.github.io/openzfs-docs/): copy-on-write storage, administration and implementation references.
- [Linux fsync manual](https://man7.org/linux/man-pages/man2/fsync.2.html): file synchronization and directory-entry caveat; an authoritative Linux man-pages project reference, not a universal remote-filesystem contract.

## Distributed-system design

- [The Google File System, 2003](https://research.google/pubs/the-google-file-system/): workload-driven design and deliberately different interface assumptions.
- [Ceph, OSDI 2006](https://www.usenix.org/conference/osdi-06/ceph-scalable-high-performance-distributed-file-system): data/metadata separation, adaptive metadata and placement. Historical architecture, not a current operational manual.
- [CephFS documentation, Reef](https://docs.ceph.com/en/reef/cephfs/): MDS, clients and RADOS; select the matching release when deploying.
- [Lustre architecture](https://wiki.lustre.org/images/6/64/LustreArchitecture-v4.pdf): metadata/data services and layouts; historical overview.
- [Lustre operations manual](https://doc.lustre.org/lustre_manual.pdf): choose sections corresponding to the version being studied.
- [Raft paper and resources](https://raft.github.io/): replicated-log ordering and leader change; study separately before automatic filesystem failover.

## AI/ML evaluation and commercial candidates

- [MLPerf Storage](https://mlcommons.org/benchmarks/storage/): reproducible storage workloads; inspect rules and submission configurations before comparison.
- [WEKA architecture key concepts](https://www.weka.io/wp-content/uploads/files/resources/WEKA-Architecture-Key-Concepts.pdf): vendor explanation of distributed metadata and data structures; not independent comparative evidence.
- [FSx for Lustre S3 repositories](https://docs.aws.amazon.com/fsx/latest/LustreGuide/fsx-data-repositories.html): supported integration and deployment limitations.
- [VAST architecture overview](https://www.vastdata.com/platform/how-it-works): optional contrasting disaggregated shared-everything design study; vendor source, not a measured ranking.

## Performance tools

- [fio documentation](https://fio.readthedocs.io/en/latest/fio_doc.html)
- [IOR and MDTest project](https://github.com/hpc/ior)

Record the exact version, commit, options and measurement boundaries in each experiment. No third-party source text or code has been copied into the sample implementations.

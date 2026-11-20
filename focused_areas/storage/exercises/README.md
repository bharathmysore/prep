# Storage exercises

[Storage learning hub](../README.md) · [Samples](../samples/README.md)

All current exercise specifications are saved within this storage directory:

- [Implementation workbook](../expertise/implementation-workbook.md): incremental mechanism ladder and first explainable-lookup exercise.
- [Lab portfolio](../expertise/labs.md): durable LSM, COW B+ tree, transactional record layer, time-series store, disconnected client and telemetry capstone.
- [Learning path](../learning-path.md): architectural mastery checks.
- [Research program](../expertise/research-program.md): derivation, reproduction, original contributions and architecture reviews.
- [Paper study](../expertise/paper-study.md): reconstruction, critique and reproduction exercises.
- [RocksLab exercises and sample](../samples/rockslab-week1/README.md): existing starter model and its documented build instructions.

Exercise implementations beyond the existing RocksLab are pending. Save new code under `../samples/` and supporting experiment outputs beside the corresponding sample, with its assumptions, revision and execution status recorded.

## Find the code for an exercise

Use [the implementation ladder](../expertise/implementation-workbook.md#the-implementation-ladder) for production code anchors beside each mechanism, and [the sample code map](../samples/README.md#saved-sample-code) for the existing teaching model. The first exercise starts at [FindNewest](../samples/rockslab-week1/src/mini_lsm.cc:100) and [the existing tests](../samples/rockslab-week1/tests/mini_lsm_test.cc:18).

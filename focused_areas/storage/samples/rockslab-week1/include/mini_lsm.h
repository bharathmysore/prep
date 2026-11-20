#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <vector>

// A teaching model of the parts of an LSM tree introduced in Week 1.
// It is intentionally single-threaded and in-memory: it has no WAL, disk I/O,
// block cache, Bloom filters, or RocksDB dependency.
class MiniLsm {
 public:
  using Key = std::string;
  using Value = std::string;

  explicit MiniLsm(std::size_t memtable_limit);

  void Put(Key key, Value value);
  void Delete(Key key);
  std::optional<Value> Get(const Key& key) const;
  std::vector<std::pair<Key, Value>> Scan(const Key& begin,
                                           const Key& end) const;

  // Flushes the mutable MemTable into a newest-first immutable sorted table.
  void Flush();
  // Merges all immutable tables, retaining the newest version of every key.
  void Compact();

  std::size_t mutable_entries() const;
  std::size_t sstable_count() const;
  std::string DebugString() const;

 private:
  struct Entry {
    std::optional<Value> value;  // nullopt is a deletion tombstone.
  };
  using Table = std::map<Key, Entry>;

  const Entry* FindNewest(const Key& key) const;
  void MaybeFlush();

  std::size_t memtable_limit_;
  Table memtable_;
  std::vector<Table> sstables_;  // Index 0 is the newest immutable table.
};

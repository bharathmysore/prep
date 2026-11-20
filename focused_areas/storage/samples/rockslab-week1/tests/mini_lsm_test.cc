#include "mini_lsm.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void Check(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAILED: " << message << "\n";
    std::exit(1);
  }
}

}  // namespace

int main() {
  MiniLsm db(/*memtable_limit=*/2);

  db.Put("a", "one");
  db.Put("b", "two");  // Automatic flush.
  Check(db.sstable_count() == 1, "MemTable should flush at its configured limit");
  Check(db.Get("a") == "one", "flushed values should remain readable");

  db.Put("a", "new-one");
  Check(db.Get("a") == "new-one", "MemTable must override older SSTables");
  db.Delete("b");       // Automatic flush, including a tombstone.
  Check(!db.Get("b"), "a tombstone must hide an older value");

  const auto before = db.Scan("a", "z");
  Check(before.size() == 1 && before[0].first == "a" &&
            before[0].second == "new-one",
        "a scan must resolve versions and omit tombstones");

  db.Compact();
  Check(db.sstable_count() == 1, "compaction should merge SSTables");
  Check(db.Get("a") == "new-one", "compaction must preserve newest values");
  Check(!db.Get("b"), "compaction must preserve deletions");

  std::cout << "All MiniLsm tests passed\n";
}

#include "mini_lsm.h"

#include <sstream>
#include <stdexcept>

MiniLsm::MiniLsm(std::size_t memtable_limit) : memtable_limit_(memtable_limit) {
  if (memtable_limit_ == 0) {
    throw std::invalid_argument("memtable_limit must be greater than zero");
  }
}

void MiniLsm::Put(Key key, Value value) {
  memtable_[std::move(key)] = Entry{std::move(value)};
  MaybeFlush();
}

void MiniLsm::Delete(Key key) {
  memtable_[std::move(key)] = Entry{std::nullopt};
  MaybeFlush();
}

std::optional<MiniLsm::Value> MiniLsm::Get(const Key& key) const {
  const Entry* entry = FindNewest(key);
  if (entry == nullptr || !entry->value.has_value()) {
    return std::nullopt;
  }
  return entry->value;
}

std::vector<std::pair<MiniLsm::Key, MiniLsm::Value>> MiniLsm::Scan(
    const Key& begin, const Key& end) const {
  // Each table may contain an older value of a key. Resolve newest-to-oldest
  // first, then return the surviving live entries in sorted key order.
  Table visible;
  auto add_unseen = [&visible, &begin, &end](const Table& table) {
    for (auto it = table.lower_bound(begin); it != table.end() && it->first < end;
         ++it) {
      visible.emplace(it->first, it->second);
    }
  };
  add_unseen(memtable_);
  for (const Table& table : sstables_) {
    add_unseen(table);
  }

  std::vector<std::pair<Key, Value>> result;
  for (const auto& [key, entry] : visible) {
    if (entry.value.has_value()) {
      result.emplace_back(key, *entry.value);
    }
  }
  return result;
}

void MiniLsm::Flush() {
  if (memtable_.empty()) {
    return;
  }
  sstables_.insert(sstables_.begin(), std::move(memtable_));
  memtable_.clear();
}

void MiniLsm::Compact() {
  Flush();
  if (sstables_.size() < 2) {
    return;
  }

  Table merged;
  // Newest table wins. emplace deliberately does not replace an existing key.
  for (const Table& table : sstables_) {
    for (const auto& [key, entry] : table) {
      merged.emplace(key, entry);
    }
  }
  sstables_.assign(1, std::move(merged));
}

std::size_t MiniLsm::mutable_entries() const { return memtable_.size(); }

std::size_t MiniLsm::sstable_count() const { return sstables_.size(); }

std::string MiniLsm::DebugString() const {
  std::ostringstream output;
  const auto print_table = [&output](const std::string& name, const Table& table) {
    output << name << ":";
    for (const auto& [key, entry] : table) {
      output << " " << key << "=";
      output << (entry.value ? *entry.value : "<tombstone>");
    }
    output << "\n";
  };
  print_table("MemTable", memtable_);
  for (std::size_t i = 0; i < sstables_.size(); ++i) {
    print_table("SSTable[" + std::to_string(i) + "]", sstables_[i]);
  }
  return output.str();
}

const MiniLsm::Entry* MiniLsm::FindNewest(const Key& key) const {
  const auto find = [&key](const Table& table) -> const Entry* {
    const auto it = table.find(key);
    return it == table.end() ? nullptr : &it->second;
  };
  if (const Entry* entry = find(memtable_); entry != nullptr) {
    return entry;
  }
  for (const Table& table : sstables_) {
    if (const Entry* entry = find(table); entry != nullptr) {
      return entry;
    }
  }
  return nullptr;
}

void MiniLsm::MaybeFlush() {
  if (memtable_.size() >= memtable_limit_) {
    Flush();
  }
}

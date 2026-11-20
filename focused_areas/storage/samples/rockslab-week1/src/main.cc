#include "mini_lsm.h"

#include <iostream>
#include <sstream>
#include <string>

namespace {

void PrintHelp() {
  std::cout << "Commands:\n"
            << "  put <key> <value>    write a value\n"
            << "  get <key>            read the newest value\n"
            << "  delete <key>         write a tombstone\n"
            << "  scan <begin> <end>   scan [begin, end)\n"
            << "  flush                convert the MemTable to an SSTable\n"
            << "  compact              merge SSTables; newest values win\n"
            << "  state                show the teaching model's internal state\n"
            << "  help | quit\n";
}

}  // namespace

int main() {
  MiniLsm db(/*memtable_limit=*/3);
  std::cout << "RocksLab Week 1 (MemTable limit: 3 distinct keys)\n";
  PrintHelp();

  std::string line;
  while (std::cout << "> " && std::getline(std::cin, line)) {
    std::istringstream input(line);
    std::string command;
    input >> command;

    if (command == "put") {
      std::string key;
      std::string value;
      if (input >> key >> value) {
        db.Put(std::move(key), std::move(value));
        std::cout << "OK\n";
      } else {
        std::cout << "usage: put <key> <value>\n";
      }
    } else if (command == "get") {
      std::string key;
      if (!(input >> key)) {
        std::cout << "usage: get <key>\n";
      } else if (const auto value = db.Get(key)) {
        std::cout << *value << "\n";
      } else {
        std::cout << "NOT_FOUND\n";
      }
    } else if (command == "delete") {
      std::string key;
      if (input >> key) {
        db.Delete(std::move(key));
        std::cout << "OK\n";
      } else {
        std::cout << "usage: delete <key>\n";
      }
    } else if (command == "scan") {
      std::string begin;
      std::string end;
      if (input >> begin >> end) {
        for (const auto& [key, value] : db.Scan(begin, end)) {
          std::cout << key << "=" << value << "\n";
        }
      } else {
        std::cout << "usage: scan <begin> <end>\n";
      }
    } else if (command == "flush") {
      db.Flush();
      std::cout << "flushed\n";
    } else if (command == "compact") {
      db.Compact();
      std::cout << "compacted\n";
    } else if (command == "state") {
      std::cout << db.DebugString();
    } else if (command == "help") {
      PrintHelp();
    } else if (command == "quit" || command == "exit") {
      return 0;
    } else if (!command.empty()) {
      std::cout << "unknown command; try help\n";
    }
  }
}

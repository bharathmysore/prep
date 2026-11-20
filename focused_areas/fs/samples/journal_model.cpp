// E3: deterministic persistence model, NOT an actual on-disk journal.
// Each assignment is atomic and immediately durable. No reordering or torn writes.
#include <iostream>
#include <optional>
#include <stdexcept>

struct Crash {};
struct Home { int x = 0; int y = 0; };
struct Disk {
    Home home;
    std::optional<Home> redo;
    bool committed = false;
};
class CrashAfter {
    int boundary_;
    int count_ = 0;
public:
    explicit CrashAfter(int boundary) : boundary_(boundary) {
        if (boundary == 0) throw Crash{};
    }
    void persisted() { if (++count_ == boundary_) throw Crash{}; }
};
void transaction(Disk& disk, int crash_after) {
    if (disk.redo || disk.committed) throw std::logic_error("recover before next transaction");
    CrashAfter crash(crash_after);
    disk.redo = Home{10, 20}; crash.persisted(); // 1: durable redo payload
    disk.committed = true; crash.persisted();   // 2: durable commit, acknowledgment eligible
    disk.home.x = disk.redo->x; crash.persisted(); // 3: partial home state is possible
    disk.home.y = disk.redo->y; crash.persisted(); // 4: complete home state
    // One modeled atomic journal-clear event. Real formats need safe generations/reuse.
    disk.redo.reset(); disk.committed = false; crash.persisted(); // 5
}
void recover(Disk& disk, int crash_after = -1) {
    CrashAfter crash(crash_after);
    if (disk.committed) {
        if (!disk.redo) throw std::runtime_error("invalid committed journal");
        disk.home.x = disk.redo->x; crash.persisted();
        disk.home.y = disk.redo->y; crash.persisted();
    }
    disk.redo.reset(); disk.committed = false; crash.persisted();
}
void check(const Disk& disk, bool new_state) {
    const int expected = new_state ? 10 : 0;
    if (disk.home.x != expected || disk.home.y != expected * 2 ||
        disk.redo || disk.committed) throw std::runtime_error("recovery invariant failed");
}
int main() {
    try {
        int histories = 0;
        for (int tx_cut = 0; tx_cut <= 5; ++tx_cut) {
            Disk snapshot;
            try { transaction(snapshot, tx_cut); } catch (const Crash&) {}
            // Commit is durable at boundary 2. Recoveries before that retain old state.
            for (int recovery_cut = 0; recovery_cut <= 3; ++recovery_cut) {
                Disk trial = snapshot;
                try { recover(trial, recovery_cut); } catch (const Crash&) {}
                recover(trial); check(trial, tx_cut >= 2);
                recover(trial); check(trial, tx_cut >= 2); // idempotence
                ++histories;
            }
        }
        Disk complete; transaction(complete, -1); recover(complete); check(complete, true);
        Disk invalid; invalid.committed = true;
        bool rejected = false;
        try { recover(invalid); } catch (const std::runtime_error&) { rejected = true; }
        if (!rejected) throw std::runtime_error("invalid commit accepted");
        std::cout << "PASS journal model: " << histories
                  << " crash/recovery histories, repeat replay, invalid commit\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n'; return 1;
    }
}

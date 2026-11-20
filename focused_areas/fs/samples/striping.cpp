// E7: immutable range mapping, not a network or coherence implementation.
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
using U = std::uint64_t;
struct Fragment { U logical; U target; U target_offset; U length; };
std::vector<Fragment> split(U offset, U length, U stripe, U targets) {
    if (stripe == 0 || targets == 0) throw std::invalid_argument("zero layout geometry");
    // Require the exclusive endpoint to fit in U.
    if (length > std::numeric_limits<U>::max() - offset)
        throw std::overflow_error("range endpoint overflow");
    std::vector<Fragment> result;
    while (length > 0) {
        const U index = offset / stripe;
        const U within = offset % stripe;
        const U bytes = std::min(length, stripe - within);
        result.push_back({offset, index % targets, (index / targets) * stripe + within, bytes});
        offset += bytes;
        length -= bytes;
    }
    return result;
}
void require(bool ok) { if (!ok) throw std::runtime_error("mapping invariant failed"); }
template<class F> void rejects(F f) {
    bool rejected = false;
    try { f(); } catch (const std::exception&) { rejected = true; }
    require(rejected);
}
int main() {
    try {
        auto example = split(3, 14, 4, 3);
        require(example.size() == 5);
        require(example[0].target == 0 && example[0].target_offset == 3 && example[0].length == 1);
        require(example[3].target == 0 && example[3].target_offset == 4 && example[3].length == 4);
        require(example[4].target == 1 && example[4].target_offset == 4 && example[4].length == 1);
        std::size_t cases = 0;
        for (U stripe : {U{1}, U{4}, U{7}}) {
            for (U targets : {U{1}, U{2}, U{3}}) {
                for (U offset = 0; offset < 32; ++offset) {
                    for (U length = 0; length < 40; ++length) {
                        U next = offset, total = 0;
                        for (const auto& f : split(offset, length, stripe, targets)) {
                            require(f.logical == next && f.target < targets && f.length > 0);
                            require(f.length <= stripe - (f.logical % stripe));
                            const U inverse = ((f.target_offset / stripe) * targets + f.target) * stripe
                                              + f.target_offset % stripe;
                            require(inverse == f.logical);
                            next += f.length; total += f.length;
                        }
                        require(total == length && next == offset + length);
                        ++cases;
                    }
                }
            }
        }
        rejects([] { split(0, 1, 0, 3); });
        rejects([] { split(0, 1, 4, 0); });
        rejects([] { split(std::numeric_limits<U>::max(), 1, 4, 3); });
        require(split(std::numeric_limits<U>::max(), 0, 4, 3).empty());
        std::cout << "PASS striping: " << cases << " reconstruction cases and invalid ranges\n";
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n'; return 1;
    }
}

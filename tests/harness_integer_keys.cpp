/// @brief Smoke‑test for zero‑free integer key generation and GlGh MPHF integration.
///
/// Verifies that `generateInputData` with `integerKeys=dense|random` produces:
///   - N keys, each exactly 4 bytes
///   - no byte 0x00 in any key
///   - all keys distinct
///   - `MPHF<GlGhStorage,NoKey>::build()` succeeds on the recovered uint32 keys
///   - `mphf.query()` is a bijection onto [0, N)
///
/// Build (macOS — the framework does not configure on ARM):
///   c++ -std=c++20 -D_LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS             \
///       -Ilib -Iextlib/util/include -Iextlib/cltj/include -I$HOME/include \
///       -o harness_integer_keys                                          \
///       lib/Contender.cpp tests/harness_integer_keys.cpp                 \
///       -L$HOME/lib -lsdsl

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Contender.h"
#include "hashing/mphf_bdz.hpp"
#include "hashing/storage/glgh.hpp"
#include "hashing/key_policies.hpp"

using Mphf = cltj::hashing::MPHF<cltj::hashing::GlGhStorage, cltj::hashing::policies::NoKey>;

// `generateInputData` is defined in lib/Contender.cpp but not declared in Contender.h.
std::vector<std::string> generateInputData(
    size_t N, uint64_t seed, const std::string &integerKeys
);

static int testMode(const std::string &mode, size_t N, uint64_t seed) {
    std::cout << "\n=== " << mode << "  N=" << N << "  seed=" << seed << " ===" << std::endl;

    Contender::integerKeys = mode;
    std::vector<std::string> keys = generateInputData(N, seed, Contender::integerKeys);

    // 1. Every key is exactly 4 bytes.
    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i].size() != sizeof(uint32_t)) {
            std::cerr << "FAIL: key[" << i << "] length " << keys[i].size()
                      << " (expected 4)" << std::endl;
            return 1;
        }
    }
    std::cout << "  [OK] length == 4" << std::endl;

    // 2. No byte is 0x00.
    for (size_t i = 0; i < keys.size(); ++i) {
        for (char c : keys[i]) {
            if (c == 0) {
                std::cerr << "FAIL: zero byte in key[" << i << "]" << std::endl;
                return 1;
            }
        }
    }
    std::cout << "  [OK] no zero bytes" << std::endl;

    // 3. All keys are distinct.
    std::vector<std::string> sorted = keys;
    std::sort(sorted.begin(), sorted.end());
    auto dup = std::adjacent_find(sorted.begin(), sorted.end());
    if (dup != sorted.end()) {
        std::cerr << "FAIL: duplicate keys" << std::endl;
        return 1;
    }
    std::cout << "  [OK] all distinct" << std::endl;

    // 4. Recover uint32 keys and build MPHF.
    std::vector<uint32_t> uintKeys;
    uintKeys.reserve(N);
    for (const auto &s : keys) {
        uint32_t k;
        std::memcpy(&k, s.data(), sizeof(k));
        uintKeys.push_back(k);
    }

    Mphf mphf;
    if (!mphf.build(uintKeys)) {
        std::cerr << "FAIL: mphf.build() returned false" << std::endl;
        return 1;
    }
    std::cout << "  [OK] build  retries=" << mphf.retry_count()
              << "  residual=" << mphf.n_residual() << std::endl;

    // 5. query() is a bijection onto [0, N).
    std::vector<bool> seen(N, false);
    for (uint32_t k : uintKeys) {
        uint32_t slot = mphf.query(k);
        if (slot >= N) {
            std::cerr << "FAIL: query(" << k << ") = " << slot
                      << " out of range [0," << N << ")" << std::endl;
            return 1;
        }
        if (seen[slot]) {
            std::cerr << "FAIL: collision at slot " << slot << std::endl;
            return 1;
        }
        seen[slot] = true;
    }
    std::cout << "  [OK] bijection" << std::endl;

    return 0;
}

int main() {
    int ret = 0;

    ret |= testMode("dense",  100000, 42);
    ret |= testMode("dense", 1000000, 42);

    ret |= testMode("random",  100000, 42);
    ret |= testMode("random", 1000000, 42);

    if (ret == 0) {
        std::cout << "\n=== ALL PASS ===" << std::endl;
    } else {
        std::cerr << "\n=== FAIL ===" << std::endl;
    }
    return ret;
}

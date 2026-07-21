#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include "Contender.h"
#include "hashing/mphf_bdz.hpp"
#include "hashing/storage/glgh.hpp"
#include "hashing/storage/packed_glgh.hpp"
#include "hashing/key_policies.hpp"
#include "hashing/mod_policies.hpp"
#include <type_traits>

namespace cltj_hashing = cltj::hashing;

template <typename Storage = cltj_hashing::GlGhStorage,
          typename ModPolicy = cltj_hashing::policies::NativeMod>
class GlGhContender : public Contender {
    cltj_hashing::MPHF<Storage, cltj_hashing::policies::NoKey, ModPolicy> mphf_;
    std::vector<uint32_t> uint_keys_;

    static uint32_t readIntKey(const std::string &key) {
        uint32_t k;
        std::memcpy(&k, key.data(), sizeof(k));
        return k;
    }

public:
    GlGhContender(size_t N, double loadFactor)
        : Contender(N, 1.0) { (void) loadFactor; }

    std::string name() override {
        std::string s;
        if constexpr (std::is_same_v<Storage, cltj_hashing::PackedGlGhStorage>)
            s = "PackedGlGh";
        else
            s = "GlGh";
        if constexpr (std::is_same_v<ModPolicy, cltj_hashing::policies::FastMod>)
            s += "-fastmod";
        s += " glghRetries=" + std::to_string(mphf_.retry_count());
        s += " glghResidual=" + std::to_string(mphf_.n_residual());
        return s;
    }

    void beforeConstruction(const std::vector<std::string> &keys) override {
        uint_keys_.clear();
        uint_keys_.reserve(keys.size());
        for (const auto &str : keys) {
            assert(str.size() == sizeof(uint32_t));
            uint_keys_.push_back(readIntKey(str));
        }
    }

    void construct(const std::vector<std::string> &keys) override {
        (void) keys;
        if (!mphf_.build(uint_keys_)) {
            throw std::logic_error("GlGh build failed after retries");
        }
    }

    size_t sizeBits() override {
        return 8 * mphf_.size_in_bytes();
    }

    void performQueries(std::span<std::string> keys) override {
        auto fn = [this](std::string &key) {
            return static_cast<size_t>(mphf_.query(readIntKey(key)));
        };
        doPerformQueries(keys, fn);
    }

    void performTest(std::span<std::string> keys) override {
        auto fn = [this](std::string &key) {
            return static_cast<size_t>(mphf_.query(readIntKey(key)));
        };
        doPerformTest(keys, fn);
    }
};

void glghContenderRunner(size_t N, double loadFactor);
void glghFastmodContenderRunner(size_t N, double loadFactor);
void packedGlghContenderRunner(size_t N, double loadFactor);
void packedGlghFastmodContenderRunner(size_t N, double loadFactor);

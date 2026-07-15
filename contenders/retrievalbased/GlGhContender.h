#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "Contender.h"
#include "hashing/mphf_bdz.hpp"
#include "hashing/storage/glgh.hpp"
#include "hashing/key_policies.hpp"

namespace cltj_hashing = cltj::hashing;

class GlGhContender : public Contender {
    cltj_hashing::MPHF<cltj_hashing::GlGhStorage, cltj_hashing::policies::NoKey> mphf_;
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
        auto s = std::string("GlGh");
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

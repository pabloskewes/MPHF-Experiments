#include "Contender.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <chrono>
#include <bytehamster/util/XorShift64.h>
#include <unistd.h>
#include <thread>
#include <span>
#include "PerformanceCounter.h"

/// @brief  Bijection [0, 255^4) -> uint32 whose 4 bytes are all in [1,255].
///
/// Writes the value in base 255, then maps each digit d (0–254) to the byte
/// d+1 (1–255), so no byte is 0x00.  Needed because cmph and the Rust FFI read
/// keys with strlen() during construction: a zero byte would truncate the key.
static uint32_t zeroFreeKey(uint64_t value) {
    uint32_t result = 0;
    for (int bytePos = 0; bytePos < 4; bytePos++) {
        int digit = static_cast<int>(value % 255);  // least significant digit in base 255
        value /= 255;                               // shift right by one base-255 digit
        int byte = digit + 1;                       // map [0,254] → [1,255], never zero
        result  |= static_cast<uint32_t>(byte) << (8 * bytePos);
    }
    return result;
}

/// @brief  Generate N distinct zero‑free uint32 keys, then shuffle them.
/// @param dense  If true: consecutive keys (0,1,…); otherwise random from [0, 255^4).
static std::vector<uint32_t> generateIntegerKeys(
    size_t N, bool dense, bytehamster::util::XorShift64 &prng
) {
    std::vector<uint32_t> keys;
    keys.reserve(N);
    if (dense) {
        for (size_t i = 0; i < N; i++) {
            keys.push_back(zeroFreeKey(i));
        }
    } else {
        while (keys.size() < N) {
            while (keys.size() < N) {
                keys.push_back(zeroFreeKey(prng(Contender::ZERO_FREE_UNIVERSE)));
            }
            std::sort(keys.begin(), keys.end());
            keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
        }
    }
    // Shuffle keys (Fisher-Yates)
    for (size_t i = keys.size(); i > 1; i--) {
        std::swap(keys[i - 1], keys[prng(i)]);
    }
    return keys;
}

std::vector<std::string> generateInputData(
    size_t N, uint64_t seed, const std::string &integerKeys
) {
    if (!integerKeys.empty()) {
        bytehamster::util::XorShift64 prng(seed);
        std::cout << "Generating input" << std::flush;
        std::vector<uint32_t> intKeys = generateIntegerKeys(N, integerKeys == "dense", prng);
        std::vector<std::string> inputData;
        inputData.reserve(N);
        for (uint32_t k : intKeys) {
            // Native-endian, only consumed in-process.
            inputData.emplace_back(reinterpret_cast<const char*>(&k), sizeof(k));
        }
        std::cout << "\rInput generation complete." << std::endl;
        return inputData;
    }

    std::vector<std::string> inputData;
    inputData.reserve(N);
    bytehamster::util::XorShift64 prng(seed);
    std::cout<<"Generating input"<<std::flush;
    char string[200];
    for (size_t i = 0; i < N; i++) {
        if ((i % (N/5)) == 0) {
            std::cout<<"\rGenerating input: "<<100l*i/N<<"%"<<std::flush;
        }
        size_t length = 10 + prng((30 - 10) * 2);
        for (std::size_t k = 0; k < (length + sizeof(uint64_t))/sizeof(uint64_t); ++k) {
            ((uint64_t*) string)[k] = prng();
        }
        // Repair null bytes
        for (std::size_t k = 0; k < length; ++k) {
            if (string[k] == 0) {
                string[k] = 1 + prng(254);
            }
        }
        string[length] = 0;
        inputData.emplace_back(string, length);
    }
    std::cout<<"\rInput generation complete."<<std::endl;
    return inputData;
}

void Contender::run(bool shouldPrintResult) {
    std::cout <<  std::endl;
    std::cout << "Contender: " << name().substr(0, name().find(' ')) << std::endl;

    if (seed == 0) {
        auto time = std::chrono::system_clock::now();
        seed = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
    }
    bytehamster::util::XorShift64 prng(seed);
    for (size_t i = 0; i < 10; i++) {
        prng(); // Ensure that first few generated seeds don't have too many zeroes when users pick small seeds
    }
    std::cout << "Seed: " << seed << std::endl;
    std::vector<std::string> keys = generateInputData(N, prng(), integerKeys);
    beforeConstruction(keys);

    std::cout << "Cooldown" << std::endl;
    usleep(1000*1000);
    std::cout << "Constructing" << std::endl;

    PerformanceCounter constructionCounter;
    constructionCounter.start();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    try {
        construct(keys);
    } catch (const std::exception& e) {
        std::cout<<"Error: "<<e.what()<<std::endl;
        return;
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    constructionCounter.stop();
    constructionTimeMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    constructionCacheMisses = constructionCounter.getCount();

    if (!skipTests) {
        std::cout<<"Testing"<<std::endl;
        performTest(keys);
    }

    queryTimeMilliseconds = 0;
    if (numQueries > 0) {
        std::cout<<"Preparing query plan"<<std::endl;
        std::vector<std::string> queryPlan;
        queryPlan.reserve(numQueries * numQueryThreads);
        for (size_t i = 0; i < numQueries * numQueryThreads; i++) {
            queryPlan.push_back(keys[prng(N)]);
        }
        beforeQueries(queryPlan);
        std::cout << "Cooldown" << std::endl;
        usleep(1000*1000);
        std::cout<<"Querying"<<std::endl;
        if (numQueryThreads == 1) {
            PerformanceCounter queryCounter;
            queryCounter.start();
            begin = std::chrono::steady_clock::now();
            performQueries(queryPlan);
            end = std::chrono::steady_clock::now();
            queryCounter.stop();
            queryTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            queryCacheMisses = queryCounter.getCount();
        } else {
            std::vector<std::thread> threads;
            begin = std::chrono::steady_clock::now();
            for (size_t i = 0; i < numQueryThreads; i++) {
                std::span<std::string> querySpan(queryPlan.begin() + i * numQueries,
                                                 queryPlan.begin() + (i + 1) * numQueries);
                threads.emplace_back([&querySpan, this] {
                    performQueries(querySpan);
                });
            }
            for (size_t i = 0; i < numQueryThreads; i++) {
                threads.at(i).join();
            }
            end = std::chrono::steady_clock::now();
            queryTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        }
    }
    if (shouldPrintResult) {
        printResult();
    }
}

void Contender::printResult(const std::string &additional) {
    // Some competitors print stuff when determining their space consumption
    double bitsPerElement = (double) sizeBits() / N;
    size_t totalQueries = numQueries * numQueryThreads;
    std::cout << "RESULT"
              << " name=" << name()
              << " bitsPerElement=" << bitsPerElement
              << " constructionTimeMilliseconds=" << (constructionTimeMicroseconds < 10000
                                    ? std::to_string(0.001 * constructionTimeMicroseconds)
                                    : std::to_string(constructionTimeMicroseconds / 1000))
              << " constructionCacheMissesPerKey=" << (1.0f * constructionCacheMisses / N)
              << " queryTimeMilliseconds=" << queryTimeMilliseconds
              << " numQueries=" << numQueries
              << " numQueriesTotal=" << totalQueries
              << " cacheMissesPerQuery=" << (totalQueries > 0 ? (1.0f * queryCacheMisses / totalQueries) : 0)
              << " N=" << N
              << " integerKeys=" << (integerKeys.empty() ? std::string("off") : integerKeys)
              << " loadFactor=" << loadFactor
              << " threads=" << numThreads
              << " queryThreads=" << numQueryThreads
              << additional
              << std::endl;
}

size_t Contender::numQueries = 5e7;
size_t Contender::numThreads = 1;
size_t Contender::numQueryThreads = 1;
size_t Contender::seed = 0;
bool Contender::skipTests = false;
std::string Contender::integerKeys = "";

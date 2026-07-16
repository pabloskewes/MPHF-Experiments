#include "RustPhastPlusWrappedContender.h"

// Coarse sweep: same ranges as upstream, step 25 instead of 5, Elias-Fano only. The full sweep is
// 1652 configurations (826 x 2 encoders), ~91% of the whole benchmark, and it redraws a frontier
// that Beling & Sanders already published (ALENEX 2026, arXiv:2504.17918, Table 5.3), whose every
// point but two is Elias-Fano.

void rustPHastPlusWrappedContenderRunnerWithEf(size_t N, bool use_ef) {
    // ---------- multiplier 1 ----------
    for (size_t bucket_size_100 = 370; bucket_size_100 <= 540; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 8, bucket_size_100, 512, use_ef).run();
    for (size_t bucket_size_100 = 360; bucket_size_100 <= 540; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 8, bucket_size_100, 1024, use_ef).run();

    for (size_t bucket_size_100 = 440; bucket_size_100 <= 565; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 9, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 430; bucket_size_100 <= 660; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 10, bucket_size_100, 1024, use_ef).run();
    for (size_t bucket_size_100 = 420; bucket_size_100 <= 645; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 10, bucket_size_100, 2048, use_ef).run();

    for (size_t bucket_size_100 = 420; bucket_size_100 <= 695; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 11, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 460; bucket_size_100 <= 750; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 1, 12, bucket_size_100, 0, use_ef).run();


    // ---------- multiplier 2 ----------
    for (size_t bucket_size_100 = 350; bucket_size_100 <= 515; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 2, 8, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 390; bucket_size_100 <= 580; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 2, 9, bucket_size_100, 1024, use_ef).run();
    for (size_t bucket_size_100 = 390; bucket_size_100 <= 580; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 2, 9, bucket_size_100, 2048, use_ef).run();

    for (size_t bucket_size_100 = 400; bucket_size_100 <= 640; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 2, 10, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 445; bucket_size_100 <= 690; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 2, 11, bucket_size_100, 0, use_ef).run();


    // ---------- multiplier 3 ----------
    for (size_t bucket_size_100 = 340; bucket_size_100 <= 510; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 3, 8, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 390; bucket_size_100 <= 570; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 3, 9, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 360; bucket_size_100 <= 620; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 3, 10, bucket_size_100, 2048, use_ef).run();
    for (size_t bucket_size_100 = 360; bucket_size_100 <= 640; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 3, 10, bucket_size_100, 4096, use_ef).run();

    for (size_t bucket_size_100 = 360; bucket_size_100 <= 685; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 3, 11, bucket_size_100, 0, use_ef).run();

    for (size_t bucket_size_100 = 450; bucket_size_100 <= 750; bucket_size_100 += 25)
        RustPhastPlusWrappedContender(N, 3, 12, bucket_size_100, 0, use_ef).run();
}

void rustPHastPlusWrappedContenderRunner(size_t N) {
    rustPHastPlusWrappedContenderRunnerWithEf(N, true);
}

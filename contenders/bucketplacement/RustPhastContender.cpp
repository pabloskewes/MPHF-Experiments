#include "RustPhastContender.h"

// Coarse sweep of PHast's Pareto frontier: same ranges as upstream, step 20 instead of 5-10.
// The full sweep is ~300 configurations, which redraws a frontier that Beling & Sanders already
// published (ALENEX 2026, arXiv:2504.17918, Table 5.2). A coarse sweep still resolves the frontier
// on this machine while costing ~3x less, and unlike a curated list it does not assume the paper's
// optimal parameters carry over to a different CPU and key count.
// bits_per_seed=13 is dropped: it is not in the paper, only in the upstream sweep.
void rustPHastContenderRunner(size_t N, bool use_ef) {
    for (size_t bucket_size_100 = 100; bucket_size_100 <= 300; bucket_size_100 += 20)
        RustPhastContender(N, 4, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 160; bucket_size_100 <= 330; bucket_size_100 += 20)
        RustPhastContender(N, 5, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 260; bucket_size_100 <= 350; bucket_size_100 += 20)
        RustPhastContender(N, 6, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 320; bucket_size_100 <= 410; bucket_size_100 += 20)
        RustPhastContender(N, 7, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 350; bucket_size_100 <= 475; bucket_size_100 += 20)
        RustPhastContender(N, 8, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 470; bucket_size_100 <= 530; bucket_size_100 += 20)
        RustPhastContender(N, 9, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 530; bucket_size_100 <= 610; bucket_size_100 += 20)
        RustPhastContender(N, 10, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 570; bucket_size_100 <= 680; bucket_size_100 += 20)
        RustPhastContender(N, 11, bucket_size_100, use_ef).run();

    for (size_t bucket_size_100 = 650; bucket_size_100 <= 740; bucket_size_100 += 20)
        RustPhastContender(N, 12, bucket_size_100, use_ef).run();
}

void rustPHastContenderRunner(size_t N) {
    rustPHastContenderRunner(N, true);
    rustPHastContenderRunner(N, false);
}

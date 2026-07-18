#include "SicHashContender.h"

void sicHashContenderRunner(size_t N, double loadFactor, bool minimalOnly, bool onlyPartial) {
    (void) minimalOnly;
    (void) onlyPartial;
    SicHashContender<true, 64, 3>(N, loadFactor, sichash::SicHashConfig().spaceBudget(1.9, 0.0)).run();
    SicHashContender<true, 64, 3>(N, loadFactor, sichash::SicHashConfig().spaceBudget(2.2, 0.0)).run();
}

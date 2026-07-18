#include "PTHashContender.h"

void ptHashContenderRunner(size_t N, double loadFactor, bool minimalOnly) {
    (void) loadFactor;
    (void) minimalOnly;
    PTHashContender<true, pthash::elias_fano>(N, 0.95, 6.0).run();
    PTHashContender<true, pthash::compact_compact>(N, 0.99, 4.0).run();
}

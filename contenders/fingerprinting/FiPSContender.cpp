#include "FiPSContender.h"

void fiPSContenderRunner(size_t N) {
    FiPSContender<512, uint32_t, false>(N, 5.0).run();
    FiPSContender<512, uint32_t, false>(N, 1.5).run();
}

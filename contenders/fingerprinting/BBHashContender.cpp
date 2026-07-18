#include "BBHashContender.h"

void bbHashContenderRunner(size_t N) {
    BBHashContender(N, 5.0, 0).run();
    BBHashContender(N, 1.5, 0).run();
}
#include "RustPhastPlusWrappedContender.h"

void rustPHastPlusWrappedContenderRunner(size_t N) {
    RustPhastPlusWrappedContender(N, 1, 11, 660, 0, true).run();
    RustPhastPlusWrappedContender(N, 2, 8, 435, 0, true).run();
}

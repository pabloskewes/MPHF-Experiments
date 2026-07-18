#include "RustPhastContender.h"

void rustPHastContenderRunner(size_t N) {
    RustPhastContender(N, 7, 370, true).run();
    RustPhastContender(N, 11, 630, true).run();
}

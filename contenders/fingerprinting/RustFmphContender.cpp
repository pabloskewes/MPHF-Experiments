#include "RustFmphContender.h"

void rustFmphContenderRunner(size_t N) {
    RustFmphContender(N, 5.0).run();
    RustFmphContender(N, 1.5).run();
}

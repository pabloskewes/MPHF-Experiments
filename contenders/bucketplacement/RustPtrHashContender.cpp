#include "RustPtrHashContender.h"

void rustPtrHashContenderRunner(size_t N) {
    RustPtrHashContender(N, RustPtrHashContender::VARIANT_LINEAR_VEC, 3.0).run();
    RustPtrHashContender(N, RustPtrHashContender::VARIANT_CUBIC_EF, 4.0).run();
}

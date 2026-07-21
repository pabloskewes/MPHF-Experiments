#include "GlGhContender.h"

namespace {
template <typename Storage, typename ModPolicy>
void runGlGh(size_t N, double loadFactor) {
    if (Contender::integerKeys.empty()) {
        std::cerr << "GlGh requires --integerKeys dense|random: keys from std::hash collide, "
                     "and duplicate keys are unpeelable" << std::endl;
        return;
    }
    GlGhContender<Storage, ModPolicy>(N, loadFactor).run();
}
}  // namespace

void glghContenderRunner(size_t N, double loadFactor) {
    runGlGh<cltj_hashing::GlGhStorage, cltj_hashing::policies::NativeMod>(N, loadFactor);
}

void glghFastmodContenderRunner(size_t N, double loadFactor) {
    runGlGh<cltj_hashing::GlGhStorage, cltj_hashing::policies::FastMod>(N, loadFactor);
}

void packedGlghContenderRunner(size_t N, double loadFactor) {
    runGlGh<cltj_hashing::PackedGlGhStorage, cltj_hashing::policies::NativeMod>(N, loadFactor);
}

void packedGlghFastmodContenderRunner(size_t N, double loadFactor) {
    runGlGh<cltj_hashing::PackedGlGhStorage, cltj_hashing::policies::FastMod>(N, loadFactor);
}

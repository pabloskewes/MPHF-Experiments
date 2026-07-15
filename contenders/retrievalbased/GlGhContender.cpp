#include "GlGhContender.h"

void glghContenderRunner(size_t N, double loadFactor) {
    if (Contender::integerKeys.empty()) {
        std::cerr << "GlGh requires --integerKeys=dense|random: keys from std::hash collide, "
                     "and duplicate keys are unpeelable" << std::endl;
        return;
    }
    GlGhContender(N, loadFactor).run();
}

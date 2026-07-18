#include "ConsensusContender.h"

void consensusContenderRunner(size_t N) {
    ConsensusContender<512, 0.1>(N).run();
}

#include "RecSplitContender.h"

void recSplitContenderRunner(size_t N) {
    RecSplitContender<8>(N, 100).run();
}

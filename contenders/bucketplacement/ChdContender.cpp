#include "ChdContender.h"

void chdContenderRunner(size_t N, double loadFactor) {
    for (int keysPerBucket = 4; keysPerBucket <= 6; keysPerBucket += 2) {
        {ChdContender(N, loadFactor, loadFactor, keysPerBucket, false).run();}
        {ChdContender(N, loadFactor, loadFactor, keysPerBucket, true).run();}
    }
}

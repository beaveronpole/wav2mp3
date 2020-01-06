//
// Created by beaver on 18.12.2019.
//

#include "WaveDataReaderCreator.h"

using namespace std;

BaseWaveDataReader *WaveDataReaderCreator::createDataReader(uint32_t bitsPerSample, bool isFloat) {
    if (bitsPerSample == 32 && isFloat)
        return new FloatWaveDataReader();
    else if (bitsPerSample == 64 && isFloat)
        return new DoubleWaveDataReader();
    else if (bitsPerSample > 8) {
        return new SignedIntWaveDataReader();
    }
    else if (bitsPerSample > 1 && bitsPerSample <= 8 && bitsPerSample > 0)
        return new UnsignedIntWaveDataReader();
    return NULL;
}

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
    else if (bitsPerSample > 8 && bitsPerSample <= 16) {
        return new SignedIntWaveDataReader<int16_t>();
    }
    else if (bitsPerSample > 16 && bitsPerSample <= 32) {
        return new SignedIntWaveDataReader<int32_t>();
    }
    else if (bitsPerSample <= 8 && bitsPerSample > 0)
        return new UnsignedIntWaveDataReader();
    cerr << "Error on creation wave data reader. Smth weird: is float = " << isFloat << ", bitSize = " << bitsPerSample << endl;
    return NULL;
}

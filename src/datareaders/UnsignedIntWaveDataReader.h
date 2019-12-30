//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_UNSIGNEDINTWAVEDATAREADER_H
#define WAV2MP3_UNSIGNEDINTWAVEDATAREADER_H

#include <cmath>

#include "BaseWaveDataReader.h"

/*
 * Read unsigned integer data from file
 * Sample size may be 2..8 bits (in theory)
 */

class UnsignedIntWaveDataReader : public BaseWaveDataReader {
protected:
    virtual void fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize, vector<vector<int32_t> *> *vector);
};


#endif //WAV2MP3_UNSIGNEDINTWAVEDATAREADER_H

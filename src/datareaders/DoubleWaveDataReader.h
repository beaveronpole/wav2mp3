//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_DOUBLEWAVEDATAREADER_H
#define WAV2MP3_DOUBLEWAVEDATAREADER_H

#include "BaseWaveDataReader.h"

/*
 * Read double (float64) data from file
 */

class DoubleWaveDataReader : public BaseWaveDataReader {
protected:
    virtual void fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize, vector<vector<int32_t> *> *vector);
};


#endif //WAV2MP3_DOUBLEWAVEDATAREADER_H

//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_FLOATWAVEDATAREADER_H
#define WAV2MP3_FLOATWAVEDATAREADER_H

#include "BaseWaveDataReader.h"

/*
 * Read float (float32) data from file
 */

class FloatWaveDataReader : public BaseWaveDataReader{
protected:
    virtual void fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize, vector<vector<int32_t> *> *vector);
};


#endif //WAV2MP3_FLOATWAVEDATAREADER_H

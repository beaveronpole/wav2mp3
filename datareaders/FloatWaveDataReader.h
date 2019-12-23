//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_FLOATWAVEDATAREADER_H
#define WAV2MP3_FLOATWAVEDATAREADER_H

#include "BaseWaveDataReader.h"

class FloatWaveDataReader : public BaseWaveDataReader{
protected:
    virtual void fillDataStorage(vector<vector<int32_t> *> *vector);
};


#endif //WAV2MP3_FLOATWAVEDATAREADER_H

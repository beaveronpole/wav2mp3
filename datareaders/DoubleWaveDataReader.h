//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_DOUBLEWAVEDATAREADER_H
#define WAV2MP3_DOUBLEWAVEDATAREADER_H

#include "BaseWaveDataReader.h"


class DoubleWaveDataReader : public BaseWaveDataReader {
protected:
    virtual void fillDataStorage(vector<vector<int32_t> *> *vector);
};


#endif //WAV2MP3_DOUBLEWAVEDATAREADER_H

//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_SIGNEDINTWAVEDATAREADER_H
#define WAV2MP3_SIGNEDINTWAVEDATAREADER_H

#include "BaseWaveDataReader.h"

/*
 * Read signed integer data from file
 * Sample size may be 9..32 bits (in theory)
 */

class SignedIntWaveDataReader : public BaseWaveDataReader {
public:
protected:
    virtual void fillDataStorage(vector<uint8_t> *rawBuf,
                                 uint32_t rawBufDataSize,
                                 vector<vector<int32_t> *> *dataOut);
};


#endif //WAV2MP3_SIGNEDINTWAVEDATAREADER_H

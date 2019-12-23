//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_SIGNEDINTWAVEDATAREADER_H
#define WAV2MP3_SIGNEDINTWAVEDATAREADER_H

#include "BaseWaveDataReader.h"

/* Class for reading signed wave data from given file from given position of given size
 *
 */

class SignedIntWaveDataReader : public BaseWaveDataReader{
public:
protected:
    virtual void fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize, vector<vector<int32_t> *> *vector);
};


#endif //WAV2MP3_SIGNEDINTWAVEDATAREADER_H

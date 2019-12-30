//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_SIGNEDINTWAVEDATAREADER_H
#define WAV2MP3_SIGNEDINTWAVEDATAREADER_H

#include "BaseWaveDataReader.h"

/* Class for reading signed wave data from given file from given position of given size
 *
 */

template <class T>
class SignedIntWaveDataReader : public BaseWaveDataReader {
public:
protected:
    virtual void fillDataStorage(vector<uint8_t> *rawBuf,
                                 uint32_t rawBufDataSize,
                                 vector<vector<int32_t> *> *dataOut) {

        // on start all variables are checked in parent's getData

        //TODO try to read silence
        //TODO check file size and data size
        //TODO check if EOF
        //TODO think about stream encoding

        uint8_t shift_value = 8*sizeof(int32_t) - m_bitsPerSample;
        T value = 0;
        int32_t outValue;
        for (int i = 0, ch = 0;
             i < rawBufDataSize; //TODO think about last sample, it is after SIZE
             i += m_bytesPerSample, ch = i % (m_channelsCount * m_bytesPerSample) == 0 ? 0 : (ch + 1)) {
            if (ch >= 2)
                continue;
            value = *(T*)(rawBuf->data()+i);
            outValue = value;
            dataOut->at(ch)->push_back(outValue << shift_value);
        }
    }
};


#endif //WAV2MP3_SIGNEDINTWAVEDATAREADER_H

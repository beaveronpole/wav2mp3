//
// Created by beaver on 18.12.2019.
//

#include "SignedIntWaveDataReader.h"

using namespace std;

void SignedIntWaveDataReader::fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize,
                                              vector<vector<int32_t> *> *dataOut) {
    // on start all variables are checked in parent's getData

    //TODO try to read silence
    //TODO check file size and data size
    //TODO check if EOF
    //TODO think about stream encoding

    uint8_t shift_value = 32 - m_bitsPerSample;
    for (int i = 0, ch = 0 ;
         i < rawBufDataSize;
         i += m_bytesPerSample, ch = i%(m_channelsCount*m_bytesPerSample)==0?0:(ch+1)){
        if (ch >= 2)
            continue;
        dataOut->at(ch)->push_back( (*(int32_t*)(rawBuf->data()+i)) << shift_value);
    }
}

//
// Created by beaver on 18.12.2019.
//

#include "UnsignedIntWaveDataReader.h"

void UnsignedIntWaveDataReader::fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize,
                                                vector<vector<int32_t> *> *dataOut) {
    // on start all variables are checked in parent's getData
    uint8_t shift_value = 32 - m_bitsPerSample;
    int32_t midValue = pow(2, m_bitsPerSample-1);
    for (int i = 0, ch = 0 ;
         i < rawBufDataSize;
         i += m_bytesPerSample, ch = i%(m_channelsCount*m_bytesPerSample)==0?0:(ch+1)){
        if (ch >= 2)
            continue;
        dataOut->at(ch)->push_back( ((int32_t)(*(uint8_t*)(rawBuf->data()+i)) - midValue) << shift_value);
    }
}

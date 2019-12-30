//
// Created by beaver on 18.12.2019.
//

#include "FloatWaveDataReader.h"

void FloatWaveDataReader::fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize,
                                          vector<vector<int32_t> *> *dataOut) {
    // on start all variables are checked in parent's getData
    for (int i = 0, ch = 0 ;
         i < rawBufDataSize;
         i += m_bytesPerSample, ch = i%(m_channelsCount*m_bytesPerSample)==0?0:(ch+1)){
        if (ch >= 2)
            continue;
        dataOut->at(ch)->push_back( (*(float*)(rawBuf->data()+i))*INT32_MAX);
    }
}

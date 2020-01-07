//
// Created by beaver on 18.12.2019.
//

#include "SignedIntWaveDataReader.h"

using namespace std;

void SignedIntWaveDataReader::fillDataStorage(vector<uint8_t> *rawBuf,
                                              uint32_t rawBufDataSize,
                                              vector<vector<int32_t> *> *dataOut) {
    // on start all variables are checked in parent's getData
    uint8_t shift_value = 8*sizeof(int32_t) - m_bitsPerSample;
    int32_t value = 0;
    for (uint32_t i = 0, ch = 0;
         i < rawBufDataSize;
         i += m_bytesPerSample, ch = i % (m_channelsCount * m_bytesPerSample) == 0 ? 0 : (ch + 1)) {
        if (ch >= 2)
            continue;
        value = *(int32_t*)(rawBuf->data()+i);
        dataOut->at((int)ch)->push_back(value << shift_value);
    }
}

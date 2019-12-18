//
// Created by beaver on 18.12.2019.
//

#include "SignedIntWaveDataReader.h"

using namespace std;

void SignedIntWaveDataReader::fullDataStorage(vector<vector<int32_t> *> * dataOut) {
    // on start all variables are checked in parent's getData

    //TODO try to read silence
    //TODO check file size and data size
    //TODO check if EOF
    //TODO think about stream encoding
    uint8_t shift_value = 32 - m_bitsPerSample;
    while(true){
        int status;
        int32_t tmp_sample = 0;

        for (int channel_number = 0; channel_number < m_channelsCount; channel_number++) {
            //read sample
            status = fread(&tmp_sample, m_bytesPerSample, 1, m_fd);
            if (status != 1) {
                cerr << "Error on reading data chunk." << endl;
                return;
            }
            dataOut->at(channel_number)->push_back(tmp_sample << shift_value);
        }
    }
}

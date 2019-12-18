//
// Created by beaver on 18.12.2019.
//

#include "BaseWaveDataReader.h"

BaseWaveDataReader::BaseWaveDataReader():
        m_fd(NULL),
        m_dataSize_bytes(0),
        m_channelsCount(0)
{
}

void BaseWaveDataReader::init(FILE *fd, uint32_t dataSize_bytes, uint32_t channelsCount, uint32_t bitsPerSample) {
    m_bitsPerSample = bitsPerSample;
    if (m_bitsPerSample < 2){
        cerr << "Error in sample size." << endl;
        return;
    }

    m_fd = fd;
    m_dataSize_bytes = dataSize_bytes;
    m_channelsCount = channelsCount;

    if (m_bitsPerSample <= 8){
        m_bytesPerSample = 1;
    }
    else if (m_bitsPerSample <= 16){
        m_bytesPerSample = 2;
    }
    else if (m_bitsPerSample <= 32){
        m_bytesPerSample = 4;
    }
    else{
        m_bytesPerSample  = 8;
    }
}

void BaseWaveDataReader::clearDataVectors(vector<vector<int32_t>* >* v) {
    if (v){
        for (int i =0; i < v->size(); ++i){
            (*v)[i]->clear();
            delete (*v)[i];
        }
    }
}

vector<vector<int32_t>* > *BaseWaveDataReader::getData(uint32_t samplesPerChannel) {
    if (m_fd == NULL || m_dataSize_bytes == 0 || m_channelsCount == 0){
        cerr << "Error on get data. Data reader is not initialized" << endl;
        return NULL;
    }
    vector<vector<int32_t>* >* out = new vector<vector<int32_t>* >();
    for (int i =0; i < m_channelsCount; i++){
        vector<int32_t>* tmp_v = new vector<int32_t>();
        tmp_v->reserve(samplesPerChannel);
        out->push_back( tmp_v );
    }
    fullDataStorage(out); // function for specialization of children classes
    return out;
}

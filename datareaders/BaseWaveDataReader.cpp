//
// Created by beaver on 18.12.2019.
//

#include "BaseWaveDataReader.h"

BaseWaveDataReader::BaseWaveDataReader():
        m_fd(NULL),
        m_channelsCount(0)
{
}

void BaseWaveDataReader::init(FILE *fd, uint32_t channelsCount, uint32_t bitsPerSample) {
    m_bitsPerSample = bitsPerSample;
    if (m_bitsPerSample < 2){
        cerr << "Error in sample size." << endl;
        return;
    }

    m_fd = fd;
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

uint32_t BaseWaveDataReader::getData(vector<vector<int32_t>* >* buf, uint32_t size_samples) {
    //TODO read bytes count!! in args (now it reads till file's end)
    if (m_fd == NULL || m_channelsCount == 0){
        cerr << "Error on get data. Data reader is not initialized" << endl;
        return NULL;
    }

    //TODO read data from file HERE!!! ANd put it to subclasses
    uint32_t sizeForReading = size_samples * m_channelsCount * m_bytesPerSample;
    m_rawBuffer.reserve(sizeForReading);
    //read from file available data
    size_t status = fread(m_rawBuffer.data(), sizeForReading, 1, m_fd);
    if (status != 1) {
        cerr << "Error on reading data from file." << endl;
        return 0;
    }
    buf->at(0)->clear();
    buf->at(1)->clear();
    fillDataStorage(&m_rawBuffer, sizeForReading, buf); // function for specialization of children classes
    return buf[0].size();
}

//
// Created by beaver on 18.12.2019.
//

#include "BaseWaveDataReader.h"

BaseWaveDataReader::BaseWaveDataReader():
        m_fd(NULL),
        m_channelsCount(0)
{
}

void BaseWaveDataReader::init(FILE *fd, uint32_t channelsCount, uint32_t bitsPerSample, uint32_t bytesPerSample) {
    m_bitsPerSample = bitsPerSample;
    if (m_bitsPerSample < 2){
        SIMPLE_LOGGER.addErrorLine("Error in sample size.\n");
        return;
    }

    m_fd = fd;
    m_channelsCount = channelsCount;
    m_bytesPerSample = bytesPerSample;
}

uint32_t BaseWaveDataReader::getData(vector<vector<int32_t>* >* buf, uint32_t size_samples) {
    if (m_fd == NULL || m_channelsCount == 0){
        SIMPLE_LOGGER.addErrorLine("Error on get data. Data reader is not initialized.\n");
        return 0;
    }

    // get size in bytes for reading from file
    uint32_t sizeForReading_bytes = size_samples * m_channelsCount * m_bytesPerSample;
    m_rawBuffer.reserve(sizeForReading_bytes);
    //read from file available data
    size_t status = fread(m_rawBuffer.data(), sizeForReading_bytes, 1, m_fd);
    if (status != 1) {
        SIMPLE_LOGGER.addErrorLine("Error on reading data from file.\n");
        return 0;
    }
    //subclass virtual function
    fillDataStorage(&m_rawBuffer, sizeForReading_bytes, buf); // function for specialization of children classes
    return buf[0].size();
}

BaseWaveDataReader::~BaseWaveDataReader() {
    m_rawBuffer.clear();
}


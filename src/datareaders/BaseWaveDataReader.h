//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_BASEWAVEDATAREADER_H
#define WAV2MP3_BASEWAVEDATAREADER_H

/* This class is a base class for reading data from file
 * Its children have to process the data of different types
 * (byte, short, word, float)
 */

#include <vector>
#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <cmath>

#include "../SimpleLogger.h"
#include "../FileHelper.h"

using namespace std;

class BaseWaveDataReader {
public:
    explicit BaseWaveDataReader();
    void init(FILE **fd, uint32_t channelsCount, uint32_t bitsPerSample, uint32_t bytesPerSample);

    // interface function try to get data of size_samples (in samples, converts to bytes inside) from a file
    uint32_t getData(vector<vector<int32_t> *>* buf, uint32_t size_samples) ;

    virtual ~BaseWaveDataReader();

protected:
    // function specialized in children classes (calls from interface function getData)
    // casts all data types to int32 -> then int32 goes to lame function
    virtual void fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize, vector<vector<int32_t> *> *out) = 0;

    FILE** m_fd;
    uint32_t m_channelsCount;
    uint32_t m_bitsPerSample;
    uint32_t m_bytesPerSample;

    // storage for raw data (read from file)
    vector<uint8_t> m_rawBuffer;
};


#endif //WAV2MP3_BASEWAVEDATAREADER_H

//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_BASEWAVEDATAREADER_H
#define WAV2MP3_BASEWAVEDATAREADER_H

/* This class is a base class for reading data from file
 * Children of it have to process the data of different types
 * (byte, short, word, float)
 *
 */

#include <vector>
#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <cmath>

using namespace std;

class BaseWaveDataReader {
public:
    BaseWaveDataReader();
    void init(FILE *fd, uint32_t channelsCount, uint32_t bitsPerSample);

    // function try to get data from a file
    uint32_t getData(vector<vector<int32_t> *>* buf, uint32_t size_samples) ;

    virtual ~BaseWaveDataReader();

protected:

    // fucnction specialized in children classes
    virtual void fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize, vector<vector<int32_t> *> *out) = 0;

    FILE* m_fd;
    uint32_t m_channelsCount;
    uint32_t m_bitsPerSample;
    uint32_t m_bytesPerSample;
    vector<uint8_t> m_rawBuffer;
};


#endif //WAV2MP3_BASEWAVEDATAREADER_H

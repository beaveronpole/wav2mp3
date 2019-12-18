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

using namespace std;

class BaseWaveDataReader {
public:
    BaseWaveDataReader();
    void init(FILE *fd, uint32_t dataSize_bytes, uint32_t channelsCount, uint32_t bitsPerSample);

    // function try to get data from a file
    vector< vector<int32_t>* >* getData(uint32_t samplesPerChannel = 0) ;

    //simple helper for deletion data vectors
    static void clearDataVectors(vector< vector<int32_t>* >*);

protected:

    // fucnction specialized in children classes
    virtual void fullDataStorage(vector< vector<int32_t>* >*) = 0;

    FILE* m_fd;
    uint32_t m_dataSize_bytes;
    uint32_t m_channelsCount;
    uint32_t m_bitsPerSample;
    uint32_t m_bytesPerSample;
};


#endif //WAV2MP3_BASEWAVEDATAREADER_H

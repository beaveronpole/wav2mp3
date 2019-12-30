//
// Created by beaver on 23.12.2019.
//

#ifndef WAV2MP3_SIGNALDATAENCODER_H
#define WAV2MP3_SIGNALDATAENCODER_H

#include <stdint.h>
#include <string>
#include <iostream>
#include <cstdio>

#include "lame/lame.h"

using namespace std;

/* class is a smth like adapter for lame function
 * Settings for converting are hardcoded inside the class
 */

class SignalDataEncoder {
public:
    SignalDataEncoder(const string &outputFileName,
                      uint32_t channelsCount,
                      uint32_t samplesPerSec,
                      uint32_t encodingChunkSize_samples);

    //function put data to encoder and save encoded data to file
    int32_t putDataForEncoding(int32_t *dataLeft, int32_t *dataRight, int32_t dataSize);

    //makes lame flush and close file
    int32_t finishEncoding();

    virtual ~SignalDataEncoder();

private:
    lame_global_flags* m_gfp;
    FILE* m_fd;
    uint8_t* m_mp3buffer;
    int32_t m_mp3bufferSize_bytes;
};


#endif //WAV2MP3_SIGNALDATAENCODER_H

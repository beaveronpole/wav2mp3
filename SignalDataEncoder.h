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
    SignalDataEncoder(const string &outputFileName, uint32_t channelsCount, uint32_t samplesPerSec);
    uint32_t putDataForEncoding(int32_t* data, uint32_t dataSize);
    void finishEncoding();

private:
    lame_global_flags* m_gfp;
    FILE* m_fd;
};


#endif //WAV2MP3_SIGNALDATAENCODER_H

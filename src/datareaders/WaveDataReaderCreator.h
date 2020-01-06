//
// Created by beaver on 18.12.2019.
//

#ifndef WAV2MP3_WAVEDATAREADERCREATOR_H
#define WAV2MP3_WAVEDATAREADERCREATOR_H

#include <iostream>

#include "SignedIntWaveDataReader.h"
#include "UnsignedIntWaveDataReader.h"
#include "FloatWaveDataReader.h"
#include "DoubleWaveDataReader.h"
#include "../SimpleLogger.h"

/* Creates the correct data reader by given params.
 * Smth like datareader factory
 */

class WaveDataReaderCreator {
public:
    static BaseWaveDataReader* createDataReader(uint32_t bitsPerSample, bool isFloat);
};


#endif //WAV2MP3_WAVEDATAREADERCREATOR_H

//
// Created by beaver on 12/31/19.
//

#ifndef WAV2MP3_WAVFILESCONVERTER_H
#define WAV2MP3_WAVFILESCONVERTER_H

#include <pthread.h>
#include <iostream>
#include <list>
#include <string>

#include <unistd.h>

#include "WAVFileConverter.h"

/*
 * Class makes several workers to encode file from given list
 */

class WAVFilesConverter {
public:
    WAVFilesConverter();

};


#endif //WAV2MP3_WAVFILESCONVERTER_H

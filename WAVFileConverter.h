//
// Created by beaver on 23.12.2019.
//

#ifndef WAV2MP3_WAVFILECONVERTER_H
#define WAV2MP3_WAVFILECONVERTER_H

#include <string>

#include "WavFileReader.h"
#include "SignalDataEncoder.h"


using namespace std;

/*
 * This class connects file WavFileReader with SignalDataEncoder
 * Creates output file with the same name
 *
 * WE MUST HAVE ONLY ONE OBJECT FOR ONE THREAD !!!!!!!
 */

class WAVFileConverter {
public:
    WAVFileConverter();
    void processFile(const string& fileName);

private:

    // check if file has extension ".wav" or ".wave"
    bool checkExtension(const string& fileName);


};


#endif //WAV2MP3_WAVFILECONVERTER_H

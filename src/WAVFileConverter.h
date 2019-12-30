//
// Created by beaver on 23.12.2019.
//

#ifndef WAV2MP3_WAVFILECONVERTER_H
#define WAV2MP3_WAVFILECONVERTER_H

#include <string>
#include <vector>

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
    virtual ~WAVFileConverter();

    WAVFileConverter(uint32_t encodingChunkSize_samples = 4096);
    void processFile(const string& fileName);

    // check if file has extension ".wav" or ".wave"
    static bool checkExtension(const string& fileName);

    // make file name with mp3 extension from given file name (*.wav)
    static string makeMP3FileName(const string& fileName);

private:
    void cleanBuffer();

    //TODO recreate reader for every new file
    WavFileReader* m_reader;

    //TODO recreate encoder for every new file
    SignalDataEncoder* m_encoder;

    uint32_t m_encodingChunkSize_samples;
    vector<vector<int32_t> *>* m_readerBuf;
};


#endif //WAV2MP3_WAVFILECONVERTER_H
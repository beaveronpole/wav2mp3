//
// Created by beaver on 23.12.2019.
//

#ifndef WAV2MP3_WAVFILECONVERTER_H
#define WAV2MP3_WAVFILECONVERTER_H

#include <string>
#include <vector>

#include "WavFileReader.h"
#include "SignalDataEncoder.h"
#include "SimpleLogger.h"


using namespace std;

/*
 * This class connects file WavFileReader with SignalDataEncoder
 * Creates output file with the same name
 *
 * WE MUST HAVE ONLY ONE OBJECT FOR EACH THREAD !!!!!!!
 */

class WAVFileConverter {
public:

    explicit WAVFileConverter(uint32_t encodingChunkSize_samples = 4096);
    void processFile(const string& fileName);

    // checks if file has extension ".wav" or ".wave"
    static bool checkExtension(const string& fileName);

    // makes file name with mp3 extension from given file name (*.wav)
    static string makeMP3FileName(const string& fileName);

    virtual ~WAVFileConverter();

private:
    //dummy function for cleaning buffer of 2 channels
    void cleanBuffer();

    // creates reader and encoder while processFile function
    // deletes them there too
    WavFileReader* m_reader;
    SignalDataEncoder* m_encoder;

    // parameter to store buffer size for putin (joke))
    // stores data chunk for one encode iteration: read chunk with this size from file -> put this chunk to encoder
    uint32_t m_encodingChunkSize_samples;

    //simple buffer for raw data
    vector<vector<int32_t> *>* m_readerBuf;
};


#endif //WAV2MP3_WAVFILECONVERTER_H

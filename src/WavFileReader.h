//
// Created by user on 11.12.2019.
//

#ifndef TEST_LAME_WAVFILEREADER_H
#define TEST_LAME_WAVFILEREADER_H


#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>
#include <stdint.h>
#include <map>
#include <vector>
#include <cerrno>
#include <algorithm>

#include "datareaders/datareaders.h"
#include "FileHelper.h"


//TODO think about PAD BYTE after chunk data. page 11, first standard

/*
 * This file  contains a description of a simple WAV file parser with related data structures.
 *
 * It can work with several PCM WAV formats:
 * unsigned 8 bit
 * signed 16 bit
 * signed 24 bit
 * signed 32 bit
 * float 32 bit
 * float 64 bit
 *
 * It should work with other formats (like unsigned 5 bit, or signed 12 bit, but i have no files to check)
 */

using namespace std;

//useful thing for RIFF format
struct FourCC{
    uint8_t fourcc[4];

    inline bool check(const string& str){
        return memcmp(str.c_str(), fourcc, 4) == 0;
    }

    inline FourCC(){
        memset(this->fourcc, 0, sizeof(this->fourcc));
    }
};

//thing for chunks with size
struct ChunkHeader{
    FourCC fourcc; //must be RIFF
    uint32_t size;

    inline ChunkHeader(): size(0)
    {}
};

//struct helps to store the data about chunk position with chunk description
struct ChunkHeaderDescription{
    ChunkHeader header;
    int64_t pos; // position in file of chunk data begin

    inline ChunkHeaderDescription():pos(0)
    {
    }
};

// just another name for FourCC for store WAVE
struct FileFormat{
    FourCC fourcc; //must be WAVE
};

// 'fmt ' for PCM and FLOAT, chunk description
struct WAVEFormat{
    uint16_t formatTag; // format category
    uint16_t channels; //number of channels
    uint32_t samplesPerSec; // sampling rate
    uint32_t avgBytesPerSec; // for buffer estimation
    uint16_t dataBlockAlign; //data block size
    uint16_t bitsPerSample; //number of bits per sample
    uint16_t cbSize; //size of the extension


    inline WAVEFormat():
            formatTag(0),
            channels(0),
            samplesPerSec(0),
            avgBytesPerSec(0),
            dataBlockAlign(0),
            bitsPerSample(0),
            cbSize(0)
    {}

    //allow us to print format data. It uses only for debugging, so it is not necessary to overload stream operators
    inline void print(){
        cout << "WAVEFormat:" << endl;
        cout << "\tformat tag: " << this->formatTag << endl;
        cout << "\tchannels: " << this->channels << endl;
        cout << "\tsamples per sec: " << this->samplesPerSec << endl;
        cout << "\tavg bytes per sec: " << this->avgBytesPerSec << endl;
        cout << "\tdata block align: " << this->dataBlockAlign << " bytes" << endl;
        cout << "\tbits per sample: " << this->bitsPerSample << endl;
        cout << "\tsize of the extension: " << this->cbSize << endl;
    }
};


struct WAVFileHeader{
    ChunkHeader ck_id; // RIFF
    FileFormat fmt; // WAVE
    ChunkHeader ck_fmt; // fmt
    WAVEFormat descr; // fmt content (PCM or FLOAT)

    inline WAVFileHeader()
    {}
};

// main struct for wav file and state description
struct WAVFileDescriptor{
    string fileName;
    uint32_t totalFileSize;
    WAVFileHeader header;
    uint32_t samplesPerChannelInCurrentChunk; // contains calculated value of samples for each channel in current chunk
    uint32_t sampleSize_bytes; // contains sample size in bytes (12 bit -> 2 bytes)
    bool hasPCMData; // we work only with PCM-> it should be true
    bool hasFloatFormat; //if we have deal with float data (format tag == 3) -> it is true
    FILE* fd;

    inline WAVFileDescriptor():
            fileName(""),
            totalFileSize(0),
            samplesPerChannelInCurrentChunk(0),
            sampleSize_bytes(0),
            hasPCMData(false),
            hasFloatFormat(false),
            fd(NULL)
    {}
};

class WavFileReader {
public:
    explicit WavFileReader(const string &fileName);

    // read data of size (in samples) from the file and put it to the buffer
    // why is size_samples here but not vector capacity? It should be more evident
    void getData(vector<vector<int32_t> *>* buf, uint32_t size_samples);

    inline WAVFileDescriptor getFileInfo(){return m_wavFileDescr;}

    enum WaveDataFormat{
        WAVE_FORMAT_UNKNOWN = 0,
        WAVE_FORMAT_PCM = 1, //work only with simple integer PCM data, as it is a test task
        WAVE_FORMAT_FLOAT = 3 //work with float data
    };

    enum WaveFileReaderStatus{
        WAVEFILEREADER_STATUS_OK,
        WAVEFILEREADER_STATUS_FAIL,
    };

    inline WaveFileReaderStatus status() {return m_status;}

    virtual ~WavFileReader();

private:
    // parse file header to the first data chunk (if it exists)
    void parseWAVFileHead(const string &fileName);

    // find next chunk with name "name"
    // we can read chunk neatly, because of size 8 bytes
    // returns chunk header
    // cursor is set on data of chunk
    ChunkHeaderDescription goToNextChunk(FILE **fd, bool fromCurrentPos = true, const string &name = "");

    // checks if file has WAVE data
    bool isWAVEFile();

    // gets format description for PCM data
    void getFormatDescription(ChunkHeader chunkHeader);

    // get bytes per sample (12bit sample-> 2 bytes) (uses its m_wavFileDescr)
    uint32_t getBytesPerSample();

    // simple calculator of samples per channel using local m_wavFileDescr
    uint32_t getSamplesPerChannel(uint32_t dataSize_bytes);

    //return remain file size from a current position
    uint64_t getFileTailSize_bytes();

    WAVFileDescriptor m_wavFileDescr;

    // contains data reader made by smth like factory (WaveDataReaderCreator)
    // it is differs for different data type in file (look into folder ./datareaders)
    BaseWaveDataReader* m_datareader;

    // contains the current chunk information (sometimes..)
    ChunkHeaderDescription m_currentChunkHeaderDescription;

    // contains amount of data that was read from current chunk
    uint32_t m_readDataSizeOfCurrentChunk_bytes;

    WaveFileReaderStatus m_status;
};


#endif //TEST_LAME_WAVFILEREADER_H

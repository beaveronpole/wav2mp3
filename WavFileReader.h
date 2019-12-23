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

#include "datareaders/datareaders.h"


//TODO think about PAD BYTE after chunk data. page 11, first standard

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

    ChunkHeader(): size(0)
    {}
};

struct FileFormat{
    FourCC fourcc; //must be WAVE
};

// 'fmt ' for PCM and FLOAT
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

    inline void print(){
        cout << "WAVEFormat:" << endl;
        cout << "format tag: " << this->formatTag << endl;
        cout << "channels: " << this->channels << endl;
        cout << "samples per sec: " << this->samplesPerSec << endl;
        cout << "avg bytes per sec: " << this->avgBytesPerSec << endl;
        cout << "data block align: " << this->dataBlockAlign << " bytes" << endl;
        cout << "bits per sample: " << this->bitsPerSample << endl;
        cout << "size of the extension: " << this->cbSize << endl;
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

struct WAVFileDescriptor{
    string fileName;
    uint32_t totalFileSize;
    WAVFileHeader header;
    uint32_t samplesPerChannel;
    uint32_t sampleSize_bytes;
    bool hasPCMData;
    bool hasFloatFormat;
    FILE* fd;

    inline WAVFileDescriptor():
            fileName(""),
            totalFileSize(0),
            samplesPerChannel(0),
            sampleSize_bytes(0),
            hasPCMData(false),
            hasFloatFormat(false),
            fd(NULL)
    {}
};

class WavFileReader {
public:
    explicit WavFileReader(std::string fileName);

    //read data of size from file and put to vector
    void getData(vector<vector<int32_t> *> *buf, uint32_t size);

    inline WAVFileDescriptor getFileInfo(){return m_wavFileDescr;}

    enum WaveDataFormat{
        WAVE_FORMAT_UNKNOWN = 0,
        WAVE_FORMAT_PCM = 1, //work only with simple integer PCM data, as it is a test task
        WAVE_FORMAT_FLOAT = 3 //work with float data
    };

    virtual ~WavFileReader();


private:
    //parse file header till the first data chunk if it exists
    void parseWAVFileHead(const string &fileName);

    //find next chunk with name
    //we can read chunk neatly, because of size 8 bytes
    //returns chunk header
    //cursor is set on data of chunk
    ChunkHeader goToNextChunk(FILE* fd, const string &name = "");

    // function tries to open file, checks extension WAV
    bool openWavFile(const string &fileName);

    // function returns total file size of given file descriptor
    uint32_t getFileSize(FILE* fd);

    //checks if file has WAVE data
    bool isWAVEFile();

    //gets format description for PCM data
    void getFormatDescription(ChunkHeader chunkHeader);

    //get bytes per sample (12bit sample-> 2 bytes)
    uint32_t getBytesPerSample();

    //
    uint32_t getSamplesPerChannel(uint32_t dataSize_bytes);

    bool hasFileEnoughDataForRead(size_t dataSize, FILE* fd);

    //return remain file size from the cursor position
    uint32_t getFileTailSize(FILE* fd);

    // helps to read different data types from file with checking available data size in file
    //TODO everywhere put this FUNCTION
    bool readFromFileWithCheck(FILE* fd, uint8_t* buffer, uint32_t dataSize);

    bool seekInFileWithCheck(FILE* fd, uint32_t seekSize);

    WAVFileDescriptor m_wavFileDescr;
};


#endif //TEST_LAME_WAVFILEREADER_H

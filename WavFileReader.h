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

#define WAVE_FORMAT_PCM 1 //work only with simple PCM stereo data, as it is a test task

using namespace std;

struct FourCC{
    uint8_t fourcc[4];

    inline bool check(const string& str){
        return memcmp(str.c_str(), fourcc, 4) == 0;
    }

    inline FourCC(){
        memset(this->fourcc, 0, sizeof(this->fourcc));
    }
};

struct ChunkHeader{
    FourCC fourcc; //must be RIFF
    uint32_t size;

    ChunkHeader(): size(0)
    {}
};

struct FileFormat{
    FourCC fourcc; //must be WAVE
};


// WAVE_FORMAT_PCM 'fmt '
struct WAVEFormatPCM{
    uint16_t formatTag; // format category, PCM == 1
    uint16_t channels; //number of channels
    uint32_t samplesPerSec; // sampling rate
    uint32_t avgBytesPerSec; // for buffer estimation
    uint16_t dataBlockAlign; //data block size
    uint16_t bitsPerSample; //number of bits per sample

    inline WAVEFormatPCM():
            formatTag(0),
            channels(0),
            samplesPerSec(0),
            avgBytesPerSec(0),
            dataBlockAlign(0),
            bitsPerSample(0)
    {}

    inline void print(){
        cout << "WAVEFormatPCM:" << endl;
        cout << "format tag: " << this->formatTag << endl;
        cout << "channels: " << this->channels << endl;
        cout << "samples per sec: " << this->samplesPerSec << endl;
        cout << "avg bytes per sec: " << this->avgBytesPerSec << endl;
        cout << "data block align: " << this->dataBlockAlign << " bytes" << endl;
        cout << "bits per sample: " << this->bitsPerSample << endl;
    }
};

struct WaveData{
    uint8_t chunkName[4]; // data
    uint32_t size; // data size

    inline WaveData():size(0){
        memset(this, 0, sizeof(WaveData));
    }
};

struct WAVFileHeader{
    ChunkHeader ck_id; // RIFF
    FileFormat fmt; // WAVE
    ChunkHeader ck_fmt; // fmt
    WAVEFormatPCM descr; // fmt content
    ChunkHeader ck_data; // data chunk
};

struct WAVFileDescriptor{
    string fileName;
    uint32_t totalFileSize;
    WAVFileHeader header;
    uint32_t pcmDataSize_bytes;
    uint32_t samplesPerChannel;
    uint32_t sampleSize_bytes;
    bool hasPCMData;
    bool hasData;
    FILE* fd;

    inline WAVFileDescriptor():
            fileName(""),
            totalFileSize(0),
            pcmDataSize_bytes(0),
            samplesPerChannel(0),
            sampleSize_bytes(0),
            hasPCMData(false),
            hasData(false),
            fd(NULL)
    {}
};

class WavFileReader {
public:
    WavFileReader(std::string fileName);
    uint8_t* data_buf;
    uint32_t data_size;
    inline uint8_t* getData() {return data_buf;}
    inline uint32_t getDataSize() {return data_size;}


private:
    void parseWAVFile(const string &fileName);
    void readData(WAVFileDescriptor* decr);

    // check if file has extension ".wav" or ".wave"
    bool isWAVextension(const string &fileName);

    // function tries to open file, checks extension WAV
    bool openWavFile(const string &fileName);

    // function returns total file size of given file descriptor
    uint32_t getFileSize(FILE* fd);

    // read first chunk of file and check if it is RIFF
    // fills part of wav file descriptor
    bool isRIFFFile();

    //checks if file has WAVE data
    bool isWAVEFile();

    //gets format description for PCM data
    void getFormatDescription();

    //read signal data with silence blocks
    void goToNextDataBegin();

    //get bytes per sample (12bit sample-> 2 bytes)
    uint32_t getBytesPerSample();

    //
    uint32_t getSamplesPerChannel();

    bool hasFileEnoughDataForRead(size_t dataSize);

    WAVFileDescriptor m_wavFileDescr;
    vector<int32_t> buf_pcm32_l;
    vector<int32_t> buf_pcm32_r;
};


#endif //TEST_LAME_WAVFILEREADER_H

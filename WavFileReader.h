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

#define WAVE_FORMAT_PCM 1

using namespace std;

struct FourCC{
    uint8_t fourcc[4];

    inline bool check(string str){
        if (memcmp(str.c_str(), fourcc, 4) == 0){
            return true;
        }
        return false;
    }

    inline FourCC(){
        memset(this->fourcc, 0, sizeof(this->fourcc));
    }
};

struct ChunkID{
    FourCC fourcc; //must be RIFF
    uint32_t size;

    ChunkID():size(0)
    {}

    inline bool isRIFF(){
        return fourcc.check("RIFF");
    }
};

struct FileFormat{
    FourCC fourcc; //must be WAVE

    inline bool isWAVE(){
        return fourcc.check("WAVE");
    }
};

// WAVE_FORMAT_PCM
struct WAVEFormatPCM{
//    FourCC fourcc; // 'fmt '
//    uint32_t chunkSize; // size of chunk header
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
        cout << "data block align: " << this->dataBlockAlign << endl;
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
    ChunkID ck_id;
    FileFormat fmt;
    WAVEFormatPCM descr;
};

struct WAVFileDescriptor{
    string fileName;
    uint32_t totalFileSize;
    WAVFileHeader header;
    uint32_t pcmDataSize_bytes;
    uint8_t* pcmData;

    inline WAVFileDescriptor():
            fileName(""),
            totalFileSize(0),
            pcmDataSize_bytes(0),
            pcmData(NULL)
    {}

    inline ~WAVFileDescriptor(){
        delete[] pcmData;
    }
};

struct WAVHeader
{
    // WAV-формат начинается с RIFF-заголовка:

    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    uint8_t chunkId[4];

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    uint32_t chunkSize;

    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    uint8_t format[4];

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    uint8_t subchunk1Id[4];

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    uint32_t subchunk1Size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    uint16_t audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    uint16_t numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    uint32_t sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    uint32_t byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    uint16_t blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    uint16_t bitsPerSample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    uint8_t subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    uint32_t subchunk2Size;

    // Далее следуют непосредственно Wav данные.
};


class WavFileReader {
public:
    WavFileReader(std::string fileName);
    WAVHeader m_header;
    uint8_t* data_buf;
    uint32_t data_size;
    inline uint8_t* getData() {return data_buf;}
    inline uint32_t getDataSize() {return data_size;}


private:
    WAVFileDescriptor parseWAVFile(std::string fileName);
};


#endif //TEST_LAME_WAVFILEREADER_H

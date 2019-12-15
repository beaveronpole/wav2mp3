//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"

using namespace std;

WavFileReader::WavFileReader(std::string fileName) {
    FILE* fd;
    fd = fopen(fileName.c_str(), "r");
    if (fd == NULL){
        cout << "File open error, name = " << fileName << endl;
    } else{ // file open OK
        fseek(fd, 0, SEEK_END);
        uint32_t file_size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        this->data_buf = new uint8_t[file_size - sizeof(WAVHeader)];
        WAVHeader* header = new WAVHeader;

        fread(header, 1, sizeof(WAVHeader), fd);
        cout << "header->chunkId = " << string((char*)header->chunkId, 4) << endl;
        cout << "header->chunkSize = " << header->chunkSize << endl;
        cout << "header->format = " << string((char*)header->format, 4) << endl;
        cout << "header->subchunk1Id = " << string((char*)header->subchunk1Id, 4) << endl;
        cout << "header->subchunk1Size = " << header->subchunk1Size << endl;
        cout << "header->audioFormat = " << header->audioFormat << endl;
        cout << "header->numChannels = " << header->numChannels << endl;
        cout << "header->sampleRate = " << header->sampleRate << endl;
        cout << "header->byteRate = " << header->byteRate << endl;
        cout << "header->blockAlign = " << header->blockAlign << endl;
        cout << "header->bitsPerSample = " << header->bitsPerSample << endl;
        cout << "header->subchunk2Id = " << string((char*)header->subchunk2Id, 4) << endl;
        cout << "header->subchunk2Size = " << header->subchunk2Size << endl;

        this->m_header = *header;
        this->data_size = file_size - sizeof(WAVHeader);
        fread(this->data_buf, this->data_size, sizeof(uint8_t), fd);
    }


}

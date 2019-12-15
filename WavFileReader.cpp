//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"

using namespace std;

WavFileReader::WavFileReader(std::string fileName) {
    parseWAVFile(fileName);
}

WAVFileDescriptor WavFileReader::parseWAVFile(std::string fileName) {
    //open file
    FILE* fd;
    fd = fopen(fileName.c_str(), "r");
    WAVFileDescriptor wav_descr;
    FourCC fourcc_tmp;
    uint32_t size_tmp = 0;
    if (fd == NULL) {
        cout << "File open error, name = " << fileName << endl;
        return wav_descr;
    }
    fseek(fd, 0, SEEK_END);
    wav_descr.totalFileSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    //check file size
    if (wav_descr.totalFileSize <= sizeof(WAVFileDescriptor)){
        cout << "File too small."<<endl;
        return wav_descr;
    }

    // TODO check ALL read returns

    // TODO check if file has enough length every time

    // TODO make simple function for every step

    //check is RIFF
    fread(&(wav_descr.header.ck_id), 1, sizeof(ChunkID), fd);
    if (!wav_descr.header.ck_id.isRIFF()){
        cout << "File is not RIFF." << endl;
        return wav_descr;
    }

    //check wave
    fread(&(wav_descr.header.fmt), 1, sizeof(FileFormat), fd);
    if (!wav_descr.header.fmt.isWAVE()){
        cout << "File is not WAVE." << endl;
        return wav_descr;
    }

    //check next chunk (could be different)
    fread(&(fourcc_tmp), 1, sizeof(FourCC), fd);
    fread(&(size_tmp), 1, sizeof(size_tmp), fd);
    while(!fourcc_tmp.check("fmt ")){
        fseek(fd, size_tmp, SEEK_CUR);
        fread(&(fourcc_tmp), 1, sizeof(FourCC), fd);
        fread(&(size_tmp), 1, sizeof(size_tmp), fd);
    }

    //check is PCM
    fread(&(wav_descr.header.descr), 1, sizeof(WAVEFormatPCM), fd);
    wav_descr.header.descr.print();
    if (wav_descr.header.descr.formatTag != WAVE_FORMAT_PCM){
        cout << "file contains not PCM data." << endl;
        return wav_descr;
    }


//    this->data_buf = new uint8_t[file_size - sizeof(WAVHeader)];
    //check size, read all into memory

    //check extension
    //check wave
    //skip chunks
    //fill data struct
    return wav_descr;
}

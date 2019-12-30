//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"
#include "lame/lame.h"

using namespace std;

WavFileReader::WavFileReader(std::string fileName):
        m_datareader(NULL){
    parseWAVFileHead(fileName);
    cout << "Samples per channel = " << m_wavFileDescr.samplesPerChannelInCurrentChunk << endl;
    //TODO try to read silence
    //TODO check file size and data size
    //TODO skip channels more than 2
    //TODO check if EOF
    //TODO think about stream encoding
    m_datareader = WaveDataReaderCreator::createDataReader(m_wavFileDescr.header.descr.bitsPerSample,
            m_wavFileDescr.hasFloatFormat);
    if (m_datareader == NULL){
        cerr << "Unsupported format." << endl;
        return;
    }
    m_datareader->init(m_wavFileDescr.fd, m_wavFileDescr.header.descr.channels,
                       m_wavFileDescr.header.descr.bitsPerSample, getBytesPerSample());
}

void WavFileReader::parseWAVFileHead(const string &fileName) {
    ChunkHeaderDescription chunkHeaderTmp;
    if (!openWavFile(fileName)) return;
    m_wavFileDescr.totalFileSize = getFileSize(m_wavFileDescr.fd);
    if (m_wavFileDescr.totalFileSize == 0){
        cerr << "Error. File size is zero" << endl;
        fclose(m_wavFileDescr.fd);
        return;
    }
    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, true);
    if (!chunkHeaderTmp.header.fourcc.check("RIFF")){
        cerr << "File has not RIFF data inside. " << endl;
        return;
    }
    m_wavFileDescr.header.ck_id = chunkHeaderTmp.header;
    if (!isWAVEFile()) return;
    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, true, "fmt ");
    m_currentChunkHeaderDescription = chunkHeaderTmp;
    getFormatDescription(chunkHeaderTmp.header);
    if (!m_wavFileDescr.hasPCMData) return;

    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, false, "data");
    m_currentChunkHeaderDescription = chunkHeaderTmp;
    // check if file size more than size in chunk
    if (chunkHeaderTmp.header.size == 0){ // no data
        return;
    }
    m_readDataSizeOfCurrentChunk_bytes = 0;

    // TODO check ALL read returns

    // TODO check if file has enough length every time

    // TODO make simple function for every step

    m_wavFileDescr.sampleSize_bytes = getBytesPerSample();
    //TODO get real data size in chunk
    uint32_t fileTailSize_bytes = getFileTailSize_bytes(m_wavFileDescr.fd);
    m_wavFileDescr.samplesPerChannelInCurrentChunk = getSamplesPerChannel(chunkHeaderTmp.header.size > fileTailSize_bytes ? fileTailSize_bytes : chunkHeaderTmp.header.size);
}

bool WavFileReader::openWavFile(const string &fileName) {
    FILE* fd;
    fd = fopen(fileName.c_str(), "r");
    if (fd == NULL){
        strerror(errno);
        return false;
    }
    m_wavFileDescr.fd = fd;
    return true;
}

uint32_t WavFileReader::getFileSize(FILE *fd) {
    // TODO is return 0 close file
    if (fd == NULL){
        cerr << "Error. File descriptor is not open." << endl;
        return 0;
    }
    int32_t cur_pos = ftell(fd); //save the position on enter
    if (cur_pos < 0){
        strerror(errno);
        return 0;
    }
    int32_t status = fseek(fd, 0, SEEK_END);
    if (status != 0){
        strerror(errno);
        return 0;
    }
    int32_t fileSize = ftell(fd);
    if (fileSize < 0){
        strerror(errno);
        return 0;
    }
    status = fseek(fd, cur_pos, SEEK_SET); //set back to position on enter
    if (status != 0){
        strerror(errno);
        return 0;
    }
    return fileSize;
}

bool WavFileReader::isWAVEFile() {

    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                          (uint8_t*)&(m_wavFileDescr.header.fmt),
                          sizeof(m_wavFileDescr.header.fmt))){
        return false;
    }
    return m_wavFileDescr.header.fmt.fourcc.check("WAVE");
}

void WavFileReader::getFormatDescription(ChunkHeader chunkHeader) {
    //we are at fmt chunk
    m_wavFileDescr.header.ck_fmt = chunkHeader;

    //TODO check format HERE!!!
    //read format tag
    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                               (uint8_t*)&(m_wavFileDescr.header.descr.formatTag),
                               sizeof(m_wavFileDescr.header.descr.formatTag) ))
        return;

    switch (m_wavFileDescr.header.descr.formatTag){
        case WAVE_FORMAT_FLOAT:
            m_wavFileDescr.hasFloatFormat = true;
        case WAVE_FORMAT_PCM:
            m_wavFileDescr.hasPCMData = true;
            break;
        default: //unknown format
            return;
    }

    //read channels count
    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                               (uint8_t*)&(m_wavFileDescr.header.descr.channels),
                               sizeof(m_wavFileDescr.header.descr.channels) ))
        return;

    //read samples per sec
    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                               (uint8_t*)&(m_wavFileDescr.header.descr.samplesPerSec),
                               sizeof(m_wavFileDescr.header.descr.samplesPerSec) ))
        return;

    //read avg bytes
    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                               (uint8_t*)&(m_wavFileDescr.header.descr.avgBytesPerSec),
                               sizeof(m_wavFileDescr.header.descr.avgBytesPerSec) ))
        return;

    //read data block align
    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                               (uint8_t*)&(m_wavFileDescr.header.descr.dataBlockAlign),
                               sizeof(m_wavFileDescr.header.descr.dataBlockAlign) ))
        return;

    //read bits per sample
    if (!readFromFileWithCheck(m_wavFileDescr.fd,
                               (uint8_t*)&(m_wavFileDescr.header.descr.bitsPerSample),
                               sizeof(m_wavFileDescr.header.descr.bitsPerSample) ))
        return;

    //read cb size
    if ( m_wavFileDescr.hasFloatFormat &&
         !readFromFileWithCheck(m_wavFileDescr.fd,
                                (uint8_t*)&(m_wavFileDescr.header.descr.cbSize),
                                sizeof(m_wavFileDescr.header.descr.cbSize) ))
        return;

    m_wavFileDescr.header.descr.print();
}

bool WavFileReader::hasFileEnoughDataForRead(size_t dataSize, FILE* fd) {
    int32_t filePos = ftell(fd);
    return !(filePos < 0 || (m_wavFileDescr.totalFileSize - filePos) < dataSize);
}


uint32_t WavFileReader::getBytesPerSample() {
    uint32_t sampleSize_bytes  = 8;
    if (m_wavFileDescr.header.descr.bitsPerSample <= 8){
        sampleSize_bytes = 1;
    }
    else if (m_wavFileDescr.header.descr.bitsPerSample <= 16){
        sampleSize_bytes = 2;
    }
    else if (m_wavFileDescr.header.descr.bitsPerSample <= 24){
        sampleSize_bytes = 3;
    }
    else if (m_wavFileDescr.header.descr.bitsPerSample <= 32){
        sampleSize_bytes = 4;
    }
    return sampleSize_bytes;
}

uint32_t WavFileReader::getSamplesPerChannel(uint32_t dataSize_bytes) {
    //TODO check channels is ZERO
    return dataSize_bytes / m_wavFileDescr.header.descr.channels / m_wavFileDescr.sampleSize_bytes;
}

ChunkHeaderDescription WavFileReader::goToNextChunk(FILE *fd, bool fromCurrentPos, const string &name) {
    //TODO if file error- find correct chunk byte per byte
    //    m_currentChunkStartDataPosition; //holds the position in file of current chunk
    //    ChunkHeader m_currentChunkHeader;
    ChunkHeaderDescription out;
    //put position on end of current chunk
    if (!fromCurrentPos){
        bool seekStatus = seekInFileWithCheck(fd, m_currentChunkHeaderDescription.pos + m_currentChunkHeaderDescription.header.size, SEEK_SET);
        //TODO process seek status error
    }

    do {
        if (readFromFileWithCheck(fd, (uint8_t*)&(out.header), sizeof(ChunkHeader)) ){
            out.pos = ftell(fd);
            if (out.pos == -1){
                cerr << "Error on ftell in goToNextChunk"<<endl;
                out.header.size = 0;
                return out;
            }
            if (out.header.fourcc.check(name) || name.empty()){ //checked the name of chunk (if empty- return just next chunk)
                return out;
            }
            else{ //name is not correct, seek size, and go again
                if (!seekInFileWithCheck(fd, out.header.size)){ //if error on seek- return chunk with size == 0
                    break;
                }
            }
        }
        else{
            out.header.size = 0; //we can check the size of returned chunk on error
            return out;
        }
    } while(true);
    out.header.size = 0; //we can check the size of returned chunk on error
    return out;
}

uint32_t WavFileReader::getFileTailSize_bytes(FILE *fd) {
    int32_t filePos = ftell(m_wavFileDescr.fd);
    if (filePos < 0){
        cerr << "Error on get file tail size." << endl;
        return 0;
    }
    int32_t tailSize = m_wavFileDescr.totalFileSize - filePos;
    return tailSize;
}

bool WavFileReader::readFromFileWithCheck(FILE *fd, uint8_t *buffer, uint32_t dataSize) {
    if (!hasFileEnoughDataForRead(dataSize, fd) ){
        cerr << "Error on reading data from file - is too small." << endl;
        return false;
    }
    size_t status = fread(buffer, sizeof(uint8_t), dataSize, fd);
    if (feof(fd)){
        //dont show error message on end of file
        return false;
    }
    if (status != dataSize){
        cerr << "Error on reading file (read return " << status << " expected = " << dataSize << ")" << endl;
        return false;
    }
    return true;
}

bool WavFileReader::seekInFileWithCheck(FILE *fd, uint32_t seekSize, int __whence) {
    int status = fseek(fd, seekSize, __whence);
    if (status != 0){
        cerr << "Error on seeking file. status = " << status << endl;
        return false;
    }
    return true;
}

void WavFileReader::getData(vector<vector<int32_t> *> *buf, uint32_t size_samples) {
    //TODO think about reading LIST with silence and data

    if (m_datareader == NULL){
        return;
    }

    int32_t availableInCurrentChunk_samples = m_wavFileDescr.samplesPerChannelInCurrentChunk - m_readDataSizeOfCurrentChunk_bytes/m_wavFileDescr.header.descr.dataBlockAlign;
    if (availableInCurrentChunk_samples <= 0){
        return;
    }
    //read samples from current chunk
    {
        //TODO calculate data size available for reading
        //TODO if we are in silence- read fake silence data
        int32_t countForRead_samples = std::min(availableInCurrentChunk_samples, (int32_t)size_samples);
        uint32_t size_bytes = countForRead_samples * m_wavFileDescr.header.descr.dataBlockAlign;
        uint32_t read_samples = m_datareader->getData(buf, countForRead_samples);
        if (read_samples > 0){
            m_readDataSizeOfCurrentChunk_bytes += size_bytes;
        }
    }
}

WavFileReader::~WavFileReader() {
    //TODO fill destructor
    fclose(m_wavFileDescr.fd);
    delete m_datareader;
}


//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"
#include "lame/lame.h"

using namespace std;

WavFileReader::WavFileReader(const string &fileName):
        m_datareader(NULL),
        m_status(WAVEFILEREADER_STATUS_OK),
        m_readDataSizeOfCurrentChunk_bytes(0){
    m_wavFileDescr.fileName = fileName;
    parseWAVFileHead(fileName);

    // if data in header unsupported - m_datareader will be NULL after call createDataReader
    m_datareader = WaveDataReaderCreator::createDataReader(m_wavFileDescr.header.descr.bitsPerSample,
            m_wavFileDescr.hasFloatFormat);
    if (m_datareader == NULL){
        SIMPLE_LOGGER.addErrorLine("Unsupported file format in " + fileName + "\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    m_datareader->init(m_wavFileDescr.fd, m_wavFileDescr.header.descr.channels,
                       m_wavFileDescr.header.descr.bitsPerSample, getBytesPerSample());
}

void WavFileReader::parseWAVFileHead(const string &fileName) {
    ChunkHeaderDescription chunkHeaderTmp;
    if (!openWavFile(fileName)) {
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    m_wavFileDescr.totalFileSize = getFileSize(m_wavFileDescr.fd);
    if (m_wavFileDescr.totalFileSize == 0){
        SIMPLE_LOGGER.addErrorLine("Error. File size is zero\n");
        fclose(m_wavFileDescr.fd);
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, true);
    if (!chunkHeaderTmp.header.fourcc.check("RIFF")){
        SIMPLE_LOGGER.addErrorLine("File has not RIFF data inside.\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    m_wavFileDescr.header.ck_id = chunkHeaderTmp.header;
    if (!isWAVEFile()) {
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, true, "fmt ");
    m_currentChunkHeaderDescription = chunkHeaderTmp;
    getFormatDescription(chunkHeaderTmp.header);
    if (!m_wavFileDescr.hasPCMData) {
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }

    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, true, "data");
    m_currentChunkHeaderDescription = chunkHeaderTmp;
    // check if file size more than size in chunk
    if (chunkHeaderTmp.header.size == 0){ // no data
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    m_readDataSizeOfCurrentChunk_bytes = 0;
    m_wavFileDescr.sampleSize_bytes = getBytesPerSample();
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

uint64_t WavFileReader::getFileSize(FILE *fd) {
    if (fd == NULL){
        SIMPLE_LOGGER.addErrorLine("Error. File descriptor is not open.\n");
        return 0;
    }
    int64_t cur_pos = ftell(fd); //save the position on enter
    if (cur_pos < 0){
        SIMPLE_LOGGER.addErrorLine("Error on reading file.\n");
        return 0;
    }
    int64_t status = fseek(fd, 0, SEEK_END);
    if (status != 0){
        SIMPLE_LOGGER.addErrorLine("Error on reading file.\n");
        return 0;
    }
    int64_t fileSize = ftell(fd);
    if (fileSize < 0){
        SIMPLE_LOGGER.addErrorLine("Error on reading file.\n");
        return 0;
    }
    status = fseek(fd, cur_pos, SEEK_SET); //set back to position on enter
    if (status != 0){
        SIMPLE_LOGGER.addErrorLine("Error on reading file.\n");
        return 0;
    }
    return (uint32_t)fileSize;
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

//    m_wavFileDescr.header.descr.print();
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
    if (m_wavFileDescr.header.descr.channels == 0){
        SIMPLE_LOGGER.addErrorLine("Error in channels count.\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return 0;
    }
    if (m_wavFileDescr.sampleSize_bytes == 0){
        SIMPLE_LOGGER.addErrorLine("Error in sample size.\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return 0;
    }
    // dont want to divide by zero...
    return dataSize_bytes / m_wavFileDescr.header.descr.channels / m_wavFileDescr.sampleSize_bytes;
}

ChunkHeaderDescription WavFileReader::goToNextChunk(FILE *fd, bool fromCurrentPos, const string &name) {
    ChunkHeaderDescription out;
    //put position on end of current chunk
    if (!fromCurrentPos){
        bool seekStatus = seekInFileWithCheck(fd, m_currentChunkHeaderDescription.pos + m_currentChunkHeaderDescription.header.size, SEEK_SET);
        if (!seekStatus){
            return out;
        }
    }

    do {
        if (readFromFileWithCheck(fd, (uint8_t*)&(out.header), sizeof(ChunkHeader)) ){
            out.pos = ftell(fd);
            if (out.pos == -1){
                SIMPLE_LOGGER.addErrorLine("Error on ftell in goToNextChunk.\n");
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

uint64_t WavFileReader::getFileTailSize_bytes(FILE *fd) {
    int64_t filePos = ftell(fd);
    if (filePos < 0){
        SIMPLE_LOGGER.addErrorLine("Error on get file tail size.\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return 0;
    }
    int64_t tailSize = m_wavFileDescr.totalFileSize - filePos;
    if (tailSize > 0)
        return (uint32_t)tailSize;
    return 0;
}

bool WavFileReader::readFromFileWithCheck(FILE *fd, uint8_t *buffer, uint32_t dataSize) {
    if (!hasFileEnoughDataForRead(dataSize, fd) ){
        SIMPLE_LOGGER.addErrorLine("Error on reading data from file " + m_wavFileDescr.fileName + " : too short.\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return false;
    }
    size_t status = fread(buffer, sizeof(uint8_t), dataSize, fd);
    if (feof(fd)){
        //dont show error message on end of file
        return false;
    }
    if (status != dataSize){
        m_status = WAVEFILEREADER_STATUS_FAIL;
        SIMPLE_LOGGER.addErrorLine("Error on reading file (read return " + toStr(status) + " expected: " + toStr(dataSize) + ")\n");
        return false;
    }
    return true;
}

bool WavFileReader::seekInFileWithCheck(FILE *fd, uint32_t seekSize, int __whence) {
    int status = fseek(fd, seekSize, __whence);
    if (status != 0){
        SIMPLE_LOGGER.addErrorLine("Error on seeking file. status: " + toStr(status) + "\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return false;
    }
    return true;
}

void WavFileReader::getData(vector<vector<int32_t> *> *buf, uint32_t size_samples) {
    // if we want to work with LIST chunks (silence + data)- we should do it here in this function
    if (m_datareader == NULL){
        return;
    }

    int32_t availableInCurrentChunk_samples = m_wavFileDescr.samplesPerChannelInCurrentChunk - m_readDataSizeOfCurrentChunk_bytes/m_wavFileDescr.header.descr.dataBlockAlign;
    if (availableInCurrentChunk_samples <= 0){
        return;
    }
    //read samples from current chunk
    {
        int32_t countForRead_samples = std::min(availableInCurrentChunk_samples, (int32_t)size_samples);
        uint32_t size_bytes = countForRead_samples * m_wavFileDescr.header.descr.dataBlockAlign;
        uint32_t read_samples = m_datareader->getData(buf, countForRead_samples);
        if (read_samples > 0){
            m_readDataSizeOfCurrentChunk_bytes += size_bytes;
        }
    }
}

WavFileReader::~WavFileReader() {
    if (m_wavFileDescr.fd) {
        fclose(m_wavFileDescr.fd);
    }
    delete m_datareader;
}


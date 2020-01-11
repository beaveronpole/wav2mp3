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

    //check is PCM
    if (!m_wavFileDescr.hasPCMData || m_status == WAVEFILEREADER_STATUS_FAIL){
        SIMPLE_LOGGER.addErrorLine("Unsupported file format in " + fileName + "\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    // if data in header unsupported - m_datareader will be NULL after call createDataReader
    m_datareader = WaveDataReaderCreator::createDataReader(m_wavFileDescr.header.descr.bitsPerSample,
                                                           m_wavFileDescr.hasFloatFormat);
    if (m_datareader == NULL){
        SIMPLE_LOGGER.addErrorLine("Unsupported file format in " + fileName + "\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    m_datareader->init(&m_wavFileDescr.fd, m_wavFileDescr.header.descr.channels,
                       m_wavFileDescr.header.descr.bitsPerSample, getBytesPerSample());
}

void WavFileReader::parseWAVFileHead(const string &fileName) {
    ChunkHeaderDescription chunkHeaderTmp;
    if (FileHelper::open(fileName, &m_wavFileDescr.fd, "rb") != FileHelper::FILEHELPERSTATUS_OK){
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }

    if (FileHelper::fileSize(&m_wavFileDescr.fd, &(m_wavFileDescr.totalFileSize)) != FileHelper::FILEHELPERSTATUS_OK){
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }

    chunkHeaderTmp = goToNextChunk(&m_wavFileDescr.fd, true);
    if (m_status == WAVEFILEREADER_STATUS_FAIL) return;
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
    chunkHeaderTmp = goToNextChunk(&m_wavFileDescr.fd, true, "fmt ");
    if (m_status == WAVEFILEREADER_STATUS_FAIL) return;
    m_currentChunkHeaderDescription = chunkHeaderTmp;
    getFormatDescription(chunkHeaderTmp.header);
    if (m_status == WAVEFILEREADER_STATUS_FAIL) return;
    chunkHeaderTmp = goToNextChunk(&m_wavFileDescr.fd, true, "data");
    if (m_status == WAVEFILEREADER_STATUS_FAIL) return;
    m_currentChunkHeaderDescription = chunkHeaderTmp;
    // check if file size more than size in chunk
    if (chunkHeaderTmp.header.size == 0){ // no data
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    m_readDataSizeOfCurrentChunk_bytes = 0;
    m_wavFileDescr.sampleSize_bytes = getBytesPerSample();
    uint32_t fileTailSize_bytes = getFileTailSize_bytes();
    m_wavFileDescr.samplesPerChannelInCurrentChunk = getSamplesPerChannel(chunkHeaderTmp.header.size > fileTailSize_bytes ? fileTailSize_bytes : chunkHeaderTmp.header.size);
}

bool WavFileReader::isWAVEFile() {

    if (FileHelper::read((uint8_t *) &(m_wavFileDescr.header.fmt),
                         sizeof(m_wavFileDescr.header.fmt),
                         &m_wavFileDescr.fd)!=FileHelper::FILEHELPERSTATUS_OK){
        return false;
    }
    return m_wavFileDescr.header.fmt.fourcc.check("WAVE");
}

void WavFileReader::getFormatDescription(ChunkHeader chunkHeader) {
    //we are at fmt chunk
    m_wavFileDescr.header.ck_fmt = chunkHeader;
    if (chunkHeader.size > sizeof(m_wavFileDescr.header.descr)){ //unknown format!
        SIMPLE_LOGGER.addErrorLine("Error in reading file " + toStr(m_wavFileDescr.fileName) + "\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    if (FileHelper::read((uint8_t*)&(m_wavFileDescr.header.descr),
                         chunkHeader.size,
                         &m_wavFileDescr.fd) != FileHelper::FILEHELPERSTATUS_OK) {
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return;
    }
    switch (m_wavFileDescr.header.descr.formatTag){
        case WAVE_FORMAT_FLOAT:
            m_wavFileDescr.hasFloatFormat = true; //set both here, no break needed
        case WAVE_FORMAT_PCM:
            m_wavFileDescr.hasPCMData = true;
            break;
        default: //unknown format
            return;
    }
//    m_wavFileDescr.header.descr.print();
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

ChunkHeaderDescription WavFileReader::goToNextChunk(FILE **fd, bool fromCurrentPos, const string &name) {
    ChunkHeaderDescription out;
    //put position on end of current chunk
    if (!fromCurrentPos){
        if (FileHelper::seek(fd, m_currentChunkHeaderDescription.pos +
                                 m_currentChunkHeaderDescription.header.size, SEEK_SET) != FileHelper::FILEHELPERSTATUS_OK){
            m_status = WAVEFILEREADER_STATUS_FAIL;
            return out;
        }
    }

    do {
        if (FileHelper::read((uint8_t *)&(out.header), sizeof(ChunkHeader), fd) == FileHelper::FILEHELPERSTATUS_OK){
            if (FileHelper::tell(fd, &(out.pos)) != FileHelper::FILEHELPERSTATUS_OK){
                out.header.size = 0;
                return out;
            }
            if (out.header.fourcc.check(name) || name.empty()){ //checked the name of chunk (if empty- return just next chunk)
                return out;
            }
            else{ //name is not correct, seek size, and go again
                if (FileHelper::seek(fd, out.header.size, SEEK_CUR) != FileHelper::FILEHELPERSTATUS_OK){ //if error on seek- return chunk with size == 0
                    m_status = WAVEFILEREADER_STATUS_FAIL;
                    break;
                }
            }
        }
        else{
            out.header.size = 0; //we can check the size of returned chunk on error
            m_status = WAVEFILEREADER_STATUS_FAIL;
            return out;
        }
    } while(true);
    out.header.size = 0; //we can check the size of returned chunk on error
    return out;
}

uint64_t WavFileReader::getFileTailSize_bytes() {
    int64_t filePos;
    if (FileHelper::tell(&m_wavFileDescr.fd, &filePos) != FileHelper::FILEHELPERSTATUS_OK){
        SIMPLE_LOGGER.addErrorLine("Error on get file tail size.\n");
        m_status = WAVEFILEREADER_STATUS_FAIL;
        return 0;
    }
    int64_t tailSize = m_wavFileDescr.totalFileSize - filePos;
    if (tailSize > 0)
        return (uint32_t)tailSize;
    return 0;
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
        FileHelper::close(&m_wavFileDescr.fd);
        m_wavFileDescr.fd = NULL;
    }
    delete m_datareader;
}


//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"
#include "lame/lame.h"

using namespace std;

WavFileReader::WavFileReader(std::string fileName) {
    parseWAVFileHead(fileName);
    cout << "Samples per channel = " << m_wavFileDescr.samplesPerChannel << endl;
    //TODO try to read silence
    //TODO check file size and data size
    //TODO skip channels more than 2
    //TODO check if EOF
    //TODO think about stream encoding
    BaseWaveDataReader* datareader = WaveDataReaderCreator::createDataReader(m_wavFileDescr.header.descr.bitsPerSample, m_wavFileDescr.hasFloatFormat);
    datareader->init(m_wavFileDescr.fd, m_wavFileDescr.header.descr.channels, m_wavFileDescr.header.descr.bitsPerSample);

    //TODO read in loop may be? By chunks
    vector< vector<int32_t>* >* signalData = datareader->getData();


    uint32_t mp3buffer_size_bytes = 1.25 * m_wavFileDescr.samplesPerChannel + 7200;
    uint8_t * mp3buffer = new uint8_t[mp3buffer_size_bytes];

    lame_global_flags *gfp;
    gfp = lame_init();

    lame_set_num_channels(gfp, m_wavFileDescr.header.descr.channels);
    lame_set_in_samplerate(gfp, m_wavFileDescr.header.descr.samplesPerSec);
    lame_set_brate(gfp,256);
    lame_set_mode(gfp, static_cast<MPEG_mode>(1));
    lame_set_quality(gfp,5);   /* 2=high  5 = medium  7=low */
    int ret_code = lame_init_params(gfp);

    cout << "Ret code = " << ret_code << endl;
    FILE * out = fopen("./out.mp3", "w");

    cout << "----------------" << endl;

    //TODO check put data to encoder by chunks (not WAV chunks, but random size chunks)

    uint32_t step = 8000;
    cout << "signalData[0].size() = " << signalData->at(0)->size() << endl;
    for (int i = 0 ; i < signalData->at(0)->size(); i+=step) {
        int return_encode = lame_encode_buffer_int(gfp,
                                                   signalData->at(0)->data() + i,
                                                   (signalData->size() > 1 ? signalData->at(1)->data()+i : signalData->at(
                                                           0)->data()+i),
                                                   (int) step,
                                                   mp3buffer,
                                                   (int) mp3buffer_size_bytes);
        cout << "Encode return " << return_encode << endl;
        if (return_encode)
            fwrite(mp3buffer, return_encode, 1, out);
    }

    int return_flush = lame_encode_flush(
            gfp,    /* global context handle                 */
            mp3buffer, /* pointer to encoded MP3 stream         */
            (int)mp3buffer_size_bytes);  /* number of valid octets in this stream */
    if (return_flush > 0) {
        fwrite(mp3buffer, return_flush, 1, out);
    }
    cout << "Encode flush return " << return_flush << endl;
    fclose(out);
    fclose(m_wavFileDescr.fd);


}

void WavFileReader::parseWAVFileHead(const string &fileName) {
    ChunkHeader chunkHeaderTmp;
    if (!isWAVextension(fileName)) return;
    if (!openWavFile(fileName)) return;
    m_wavFileDescr.totalFileSize = getFileSize(m_wavFileDescr.fd);
    if (m_wavFileDescr.totalFileSize == 0){
        cerr << "Error. File size is zero" << endl;
        fclose(m_wavFileDescr.fd);
        return;
    }
    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd);
    if (!chunkHeaderTmp.fourcc.check("RIFF")){
        cerr << "File has not RIFF data inside. " << endl;
        return;
    }
    if (!isWAVEFile()) return;
    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, "fmt ");
    getFormatDescription(chunkHeaderTmp);
    if (!m_wavFileDescr.hasPCMData) return;

    chunkHeaderTmp = goToNextChunk(m_wavFileDescr.fd, "data");
    // check if file size more than size in chunk
    if (chunkHeaderTmp.size == 0){ // no data
        return;
    }


    // TODO check ALL read returns

    // TODO check if file has enough length every time

    // TODO make simple function for every step

    m_wavFileDescr.sampleSize_bytes = getBytesPerSample();
    //TODO get real data size in chunk
    uint32_t fileTailSize = getFileTailSize(m_wavFileDescr.fd);

    m_wavFileDescr.samplesPerChannel = getSamplesPerChannel(chunkHeaderTmp.size > fileTailSize?fileTailSize:chunkHeaderTmp.size);
    return;
}

bool WavFileReader::isWAVextension(const string &fileName) {
    if (fileName.empty())
        return false;
    int32_t last_separator_pos = fileName.find_last_of(PATH_SEPARATOR);
    int32_t last_dot_pos = fileName.find_last_of('.');
    if (last_dot_pos - last_separator_pos <= 1){
        return false;
    }
    string extension = fileName.substr(last_dot_pos + 1);
    if (extension.empty())
        return false;
    return extension == "wav" || extension == "wave";
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
    //
//
//
//    //TODO check global status WAV
//    if (!hasFileEnoughDataForRead(sizeof(FileFormat), m_wavFileDescr.fd) ){
//        cerr << "Error on reading FORMAT FourCC, file is too small." << endl;
//        return false;
//    }
//
//    int readStatus = fread(&(m_wavFileDescr.header.fmt), sizeof(FileFormat), 1, m_wavFileDescr.fd);
//    if (readStatus < 1 || !m_wavFileDescr.header.fmt.fourcc.check("WAVE")){
//        cerr << "Error on reading file format or file format is not WAVE." << endl;
//        return false;
//    }
//    return true;
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
    else if (m_wavFileDescr.header.descr.bitsPerSample <= 32){
        sampleSize_bytes = 4;
    }
    return sampleSize_bytes;
}

uint32_t WavFileReader::getSamplesPerChannel(uint32_t dataSize_bytes) {
    //TODO check channels is ZERO
    return dataSize_bytes / m_wavFileDescr.header.descr.channels / m_wavFileDescr.sampleSize_bytes;
}

ChunkHeader WavFileReader::goToNextChunk(FILE* fd, const string &name) {
    ChunkHeader out;
    do {
        if (readFromFileWithCheck(fd, (uint8_t*)&out, sizeof(ChunkHeader)) ){
            if (out.fourcc.check(name) || name.empty()){ //checked the name of chunk (if empty- return just next chunk)
                return out;
            }
            else{ //name is not correct, seek size, and go again
                if (!seekInFileWithCheck(fd, out.size)){ //if error on seek- return chunk with size == 0
                    break;
                }
            }
        }
    } while(true);
    out.size = 0; //we can check the size of returned chunk on error
    return out;
}

uint32_t WavFileReader::getFileTailSize(FILE *fd) {
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
    if (status != dataSize){
        cerr << "Error on reading file (read return " << status << " expected = " << dataSize << ")" << endl;
        return false;
    }
    return true;
}

bool WavFileReader::seekInFileWithCheck(FILE *fd, uint32_t seekSize) {
    if (!hasFileEnoughDataForRead(seekSize, fd) ){
        cerr << "Error on seeking the file (is too small)." << endl;
        return false;
    }
    int status = fseek(fd, seekSize, SEEK_CUR);
    if (status != 0){
        cerr << "Error on seeking file. status = " << status << endl;
        return false;
    }
    return true;
}

void WavFileReader::getData(vector<vector<int32_t> *> *buf, uint32_t size) {

}


//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"
#include "lame/lame.h"

using namespace std;

WavFileReader::WavFileReader(std::string fileName) {
    parseWAVFile(fileName);
    cout << "Samples per channel = " << m_wavFileDescr.samplesPerChannel << endl;
    this->readData(&m_wavFileDescr);

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

    int return_encode = lame_encode_buffer_int(gfp,
                                               (int*)this->buf_pcm32_l.data(),
                                               (int*)this->buf_pcm32_r.data(),
                                               (int)m_wavFileDescr.samplesPerChannel,
                                               mp3buffer,
                                               (int)mp3buffer_size_bytes);
    cout << "Encode return " << return_encode << endl;
    fwrite(mp3buffer, return_encode, 1, out);

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

void WavFileReader::parseWAVFile(const string &fileName) {
    if (!isWAVextension(fileName)) return;
    if (!openWavFile(fileName)) return;
    m_wavFileDescr.totalFileSize = getFileSize(m_wavFileDescr.fd);
    if (m_wavFileDescr.totalFileSize == 0){
        fclose(m_wavFileDescr.fd);
        return;
    }
    if (!isRIFFFile()) return;
    if (!isWAVEFile()) return;
    getFormatDescription();
    if (!m_wavFileDescr.hasPCMData) return;
    goToNextDataBegin();
    if (!m_wavFileDescr.hasData) return;


    // TODO check ALL read returns

    // TODO check if file has enough length every time

    // TODO make simple function for every step

    m_wavFileDescr.sampleSize_bytes = getBytesPerSample();
    m_wavFileDescr.samplesPerChannel = getSamplesPerChannel();


    // we are at data begin
    // read all data
    // create buffer
    // fill buffer


    // check sample size
    // check channels size


//    this->data_buf = new uint8_t[file_size - sizeof(WAVHeader)];
    //check size, read all into memory

    //check extension
    //check wave
    //skip chunks
    //fill data struct
    return;
}

void WavFileReader::readData(WAVFileDescriptor* descr) {

    buf_pcm32_l.reserve(descr->samplesPerChannel);
    buf_pcm32_r.reserve(descr->samplesPerChannel);

    //TODO make for 8bit unsigned data
    //TODO try to read silence
    //TODO check file size and data size
    while(true){
        int status;
        uint32_t tmp = 0x0;
        if (descr->header.descr.channels == 2){
            if (feof(descr->fd)){
                break;
            }

            status = fread(&tmp, descr->sampleSize_bytes, 1, descr->fd);
            if (status <= 0){
                break;
            }
            //TODO make shift instead of this
//            buf_pcm32_l.push_back((INT32_MAX/INT16_MAX)*tmp);
            buf_pcm32_l.push_back(tmp<<16);

            status = fread(&tmp, descr->sampleSize_bytes, 1, descr->fd);
            if (status <= 0){
                break;
            }
//            buf_pcm32_r.push_back((INT32_MAX/INT16_MAX)*tmp);
            buf_pcm32_r.push_back(tmp<<16);
        }

    }

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

bool WavFileReader::isRIFFFile() {
    //TODO check global status WAV
    if (!hasFileEnoughDataForRead(sizeof(ChunkHeader))){
        cerr << "File is too small!" << endl;
        return false;
    }
    if (fread(&(m_wavFileDescr.header.ck_id), sizeof(ChunkHeader), 1, m_wavFileDescr.fd) != 1){
        cerr << "Error on reading main chunk ID RIFF" << endl;
        return false;
    }
    if (!m_wavFileDescr.header.ck_id.fourcc.check("RIFF")){
        cout << "File is not RIFF." << endl;
        return false;
    }
    return true;
}

bool WavFileReader::isWAVEFile() {
    //TODO check global status WAV
    if (!hasFileEnoughDataForRead(sizeof(FileFormat)) ){
        cerr << "Error on reading FORMAT FourCC, file is too small." << endl;
        return false;
    }

    int readStatus = fread(&(m_wavFileDescr.header.fmt), sizeof(FileFormat), 1, m_wavFileDescr.fd);
    if (readStatus < 1 || !m_wavFileDescr.header.fmt.fourcc.check("WAVE")){
        cerr << "Error on reading file format or file format is not WAVE." << endl;
        return false;
    }
    return true;
}

void WavFileReader::getFormatDescription() {
    ChunkHeader chunkHeaderTmp;
    do{
        if (!hasFileEnoughDataForRead(sizeof(ChunkHeader)) ){
            cerr << "Error on reading format description, file is too small." << endl;
            return;
        }
        size_t status = fread(&(chunkHeaderTmp), sizeof(ChunkHeader), 1, m_wavFileDescr.fd);
        if (status != 1){
            cerr << "Error on reading file format data" << endl;
            return;
        }
    } while(!chunkHeaderTmp.fourcc.check("fmt "));

    m_wavFileDescr.header.ck_fmt = chunkHeaderTmp;

    if (!hasFileEnoughDataForRead(sizeof(WAVEFormatPCM)) ){
        cerr << "Error on reading format description, file is too small." << endl;
        return;
    }
    size_t status = fread(&(m_wavFileDescr.header.descr), sizeof(WAVEFormatPCM), 1, m_wavFileDescr.fd);
    if (status != 1){
        cerr << "Error on reading format description." << endl;
        return;
    }
    m_wavFileDescr.header.descr.print();
    if (m_wavFileDescr.header.descr.formatTag != WAVE_FORMAT_PCM){
        cout << "File contains not PCM data." << endl;
        return;
    }
    m_wavFileDescr.hasPCMData = true;
}

bool WavFileReader::hasFileEnoughDataForRead(size_t dataSize) {
    int32_t filePos = ftell(m_wavFileDescr.fd);
    return !(filePos < 0 || (m_wavFileDescr.totalFileSize - filePos) < dataSize);
}

void WavFileReader::goToNextDataBegin() {
    ChunkHeader chunkHeaderTmp;
    do{
        if (!hasFileEnoughDataForRead(sizeof(ChunkHeader)) ){
            cerr << "Error on reading format description, file is too small." << endl;
            return;
        }
        size_t status = fread(&(chunkHeaderTmp), sizeof(ChunkHeader), 1, m_wavFileDescr.fd);
        if (status != 1){
            cerr << "Error on reading file format data" << endl;
            return;
        }
    } while(!chunkHeaderTmp.fourcc.check("data"));
    //TODO data chunk may be bigger then file size
    int32_t filePos = ftell(m_wavFileDescr.fd);
    if (filePos < 0){
        cerr << "Error on reading data chunk description" << endl;
    }
    int32_t remainData = m_wavFileDescr.totalFileSize - filePos;
    m_wavFileDescr.hasData = true;
    m_wavFileDescr.header.ck_data = chunkHeaderTmp;
    m_wavFileDescr.pcmDataSize_bytes = m_wavFileDescr.header.ck_data.size > remainData ? remainData : m_wavFileDescr.header.ck_data.size;
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

uint32_t WavFileReader::getSamplesPerChannel() {
    //TODO check channels is ZERO
    return m_wavFileDescr.pcmDataSize_bytes / m_wavFileDescr.header.descr.channels / m_wavFileDescr.sampleSize_bytes;
}

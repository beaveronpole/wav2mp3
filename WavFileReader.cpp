//
// Created by user on 11.12.2019.
//

#include "WavFileReader.h"
#include "lame/lame.h"

using namespace std;

WavFileReader::WavFileReader(std::string fileName) {
    WAVFileDescriptor descr = parseWAVFile(fileName);
    this->readData(&descr);

    uint32_t mp3buffer_size_bytes = 1.25*descr.samplesPerChannel + 7200;
    uint8_t * mp3buffer = new uint8_t[mp3buffer_size_bytes];

    lame_global_flags *gfp;
    gfp = lame_init();

    lame_set_num_channels(gfp,descr.header.descr.channels);
    lame_set_in_samplerate(gfp,descr.header.descr.samplesPerSec);
    lame_set_brate(gfp,128);
    lame_set_mode(gfp, static_cast<MPEG_mode>(1));
    lame_set_quality(gfp,5);   /* 2=high  5 = medium  7=low */
    int ret_code = lame_init_params(gfp);

    cout << "Ret code = " << ret_code << endl;

    int return_encode = lame_encode_buffer_int(gfp,
                                               (int*)this->buf_pcm32_l.data(),
                                               (int*)this->buf_pcm32_r.data(),
                                               (int)descr.samplesPerChannel,
                                               mp3buffer,
                                               (int)mp3buffer_size_bytes);
    cout << "Encode return " << return_encode << endl;

    FILE * out = fopen("./out.mp3", "w");
    fwrite(mp3buffer, return_encode, 1, out);
    fclose(out);
    fclose(descr.fd);


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

    // read data chunk header
    fread(&(fourcc_tmp), 1, sizeof(FourCC), fd);
    fread(&(size_tmp), 1, sizeof(size_tmp), fd);
    while(!fourcc_tmp.check("data")){
        fseek(fd, size_tmp, SEEK_CUR);
        fread(&(fourcc_tmp), 1, sizeof(FourCC), fd);
        fread(&(size_tmp), 1, sizeof(size_tmp), fd);
    }
    wav_descr.pcmDataSize_bytes = size_tmp;

    cout << "Data size = " << size_tmp << endl;

    uint32_t sampleSize_bytes  = 8;
    if (wav_descr.header.descr.bitsPerSample <= 8){
        sampleSize_bytes = 1;
    }
    else if (wav_descr.header.descr.bitsPerSample <= 16){
        sampleSize_bytes = 2;
    }
    else if (wav_descr.header.descr.bitsPerSample <= 32){
        sampleSize_bytes = 4;
    }
    wav_descr.sampleSize_bytes = sampleSize_bytes;
    wav_descr.samplesPerChannel = wav_descr.pcmDataSize_bytes/wav_descr.header.descr.channels/sampleSize_bytes;

    wav_descr.fd = fd;

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
    return wav_descr;
}

void WavFileReader::readData(WAVFileDescriptor* descr) {

    buf_pcm32_l.reserve(descr->samplesPerChannel);
    buf_pcm32_r.reserve(descr->samplesPerChannel);

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
            buf_pcm32_l.push_back((INT32_MAX/INT16_MAX)*tmp);

            status = fread(&tmp, descr->sampleSize_bytes, 1, descr->fd);
            if (status <= 0){
                break;
            }
            buf_pcm32_r.push_back(tmp);
        }

    }

}

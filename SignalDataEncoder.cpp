//
// Created by beaver on 23.12.2019.
//

#include "SignalDataEncoder.h"

SignalDataEncoder::SignalDataEncoder(const string &outputFileName,
                                     uint32_t channelsCount,
                                     uint32_t samplesPerSec,
                                     uint32_t encodingChunkSize) :
        m_fd(NULL)
{
    m_gfp = lame_init();
    lame_set_num_channels(m_gfp, channelsCount);
    lame_set_in_samplerate(m_gfp, samplesPerSec);
    lame_set_brate(m_gfp,256);
    lame_set_mode(m_gfp, static_cast<MPEG_mode>(1));
    lame_set_quality(m_gfp,5);   /* 2=high  5 = medium  7=low */
    int ret_code = lame_init_params(m_gfp);
    cout << "Ret code = " << ret_code << endl;

    m_mp3bufferSize_bytes = encodingChunkSize + 7200; //some tail for sure
    m_mp3buffer = new uint8_t[m_mp3bufferSize_bytes];

    m_fd = fopen(outputFileName.c_str(), "w");
    if (m_fd == NULL){
        cerr << "Error on creating output MP3 file with name = " << outputFileName << endl;
    }
}

int32_t SignalDataEncoder::putDataForEncoding(int32_t *dataLeft,
                                              int32_t *dataRight,
                                              int32_t dataSize) {
    int encodedSize = lame_encode_buffer_int(m_gfp,
                                               dataLeft,
                                               dataRight,
                                               dataSize,
                                               m_mp3buffer,
                                               m_mp3bufferSize_bytes);
    cout << "Encode return " << encodedSize << endl;
    if (encodedSize > 0) {
        size_t status = fwrite(m_mp3buffer, encodedSize, 1, m_fd);
        if (status != 1){
            cerr << "Error on writing output file" << endl;
            fclose(m_fd);
            m_fd = NULL;
            return -1;
        }
    }
    return encodedSize;
}

int32_t SignalDataEncoder::finishEncoding() {
    int returnFlush = lame_encode_flush(
            m_gfp,    /* global context handle                 */
            m_mp3buffer, /* pointer to encoded MP3 stream         */
            m_mp3bufferSize_bytes);  /* number of valid octets in this stream */
    if (returnFlush > 0) {
        size_t status = fwrite(m_mp3buffer, returnFlush, 1, m_fd);
        if (status != 1){
            cerr << "Error on writing output file" << endl;
            fclose(m_fd);
            m_fd = NULL;
            return -1;
        }
    }
    cout << "Encode flush return " << returnFlush << endl;
    fclose(m_fd);
}

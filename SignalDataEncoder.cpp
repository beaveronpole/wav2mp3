//
// Created by beaver on 23.12.2019.
//

#include "SignalDataEncoder.h"

SignalDataEncoder::SignalDataEncoder(const string &outputFileName,
                                     uint32_t channelsCount,
                                     uint32_t samplesPerSec):
                                     m_fd(NULL){
    m_gfp = lame_init();
    lame_set_num_channels(m_gfp, channelsCount);
    lame_set_in_samplerate(m_gfp, samplesPerSec);
    lame_set_brate(m_gfp,256);
    lame_set_mode(m_gfp, static_cast<MPEG_mode>(1));
    lame_set_quality(m_gfp,5);   /* 2=high  5 = medium  7=low */
    int ret_code = lame_init_params(m_gfp);
    cout << "Ret code = " << ret_code << endl;

    m_fd = fopen(outputFileName.c_str(), "w");
    if (m_fd == NULL){
        cerr << "Error on creating output MP3 file with name = " << outputFileName << endl;
    }
}

uint32_t SignalDataEncoder::putDataForEncoding(int32_t *data, uint32_t dataSize) {
    return 0;
}

void SignalDataEncoder::finishEncoding() {

}

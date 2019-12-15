#include <iostream>
#include <cstdlib>


#include "lame/lame.h"
#include "WavFileReader.h"

using namespace std;

int main() {
    std::cout << "Hello, World!" << std::endl;

    lame_global_flags *gfp;
    gfp = lame_init();

    lame_set_num_channels(gfp,2);
    lame_set_in_samplerate(gfp,44100);
    lame_set_brate(gfp,128);
    lame_set_mode(gfp, static_cast<MPEG_mode>(1));
    lame_set_quality(gfp,2);   /* 2=high  5 = medium  7=low */
    int ret_code = lame_init_params(gfp);

    if (ret_code < 0){
        cout << "Error in lame" << endl;
        exit(-1);
    }

    //int mp3buffer_size (bytes) = 1.25*num_samples + 7200;
    WavFileReader reader("./testcase.wav");

    uint32_t mp3buffer_size = 1.25*(reader.m_header.subchunk2Size/2) + 7200;

//
//
//    int lame_encode_buffer(gfp,
//                           short int leftpcm[], short int rightpcm[],
//                           int num_samples,char *mp3buffer,int  mp3buffer_size);


    return 0;
}
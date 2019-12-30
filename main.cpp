#include <iostream>
#include <cstdlib>


#include "lame/lame.h"
#include "WAVFileConverter.h"

using namespace std;

int main() {

//    WavFileReader reader("./testcase.wav");
//    WavFileReader reader("./file_example_WAV_1MG.wav");
//    WavFileReader reader("./M1F1-uint8-AFsp.wav");
    WavFileReader reader("./M1F1-float32-AFsp.wav");
    WAVFileConverter converter;
    converter.processFile("./M1F1-float32-AFsp.wav");

    return 0;
}
#include <iostream>
#include <cstdlib>


#include "lame/lame.h"
#include "WavFileReader.h"

using namespace std;

int main() {

//    WavFileReader reader("./testcase.wav");
//    WavFileReader reader("./file_example_WAV_1MG.wav");
//    WavFileReader reader("./M1F1-uint8-AFsp.wav");
    WavFileReader reader("./M1F1-float32-AFsp.wav");

    return 0;
}
#include <iostream>
#include <cstdlib>


#include "lame/lame.h"
#include "WAVFileConverter.h"

using namespace std;

int main() {
    WAVFileConverter converter;
//    converter.processFile("./M1F1-float32-AFsp.wav");
//    converter.processFile("./M1F1-float64-AFsp.wav");
    converter.processFile("./M1F1-int12-AFsp.wav");
//    converter.processFile("./M1F1-int16-AFsp.wav");
//    converter.processFile("./M1F1-int24-AFsp.wav");
//    converter.processFile("./M1F1-int32-AFsp.wav");
//    converter.processFile("./M1F1-uint8-AFsp.wav");
//    converter.processFile("./file_example_WAV_1MG.wav");
//    converter.processFile("./file_example_WAV_2MG.wav");
//    converter.processFile("./file_example_WAV_5MG.wav");
//    converter.processFile("./file_example_WAV_10MG.wav");

    converter.processFile("./sine8.wav");
    converter.processFile("./sine16.wav");
    converter.processFile("./sine24.wav");
    converter.processFile("./sine24s.wav");
    converter.processFile("./sine32.wav");
    converter.processFile("./sinefloat.wav");
    converter.processFile("./sinefloat64.wav");
    return 0;
}
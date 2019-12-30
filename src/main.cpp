#include <iostream>
#include <cstdlib>


#include "lame/lame.h"
#include "WAVFileConverter.h"

#include <ctime>

using namespace std;

int main() {
    WAVFileConverter converter(4096);
//    converter.processFile("./M1F1-int12-AFsp.wav");
//    converter.processFile("./sine8.wav");
//    converter.processFile("./sine16.wav");
//    converter.processFile("./sine24.wav");
//    for (int i = 0 ; i < 100000; i++){
//        converter.processFile("./sine24s_long.wav");
//    }
//    converter.processFile("./sine24s.wav");
//    converter.processFile("./sine32.wav");
//    converter.processFile("./sinefloat.wav");
//    converter.processFile("./sinefloat64.wav");
//    converter.processFile("./sine16_broken.wav");
//    converter.processFile("./8_Channel_ID.wav");

    time_t start, stop;
    time(&start);  /* get current time; same as: timer = time(NULL)  */
    converter.processFile("./sine24s_long.wav");
    time(&stop);
    double seconds = difftime(stop, start);
    cout << "took = " << seconds << " s" << endl;

    return 0;
}
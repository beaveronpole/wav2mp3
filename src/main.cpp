#include <iostream>

#include "FilesListMaker.h"
#include "WAVFilesConverter.h"
#include "SimpleLogger.h"

#include <ctime>

using namespace std;

int main(int argc, char** argv) {
    list<string> filesList;
    if (argc > 1) {
        filesList = FilesListMaker::makeFilesList(argv[1]);
    }
    else{
        SIMPLE_LOGGER.showError("No directory set. You should run the program with first parameter contains directory with files for encoding: wav2mp3 ./directory/directory\n");
        return 0;
    }
    SIMPLE_LOGGER.show("Files in the given folder: " + toStr(filesList.size()) + "\n");
    WAVFilesConverter* flsConv = WAVFilesConverter::instance();
    flsConv->startEncoding(&filesList);
    flsConv->wait();
    SIMPLE_LOGGER.show("Finish encoding files.\n");
    delete flsConv;
    SIMPLE_LOGGER.stop();
//    sleep(100);

//    time_t start, stop;
//    time(&start);  /* get current time; same as: timer = time(NULL)  */
//    converter.processFile("./sine24s_long.wav");
//    time(&stop);
//    double seconds = difftime(stop, start);
//    cout << "took = " << seconds << " s" << endl;
    return 0;
}
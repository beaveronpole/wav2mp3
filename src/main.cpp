#include <iostream>

#include "FilesListMaker.h"
#include "WAVFilesConverter.h"

#include <sys/time.h>

using namespace std;

int main(int argc, char** argv) {
    list<string>* filesList;
    if (argc > 1) {
        filesList = FilesListMaker::makeFilesList(argv[1]);
//        FilesListMaker::printFilesList(filesList);
    }
    else{
        SIMPLE_LOGGER.showError("No directory set. You should run the program with first parameter contains directory with files for encoding: wav2mp3 ./directory/directory\n");
        return 0;
    }
    SIMPLE_LOGGER.show("Files in the folder: " + toStr(filesList->size()) + "\n");

    timeval tp;
    uint64_t start, stop;
    gettimeofday(&tp, NULL);
    start = tp.tv_sec * 1000 + tp.tv_usec / 1000;

    WAVFilesConverter* flsConv = WAVFilesConverter::instance();
    flsConv->startEncoding(filesList);
    delete filesList;
    flsConv->wait();
    flsConv->kill();
    SIMPLE_LOGGER.show("\nFinished encoding files.\n");
    SIMPLE_LOGGER.stop();

    gettimeofday(&tp, NULL);
    stop = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    cout << "Time: " << stop - start << " msec" << endl;

    return 0;
}
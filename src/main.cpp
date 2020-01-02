#include <iostream>

#include "FilesListMaker.h"
#include "WAVFilesConverter.h"

#include <ctime>

using namespace std;

int main(int argc, char** argv) {
    list<string> filesList;
    if (argc > 1) {
        filesList = FilesListMaker::makeFilesList(argv[1]);
    }
    else{
        cerr << "No directory set. You should run the program with first parameter contains directory with files for encoding: wav2mp3 ./directory/directory" << endl;
        return 0;
    }
    cout << "Got files list size = " << filesList.size() << endl;

    WAVFilesConverter* flsConv = WAVFilesConverter::instance();
    flsConv->startEncoding(&filesList);
    flsConv->wait();
    cout << "Finish encoding files" << endl;

//    time_t start, stop;
//    time(&start);  /* get current time; same as: timer = time(NULL)  */
//    converter.processFile("./sine24s_long.wav");
//    time(&stop);
//    double seconds = difftime(stop, start);
//    cout << "took = " << seconds << " s" << endl;

    return 0;
}
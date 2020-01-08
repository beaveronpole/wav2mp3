//
// Created by beaver on 12/31/19.
//

#ifndef WAV2MP3_FILESLISTMAKER_H
#define WAV2MP3_FILESLISTMAKER_H

#include <dirent.h>
#include <list>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>

#ifdef __MINGW64__
//    #include <windows.h>
#endif

#include "SimpleLogger.h"

/*
 * Class makes list of files for a given directory
 */

using namespace std;

class FilesListMaker {
public:
    static list<string> * makeFilesList(const string& directory);
    static string getFullFileName(string dirName, string fileName);
    static void printFilesList(list<string>* files);
};


#endif //WAV2MP3_FILESLISTMAKER_H

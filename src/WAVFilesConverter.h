//
// Created by beaver on 12/31/19.
//

#ifndef WAV2MP3_WAVFILESCONVERTER_H
#define WAV2MP3_WAVFILESCONVERTER_H

#include <pthread.h>
#include <iostream>
#include <list>
#include <string>
#include <map>

#include <unistd.h>

#include "WAVFileConverter.h"
#include "ConverterWorker.h"

/*
 * Class makes several workers to encode file from given list
 */


class WAVFilesConverter {
public:
    //as in C++98 there is no closure (bind)- we use singleton in this case. In C++11> we would use bind from functional for this purposes.
    static WAVFilesConverter* instance();
    static void workerFinishCallBack(uint32_t workerUID);
    void startEncoding(list<string>* files);
    void wait();

    virtual ~WAVFilesConverter();

private:

    WAVFilesConverter();
    static WAVFilesConverter* m_instance;

    //manager

    //worker

    //workers array

    void init(uint32_t threadCount);
    map<uint32_t, ConverterWorker*> m_workers;

    void stopAllWorkers();

    pthread_mutex_t m_mutexCurrentFileNameIterator;
    list<string>::iterator m_currentFileName;
    list<string>* m_files;

    static void* waitFunction(void* args);

};


#endif //WAV2MP3_WAVFILESCONVERTER_H

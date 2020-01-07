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
 * Class makes several workers to encode file from given list.
 * Class is made as singleton for more convenient use
 *
 * Class creates an array of workers with size of optimal threads count.
 * Then gets a list of file names for encoding
 * after startEncoding we should call wait function- it allow to finish the program right after encoding finishes
 */


class WAVFilesConverter {
public:
    //as in C++98 there is no closure (bind)- we use singleton in this case. In C++11> we would use bind from functional for this purposes.
    static WAVFilesConverter* instance();

    //callback on worker finished the task. The worker can get a new task here
    static void workerFinishCallBack(uint32_t workerUID);

    //inits worker array and gives tast to workers
    void startEncoding(list<string>* files);

    //wait until all workers finished. And delete finished workers
    void wait();

    virtual ~WAVFilesConverter();

private:
    //singleton constructor
    WAVFilesConverter();

    //creates worker array
    bool init(uint32_t threadCount);

    //tells all workers to stop (not kill them, just ask)
    void stopAllWorkers();

    //returns an optimal number of threads
    static int getNumCPU();

    // sort file by size (large first) to increase total encoding speed
    list<string>* sortFilesBySize(list<string>*);

    //singleton instance of class
    static WAVFilesConverter* m_instance;

    //array of workers. Why map? Because we store there UIDs of worker, who knows how it gives us IDs.. it could be random, of smth else
    map<uint32_t, ConverterWorker*> m_workers;

    //locker for list of files
    pthread_mutex_t m_mutexCurrentFileNameIterator;

    //store a current iterator in files list
    list<string>::iterator m_currentFileName;

    //list of files. Why pointer? Why not)
    list<string>* m_files;
};


#endif //WAV2MP3_WAVFILESCONVERTER_H

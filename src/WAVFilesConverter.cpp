//
// Created by beaver on 12/31/19.
//

#include "WAVFilesConverter.h"

WAVFilesConverter* WAVFilesConverter::m_instance = NULL;

WAVFilesConverter::WAVFilesConverter() {
    //for linux
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    cout << "optimal threads count = " << numCPU << endl;

    //TODO sort files from SIZe!!!!!!! to make process more fast (first-last to 1 thread)

    /* on windows?
     * SYSTEM_INFO sysinfo;
     * GetSystemInfo(&sysinfo);
     * int numCPU = sysinfo.dwNumberOfProcessors;
     */

    init(numCPU);
}

void WAVFilesConverter::init(uint32_t threadCount) {
    ConverterWorker* newWorker = NULL;
    pthread_mutex_init(&m_mutexCurrentFileNameIterator, NULL);
//    threadCount = 1;
    for (uint32_t i = 0; i < threadCount; i++){
        newWorker = new ConverterWorker(workerFinishCallBack);
        m_workers[newWorker->uid()] = newWorker;
    }
}

void WAVFilesConverter::startEncoding(list<string>* files) {
    m_files = files;
    if (files->size() == 0){
        cerr << "No files in list"<<endl;
        return;
    }
    m_currentFileName = files->begin();

    for (map<uint32_t, ConverterWorker*>::iterator itr = m_workers.begin();
         itr != m_workers.end();
         itr++){
        pthread_mutex_lock( &m_mutexCurrentFileNameIterator);
        itr->second->start(*m_currentFileName);
        if (m_currentFileName != m_files->end()) {
            m_currentFileName++;
        }
        else{
            cout << "Break files" << endl;
            break;
        }
        pthread_mutex_unlock( &m_mutexCurrentFileNameIterator);

    }
    //iterate on files for workers count
    //give workers a file

    //when all workers work- return from function (or wait while all files will be processed)

    //stop workers on finish
//    sleep(100);
    cout << "FINISH startEncoding" << endl;
}


WAVFilesConverter::~WAVFilesConverter() {
    //TODO stop workers
    //TODO clean m_workers
}

//CALLS FROM ANOTHER THREAD!!!
void WAVFilesConverter::workerFinishCallBack(uint32_t workerUID) {
    WAVFilesConverter* object = WAVFilesConverter::instance();
    //TODO protect for multy threads
    cout << "Worker UId = " << workerUID << " finished!" << endl;
    //TODO set new task to the worker
    pthread_mutex_lock( &(object->m_mutexCurrentFileNameIterator));
    if (!(object->m_currentFileName == object->m_files->end()) ){
        object->m_workers[workerUID]->start(*object->m_currentFileName);
        object->m_currentFileName++;
    }
    else {
        object->stopAllWorkers(); //stops only threads those are waiting (not in progress!)
    }
    pthread_mutex_unlock( &(object->m_mutexCurrentFileNameIterator));

}


WAVFilesConverter *WAVFilesConverter::instance() {
    if (m_instance == NULL){
        m_instance = new WAVFilesConverter;
    }
    return m_instance;
}

void WAVFilesConverter::stopAllWorkers() {
    for (map<uint32_t, ConverterWorker*>::iterator itr = m_workers.begin();
         itr != m_workers.end();
         itr++){
        itr->second->stop();
    }

}

void *WAVFilesConverter::waitFunction(void *args) {
    cout << "start wait function!" << endl;
    WAVFilesConverter* object = (WAVFilesConverter*)args;
    for (map<uint32_t, ConverterWorker*>::iterator itr = object->m_workers.begin();
         itr != object->m_workers.end();
         itr++){
        pthread_join(*itr->second->getThread(), NULL);
    }
    return NULL;
}

void WAVFilesConverter::wait() {
//    pthread_create(NULL, NULL, WAVFilesConverter::waitFunction, this);
    cout << "start wait function!" << endl;
    for (map<uint32_t, ConverterWorker*>::iterator itr = m_workers.begin();
         itr != m_workers.end();
         itr++){
        pthread_join(*itr->second->getThread(), NULL);
    }
//    return NULL;
}


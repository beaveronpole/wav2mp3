//
// Created by beaver on 12/31/19.
//

#include "WAVFilesConverter.h"

WAVFilesConverter* WAVFilesConverter::m_instance = NULL;

WAVFilesConverter::WAVFilesConverter() {
    //TODO sort files from SIZe!!!!!!! to make process more fast (first-last to 1 thread)
}

void WAVFilesConverter::init(uint32_t threadCount) {
    ConverterWorker* newWorker = NULL;
    pthread_mutex_init(&m_mutexCurrentFileNameIterator, NULL);
    for (uint32_t i = 0; i < threadCount; i++){
        newWorker = new ConverterWorker(workerFinishCallBack);
        m_workers[newWorker->uid()] = newWorker;
    }
}

void WAVFilesConverter::startEncoding(list<string>* files) {
    init(getNumCPU()); //init here, for restart encoding. If Init in constructor- we have to reinit manually
    m_files = files;
    if (files->empty()){
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
            pthread_mutex_unlock( &m_mutexCurrentFileNameIterator);
            break;
        }
        pthread_mutex_unlock( &m_mutexCurrentFileNameIterator);
    }
    //return from function when all workers got 1 file, or files count < cores/threads/workers
}


//CALLS FROM ANOTHER THREAD!!!
void WAVFilesConverter::workerFinishCallBack(uint32_t workerUID) {
    WAVFilesConverter* object = WAVFilesConverter::instance();
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
        if (itr->second)
            itr->second->stop();
    }
}

void WAVFilesConverter::wait() {
    cout << "start wait function!" << endl;
    for (map<uint32_t, ConverterWorker*>::iterator itr = m_workers.begin();
         itr != m_workers.end();
         itr++){
        pthread_join(*itr->second->getThread(), NULL);
        delete itr->second;
        itr->second = NULL;
    }
    m_workers.clear();
}

int WAVFilesConverter::getNumCPU() {
    //for linux
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    cout << "optimal threads count = " << numCPU << endl;


    /* on windows?
     * SYSTEM_INFO sysinfo;
     * GetSystemInfo(&sysinfo);
     * int numCPU = sysinfo.dwNumberOfProcessors;
     */
    return numCPU;
}

WAVFilesConverter::~WAVFilesConverter() {
    //TODO stop workers
    stopAllWorkers();
    //TODO clean m_workers
    wait();
}


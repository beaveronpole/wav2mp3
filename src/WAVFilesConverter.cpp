//
// Created by beaver on 12/31/19.
//

#include "WAVFilesConverter.h"

WAVFilesConverter* WAVFilesConverter::m_instance = NULL;

WAVFilesConverter::WAVFilesConverter():
m_files(NULL){
    //we can sort files by size here to optimize encoding sequence -> first goes largest files
}

bool WAVFilesConverter::init(uint32_t threadCount) {
    ConverterWorker* newWorker = NULL;
    int status_mutex = pthread_mutex_init(&m_mutexCurrentFileNameIterator, NULL);
    if (status_mutex != 0){
        SIMPLE_LOGGER.showError("error on init mutex CurrentFileNameIterator in WAVFilesConverter.\n");
        return false;
    }
    for (uint32_t i = 0; i < threadCount; i++){
        newWorker = new ConverterWorker(workerFinishCallBack);
        if (!newWorker->canWork()){
            return false;
        }
        m_workers[newWorker->uid()] = newWorker;
    }
    return true;
}

void WAVFilesConverter::startEncoding(list<string>* files) {
    m_files = sortFilesBySize(files);
    //create worker count as minimum(files count (any in list, we know nothing about them here) <-> optimal thread count)
    int maxThreads = min((uint32_t)m_files->size(), getNumCPU());
    if (!init(maxThreads)) //init here, for restart encoding. If Init in constructor- we have to reinit manually
    {
        SIMPLE_LOGGER.showError("error in converter initialization.\n");
        return;
    }
    if (m_files->empty()){
        SIMPLE_LOGGER.showError("No files in list.\n");
        stopAllWorkers();
        return;
    }
    m_currentFileName = m_files->begin();

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
    SIMPLE_LOGGER.flush();
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
    for (map<uint32_t, ConverterWorker*>::iterator itr = m_workers.begin();
         itr != m_workers.end();
         itr++){
        itr->second->wait();
        delete itr->second; // delete finished worker (it will join thread in worker destructor)
        itr->second = NULL;
    }
    m_workers.clear();
}

uint32_t WAVFilesConverter::getNumCPU() {
    //for linux
    long int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    if (numCPU <= 0) {
        SIMPLE_LOGGER.show("Unable to get optimal thread count. Work with 1 thread.\n");
        return 1;
    }else{
        SIMPLE_LOGGER.show("Optimal threads count: " + toStr(numCPU) + "\n");
    }
    return (uint32_t)numCPU;
}

list<string>* WAVFilesConverter::sortFilesBySize(list<string> *files) {
    FILE* fd = NULL;
    uint32_t size;
    multimap<uint32_t, string, greater<uint32_t> > sizeAndNames;
    //this function doesnt think about files extension
    for (list<string>::iterator itr = files->begin(); itr != files->end(); itr++){
        fd = fopen( (*itr).c_str(), "r" );
        if (fd == NULL){
            continue;
        }
        int status = fseek(fd, 0, SEEK_END);
        if (status!=0){
            fclose(fd);
            continue;
        }
        size = ftell(fd);
        sizeAndNames.insert(make_pair(size, (*itr)));
        fclose(fd);
    }

    // dummy sorting by file size with multimap :)
    list<string>* out = new list<string>;
    for (multimap<uint32_t, string>::iterator itr = sizeAndNames.begin(); itr != sizeAndNames.end(); itr++){
        out->push_back(itr->second);
    }

    return out;
}

WAVFilesConverter::~WAVFilesConverter() {
    stopAllWorkers();
    wait();
    //all workers finished here -> no worker can call workerFinishCallBack -> mutex is in unlocked state
    pthread_mutex_destroy(&m_mutexCurrentFileNameIterator);
    m_instance = NULL;
    delete m_files;
}



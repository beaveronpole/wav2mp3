//
// Created by beaver on 1/2/20.
//

#include "ConverterWorker.h"

uint32_t ConverterWorker::ConverterWorkerUID = 0;

ConverterWorker::ConverterWorker(void (*finishCallBack)(uint32_t)):
        m_canWork(false){
    m_finishCallBack = finishCallBack;
    m_uid = ConverterWorkerUID++;
    m_converter = new WAVFileConverter(4096);
    m_canWork = initPthread();
}

bool ConverterWorker::initPthread() {
    m_threadActive = true;

    int status_mutex = pthread_mutex_init(&m_startConvertingMutex, NULL);
    if (status_mutex != 0){
        SIMPLE_LOGGER.showError("error on init thread ConverterWorker (mutex).\n");
        return false;
    }
    int status_cond = pthread_cond_init(&m_startConvertingCondition, NULL);
    if (status_cond != 0){
        SIMPLE_LOGGER.showError("error on init thread ConverterWorker (condition).\n");
        return false;
    }

    int err = pthread_create(&m_workerThread, NULL,
                             &ConverterWorker::workerFunction, this);
    if (err != 0){
        SIMPLE_LOGGER.showError("error on creation worker thread.\n");
        return false;
    }
    return true;
}

void ConverterWorker::start(const string &fileName) {
    //send signal to start the function
    m_processFileName = fileName;
    pthread_cond_signal(&m_startConvertingCondition);
}

void* ConverterWorker::workerFunction(void* args) {
    ConverterWorker* object = (ConverterWorker*)args;

    //LOOP
    while(object->m_threadActive){
        //wait for a new file
        pthread_mutex_lock(&(object->m_startConvertingMutex));
        while (object->m_processFileName.empty() && object->m_threadActive) {
            pthread_cond_wait(&(object->m_startConvertingCondition), &(object->m_startConvertingMutex));
        }
        if (!object->m_threadActive){
            pthread_mutex_unlock(&(object->m_startConvertingMutex));
            break;
        }
        //process
        string currentFileName = object->m_processFileName;
        object->m_converter->processFile(currentFileName);
        object->m_processFileName = "";
        object->m_finishCallBack(object->m_uid);
        pthread_mutex_unlock(&(object->m_startConvertingMutex));
    }
    // if we want to know when worker finished its life
//    SIMPLE_LOGGER.show("*** Worker retired UID: " + toStr(object->m_uid) + "\n");
    return NULL;
}

void ConverterWorker::stop() {
    m_threadActive = false;
    m_processFileName = "";
    pthread_cond_signal(&m_startConvertingCondition);
}

ConverterWorker::~ConverterWorker() {
    //TODO check returns
    //TODO check is lock
    pthread_mutex_destroy(&m_startConvertingMutex);
    pthread_cond_destroy(&m_startConvertingCondition);
    delete m_converter;
}

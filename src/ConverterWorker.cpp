//
// Created by beaver on 1/2/20.
//

#include "ConverterWorker.h"

uint32_t ConverterWorker::ConverterWorkerUID = 0;

ConverterWorker::ConverterWorker(void (*finishCallBack)(uint32_t)) {
    m_finishCallBack = finishCallBack;
    m_uid = ConverterWorkerUID++;
    m_converter = new WAVFileConverter(4096);
    initPthread();
}

void ConverterWorker::initPthread() {
    m_threadActive = true;

    pthread_mutex_init(&m_startConvertingMutex, NULL);
    pthread_cond_init(&m_startConvertingCondition, NULL);

    int err = pthread_create(&m_workerThread, NULL,
                             &ConverterWorker::workerFunction, this);

}

void ConverterWorker::start(const string &fileName) {
    cout << "Start file encoding " << fileName << " uid = " << this->m_uid << endl;
    //send signal to start the function
    m_processFileName = fileName;
    pthread_cond_signal(&m_startConvertingCondition);
}

void* ConverterWorker::workerFunction(void* args) {
    ConverterWorker* object = (ConverterWorker*)args;

    //LOOP
    while(object->m_threadActive){
        //wait for a new file
        while (object->m_processFileName.empty() && object->m_threadActive) {
            pthread_cond_wait(&(object->m_startConvertingCondition), &(object->m_startConvertingMutex));
        }
        if (!object->m_threadActive){
            break;
        }
        //process
        string currentFileName = object->m_processFileName;
        object->m_converter->processFile(currentFileName);
        cout << "Worker " << object->m_uid << " finish with file name = " << currentFileName << endl;
        object->m_processFileName = "";
        object->m_finishCallBack(object->m_uid);
    }
    cout << "********* Totally Finish worker UID = " << object->m_uid << endl;
    return NULL;
}

void ConverterWorker::stop() {
    m_threadActive = false;
    m_processFileName = "";
    pthread_cond_signal(&m_startConvertingCondition);
}

ConverterWorker::~ConverterWorker() {
    //TODO check returns
    pthread_mutex_destroy(&m_startConvertingMutex);
    pthread_cond_destroy(&m_startConvertingCondition);
    delete m_converter;
}

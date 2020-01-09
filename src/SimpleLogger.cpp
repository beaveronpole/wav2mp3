//
// Created by beaver on 1/4/20.
//

#include "SimpleLogger.h"

SimpleLogger* SimpleLogger::m_instance = NULL;

SimpleLogger::SimpleLogger():
        available(false),
        m_enable(true){
    int status_mutex = pthread_mutex_init(&m_mapsMutex, NULL);
    if (status_mutex != 0){
        cerr << "error on init mutex SimpleLogger." << endl;
        return;
    }else{
        available = true;
    }
    pthread_create(&m_progressThread, NULL, SimpleLogger::loggerLoop, this);
}

SimpleLogger *SimpleLogger::instance() {
    if (m_instance == NULL){
        m_instance = new SimpleLogger;
    }
    if (m_instance->available)
        return m_instance;
    return NULL;
}

void SimpleLogger::show(const string &str) {
    pthread_mutex_lock(&m_mapsMutex);
    cout << str;
    fflush(stdout);
    pthread_mutex_unlock(&m_mapsMutex);
}

void SimpleLogger::showError(const string &str, int error) {
    pthread_mutex_lock(&m_mapsMutex);
    cerr << str;
    if (error){
        cerr << strerror(error) << endl;
    }
    fflush(stderr);
    pthread_mutex_unlock(&m_mapsMutex);
}

SimpleLogger* SimpleLogger::addLine(const string &str) {
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&m_mapsMutex);
    map<pthread_t, string>::iterator itr = m_threadStrings.find(tid);
    if (itr == m_threadStrings.end()){
        m_threadStrings[tid] = str;
    }
    else{
        (*itr).second += str;
    }
    pthread_mutex_unlock(&m_mapsMutex);
    return this;
}

SimpleLogger* SimpleLogger::addErrorLine(const string &str, int error) {
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&m_mapsMutex);
    map<pthread_t, string>::iterator itr = m_threadErrorStrings.find(tid);
    if (itr == m_threadErrorStrings.end()){
        m_threadErrorStrings[tid] = "\t"+str;
        if (error) m_threadErrorStrings[tid] += strerror(error);
    }
    else{
        (*itr).second += "\t"+str;
        if (error) (*itr).second += strerror(error);
    }
    pthread_mutex_unlock(&m_mapsMutex);
    return this;
}

void SimpleLogger::flush() {
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&m_mapsMutex);
    fflush(stdout);
    fflush(stderr);

    map<pthread_t, string>::iterator itr = m_threadStrings.find(tid);
    if (itr != m_threadStrings.end()){
        cout << "----------------------------" << endl;
        cout << (*itr).second;
        fflush(stdout);
        m_threadStrings.erase(itr);
    }

    map<pthread_t, string>::iterator errItr = m_threadErrorStrings.find(tid);
    if (errItr != m_threadErrorStrings.end()){
        if (itr == m_threadStrings.end()){
            cout << "----------------------------" << endl;
        }
        cerr << (*errItr).second;
        fflush(stderr);
        m_threadErrorStrings.erase(errItr);
    }
    pthread_mutex_unlock(&m_mapsMutex);
}

void *SimpleLogger::loggerLoop(void *args) {
    sleep(1);
    SimpleLogger* object = (SimpleLogger*)args;
    uint32_t counter = 0;
    char chars[] = {'-', '\\','|','/'};
    while(object->m_enable){
        pthread_mutex_lock(&object->m_mapsMutex);
        printf("in progress... %c\r", chars[counter % sizeof(chars)]);
        fflush(stdout);
        pthread_mutex_unlock(&object->m_mapsMutex);
        usleep(300000);
        counter+=1;
    }
    return NULL;
}

void SimpleLogger::wait() {
    pthread_join(m_progressThread, NULL);
}



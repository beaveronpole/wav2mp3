//
// Created by beaver on 1/4/20.
//

#ifndef WAV2MP3_SIMPLELOGGER_H
#define WAV2MP3_SIMPLELOGGER_H

#include <pthread.h>
#include <string>
#include <map>
#include <inttypes.h>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include "unistd.h"

using namespace std;

#define SIMPLE_LOGGER (*SimpleLogger::instance())

// function for make strings from different types like integers, doubles
template <class T>
string toStr(T value){
    stringstream stream;
    stream << value;
    return stream.str();
}

/*
 * Class allows us to log information from different threads in one place unmixed.
 * It will show put information of the thread on FLUSH function
 * It understands which thread put information to it via thread ID an stores it in map<TID, String>
 * Pretty simple-> increase String on every addLine, and show the string on flush.
 *
 * made as singleton
 */
class SimpleLogger {
public:
    static SimpleLogger* instance();

    // show string immediately
    void show(const string &);

    // show string immediately to error stream
    void showError(const string &);

    // add line of thread
    SimpleLogger* addLine(const string &);

    // add line of thread to error stream
    SimpleLogger* addErrorLine(const string &);

    // flush thread data
    void flush();

    //stop logger
    inline void stop() {m_enable = false; wait();}

    //wait for thread that shows the progress (very simple, if we not finished- we are in progress)
    void wait();

private:
    SimpleLogger();
    static SimpleLogger* m_instance;
    bool available;

    static void* loggerLoop(void* args);

    pthread_mutex_t m_mapsMutex;
    map<pthread_t, string> m_threadStrings;
    map<pthread_t, string> m_threadErrorStrings;
    bool m_enable;
    pthread_t m_progressThread;
};


#endif //WAV2MP3_SIMPLELOGGER_H

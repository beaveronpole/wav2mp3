//
// Created by beaver on 1/2/20.
//

#ifndef WAV2MP3_CONVERTERWORKER_H
#define WAV2MP3_CONVERTERWORKER_H

#include <string>
#include <pthread.h>
#include <inttypes.h>

#include "WAVFileConverter.h"

using namespace std;

class ConverterWorker {
public:
    explicit ConverterWorker( void (*finishCallBack)(uint32_t) );

    void start(const string &fileName);

    //stop the loop of thread and thread
    void stop();

    inline pthread_t* getThread(){return &m_workerThread;}

    inline uint32_t uid() {return m_uid;}

    virtual ~ConverterWorker();

private:
    //storage a UID of worker
    static uint32_t ConverterWorkerUID;
    uint32_t m_uid;

    //init phtread for worker
    void initPthread();

    //main function for converting
    static void* workerFunction(void* args);

    //calls it on finish with a file
    void (*m_finishCallBack)(uint32_t uid);

    pthread_t m_workerThread;
    pthread_cond_t m_startConvertingCondition;
    pthread_mutex_t m_startConvertingMutex;
    bool m_threadActive;
    string m_processFileName;

    WAVFileConverter* m_converter;
};


#endif //WAV2MP3_CONVERTERWORKER_H

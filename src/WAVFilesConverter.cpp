//
// Created by beaver on 12/31/19.
//

#include "WAVFilesConverter.h"

WAVFilesConverter::WAVFilesConverter() {
    //for linux
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    cout << "optimal threads count = " << numCPU << endl;

    /* on windows?
     * SYSTEM_INFO sysinfo;
     * GetSystemInfo(&sysinfo);
     * int numCPU = sysinfo.dwNumberOfProcessors;
     */
}

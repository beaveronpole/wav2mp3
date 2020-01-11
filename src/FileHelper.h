//
// Created by beaver on 09.01.2020.
//

#ifndef WAV2MP3_FILEHELPER_H
#define WAV2MP3_FILEHELPER_H

#include <cstdio>
#include <iostream>
#include <cerrno>
#include <string>
#include <inttypes.h>

#include "SimpleLogger.h"

using namespace std;

/*
 * Class helps to work with files by wrappers
 */

class FileHelper {
public:
    enum FileHelperStatus {
        FILEHELPERSTATUS_OK = 0,
        FILEHELPERSTATUS_PARTIAL = 1,
        FILEHELPERSTATUS_EOF = 2,
        FILEHELPERSTATUS_FAIL = 3
    };
    static FileHelperStatus open(const string &path, FILE **fd, const char *mode = "br");
    static FileHelperStatus close(FILE **fd);
    static FileHelperStatus read(uint8_t *buf, size_t wantToReadSize_bytes, FILE **fd, uint32_t *fact = NULL);
    static FileHelperStatus write(uint8_t *buf, size_t wantToWriteSize_bytes, FILE **fd);
    static FileHelperStatus seek(FILE **fd, uint32_t offset, int from);
    static FileHelperStatus tell(FILE **fd, int64_t *outPos);
    static FileHelperStatus fileSize(FILE **fd, uint32_t* result);
};


#endif //WAV2MP3_FILEHELPER_H

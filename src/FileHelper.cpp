//
// Created by beaver on 09.01.2020.
//

#include "FileHelper.h"

FileHelper::FileHelperStatus FileHelper::open(const string &path, FILE **fd, const char *mode) {
    *fd = fopen(path.c_str(), mode);
    if (*fd == NULL){
        SIMPLE_LOGGER.addErrorLine("Error file open " + path, errno);
        return FILEHELPERSTATUS_FAIL;
    }
    return FILEHELPERSTATUS_OK;
}

FileHelper::FileHelperStatus FileHelper::close(FILE **fd) {
    if (fclose(*fd) == 0){
        return FILEHELPERSTATUS_OK;
    }
    return FILEHELPERSTATUS_FAIL;
}

FileHelper::FileHelperStatus FileHelper::read(uint8_t *buf, size_t wantToReadSize_bytes, FILE **fd, uint32_t *fact) {
    size_t readSize = 0;
    size_t totalReadSize = 0;
    while (totalReadSize < wantToReadSize_bytes) {
        readSize = fread(buf + totalReadSize, sizeof(uint8_t), wantToReadSize_bytes - totalReadSize, *fd);
        if (readSize != wantToReadSize_bytes - totalReadSize) {
            if (ferror(*fd) != 0){ //error on reading
                SIMPLE_LOGGER.addErrorLine("Error on file read. ", errno);
                return FILEHELPERSTATUS_FAIL;
            }
            else if (feof(*fd)){ //eof reached
                if (fact)
                    *fact = totalReadSize;
                return FILEHELPERSTATUS_EOF;
            }
            else{ //partial read
                totalReadSize += readSize;
            }
        }
        else if (readSize == wantToReadSize_bytes - totalReadSize){ //read the size we wanted to read
            break;
        }
    }
    return FILEHELPERSTATUS_OK;
}

FileHelper::FileHelperStatus FileHelper::write(uint8_t *buf, size_t wantToWriteSize_bytes, FILE **fd) {
    size_t wroteSize = 0;
    size_t totalWroteSize = 0;
    while (totalWroteSize < wantToWriteSize_bytes) {
        wroteSize = fwrite(buf + totalWroteSize, sizeof(uint8_t), wantToWriteSize_bytes - totalWroteSize, *fd);
        if (wroteSize != wantToWriteSize_bytes - totalWroteSize) {
            if (ferror(*fd) != 0){ //error on writing
                SIMPLE_LOGGER.addErrorLine("Error on file write. ", errno);
                return FILEHELPERSTATUS_FAIL;
            }
            else{ //partial write
                totalWroteSize += wroteSize;
            }
        }
        else{
            break;
        }
    }
    return FILEHELPERSTATUS_OK;
}

FileHelper::FileHelperStatus FileHelper::seek(FILE **fd, uint32_t offset, int from) {
    int status = fseek(*fd, offset, from);
    if (status == -1){
        SIMPLE_LOGGER.addErrorLine("Error on seeking file. ", errno);
        return FILEHELPERSTATUS_FAIL;
    }
    return FILEHELPERSTATUS_OK;
}

FileHelper::FileHelperStatus FileHelper::tell(FILE **fd, int64_t *outPos) {
    int64_t status = ftell(*fd);
    if (status < 0){
        SIMPLE_LOGGER.addErrorLine("Error on find position in file. ", errno);
        return FILEHELPERSTATUS_FAIL;
    }
    if (outPos != NULL) {
        *outPos = status;
        return FILEHELPERSTATUS_OK;
    }
    SIMPLE_LOGGER.addErrorLine("Error on find position in file- no place to put result.");
    return FILEHELPERSTATUS_FAIL;
}

FileHelper::FileHelperStatus FileHelper::fileSize(FILE **fd, uint32_t *result) {
    if (*fd == NULL){
        SIMPLE_LOGGER.addErrorLine("Error. File descriptor is not open.\n");
        return FILEHELPERSTATUS_FAIL;
    }
    int64_t position;
    if (tell(fd, &position) != FILEHELPERSTATUS_OK) return FILEHELPERSTATUS_FAIL;
    int64_t cur_pos = position;
    if (seek(fd, 0, SEEK_END) != FILEHELPERSTATUS_OK) return FILEHELPERSTATUS_FAIL;
    if (tell(fd, &position) != FILEHELPERSTATUS_OK) return FILEHELPERSTATUS_FAIL;
    int64_t fileSize = position;
    if (seek(fd, cur_pos, SEEK_SET) != FILEHELPERSTATUS_OK) return FILEHELPERSTATUS_FAIL;
    *result = fileSize;
    return FILEHELPERSTATUS_OK;
}

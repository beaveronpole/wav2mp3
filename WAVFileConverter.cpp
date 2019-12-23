//
// Created by beaver on 23.12.2019.
//

#include "WAVFileConverter.h"

WAVFileConverter::WAVFileConverter():
        m_encodingChunkSize_samples(1024){
//TODO set sample chunk size
}

bool WAVFileConverter::checkExtension(const string &fileName) {
    if (fileName.empty())
        return false;
    int32_t last_separator_pos = fileName.find_last_of(PATH_SEPARATOR);
    int32_t last_dot_pos = fileName.find_last_of('.');
    if (last_dot_pos - last_separator_pos <= 1){
        return false;
    }
    string extension = fileName.substr(last_dot_pos + 1);
    if (extension.empty())
        return false;
    return extension == "wav" || extension == "wave";
}

string WAVFileConverter::makeMP3FileName(const string &fileName) {
    if (fileName.empty())
        return "";
    int32_t last_separator_pos = fileName.find_last_of(PATH_SEPARATOR);
    int32_t last_dot_pos = fileName.find_last_of('.');
    if (last_dot_pos - last_separator_pos <= 1){
        return "";
    }
    string extension = fileName.substr(last_dot_pos + 1);
    string fileOutName = fileName.substr(0, last_dot_pos);
    return fileOutName+".mp3";
}

void WAVFileConverter::processFile(const string &fileName) {
    //check extension
    if (!checkExtension(fileName)){
        return;
    }

    //get file name-> make MP3 file name
    string outMP3FileName = makeMP3FileName(fileName);

    //create file reader
    m_reader = new WavFileReader(fileName);
    WAVFileDescriptor WAVFileInfo = m_reader->getFileInfo();

    //create file encoder
    m_encoder = new SignalDataEncoder(outMP3FileName,
            WAVFileInfo.header.descr.channels,
            WAVFileInfo.header.descr.samplesPerSec,
            m_encodingChunkSize_samples);

    //loop for reading and encoding

}


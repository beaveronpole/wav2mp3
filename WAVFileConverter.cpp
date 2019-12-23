//
// Created by beaver on 23.12.2019.
//

#include "WAVFileConverter.h"

WAVFileConverter::WAVFileConverter() {}

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

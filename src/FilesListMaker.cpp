//
// Created by beaver on 12/31/19.
//

#include "FilesListMaker.h"

list<string> * FilesListMaker::makeFilesList(const string& directory) {
    DIR *dir;
    list<string>* outList = new list<string>;
    struct dirent *ent;
    if ((dir = opendir (directory.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(".", ent->d_name)==0 || strcmp("..", ent->d_name)==0){
                continue;
            }
            char resolved[PATH_MAX];
            char* status = realpath(directory.c_str(), resolved);
            if (status == NULL){
                SIMPLE_LOGGER.showError("Some error in reading file path for " + toStr(directory.c_str()) +" Continue...\n");
                continue;
            }
            string gotFilePath(string(resolved) + PATH_SEPARATOR + string(ent->d_name));
            outList->push_back(gotFilePath);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        SIMPLE_LOGGER.showError("Error in opening directory : " + directory + "\n");
    }
    return outList;
}

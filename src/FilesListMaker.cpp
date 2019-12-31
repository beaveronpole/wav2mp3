//
// Created by beaver on 12/31/19.
//

#include "FilesListMaker.h"

list<string> FilesListMaker::makeFilesList(const string& directory) {
    DIR *dir;
    list<string> outList;
    struct dirent *ent;
    if ((dir = opendir (directory.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(".", ent->d_name)==0 || strcmp("..", ent->d_name)==0){
                continue;
            }
            char resolved[PATH_MAX];
            realpath(directory.c_str(), resolved);
            string gotFilePath(string(resolved) + PATH_SEPARATOR + string(ent->d_name));
            cout << "got name = " << gotFilePath << endl;
            outList.push_back(gotFilePath);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        cerr << "Error in opening directory : " << directory << endl;
    }
    return outList;
}

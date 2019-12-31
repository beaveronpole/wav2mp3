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
            cout << "got name = " << ent->d_name << endl;
            outList.push_back(string(ent->d_name));
        }
        closedir (dir);
    } else {
        /* could not open directory */
        cerr << "Error in opening directory : " << directory << endl;
    }
    return outList;
}

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "error.h"
#include "basefunction.h"

int main(int argc, char *argv[]) {

    std::string backup_path = getcwd(NULL, 0);
    backup_path = backup_path + "/backup";
    if (access(backup_path.c_str(), F_OK) == -1) { // new backup
        int isCreateFolder = mkdir(backup_path.c_str(), S_IRWXU);
        if (isCreateFolder == -1)
            errorhanding(FOLDER_CREATE_FOLDER);
    }

    if (argc < 4)
        errorhanding(ARGC_ERROR_PATH);
    // argc[0]  the data path need to be operate
    // argc[1]  operate backup(0) or recover(1)

    // 遍历文件树
    std::string key = KEY;
    if (argc > 4) {
        key = argv[4];
        if (key.size() < CEN_LEN) {
            int p = CEN_LEN - key.size();
            for (int i = 0; i < p; i++)
                key = key + "0";
        }
        if (key.size() > CEN_LEN) {
            int p = key.size() - CEN_LEN;
            for (int i = 0; i < p; i++)
                key.pop_back();
        }
    }

    int operator_num = atoi(argv[2]);
    int more = atoi(argv[3]);

    produce(argv[1], operator_num, key, more);
    return 0;
}
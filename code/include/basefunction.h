#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <map>

#include "error.h"

#define basenum 7
#define modnum 1000000007

#define BACKUP 0
#define RECOVER 1
#define CHECK 2

#define KEY "00000000000000000000000000000000"
#define CEN_LEN 32

int isDir(mode_t mode);
int isFIFO(mode_t mode);
int isLink(mode_t mode);

int hash(char *path);     // get a hash number of the path
int filetype(char *path); // return the type of a file
void produce(char *path, int mode, std::string key, int en); // operate the file
int getChecksum(const char *path, mode_t mode); // get the checksum of the file

struct filetree {
    std::string path;     // the path of the file or directory
    struct stat filebuff; // the attribute
    int checksum;         // the checksum of the file
    int hard_link;        // hard link file
    int sonnum;
    int totfile;
    int linenum;
    std::vector<filetree *> son; // the sons of current node
    filetree() {
        path = "";
        checksum = 0;
        sonnum = 0;
        totfile = 0;
        linenum = 0;
        hard_link = 0;
    }
    filetree(std::string path_) : path(path_) {
        bool getfile = lstat(path.c_str(), &filebuff);
        if (getfile == -1)
            errorhanding(GET_FILE_FAIL);
        hard_link = 0;
        checksum = getChecksum(path.c_str(), filebuff.st_mode);
        linenum = 0;
    }

    void savadata(std::string name);
    bool backup(std::string name);
};

bool recover(std::string name);  // rebuild a file tree with a backup file
void readdata(std::string name); // read data from a backup file
int build(filetree *node); // build the file tree and return the number of file
int deletefile(std::string path); // remove all file

std::vector<std::pair<int, std::string>> getChecksumfromTree(filetree *root);
std::vector<std::pair<int, std::string>> getChecksumfromFile(std::string name);
std::vector<std::string> compare(std::vector<std::pair<int, std::string>> x,
                                 std::vector<std::pair<int, std::string>> y);
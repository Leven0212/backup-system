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

#define KEY "0000000000000000"
#define CEN_LEN 16

int isDir(mode_t mode);
int isFIFO(mode_t mode);
int isLink(mode_t mode);

int hash(char *path);     // get a hash number of the path
int filetype(char *path); // return the type of a file
void produce(char *path, int mode, std::string key); // operate the file
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

    void savadata(std::string name) {
        std::string data, s;

        if (!isDir(filebuff.st_mode) && !isFIFO(filebuff.st_mode)) {
            /*if (hard.find(filebuff.st_ino) != hard.end()) {
                // hard Links
                hard_link = 1;
                data = hard[filebuff.st_ino] + "\n";
            } else {*/
            // Symbolic Links
            // hard[filebuff.st_ino] = path;
            if (isLink(filebuff.st_mode)) {

                std::string dst = path;

                char slink_path[1024];
                memset(slink_path, 0, sizeof(slink_path));
                int len =
                    readlink(path.c_str(), slink_path, sizeof(slink_path));
                if (len <= 0)
                    errorhanding(LINK_FAIL);

                int l = dst.size();
                for (int i = l - 1; i >= 0; i--) {
                    if (dst[i] == '/')
                        break;
                    dst.pop_back();
                }
                if (slink_path[0] != '/')
                    dst = dst + slink_path;
                else
                    dst = slink_path;
                data = dst + "\n";

            } else {
                std::ifstream is;
                is.open(path, std::ios::binary | std::ios::in);
                if (!is.is_open())
                    errorhanding(FILE_OPEN_FAIL);
                char ch;
                while (getline(is, s))
                    data += s + "\n", linenum++;
                is.close();
            }
            // }
        }

        std::ofstream os;
        os.open(name, std::ios::app | std::ios::binary);
        os << path << " ";
        os << filebuff.st_mode << " " << filebuff.st_size << " "
           << filebuff.st_uid << " ";
        os << filebuff.st_atim.tv_sec << " " << filebuff.st_atim.tv_nsec << " "
           << filebuff.st_mtim.tv_sec << " " << filebuff.st_mtim.tv_nsec << " "
           << filebuff.st_ctim.tv_sec << " " << filebuff.st_ctim.tv_nsec << " "
           << filebuff.st_uid << " " << filebuff.st_gid << " ";
        os << checksum << " " << sonnum << " " << linenum << " " << hard_link
           << std::endl;
        os << data;
        os.close();

        for (auto i : son)
            i->savadata(name);
    }
    bool backup(std::string name) {
        std::ofstream os;
        os.open(name, std::ios::app | std::ios::binary);
        os << totfile << std::endl;
        os.close();
        savadata(name);
        return true;
    }
    bool check(std::string name) { return true; }
};

bool recover(std::string name);  // rebuild a file tree with a backup file
void readdata(std::string name); // read data from a backup file
int build(filetree *node); // build the file tree and return the number of file
int deletefile(std::string path); // remove all file

std::vector<int> getChecksumfromTree(filetree *root);
std::vector<int> getChecksumfromFile(std::string name);
bool compare(std::vector<int> x, std::vector<int> y);
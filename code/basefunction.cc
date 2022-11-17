#include "basefunction.h"
#include "error.h"
#include <stack>
#include <sys/time.h>
#include <utime.h>
#include "huffman_interface.h"
#include "AES.h"

std::map<ino_t, std::string> hard;

int isDir(mode_t mode) { return (S_ISDIR(mode)); }
int isFIFO(mode_t mode) { return (S_ISFIFO(mode)); }
int isLink(mode_t mode) { return (S_ISLNK(mode)); }

int hash(char *path) {
    long long tot = 0;
    for (int i = 0; path[i]; i++) {
        tot = (path[i] * basenum + tot) % modnum;
    }
    return tot;
}

bool filetree::backup(std::string name) {
    hard.clear();
    std::ofstream os;
    os.open(name, std::ios::app | std::ios::binary);
    os << totfile << std::endl;
    os.close();
    savadata(name);
    return true;
}
void filetree::savadata(std::string name) {
    std::string data, s;
    if (!isDir(filebuff.st_mode) && !isFIFO(filebuff.st_mode)) {
        if (hard.find(filebuff.st_ino) != hard.end()) {
            // hard Links
            linenum++;
            hard_link = 1;
            data = hard[filebuff.st_ino] + "\n";
        } else {
            // Symbolic Links
            hard[filebuff.st_ino] = path;
            if (isLink(filebuff.st_mode)) {
                linenum++;
                std::string dst = path;

                char slink_path[1024];
                memset(slink_path, 0, sizeof(slink_path));
                int len =
                    readlink(path.c_str(), slink_path, sizeof(slink_path));
                if (len <= 0)
                    errorhanding(LINK_FAIL);

                // int l = dst.size();
                // for (int i = l - 1; i >= 0; i--) {
                //     if (dst[i] == '/')
                //         break;
                //     dst.pop_back();
                // }
                // if (slink_path[0] != '/')
                //     dst = dst + slink_path;
                // else
                //     dst = slink_path;
                data = data + slink_path + "\n";

            } else {
                std::ifstream is;
                is.open(path, std::ios::binary | std::ios::in);
                if (!is.is_open())
                    errorhanding(FILE_OPEN_FAIL);
                char ch;
                while (getline(is, s)) {
                    data += "\n";
                    data += s;
                    linenum++;
                }
                data.erase(data.begin());
                is.close();

                FILE *fp = fopen(path.c_str(), "r");
                fseek(fp, -1, SEEK_END);
                ch = fgetc(fp);
                if (ch == '\n')
                    lastch = 1;
            }
        }
    }

    std::ofstream os;
    os.open(name, std::ios::app | std::ios::binary);
    os << path << " ";
    os << filebuff.st_mode << " " << filebuff.st_size << " " << filebuff.st_uid
       << " ";
    os << filebuff.st_atim.tv_sec << " " << filebuff.st_atim.tv_nsec << " "
       << filebuff.st_mtim.tv_sec << " " << filebuff.st_mtim.tv_nsec << " "
       << filebuff.st_ctim.tv_sec << " " << filebuff.st_ctim.tv_nsec << " "
       << filebuff.st_uid << " " << filebuff.st_gid << " ";
    os << checksum << " " << sonnum << " " << linenum << " " << hard_link << " "
       << lastch << std::endl;
    if (data.size())
        os << data << std::endl;
    os.close();

    for (auto i : son)
        i->savadata(name);
}

void produce(char *path, int mode, std::string key, int en) {

    std::string pwd = getcwd(NULL, 0);
    std::string aimfolder = pwd + "/backup/";
    int newName = hash(path);
    aimfolder += std::to_string(newName);
    std::string aimfile = aimfolder + "/" + std::to_string(newName);

    // check and create folder
    // "w+" build and open a file which is always empty
    if (access(aimfolder.c_str(), F_OK) == -1) { // new backup
        int isCreateFolder = mkdir(aimfolder.c_str(), S_IRWXU);
        if (isCreateFolder == -1)
            errorhanding(FOLDER_CREATE_FOLDER);
    }
    bool operate_check = true;

    char *temp = const_cast<char *>(key.c_str());
    unsigned char *keySrc = reinterpret_cast<unsigned char *>(temp);
    aes *Crypt = new aes();
    Crypt->setKey(keySrc, Crypt->AES_256);

    if (mode == BACKUP) {
        deletefile(aimfolder);
        mkdir(aimfolder.c_str(), S_IRWXU);

        FILE *fd = fopen(aimfile.c_str(), "w+");
        if (fd == NULL)
            errorhanding(FOLDER_CREATE_OPEN_FILE);
        fclose(fd);
        filetree *root = new filetree(path);
        build(root);

        operate_check = root->backup(aimfile);
        if (operate_check == false)
            errorhanding(BACKUP_FAIL);

        if (en > 0) {
            std::string huf = aimfile + ".huf";
            std::string cpt = huf + ".cpt";
            ZIP(aimfile.c_str()); // x -> x.huf
            if (en > 1) {
                Crypt->encryptFile(huf.c_str(),
                                   cpt.c_str()); // x.huf -> x.huf.cpt
                unlink(huf.c_str());
            }
            unlink(aimfile.c_str());
        }

    } else if (mode == RECOVER) {
        std::string huf = aimfile + ".huf";
        std::string cpt = huf + ".cpt";
        if (en > 0) {
            if (en > 1) {
                Crypt->decryptFile(cpt.c_str(),
                                   huf.c_str()); // x.huf.cpt -> x.huf
            }
            UnZIP(aimfile.c_str()); // x.huf -> x
        }
        if (access(path, F_OK) == 0) {
            if (deletefile(path) == -1)
                errorhanding(DELETE_FAIL);
        }

        operate_check = recover(aimfile);
        if (operate_check == false)
            errorhanding(RECOVER_FAIL);

        if (en) {
            unlink(aimfile.c_str());
            if (en > 1)
                unlink(huf.c_str());
        }
    }

    else if (mode == CHECK) {
        operate_check = true;

        std::string huf = aimfile + ".huf";
        std::string cpt = huf + ".cpt";
        if (en > 0) {
            if (en > 1) {
                Crypt->decryptFile(cpt.c_str(),
                                   huf.c_str()); // x.huf.cpt -> x.huf
            }
            UnZIP(aimfile.c_str()); // x.huf -> x
        }

        filetree *root = new filetree(path);
        build(root);
        std::vector<std::pair<int, std::string>> p1 = getChecksumfromTree(root);
        std::vector<std::pair<int, std::string>> p2 =
            getChecksumfromFile(aimfile);
        std::vector<std::string> fail = compare(p1, p2);
        if (fail.size()) {
            std::string pth = pwd + "/fail.txt";
            std::ofstream os;
            os.open(pth, std::ios::binary | std::ios::out);
            for (auto i : fail)
                os << i << "\n";
            os.close();
            errorhanding(CHECK_FAIL);
        } else
            std::cout << "Backup succeeded!\n";
        if (en) {
            unlink(aimfile.c_str());
            if (en > 1)
                unlink(huf.c_str());
        }

    } else {
        // operate error
        errorhanding(ARGC_ERROR_OPERATE);
    }
}

std::vector<std::pair<int, std::string>> getChecksumfromTree(filetree *root) {
    std::vector<std::pair<int, std::string>> res;
    res.clear();
    res.push_back(std::make_pair(root->checksum, root->path));
    for (auto i : root->son) {
        std::vector<std::pair<int, std::string>> temp = getChecksumfromTree(i);
        for (auto j : temp)
            res.push_back(j);
    }
    return res;
}

std::vector<std::pair<int, std::string>> getChecksumfromFile(std::string name) {
    std::vector<std::pair<int, std::string>> res;
    res.clear();

    int tot;
    std::ifstream is;
    is.open(name, std::ios::in | std::ios::binary);
    if (!is.is_open())
        errorhanding(FILE_OPEN_FAIL);
    is >> tot;

    for (int i = 0; i < tot; i++) {

        filetree *nowfile = new filetree();
        is >> nowfile->path >> nowfile->filebuff.st_mode >>
            nowfile->filebuff.st_size >> nowfile->filebuff.st_uid;
        is >> nowfile->filebuff.st_atim.tv_sec >>
            nowfile->filebuff.st_atim.tv_nsec >>
            nowfile->filebuff.st_mtim.tv_sec >>
            nowfile->filebuff.st_mtim.tv_nsec >>
            nowfile->filebuff.st_ctim.tv_sec >>
            nowfile->filebuff.st_ctim.tv_nsec >> nowfile->filebuff.st_uid >>
            nowfile->filebuff.st_gid;
        is >> nowfile->checksum >> nowfile->sonnum >> nowfile->linenum >>
            nowfile->hard_link >> nowfile->lastch;

        if (!isDir(nowfile->filebuff.st_mode)) {
            std::string s;
            getline(is, s);
            for (int i = 0; i < nowfile->linenum; i++)
                getline(is, s);
        }

        res.push_back(std::make_pair(nowfile->checksum, nowfile->path));
    }
    is.close();
    return res;
}
std::vector<std::string> compare(std::vector<std::pair<int, std::string>> x,
                                 std::vector<std::pair<int, std::string>> y) {

    std::vector<std::string> res;
    res.clear();

    std::map<std::string, int> M;
    M.clear();

    for (auto i : y)
        M[i.second] = i.first;

    for (auto i : x) {
        if (M.find(i.second) == M.end()) { // null
            res.push_back(i.second);
        } else {
            if (M[i.second] != i.first)
                res.push_back(i.second);
            M.erase(i.second);
        }
    }
    for (auto iter = M.begin(); iter != M.end(); ++iter) {
        res.push_back(iter->first);
    }
    return res;
}

int deletefile(std::string path) {
    struct stat st;
    if (lstat(path.c_str(), &st) == -1) {
        return -1;
    }
    if (!isDir(st.st_mode)) {
        if (unlink(path.c_str()) == -1) {
            return -1;
        }
        return 0;
    }

    DIR *dirp = opendir(path.c_str());
    if (!dirp)
        return -1;

    struct dirent *dir;
    while ((dir = readdir(dirp)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }
        std::string sub_path = path + '/' + dir->d_name;
        if (lstat(sub_path.c_str(), &st) == -1) {
            errorhanding(FILE_OPEN_FAIL);
            continue;
        }
        if (isDir(st.st_mode)) { // dir
            if (deletefile(sub_path) == -1) {
                closedir(dirp);
                return -1;
            }
            rmdir(sub_path.c_str());
        } else { // file
            unlink(sub_path.c_str());
        }
    }
    if (rmdir(path.c_str()) == -1) // delete dir itself.
    {
        closedir(dirp);
        return -1;
    }
    closedir(dirp);
    return 0;
}

int build(filetree *node) {
    int sum = 0;
    if (isDir(node->filebuff.st_mode)) {

        DIR *dir = opendir(node->path.c_str());
        if (dir == NULL)
            errorhanding(OPEN_FOLDER_FAIL);

        struct dirent *file = new dirent();
        while ((file = readdir(dir)) != NULL) {
            if (strncmp(file->d_name, ".", 1) == 0) // ingore the hidden file
                continue;

            std::string nowpath = node->path + "/" + file->d_name;
            filetree *nownode = new filetree(nowpath);
            node->son.push_back(nownode);
            sum += build(nownode);
        }
        node->sonnum = node->son.size();
        return node->totfile = sum + 1;

    } else {
        node->son.clear();
        node->sonnum = 0;
        return node->totfile = 1;
    }
}
int getChecksum(const char *path, mode_t mode) {

    int checksum = 0;

    // FIFO file or dir
    if (isDir(mode) || isFIFO(mode) || isLink(mode)) {
        char *temp = const_cast<char *>(path);
        checksum = hash(temp);
        return checksum;
    }

    // file
    std::ifstream os;
    os.open(path, std::ios::binary | std::ios::in);
    if (!os.is_open())
        errorhanding(FILE_OPEN_FAIL);

    int temp = 0, cnt = 0;
    char ch;
    while (os >> ch) {
        temp = (temp << 4) + ch;
        cnt++;
        if (cnt == 4)
            checksum += temp, cnt = 0;
    }
    if (cnt)
        checksum += temp, cnt = 0;

    os.close();
    return checksum;
}

bool recover(std::string name) {
    readdata(name);
    return true;
}

void changeAttr(filetree *nowfile) {

    int change = 0;
    change += chmod(nowfile->path.c_str(), nowfile->filebuff.st_mode);
    if (change < 0)
        errorhanding(UPDATE_FAIL);
    change += chown(nowfile->path.c_str(), nowfile->filebuff.st_uid,
                    nowfile->filebuff.st_gid);

    if (isLink(nowfile->filebuff.st_mode)) {
        struct timeval time_buf[2];
        time_buf[0].tv_sec = nowfile->filebuff.st_atim.tv_sec;
        time_buf[0].tv_usec = nowfile->filebuff.st_atim.tv_nsec / 1000;
        time_buf[1].tv_sec = nowfile->filebuff.st_mtim.tv_sec;
        time_buf[1].tv_usec = nowfile->filebuff.st_mtim.tv_nsec / 1000;
        change += lutimes(nowfile->path.c_str(), time_buf);
    } else {
        struct utimbuf timebuf;
        timebuf.actime = nowfile->filebuff.st_atim.tv_sec;
        timebuf.modtime = nowfile->filebuff.st_mtim.tv_sec;
        change += utime(nowfile->path.c_str(), &timebuf);
    }

    if (change < 0)
        errorhanding(UPDATE_FAIL);
}

void readdata(std::string name) {

    int tot;
    std::ifstream is;
    is.open(name, std::ios::in | std::ios::binary);
    if (!is.is_open())
        errorhanding(FILE_OPEN_FAIL);
    is >> tot;

    std::vector<std::pair<std::string, filetree *>> sy_link;
    std::vector<filetree *> folder;

    for (int i = 0; i < tot; i++) {

        filetree *nowfile = new filetree();
        is >> nowfile->path >> nowfile->filebuff.st_mode >>
            nowfile->filebuff.st_size >> nowfile->filebuff.st_uid;
        is >> nowfile->filebuff.st_atim.tv_sec >>
            nowfile->filebuff.st_atim.tv_nsec >>
            nowfile->filebuff.st_mtim.tv_sec >>
            nowfile->filebuff.st_mtim.tv_nsec >>
            nowfile->filebuff.st_ctim.tv_sec >>
            nowfile->filebuff.st_ctim.tv_nsec >> nowfile->filebuff.st_uid >>
            nowfile->filebuff.st_gid;
        is >> nowfile->checksum >> nowfile->sonnum >> nowfile->linenum >>
            nowfile->hard_link >> nowfile->lastch;

        std::ofstream os;
        if (isDir(nowfile->filebuff.st_mode)) {
            // dir
            if (access(nowfile->path.c_str(), F_OK) == -1) {
                int isCreateFolder = mkdir(nowfile->path.c_str(), S_IRWXU);
                if (isCreateFolder == -1)
                    errorhanding(FOLDER_CREATE_FOLDER);
            }
            folder.push_back(nowfile);
        } else {
            if (isFIFO(nowfile->filebuff.st_mode)) {
                // pipe
                mkfifo(nowfile->path.c_str(), nowfile->filebuff.st_mode);
            } else {
                if (isLink(nowfile->filebuff.st_mode)) {
                    // soft-link
                    std::string s;
                    getline(is, s); // input empty line
                    getline(is, s);
                    sy_link.push_back(std::make_pair(s, nowfile));
                    continue;
                } else {
                    // hard link
                    if (nowfile->hard_link) {
                        std::string s;
                        getline(is, s);
                        getline(is, s);
                        if (link(s.c_str(), nowfile->path.c_str()) == -1)
                            errorhanding(HARD_LINK_CREATE_FAIL);
                    } else {
                        // file
                        os.open(nowfile->path,
                                std::ios::out | std::ios::binary);
                        std::string s;
                        getline(is, s);
                        for (int i = 0; i < nowfile->linenum; i++) {
                            getline(is, s);
                            os << s;
                            if (i < nowfile->linenum - 1)
                                os << std::endl;
                            else if (nowfile->lastch)
                                os << std::endl;
                        }
                        os.close();
                    }
                }
            }
            // update the Attributes of file
            changeAttr(nowfile);
        }
    }
    int l = folder.size();
    for (int i = l - 1; i >= 0; i--)
        changeAttr(folder[i]);

    for (auto i : sy_link) {
        if (symlink(i.first.c_str(), i.second->path.c_str()) == -1)
            errorhanding(LINK_CREATE_FAIL);
        changeAttr(i.second);
    }
    is.close();
}
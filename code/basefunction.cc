#include "basefunction.h"
#include "error.h"
#include <stack>
#include <sys/time.h>
#include <utime.h>
#include "encrypt_and_decrypt.h"

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

void produce(char *path, int mode, std::string key) {

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

    if (mode == BACKUP) {
        FILE *fd = fopen(aimfile.c_str(), "w+");
        if (fd == NULL)
            errorhanding(FOLDER_CREATE_OPEN_FILE);
        fclose(fd);
        filetree *root = new filetree(path);
        build(root);

        std::map<ino_t, std::string> hard;
        hard.clear();
        operate_check = root->backup(aimfile);
        if (operate_check == false)
            errorhanding(BACKUP_FAIL);
        Encrypt(aimfile, key);
        unlink(aimfile.c_str());

    } else if (mode == RECOVER) {
        int deletacc = deletefile(path);
        /*if (deletacc == -1)
            errorhanding(DELETE_FAIL);*/
        Decrypt(aimfile, key);
        operate_check = recover(aimfile);
        if (operate_check == false)
            errorhanding(RECOVER_FAIL);
        unlink(aimfile.c_str());
    }

    else if (mode == CHECK) {
        operate_check = true;
        Decrypt(aimfile, key);
        filetree *root = new filetree(path);
        build(root);
        std::vector<int> p1 = getChecksumfromTree(root);
        std::vector<int> p2 = getChecksumfromFile(aimfile);
        bool legal = compare(p1, p2);
        if (!legal)
            std::cout << "There is a backup problem!\n";
        else
            std::cout << "Backup succeeded!\n";
        unlink(aimfile.c_str());

    } else {
        // operate error
        errorhanding(ARGC_ERROR_OPERATE);
    }
}

std::vector<int> getChecksumfromTree(filetree *root) {
    std::vector<int> res;
    res.clear();
    res.push_back(root->checksum);
    for (auto i : root->son) {
        std::vector<int> temp = getChecksumfromTree(i);
        for (auto j : temp)
            res.push_back(j);
    }
    return res;
}

std::vector<int> getChecksumfromFile(std::string name) {
    std::vector<int> res;
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
        is >> nowfile->checksum >> nowfile->sonnum >> nowfile->linenum;

        if (!isDir(nowfile->filebuff.st_mode)) {
            std::string s;
            getline(is, s);
            for (int i = 0; i < nowfile->linenum; i++)
                getline(is, s);
        }

        res.push_back(nowfile->checksum);
    }
    is.close();
    return res;
}
bool compare(std::vector<int> x, std::vector<int> y) {
    if (x.size() != y.size())
        return 0;
    for (int i = 0; i < x.size(); i++) {
        if (x[i] != y[i])
            return 0;
    }
    return 1;
}

int deletefile(std::string path) {
    struct stat st;
    if (lstat(path.c_str(), &st) == -1) {
        return -1;
    }
    if (S_ISREG(st.st_mode)) {
        if (unlink(path.c_str()) == -1) {
            return -1;
        }
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
        if (S_ISDIR(st.st_mode)) { // dir
            if (deletefile(sub_path) == -1) {
                closedir(dirp);
                return -1;
            }
            rmdir(sub_path.c_str());
        } else if (S_ISREG(st.st_mode)) { // file
            unlink(sub_path.c_str());
        } else {
            continue;
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
            nowfile->hard_link;

        std::ofstream os;
        if (isDir(nowfile->filebuff.st_mode)) {
            // dir
            if (access(nowfile->path.c_str(), F_OK) == -1) {
                int isCreateFolder = mkdir(nowfile->path.c_str(), S_IRWXU);
                if (isCreateFolder == -1)
                    errorhanding(FOLDER_CREATE_FOLDER);
            }
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
                            os << s << std::endl;
                        }
                        os.close();
                    }
                }
            }
        }
        // update the Attributes of file or dir
        changeAttr(nowfile);
    }
    for (auto i : sy_link) {
        symlink(i.first.c_str(), i.second->path.c_str());
        changeAttr(i.second);
    }
    is.close();
}
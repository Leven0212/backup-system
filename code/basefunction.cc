#include "basefunction.h"
#include "error.h"
#include <stack>

bool isDirectory(mode_t mode) { return (S_ISDIR(mode)); }

int hash(char *path) {
    long long tot = 0;
    for (int i = 0; path[i]; i++) {
        tot = (path[i] * basenum + tot) % modnum;
    }
    return tot;
}

void produce(char *path, int mode, char *key) {

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
        operate_check = root->backup(aimfile);
        if (operate_check == false)
            errorhanding(BACKUP_FAIL);
    } else if (mode == RECOVER) {
        int deletacc = deletefile(path);
        /*if (deletacc == -1)
            errorhanding(DELETE_FAIL);*/
        operate_check = recover(aimfile);
        if (operate_check == false)
            errorhanding(RECOVER_FAIL);
    }

    else if (mode == CHECK) {
        operate_check = true;
        if (operate_check == false)
            errorhanding(CHECK_FAIL);
    } else {
        // operate error
        errorhanding(ARGC_ERROR_OPERATE);
    }
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
    if (isDirectory(node->filebuff.st_mode)) {

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
int getChecksum(const char *path) {
    int checksum = 0;
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
    std::vector<filetree *> tot = readdata(name);
    int now = 0;
    filetree *root = tot[now++];
    std::stack<std::pair<filetree *, int>> S;
    while (!S.empty())
        S.pop();
    if (root->sonnum)
        S.push(std::make_pair(root, root->sonnum));

    while (!S.empty()) {
        std::pair<filetree *, int> cur = S.top();
        S.pop();
        cur.second--;
        filetree *p = tot[now++];
        cur.first->son.push_back(p);
        if (cur.second)
            S.push(cur);
        if (p->sonnum)
            S.push(std::make_pair(p, p->sonnum));
    }
    return true;
}

std::vector<filetree *> readdata(std::string name) {

    int tot;
    std::ifstream is;
    is.open(name, std::ios::in | std::ios::binary);
    if (!is.is_open())
        errorhanding(FILE_OPEN_FAIL);
    std::vector<filetree *> res;
    is >> tot;

    std::string path;
    mode_t temp;
    for (int i = 0; i < tot; i++) {
        is >> path >> temp;
        std::ofstream os;
        if (isDirectory(temp)) {
            if (access(path.c_str(), F_OK) == -1) {
                int isCreateFolder = mkdir(path.c_str(), S_IRWXU);
                if (isCreateFolder == -1)
                    errorhanding(FOLDER_CREATE_FOLDER);
            }
        } else
            os.open(path, std::ios::out | std::ios::binary);

        filetree *nowfile = new filetree(path);
        nowfile->filebuff.st_mode = temp;
        is >> nowfile->filebuff.st_size >> nowfile->filebuff.st_uid;
        is >> nowfile->filebuff.st_atim.tv_sec >>
            nowfile->filebuff.st_atim.tv_nsec >>
            nowfile->filebuff.st_mtim.tv_sec >>
            nowfile->filebuff.st_mtim.tv_nsec >>
            nowfile->filebuff.st_ctim.tv_sec >>
            nowfile->filebuff.st_ctim.tv_nsec;
        is >> nowfile->checksum >> nowfile->sonnum >> nowfile->linenum;

        if (!isDirectory(nowfile->filebuff.st_mode)) {
            std::string s;
            getline(is, s);
            for (int i = 0; i < nowfile->linenum; i++) {
                getline(is, s);
                os << s << std::endl;
            }
            os.close();
        }

        res.push_back(nowfile);
    }
    is.close();
    return res;
}
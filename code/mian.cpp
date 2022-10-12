#include <stdio.h>
#include <stdlib.h>
#include "errorhanding/error.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        errorhanding(ARGC_ERROR);
        exit(0);
    }
    // argc[0]  the data path need to be operate
    // argc[1]  operate backup(0) or recover(1)

    // 遍历文件树
}
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "error.h"
#include "basefunction.h"

int main(int argc, char *argv[]) {
    if (argc < 3)
        errorhanding(ARGC_ERROR_PATH);
    // argc[0]  the data path need to be operate
    // argc[1]  operate backup(0) or recover(1)

    // 遍历文件树
    std::string key = KEY;
    if (argc > 3)
        key = argv[3];
    int operator_num = atoi(argv[2]);
    produce(argv[1], operator_num, key);
    return 0;
}
#include "error.h"
#include <stdlib.h>
#include <cstdio>
#include <cerrno>

void errorhanding(int errorcode) {
    printf("%d\n", errno);
    switch (errorcode) {
    case 1:
        printf("error:Command line requires more parameters.\n");
        break;
    case 2:
        printf("error:Wrong parameter.\n");
        printf("please input with [path] [012] [012] [key]\n");
        break;
    case 3:
        printf("error:Directory creation failed.\n");
        break;
    case 4:
        printf("error:Ordinary file creation failed.\n");
        break;
    case 5:
        printf("error:Backup fail.\n");
        break;
    case 6:
        printf("error:Recover fail.\n");
        break;
    case 7:
        printf("error:Check fail.\n");
        break;
    case 8:
        printf("error:File opening failed.\n");
        printf("Please check the current permissions.\n");
        break;
    case 9:
        printf("error:Directory opening failed.\n");
        printf("Please check the current permissions.\n");
        break;
    case 10:
        printf("Get file name fail.\n");
        break;
    case 11:
        printf("Delete file fail.\n");
        break;
    case 12:
        printf("Failed to change file properties.\n");
        printf("Please check the permission or whether the file exists.\n");
    case 13:
        printf("Read Link file fail.\n");
        break;
    case 14:
        printf("Create symbolic Links fail.\n");
        break;
    case 15:
        printf("Create hard Links fail.\n");
        break;

    default:
        printf("Unkown error.\n");
        break;
    }
    printf("%d\n", errorcode);
    exit(0);
}
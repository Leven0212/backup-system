#include <stdio.h>

#define ARGC_ERROR_PATH 1
#define ARGC_ERROR_OPERATE 2

#define FOLDER_CREATE_FOLDER 3
#define FOLDER_CREATE_OPEN_FILE 4

#define BACKUP_FAIL 5
#define RECOVER_FAIL 6
#define CHECK_FAIL 7

#define FILE_OPEN_FAIL 8
#define OPEN_FOLDER_FAIL 9

#define GET_FILE_FAIL 10
#define DELETE_FAIL 11

void errorhanding(int errorcode);
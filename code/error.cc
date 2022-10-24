#include "error.h"
#include <stdlib.h>
#include <cstdio>
#include <cerrno>

void errorhanding(int errorcode) {
    printf("%d\n", errno);
    printf("%d\n", errorcode);
    exit(0);
}
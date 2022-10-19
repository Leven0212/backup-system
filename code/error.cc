#include "error.h"
#include <stdlib.h>
#include <cstdio>

void errorhanding(int errorcode) {
    printf("%d\n", errorcode);
    exit(0);
}
#pragma once

#include "stdio.h"
#include "stdlib.h"

int findIndex(char *s, int len, char *sub, int sublen) {
    if (sublen > 100) {
        puts("sublen > 100...");
        exit(-1);
    }

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < sublen; j++) {
            if (i + j >= len) {
                break;
            }
            if (s[i + j] != sub[j]) {
                break;
            }
            if (j == sublen - 1) {
                return i;
            }
        }
    }

    return -1;
}
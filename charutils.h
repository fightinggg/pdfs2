
#pragma once

#include <bits/stdc++.h>

using namespace std;

int findIndex(const char *s, int len, const char *sub, int sublen) {
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

    return len;
}


void splitString(const std::string &s, std::vector<std::string> &v, const std::string &c) {
    std::string::size_type pos1, pos2;
    size_t len = s.length();
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.emplace_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != len)
        v.emplace_back(s.substr(pos1));
}


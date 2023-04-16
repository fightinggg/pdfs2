
#pragma once

#include "allheader.h"

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
    v.clear();
    if (c.empty()) {
        printf("splitString ERROR");
        exit(-1);
    }
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


bool startsWith(const std::string &str, const std::string &prefix) {
    if (str.size() < prefix.size()) {
        return false;
    }
    return str.substr(0, prefix.size()) == prefix;
}


string trim(string s) {
    string res;
    bool allIsEmpty = true;
    for (auto ch: s) {
        allIsEmpty = ch == ' ' && allIsEmpty;
        if (!allIsEmpty) {
            res += ch;
        }
    }
    while (!res.empty() && res.back() == ' ') {
        res.pop_back();
    }
    return res;
}

string randomBinaryString(int size) {
    string s;
    for (int i = 0; i < size; i++) {
        s += char(rand() & 0xff);
    }
    return s;
}
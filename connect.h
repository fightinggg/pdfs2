#ifndef connect

#include <bits/stdc++.h>
#include <algorithm>
#include "stream.h"
#include "charutils.h"
#include "handler.h"



void doHttp(char head[1024], int len, struct inputStream in, int outfd) {

    string s(head, len);
    vector<string> split;

    string oldStr = "\r\n";
    string newStr = "\n";

    size_t pos = 0;
    while ((pos = s.find(oldStr, pos)) != string::npos) {
        s.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    splitString(s, split, "\n");

    string requestLine = split[0];
    split.erase(split.begin());

    map<string, string> headers;
    for (const auto &item: split) {
        vector<string> kv;
        splitString(item, kv, " ");
        headers[kv[0]] = kv[1];
    }

    vector<string> requestLineParams;
    splitString(requestLine, requestLineParams, " ");
    if (requestLineParams.size() == 3) {
        string method = requestLineParams[0];
        string url = requestLineParams[1];
        doHandler(method, url, headers, in, outfd);
    } else {
        string header = "HTTP/1.1 403 Forbidden\n\n";
        send(outfd, header.data(), header.size(), 0);
    }

}



void *processConnect(void *args) {
    char buffer[1024]; // 1KB
    int fd = *(int *) (&args);
    printf("connect fd = %d\n", fd);
    fflush(stdout);

    struct inputStream in;
    in.buf = NULL;
    initFdStream(&in, fd);

    while (1) {
        int len = readFromStream(&in, buffer, sizeof(buffer));
        if (len <= 0) {
            break;
        }

//        puts(buffer);

        int find = findIndex(buffer, len, "\n\n", 2);
        if (find == len) {
            find = findIndex(buffer, len, "\r\n\r\n", 4);
        }

        if (find != len) {
            mergeStreamUsingChars(&in, buffer + find, len - find);
            doHttp(buffer, find, in, fd);
            break;
        }

    }

    closeStream(&in);
    puts("......done");
    return NULL;
}


#endif
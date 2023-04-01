#ifndef connect

#include <bits/stdc++.h>
#include <algorithm>
#include "stream.h"
#include "charutils.h"

volatile long vv = 0;

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

    string reqdata = "Welcome TO PDFS V2.0, We Will Come back Soon, vv = " + to_string(++vv) + "\n";
    string header = "HTTP/1.1 200 OK\nContent-Length: " + to_string(reqdata.size()) + "\n\n";
    send(outfd, header.data(), header.size(), 0);
    send(outfd, reqdata.data(), reqdata.size(), 0);
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
        if (len == 0) {
            break;
        }
        puts(buffer);

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
    puts("\n..done");
}


#endif
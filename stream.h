#pragma once

#include "utils.h"
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <unistd.h>


class InputStream {
private:
    // return !=1, if nothing to read.
    virtual int read(char *) {
        printf("InputStream virtual int read");
        exit(-1);
    }

public:

    virtual void close() {
        printf("InputStream virtual void close");
        exit(-1);
    }

    // return how much bytes remained to read
    // return -1 if unknown
    virtual int size() {
        return -1;
    }

    string readNbytes(int n) {
        string res;
        while (true) {
            if (res.size() == n) {
                return res;
            }

            char ch;
            if (read(&ch) != 1) {
                return res;
            } else {
                res += ch;
            }
        }
    }
};


class FdInputStream : public InputStream {
    int fd;

    int read(char *ch) override {
        return recv(fd, ch, 1, 0);
    }

    void close() override {
        ::close(fd);
    }

public:

    explicit FdInputStream(int fd) {
        this->fd = fd;
    }
};


class StringInputStream : public InputStream {
    string s;
    int readed = 0;


    int read(char *ch) override {
        if (readed < s.size()) {
            *ch = s[readed++];
            return 1;
        } else {
            return 0;
        }
    }

    void close() override {
    }

    int size() override {
        return s.size() - readed;
    }

public:

    explicit StringInputStream(string s) {
        this->s = s;
    }
};



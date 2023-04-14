#pragma once

#include "utils.h"
#include "allheader.h"
#include "sync/block_queue.h"
#include "io/fdio.h"


class InputStream {
private:
    // return !=1, if nothing to read.
    virtual int read(char *) {
        printf("InputStream virtual int read");
        exit(-1);
    }


public:
    virtual ~InputStream() = default;

    virtual void close() {
        printf("InputStream virtual void close");
        exit(-1);
    }

    // return how much bytes remained to read
    // return -1 if unknown
    virtual int size() {
        return -1;
    }

    string readNbytes(int n = -1) {
        string res;
        while (true) {
            if (n != -1 && res.size() == n) {
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
    int len;

    int read(char *ch) override {
        if (len == -1) {
            return readFd(fd, *ch) ? 1 : 0;
        } else if (len == 0) {
            return 0;
        } else {
            len--;
            return readFd(fd, *ch) ? 1 : 0;
        }
    }

    void close() override {
        ::close(fd);
    }

public:

    explicit FdInputStream(int fd, int len = -1) {
        this->fd = fd;
        this->len = len;
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
        return ((int) s.size()) - readed;
    }

public:

    explicit StringInputStream(string s) {
        this->s = std::move(s);
//        this->s = s;
    }
};


class BlockQueueInputStream : public InputStream {
    BlockingQueue<char> blockingQueue;
    // 禁止指令重排， 强制从内存读取数据
    volatile bool _close = false;
    volatile bool _closeWrite = false;


    int read(char *ch) override {
        int i = 0;
        while (true) {
            if (_close) {
                return 0;
            }
            if (blockingQueue.pop(*ch, 100)) {
                return 1;
            }
            if (i++ == 30) {
                return 0;
            }
            if (_closeWrite) {
                _close = true;
            }
        }
    }

    void close() override {
    }

    int size() override {
        return -1;
    }

public:

    void push(char ch) {
        blockingQueue.push(ch);
    }

    void closePush() {
        _closeWrite = true;
    }

    explicit BlockQueueInputStream() = default;
};



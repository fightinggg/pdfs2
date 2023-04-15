#pragma once

#include "utils.h"
#include "allheader.h"
#include "sync/block_queue.h"
#include "io/fdio.h"
#include "Supplayer.h"


class InputStream {
public:

    // return !=1, if nothing to read.
    virtual bool read(char *) {
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
            if (!read(&ch)) {
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

    bool read(char *ch) override {
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


    bool read(char *ch) override {
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


    bool read(char *ch) override {
        int i = 0;
        while (true) {
            if (_close) {
                return false;
            }
            bool closeBeforeTimeout = _closeWrite;
            if (blockingQueue.pop(*ch, 100)) {
                return true;
            }
            if (i++ == 30) {
                printf("ERROR:BlockQueueInputStream timeout\n");
                return false;
            }
            if (closeBeforeTimeout && _closeWrite) {
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

    ~ BlockQueueInputStream() {
//        printf("BlockQueueInputStream delete");
//        fflush(stdout);
    }

    explicit BlockQueueInputStream() = default;
};


class ChunkInputStream : public InputStream {
    int fd;
    int chunkSize; // -1 is end


    bool read(char *ch) override {
        if (chunkSize == 0) {
//            ::printf("decode chunkSize");
            while (true) {
                if (readFd(fd, *ch) != 1) {
                    break;
                }
//                ::printf("%c", *ch);
//                ::fflush(stdout);

                if (*ch == '\r') {
                    continue;
                }
                if (*ch == '\n') {
                    break;
                }
                if (*ch >= '0' && *ch <= '9') {
                    chunkSize = chunkSize * 16 + (*ch - '0');
                } else if (*ch >= 'a' && *ch <= 'f') {
                    chunkSize = chunkSize * 16 + (*ch - 'a' + 10);
                } else if (*ch >= 'A' && *ch <= 'F') {
                    chunkSize = chunkSize * 16 + (*ch - 'A' + 10);
                } else {
                    int a = 1;
                    a++;
                }
            }

            if (chunkSize == 0) {
                chunkSize = -1;
            }

        }
        if (chunkSize == -1) {
            return 0;
        } else {
            chunkSize--;
            int res = readFd(fd, *ch);
            if (res == 1 && chunkSize == 0) {
                char tmp;
                readFd(fd, tmp);
                if (tmp == '\r') {
                    readFd(fd, tmp);
                }
                if (tmp != '\n') {
                    return 0;
                }
            }
            return res;

        }
    }

    void close() override {
    }

    int size() override {
        return -1;
    }

public:

    explicit ChunkInputStream(int fd) {
        this->fd = fd;
        this->chunkSize = 0;
    }
};

class SubInputStream : public InputStream {
    InputStream *in;
    int skip;
    int remain;


    bool read(char *ch) override {
        if (skip != 0) {
            printf("skip[%d]\n", skip);
            int readTotal = in->readNbytes(skip).size();
            if (readTotal != skip) {
                remain = 0;
                printf("SubInputStream READ ERROR read[%d]!=skip[%d]\n", readTotal, skip);
                fflush(stdout);
                skip = 0;
                return false;
            }
            skip = 0;
        }
        if (remain == 0) {
            return false;
        }
        remain--;
        if (in->read(ch)) {
            return true;
        } else {
            printf("SubInputStream READ ERROR readnoting, remain=%d\n", remain + 1);
            remain = 0;
            return false;
        }
    }

    void close() override {
        in->close();
    }

    int size() override {
        return remain;
    }


public:

    ~SubInputStream() override {
        delete in;
    }

    explicit SubInputStream(InputStream *in, int skip, int remain) {
        this->in = in;
        this->skip = skip;
        this->remain = remain;
    }
};


class MergeInputStream : public InputStream {
    InputStream *in;
    Supplayer<InputStream> *next;
    InputStream *nextIn;
    int remain;


    bool read(char *ch) override {
        if (remain == 0) {
            return false;
        }
        remain--;

        if (nextIn == nullptr) {
            if (in->read(ch)) {
                return true;
            } else {
                nextIn = next->get();
            }
        }
        return nextIn->read(ch);
    }

    void close() override {
        in->close();
    }

    int size() override {
        return remain;
    }


public:

    ~MergeInputStream() override {
        delete in;
        delete next;
        delete nextIn;
    }

    explicit MergeInputStream(InputStream *in, Supplayer<InputStream> *next, int size) {
        this->in = in;
        this->next = next;
        this->nextIn = nullptr;
        this->remain = size;
    }
};


class SmartPointInputStream : public InputStream {
    shared_ptr<InputStream> in;

    bool read(char *ch) override {
        return in->read(ch);
    }

    void close() override {
        in->close();
    }

    int size() override {
        return in->size();
    }


public:


    explicit SmartPointInputStream(shared_ptr<InputStream> i) {
//        printf("new Smart %ld\n", i);
        this->in = shared_ptr<InputStream>(i);
    }
};
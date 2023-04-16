#pragma once


#include "utils.h"
#include "allheader.h"
#include "sync/block_queue.h"
#include "io/fdio.h"
#include "Supplayer.h"
#include "inputstreams/InputStream.h"
#include "inputstreams/BinaryStringInputStream.h"

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
            if (_close || fdstop) {
                return false;
            }
            bool closeBeforeTimeout = _closeWrite;
            if (blockingQueue.pop(*ch, 100)) {
                return true;
            }
            if (i++ == 300) {
                printf("WARN:BlockQueueInputStream timeout\n");
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

    explicit BlockQueueInputStream() = default;
};


class ChunkInputStream : public InputStream {
    shared_ptr<InputStream> in;
    int chunkSize; // -1 is end

    void flush() {
        char ch[1];
        if (chunkSize == 0) {
            while (true) {
                if (!in->read(ch)) {
                    break;
                }

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
    }

    bool eatRN() {
        char ch;
        if (in->read(&ch) && ch == '\r') {
            if (in->read(&ch) && ch == '\n') {
                return true;
            }
        }
        return false;
    }

    bool read(char *ch) override {
        flush();
        if (chunkSize == -1) {
            return false;
        } else {
            int res = in->read(ch);
            chunkSize--;
            if (res == 1 && chunkSize == 0) {
                eatRN();
                flush();
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


    explicit ChunkInputStream(shared_ptr<InputStream> in) {
        this->in = in;
        this->chunkSize = 0;

//        string alldata = readNbytes();
//        auto copy = shared_ptr<InputStream>(new StringInputStream(alldata));
//        ::printf("alldata:\n%s\n", shared_ptr<InputStream>(new BinaryStringInputStream(copy))->readNbytes().data());
//        this->in = shared_ptr<InputStream>(new StringInputStream(alldata));
//        ::fflush(stdout);

    }
};

class SubInputStream : public InputStream {
    shared_ptr<InputStream> in;
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


    explicit SubInputStream(shared_ptr<InputStream> in, int skip, int remain) {
        this->in = in;
        this->skip = skip;
        this->remain = remain;
    }
};


class MergeInputStream : public InputStream {
    shared_ptr<InputStream> in;
    shared_ptr<Supplayer<shared_ptr<InputStream>>> next;
    shared_ptr<InputStream> nextIn;
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
                nextIn = shared_ptr<InputStream>(next->get());
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

    explicit MergeInputStream(shared_ptr<InputStream> in, shared_ptr<Supplayer<shared_ptr<InputStream>>> next,
                              int size) {
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
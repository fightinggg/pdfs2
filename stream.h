#ifndef stream
#define stream

#include "utils.h"
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <unistd.h>

struct inputStream {
    int type; // 1 = fd stream

    int fd;

    int buf_len;
    char *buf;
};


int __readFromFdStream(struct inputStream *in, char *buffer, int size);

void __closeFdStream(struct inputStream *in);

void __mergeFdStreamUsingChars(struct inputStream *pIstream, char *buf, int size);

int readFromStream(struct inputStream *in, char *buffer, int size) {
    if (in->type == 1) {
        return __readFromFdStream(in, buffer, size);
    }
    return 0;
}


void closeStream(struct inputStream *in) {
    if (in->type == 1) {
        __closeFdStream(in);
    }
}

void mergeStreamUsingChars(struct inputStream *in, char *buf, int size) {
    if (in->type == 1) {
        __mergeFdStreamUsingChars(in, buf, size);
    }
}


void initFdStream(struct inputStream *in, int fd) {
    if (in->buf != NULL) {
        puts("memory error");
        exit(-1);
    }

    in->type = 1;
    in->fd = fd;
    in->buf_len = 0;
    in->buf = NULL;
}

// --------------------
// --------------------
// --------------------
// --------------------

int __readFromFdStream(struct inputStream *in, char *buffer, int size) {
    if (in->buf_len != 0) {
        size = minInt(in->buf_len, size);
        memmove(buffer, in->buf, size);
        for (int i = size; i < in->buf_len; i++) {
            in->buf[i - size] = in->buf[i];
        }
        in->buf_len -= size;
        return size;
    }
    return recv(in->fd, buffer, size, 0);
    int readSize = 0;
    while (readSize < size) {
        int newSize = recv(in->fd, buffer + readSize, size, 0);
        readSize += newSize;
        if (newSize == 0) {
            break;
        }
    }
    return readSize;
}

void __closeFdStream(struct inputStream *in) {
    close(in->fd);
    if (in->buf != NULL) {
        delete in->buf;
    }
}

void __mergeFdStreamUsingChars(struct inputStream *in, char *buf, int size) {
    if (in->buf_len != 0) {
        puts("__mergeFdStreamUsingChars ERR ");
        exit(-1);
    }

    in->buf_len = size;
    in->buf = new char[size];
    memcpy(in->buf, buf, size);
}


#endif
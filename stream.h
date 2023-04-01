#ifndef stream
#define stream

#include "utils.h"


struct istream {
    int type; // 1 = fd stream

    int fd;

    int buf_len;
    char *buf;
};


int __readFromFdStream(struct istream *in, char *buffer, int size);

void __closeFdStream(struct istream *in);

void __mergeFdStreamUsingChars(struct istream *pIstream, char *buf, int size);

int readFromStream(struct istream *in, char *buffer, int size) {
    if (in->type == 1) {
        return __readFromFdStream(in, buffer, size);
    }
}


void closeStream(struct istream *in) {
    if (in->type == 1) {
        __closeFdStream(in);
    }
}

void mergeStreamUsingChars(struct istream *in, char *buf, int size) {
    if (in->type == 1) {
        __mergeFdStreamUsingChars(in, buf, size);
    }
}


void initFdStream(struct istream *in, int fd) {
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

int __readFromFdStream(struct istream *in, char *buffer, int size) {
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

void __closeFdStream(struct istream *in) {
    close(in->fd);
    if (in->buf != NULL) {
        free(in->buf);
    }
}

void __mergeFdStreamUsingChars(struct istream *in, char *buf, int size) {
    if (in->buf_len != 0) {
        puts("__mergeFdStreamUsingChars ERR ");
        exit(-1);
    }

    in->buf_len = size;
    in->buf = malloc(size);
    memcpy(in->buf, buf, size);
}


#endif
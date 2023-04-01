#ifndef connect
#define connect

#include "stdio.h"
#include "stream.h"
#include "charutils.h"

volatile long vv = 0;

void doHttp(char head[1024], int len, struct istream in, int outfd) {
    char header[100];
    char databuf[100];

    sprintf(databuf, "Welcome TO PDFS V2.0, We Will Come back Soon, vv = %ld", ++vv);

    sprintf(header, "HTTP/1.1 200 OK\nContent-Length: %lu\n\n", strlen(databuf));
    send(outfd, header, strlen(header), 0);
    send(outfd, databuf, strlen(databuf), 0);
}

void processConnect(void *args) {
//    return;
    char buffer[1024]; // 1KB
    int fd = ((int) args);
    printf("connect fd = %d\n", fd);
    fflush(stdout);

    struct istream in;
    in.buf = NULL;
    initFdStream(&in, fd);

    while (1) {
        int len = readFromStream(&in, buffer, sizeof(buffer));
        if (len == 0) {
            break;
        }
        puts(buffer);

        int find = findIndex(buffer, len, "\n\n", 2);
        if (find == -1) {
            find = findIndex(buffer, len, "\r\n\r\n", 4);
        }

        if (find != -1) {
            mergeStreamUsingChars(&in, buffer + find, len - find);
            doHttp(buffer, find, in, fd);
            break;
        }

    }

    closeStream(&in);
    puts("\n..done");
}


#endif
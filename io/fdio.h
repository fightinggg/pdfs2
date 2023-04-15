#pragma once

#include "../allheader.h"

volatile bool fdstop = false;

bool readBlockFd(int fd, char &ch) {
    return recv(fd, &ch, 1, 0) == 1;
}

bool readFd(int fd, char &ch) {
//    return readBlockFd(fd, ch);
    while (!fdstop) {

        fd_set fdread;
        FD_ZERO(&fdread);
        FD_SET(fd, &fdread);
        timeval timeout{1, 0};
        int ret = select(fd + 1, &fdread, nullptr, nullptr, &timeout);
        if (ret == 0) {
            continue;
        }

        if (ret == -1) {
            puts("readFd error");
            ::fflush(stdout);
            return false;
        }


        long read = recv(fd, &ch, 1, 0);
        if (read == 1) {
            return true;
        } else {
            ::printf("select=%d,fd=%d,read=%ld, nothings to read", ret, fd, read);
            ::fflush(stdout);
            sleep(1);
            continue;
            return false;
        }
    }

    // never here
    return false;
}
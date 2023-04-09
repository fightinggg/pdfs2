#pragma once

#include "../allheader.h"

volatile bool fdstop = false;

bool readFd(int fd, char &ch) {
    for (int i = 0; true; i++) {
        if (fdstop) {
            return false;
        }

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


        if (recv(fd, &ch, 1, 0) == 1) {
            return true;
        } else {
            return false;
        }
    }
}
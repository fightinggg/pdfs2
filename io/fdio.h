#pragma once

#include "../allheader.h"


volatile bool fdStop = false;

bool readFd(int fd, char &ch) {

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    while (true) {
        if (fdStop) {
            return false;
        }
        if (recv(fd, &ch, 1, 0) == 1) {
            return true;
        } else {
            usleep(1000 * 1000);
        }
    }
}
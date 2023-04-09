#pragma once

#include "../allheader.h"


bool readFd(int fd, char &ch) {

    //我们不知道内核的读缓冲区是否有数据到达，所有我们委托内核去检查读缓冲区
    //类似内核去检查的 io多路转发 函数有 eppol select  eppol_wait
    //我这里暂时用select 去检查  他是跨平台的
    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(fd, &fdread);

    int ret = select(fd + 1, &fdread, nullptr, nullptr, nullptr);
    if (ret == -1 && errno == EINTR) { // 信号
        return false;
    }


    if (recv(fd, &ch, 1, 0) == 1) {
        return true;
    } else {
        return false;
    }
}
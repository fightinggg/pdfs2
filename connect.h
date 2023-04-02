#pragma once

#include "allheader.h"
#include "handlerHttp.h"


void *processConnect(void *args) {
    doHandlerHttp(*(int *) (&args));
    pthread_detach(pthread_self());
    return nullptr;
}


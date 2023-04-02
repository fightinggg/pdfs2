#pragma once

#include "allheader.h"
#include "handlerHttp.h"


void *processConnect(void *args) {
    doHandlerHttp(*(int *) (&args));
}


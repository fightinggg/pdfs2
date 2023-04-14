#pragma once

#include "allheader.h"
#include "stream.h"

struct HttpReq {
    string host;
    int port;

    string url;
    string method;
    map<string, string> headers;
    InputStream *body;

    string httpSplit;
};

struct HttpRsp {
    int status;
    map<string, string> headers;
    InputStream *body;
};
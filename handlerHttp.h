#include "allheader.h"
#include "stream.h"
#include "rwapi.h"
#include "handlerHttpApi.h"
#include "httpMsg.h"
#include "stringUtils.h"
#include "io/fdio.h"

bool decodeHttpLine(int fd, HttpReq &req) {
    string reqLine;
    char ch;

    while (true) {
        if (!readFd(fd, ch)) {
            // nothing to read , skip
            break;
        }
        if (ch != '\r' && ch != '\n') {
            reqLine += ch;
        }
        if (ch == '\n') {
            break;
        }
    }

    vector<string> reqlinesplit;
    splitString(reqLine, reqlinesplit, " ");
    if (reqlinesplit.size() != 3) {
        return false;
    }

    req.method = reqlinesplit[0];
    req.url = reqlinesplit[1];
    return true;
}


bool decodeHttpHeaders(int fd, HttpReq &req) {
    // just skip headers
    char ch;
    int size = 0;

    while (true) {
        if (!readFd(fd, ch)) {
            // nothing to read , skip
            return true;
        }
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            size++;
            if (size == 2) {
                return true;
            }
        } else {
            size = 0;
        }
    }
}

bool decodeHttp(int fd, HttpReq &req) {
    if (!decodeHttpLine(fd, req)) {
        return false;
    }
    if (!decodeHttpHeaders(fd, req)) {
        return false;
    }
    req.body = new FdInputStream(fd);
    printf("recv: %s %s\n", req.method.data(), req.url.data());
    return true;
}


void doHandlerHttpSimple(int fd) {
    HttpReq req;
    HttpRsp rsp;
    rsp.body = nullptr;

    if (!decodeHttp(fd, req)) {
        rsp.status = 400;
    } else if (startsWith(req.url, "/api/")) {
        doApi(req, rsp);
    } else {
        doHttpDefault(req, rsp);
    }


    map<int, string> codeString;
    codeString[200] = "ok";
    codeString[400] = "BadRequest";
    codeString[403] = "Forbidden";
    codeString[404] = "NotFound";

    string header = "HTTP/1.0 " + to_string(rsp.status) + " " + codeString[rsp.status] + "\n";
    int bodySize = rsp.body == nullptr ? 0 : rsp.body->size();
    if (bodySize != -1) {
        header += "Content-Length: " + to_string(bodySize) + "\n";
    }
    header += "\n";

    send(fd, header.data(), header.size(), 0);

    if (rsp.body != nullptr) {
        while (true) {
            string data = rsp.body->readNbytes(1024); // 1KB
            if (data.empty()) {
                break;
            }
            send(fd, data.data(), data.size(), 0);
        }
    }

    rsp.body->close();
    printf("delete body:%ld\n",(long) rsp.body);
    delete rsp.body;

    delete req.body;
}

void doHandlerHttp(int fd) {
    doHandlerHttpSimple(fd);
    close(fd);
    printf("close fd=%d\n", fd);
}
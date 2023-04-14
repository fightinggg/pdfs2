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
            puts("decodeHttpLine ERROR");
            break;
        }
        if (ch != '\r' && ch != '\n') {
            reqLine += ch;
        }
        if (ch == '\n') {
            break;
        }
    }

    printf("request line: %s\n", reqLine.data());

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

    string headers;

    while (true) {
        if (!readFd(fd, ch)) {
            // nothing to read , skip
            ::printf("recv header: \n%s\n", headers.data());
            return false;
        }
        headers += ch;
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            size++;
            if (size == 2) {
                break;
            }
        } else {
            size = 0;
        }
    }
    ::printf("recv header: \n%s\n", headers.data());
    fflush(stdout);
    return true;
}

bool decodeHttp(int fd, HttpReq &req) {
    if (!decodeHttpLine(fd, req)) {
        req.body = nullptr;
        return false;
    }
    if (!decodeHttpHeaders(fd, req)) {
        req.body = nullptr;
        return false;
    }
    if (req.method == "GET") {
        req.body = new StringInputStream("");
    } else {
        req.body = new StringInputStream(""); //new FdInputStream(fd, -1); // stoi(req.headers["Contents-Length"])
    }
//    printf("recv: %s %s\n", req.method.data(), req.url.data());
    return true;
}


bool doHandlerHttpSimple(int fd) {
    HttpReq req;
    HttpRsp rsp;
    rsp.body = nullptr;

    if (!decodeHttp(fd, req)) {
        return false;
    } else if (req.method == "OPTIONS" && startsWith(req.url, "/")) {
        // doHttpApiRead(req, rsp);
        rsp.status = 200;
        ::puts("YES");
    } else if (startsWith(req.url, "/api")) {
        doApi(req, rsp);
    } else {
        doHttpDefault(req, rsp);
    }


    map<int, string> codeString;
    codeString[200] = "ok";
    codeString[400] = "BadRequest";
    codeString[403] = "Forbidden";
    codeString[404] = "NotFound";

    string header = "HTTP/1.1 " + to_string(rsp.status) + " " + codeString[rsp.status] + "\n";
    int bodySize = rsp.body == nullptr ? 0 : rsp.body->size();
    if (bodySize != -1) {
        header += "Content-Length: " + to_string(bodySize) + "\n";
    }
    header += "Connection: close\n";

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
        rsp.body->close();
        delete rsp.body;
    }
//    ::fflush(fd);



//    if (req.body != nullptr) {
    delete req.body;
//    }

    return true;
}

void readAll(int fd) {
    char ch;
    int size = 0;
    while (readFd(fd, ch)) {
        ::printf("%c", ch);
        ::fflush(stdout);

        if (ch == '\n') {
            size++;
        } else if (ch == '\r') {
        } else {
            size = 0;
        }

        if (size == 2) {
            string rsp = "HTTP/1.1 200 OK\nContent-Length: 0\nConnection: keep-alive\n\n";
            send(fd, rsp.data(), rsp.size(), 0);
        }
    }
}

void doHandlerHttp(int fd) {
//    readAll(fd);
//    ::puts("END\n");
//    fflush(stdout);
//    close(fd);
//    return;

    while (doHandlerHttpSimple(fd)) {
        fflush(stdout);
//        break;
    }
    fflush(stdout);
    close(fd);
    printf("close fd=%d\n", fd);
    fflush(stdout);
}
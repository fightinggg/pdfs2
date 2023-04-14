#include "allheader.h"
#include "stream.h"
#include "rwapi.h"
#include "handlerHttpApi.h"
#include "httpMsg.h"
#include "stringUtils.h"
#include "io/fdio.h"

bool decodeHttpLine(int fd, HttpReq &req) {
    req.httpSplit = "\n";

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
        if (ch == '\r') {
            req.httpSplit = "\r\n";
            ::printf("httpSplit=\\r\\n\n");
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
    string h2;
    for (auto item: headers) {
        if (item != '\r') {
            h2 += item;
        }
    }
    vector<string> split;
    splitString(h2, split, "\n");
    for (const auto &item: split) {
        vector<string> head;
        int i = item.find(":");
        if (i != -1) {
            req.headers[item.substr(0, i)] = item.substr(i + 1, item.size());
        }
    }
    ::printf("recv header: \n%s\n", h2.data());
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
        string len = req.headers["Content-Length"];
        if (len.empty()) {
            req.body = new StringInputStream("");
        } else {
            int lensize = stoi(len);
            ::printf("size: %s\n", len.data());
            req.body = new FdInputStream(fd, lensize);
            auto data = req.body->readNbytes(lensize);
            printf("%s\n", data.data());
            req.body = new StringInputStream(data);
        }
    }
//    printf("recv: %s %s\n", req.method.data(), req.url.data());
    return true;
}

string a = "123";

bool doHandlerHttpSimple(int fd) {
    HttpReq req;
    HttpRsp rsp;
    rsp.body = nullptr;

    if (!decodeHttp(fd, req)) {
        return false;
    } else if (req.method == "OPTIONS" && startsWith(req.url, "/")) {
        // doHttpApiRead(req, rsp);
        rsp.status = 200;
        rsp.headers["DAV"] = "1, 2";
        rsp.headers["Allow"] = " OPTIONS, LOCK, DELETE, PROPPATCH, COPY, MOVE, UNLOCK, PROPFIND";
        rsp.headers["MS-Author-Via"] = "DAV";
    } else if (req.method == "PROPFIND" && startsWith(req.url, "/")) {
        // doHttpApiRead(req, rsp);
        rsp.status = 207;
        rsp.headers["Content-Type"] = "text/xml;charset=UTF-8";
        rsp.body = new StringInputStream("<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
                                         "<D:multistatus xmlns:D=\"DAV:\">\n"
                                         "<D:response>\n"
                                         "<D:href>/</D:href>\n"
                                         "<D:propstat>\n"
                                         "<D:prop>\n"
                                         "<D:creationdate>2023-04-14T14:18:28Z</D:creationdate>\n"
                                         "<D:displayname><![CDATA[null]]></D:displayname>\n"
                                         "<D:getlastmodified>Fri, 14 Apr 2023 14:18:28 GMT</D:getlastmodified>\n"
                                         "<D:resourcetype>\n"
                                         "<D:collection/>\n"
                                         "</D:resourcetype>\n"
                                         "<D:source></D:source>\n"
                                         "<D:supportedlock><D:lockentry><D:lockscope><D:exclusive/></D:lockscope><D:locktype><D:write/></D:locktype></D:lockentry><D:lockentry><D:lockscope><D:shared/></D:lockscope><D:locktype><D:write/></D:locktype></D:lockentry></D:supportedlock>\n"
                                         "</D:prop>\n"
                                         "<D:status>HTTP/1.1 200 </D:status>\n"
                                         "</D:propstat>\n"
                                         "</D:response>\n"
                                         "<D:response>\n"
                                         "<D:href>/a.txt</D:href>\n"
                                         "<D:propstat>\n"
                                         "<D:prop>\n"
                                         "<D:creationdate>2023-04-14T14:18:28Z</D:creationdate>\n"
                                         "<D:displayname><![CDATA[a.txt]]></D:displayname>\n"
                                         "<D:getlastmodified>Fri, 14 Apr 2023 14:18:28 GMT</D:getlastmodified>\n"
                                         "<D:getcontentlength>11</D:getcontentlength>\n"
                                         "<D:getcontenttype>text/plain</D:getcontenttype>\n"
                                         "<D:getetag>W/\"11-1681481908190\"</D:getetag>\n"
                                         "<D:resourcetype/>\n"
                                         "<D:source></D:source>\n"
                                         "<D:supportedlock><D:lockentry><D:lockscope><D:exclusive/></D:lockscope><D:locktype><D:write/></D:locktype></D:lockentry><D:lockentry><D:lockscope><D:shared/></D:lockscope><D:locktype><D:write/></D:locktype></D:lockentry></D:supportedlock>\n"
                                         "</D:prop>\n"
                                         "<D:status>HTTP/1.1 200 </D:status>\n"
                                         "</D:propstat>\n"
                                         "</D:response>\n"
                                         "</D:multistatus>\n");
    } else if (req.method == "GET" && startsWith(req.url, "/a.txt")) {
        // doHttpApiRead(req, rsp);
        rsp.status = 200;
        rsp.body = new StringInputStream(a);
    } else if (req.method == "LOCK" && startsWith(req.url, "/a.txt")) {
        // doHttpApiRead(req, rsp);
        rsp.status = 200;
        rsp.headers["Content-Type"] = "application/xml; charset=utf-8";
        int t = ::time(0);
        string times = to_string(t);
        rsp.headers["Lock-Token"] = "<" + times + ">";
        rsp.body = new StringInputStream("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                                         "<D:prop xmlns:D=\"DAV:\"><D:lockdiscovery><D:activelock>\n"
                                         "\t<D:locktype><D:write/></D:locktype>\n"
                                         "\t<D:lockscope><D:exclusive/></D:lockscope>\n"
                                         "\t<D:depth>0</D:depth>\n"
                                         "\t<D:owner></D:owner>\n"
                                         "\t<D:timeout>Second-1800</D:timeout>\n"
                                         "\t<D:locktoken><D:href>" + times +
                                         "</D:href></D:locktoken>\n"
                                         "\t<D:lockroot><D:href>a.txt</D:href></D:lockroot>\n"
                                         "</D:activelock></D:lockdiscovery></D:prop>\n");
    } else if (req.method == "PUT" && startsWith(req.url, "/a.txt")) {
        rsp.status = 200;
        a = req.body->readNbytes();
        printf("a=%s", a.data());
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

    string header = "HTTP/1.1 " + to_string(rsp.status) + " " + codeString[rsp.status] + req.httpSplit;
    int bodySize = rsp.body == nullptr ? 0 : rsp.body->size();
    if (bodySize != -1) {
        header += "Content-Length: " + to_string(bodySize) + req.httpSplit;
    }
    header += "Connection: Keep-Alive" + req.httpSplit;
    for (const auto &item: rsp.headers) {
        header += item.first + ": " + item.second + req.httpSplit;
    }

    header += req.httpSplit;

    printf("\n\n===response:\n%s", header.data());
    send(fd, header.data(), header.size(), 0);

    if (rsp.body != nullptr) {
        while (true) {
            string data = rsp.body->readNbytes(1024); // 1KB
            if (data.empty()) {
                break;
            }
            printf("%s", data.data());
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
        break;
    }
    fflush(stdout);
    close(fd);
    printf("close fd=%d\n\n\n", fd);
    fflush(stdout);
}
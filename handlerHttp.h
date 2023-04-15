#include "allheader.h"
#include "stream.h"
#include "rwapi.h"
#include "handlerHttpApi.h"
#include "httpMsg.h"
#include "stringUtils.h"
#include "io/fdio.h"

struct Context {
    bool is100;
    HttpReq lastReq;
};

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
//            ::printf("httpSplit=\\r\\n\n");
        }
        if (ch == '\n') {
            break;
        }
    }

    printf("request line: [%s]\n", reqLine.data());

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
//            ::printf("recv header: \n%s\n", headers.data());
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
            req.headers[item.substr(0, i)] = trim(item.substr(i + 1, item.size()));
        }
    }
//    ::printf("recv header: \n%s\n", h2.data());
//    fflush(stdout);
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

        if (req.headers["Transfer-Encoding"] == "chunked") {
            // chunked 协议
            // Transfer-Encoding: chunked 是 HTTP 协议中用于分块传输编码的一种方式，它允许 HTTP 传输无限长度的数据流而不需要知道实际的内容大小。
            //
            //具体来说，当服务器发送响应时设置 Transfer-Encoding: chunked 头，并将消息体分成多个块。每个块前面都会有一个十六进制数字，
            // 用于指示该块的字节数。最后一个块的大小为0，表示数据已经全部传输完成。客户端在接收到每个块后，解析该块包含的字节数并将其存储
            // 到缓冲区中，直到接收到大小为0的最后一个块为止。
            //
            //使用分块传输编码的好处是可以在传输过程中动态生成和发送数据，而不需要等待整个消息生成完毕才能开始发送，从而提高了 HTTP 数据传输的效率和灵活性。
            req.body = new ChunkInputStream(fd);
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

    }
//    printf("recv: %s %s\n", req.method.data(), req.url.data());
    return true;
}


bool doHandlerHttpSimple(int fd, Context &context) {
    HttpReq req;
    HttpRsp rsp;
    rsp.body = nullptr;

    bool decode;

    if (context.is100) {
        decode = true;
        req = context.lastReq;
        req.body = new ChunkInputStream(fd);
    } else {
        decode = decodeHttp(fd, req);
    }

    if (!decode) {
        return false;
    } else if (req.method == "HEAD" && startsWith(req.url, "/a.txt")) {
        rsp.status = 200;
        rsp.headers["Content-Length"] = to_string(10240000);
        rsp.headers["accept-ranges"] = "bytes";
    } else if (req.method == "GET" && startsWith(req.url, "/a.txt")) {
        rsp.status = 200;
        string range = req.headers["Range"];
        if (range.substr(0, 6) == "bytes=") {
            string rangePair = range.substr(6, range.size());
            vector<string> split;
            splitString(rangePair, split, "-");
            if (split.size() == 2) {
                int start = stoi(split[0]);
                int end = stoi(split[1]);
//                rsp.body = new StringInputStream(a.substr(start, minInt(end - start + 1, (int) a.size())));
                rsp.body = read(start, end);
            }
        }

    } else if (req.method == "PUT" && startsWith(req.url, "/a.txt")) {
//        if (req.headers["Expect"] == "100-continue") {
//            req.headers.erase("Expect");
//            // Expect: 100-continue 是 HTTP 协议中的一个头部，用于告诉服务器在客户端发送实体正文（例如POST或PUT请求）之前，需要先进行预检。
//            // 当客户端发送带有 Expect: 100-continue 头部的请求时，服务器将返回一个 HTTP/1.1 100 Continue 状态码表示可以继续发送实体正文
//            // 。如果服务器不支持 Expect: 100-continue，会返回一个 417 Expectation Failed 状态码。
//            //
//            //使用 Expect: 100-continue 头部的主要目的是为了防止客户端发送大量数据而服务器无法处理的情况。通过进行预检，客户端可以确保服务器能够接受并处理其发送的数据，从而避免浪费网络资源和时间。
//            //
//            //需要注意的是，并非所有服务器都支持 Expect: 100-continue 头部，因此应该谨慎使用它。如果服务器不支持预检，则客户端可能需要等待超时时间才能收到响应，这会影响请求的响应时间。
//            rsp.status = 200;
//            context.is100 = true;
//            context.lastReq = req;
//        } else
        {
            string data100 = "HTTP/1.1 100\n\n";
            send(fd, data100.data(), data100.size(), 0);

            rsp.status = 200;
            string contentRange = req.headers["Content-Range"];
            if (contentRange.substr(0, 6) == "bytes ") {
                contentRange = contentRange.substr(6, contentRange.size());
                vector<string> split;
                splitString(contentRange, split, "/");
                if (split.size() == 2) {
                    contentRange = split[0];
                    splitString(contentRange, split, "-");
                    if (split.size() == 2) {
                        int start = max(0, stoi(split[0]));
                        int end = stoi(split[1]);

//                        string allData = req.body->readNbytes();

                        write(start, end, req.body);

//                        ::printf("all body:%zu\n", allData.size());
//                        fflush(stdout);

                        rsp.status = 200;
//                        context.is100 = false;
//                        context.lastReq = req;
                    }
                }
            }
        }


    } else if (req.method == "GET" && startsWith(req.url, "/read")) {
        rsp.status = 200;
        vector<string> split;
        string pair = req.url.substr(6);
        splitString(pair, split, "/");
        if (split.size() == 2) {
            int start = stoi(split[0]);
            int end = stoi(split[1]);
            end = minInt(end, start + 10240);
            rsp.body = read(start, end);
        } else {
            rsp.body = read(0, 10240);
        }
    } else {
        doHttpDefault(req, rsp);
    }


    map<int, string> codeString;
    codeString[100] = "Continue";
    codeString[200] = "ok";
    codeString[400] = "BadRequest";
    codeString[403] = "Forbidden";
    codeString[404] = "NotFound";
    codeString[417] = "Expectation Failed";

    string resline = "HTTP/1.1 " + to_string(rsp.status) + " " + codeString[rsp.status] + req.httpSplit;
    string header = resline;
    int bodySize = rsp.body == nullptr ? -1 : rsp.body->size();
    if (bodySize != -1) {
        header += "Content-Length: " + to_string(bodySize) + req.httpSplit;
    }
//        header += "Connection: Close" + req.httpSplit;
    header += "Connection: Keep-Alive" + req.httpSplit;
    for (const auto &item: rsp.headers) {
        header += item.first + ": " + item.second + req.httpSplit;
    }

    header += req.httpSplit;

    printf("res: %s", resline.data());
    send(fd, header.data(), header.size(), 0);

    long sendSize = 0;

    if (rsp.body != nullptr) {
        while (true) {
            string data = rsp.body->readNbytes(1024); // 1KB
            if (data.empty()) {
                break;
            }
            sendSize += data.size();
            send(fd, data.data(), data.size(), 0);
        }

        rsp.body->close();
        delete rsp.body;
    }
//    ::fflush(fd);



//    if (req.body != nullptr) {
    delete req.body;
//    }

    printf("send %ld bytes\n", sendSize);
    if (sendSize != bodySize && bodySize != -1) {
        return false;
    }

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

    }
}

void doHandlerHttp(int fd) {
//    readAll(fd);
//    ::puts("END\n");
//    fflush(stdout);
//    close(fd);
//    return;

    Context context;
    context.is100 = false;
    for (int i = 0; i < 10; i++) {
        if (doHandlerHttpSimple(fd, context)) {
            fflush(stdout);
        } else {
            fflush(stdout);
            break;
        }

//        if (context.is100) {
//            readAll(fd);
//            break;
//        }
    }

//    string rsp = "HTTP/1.1 200 OK\nContent-Length: 0\nConnection: keep-alive\n\n";
//    send(fd, rsp.data(), rsp.size(), 0);
//    readAll(fd);

    close(fd);
    printf("close fd=%d\n\n\n", fd);
    fflush(stdout);
}
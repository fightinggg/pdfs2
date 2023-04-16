#include "allheader.h"
#include "stream.h"
#include "rwapi.h"
#include "handlerHttpApi.h"
#include "httpMsg.h"
#include "stringUtils.h"
#include "io/fdio.h"
#include "inputstreams/BinaryStringInputStream.h"

struct Context {
    bool is100;
    HttpReq lastReq;
};


bool doHandlerHttpSimple(int fd, Context &context) {
    HttpReq req;
    HttpRsp rsp;
    rsp.body = nullptr;

    bool decode;

    if (context.is100) {
        decode = true;
        req = context.lastReq;
        req.body = shared_ptr<InputStream>(new ChunkInputStream(shared_ptr<InputStream>(new FdInputStream(fd))));
    } else {
        decode = decodeReqHttp(shared_ptr<InputStream>(new FdInputStream(fd)), req);
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

                        write(start, end - 1, req.body);

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
        string pair = req.url.substr(minInt(6, req.url.size()));
        splitString(pair, split, "/");
        if (split.size() == 2) {
            int start = stoi(split[0]);
            int end = stoi(split[1]);
            end = minInt(end, start + 10240);
            rsp.body = read(start, end);
        } else {
            rsp.body = read(0, 10240);
        }
//        static string vv = "";
//        vv += to_string(rand() % 2);
//        rsp.body = new StringInputStream(vv);
//        rsp.body = new StringInputStream(randomBinaryString(1024));
//        int size = rsp.body->size();
//        rsp.body = new Base64EncoderInputStream(rsp.body, size);
//        rsp.body = new Base64DecoderInputStream(rsp.body, size);
        rsp.body = shared_ptr<InputStream>(new BinaryStringInputStream(shared_ptr<InputStream>(rsp.body)));
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

//    printf("res: %s", resline.data());
    send(fd, header.data(), header.size(), 0);

    long sendSize = 0;

    if (rsp.body != nullptr) {
        while (true) {
            string data = rsp.body->readNbytes(1024); // 1KB
            if (data.empty()) {
                break;
            }
            sendSize += data.size();
            if (send(fd, data.data(), data.size(), 0) == -1) {
                rsp.body->close();
                printf("SEND ERROR\n");
                return false;
            }
        }

        rsp.body->close();
    }


    if (sendSize != bodySize && bodySize != -1) {
        printf("send %ld bytes, need send %d bytes\n", sendSize, bodySize);
        return false;
    } else {
        printf("send %ld bytes\n", sendSize);
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
    Context context;
    context.is100 = false;
    for (int i = 0; i < 10; i++) {
        if (doHandlerHttpSimple(fd, context)) {
            fflush(stdout);
        } else {
            fflush(stdout);
            break;
        }
    }

    close(fd);
    printf("close fd=%d\n\n\n", fd);
    fflush(stdout);
}
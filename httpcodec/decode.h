#pragma once

#include "../stream.h"
#include "../stringUtils.h"
#include "../httpMsg.h"

bool decodeHttpReqLine(shared_ptr<InputStream> in, string &httpSplit, string &method, string &url) {
    httpSplit = "";

    string reqLine;
    char ch;

    while (true) {
        if (!in->read(&ch)) {
            // nothing to read , skip
            puts("decodeHttpLine ERROR");
            break;
        }
        if (ch != '\r' && ch != '\n') {
            reqLine += ch;
        }
        if (ch == '\r') {
            httpSplit = "\r\n";
        }
        if (ch == '\n') {
            break;
        }
    }
    if (httpSplit.size() != 2) {
        httpSplit = "\n";
    }

//    printf("request line: [%s]\n", reqLine.data());

    vector<string> reqlinesplit;
    splitString(reqLine, reqlinesplit, " ");
    if (reqlinesplit.size() != 3) {
        return false;
    }

    method = reqlinesplit[0];
    url = reqlinesplit[1];
    return true;
}

bool decodeHttpRspLine(shared_ptr<InputStream> in, string &httpSplit, int &status) {
    httpSplit = "";

    string reqLine;
    char ch;

    while (true) {
        if (!in->read(&ch)) {
            // nothing to read , skip
            puts("decodeHttpLine ERROR");
            break;
        }
        if (ch != '\r' && ch != '\n') {
            reqLine += ch;
        }
        if (ch == '\r') {
            httpSplit = "\r\n";
        }
        if (ch == '\n') {
            break;
        }
    }
    if (httpSplit.size() != 2) {
        httpSplit = "\n";
    }

//    printf("rsp line: [%s]\n", reqLine.data());

    vector<string> reqlinesplit;
    splitString(reqLine, reqlinesplit, " ");

    status = stoi(reqlinesplit[1]);
    return true;
}


bool decodeHttpHeaders(shared_ptr<InputStream> in, map<string, string> &header) {
    // just skip headers
    char ch;
    int size = 0;

    string headers;

    while (true) {
        if (!in->read(&ch)) {
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
            header[item.substr(0, i)] = trim(item.substr(i + 1, item.size()));
        }
    }
    return true;
}

bool decodeReqHttp(shared_ptr<InputStream> in, HttpReq &req) {
    if (!decodeHttpReqLine(in, req.httpSplit, req.method, req.url)) {
        req.body = nullptr;
        return false;
    }
    if (!decodeHttpHeaders(in, req.headers)) {
        req.body = nullptr;
        return false;
    }
    if (req.method == "GET") {
        req.body = shared_ptr<InputStream>(new StringInputStream(""));
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
            req.body = shared_ptr<InputStream>(new ChunkInputStream(in));
        } else {

            string len = req.headers["Content-Length"];

            if (len.empty()) {
                req.body = shared_ptr<InputStream>(new StringInputStream(""));
            } else {
                int lensize = stoi(len);
                ::printf("size: %s\n", len.data());
                req.body = in;
                auto data = req.body->readNbytes(lensize);
                printf("%s\n", data.data());
                req.body = shared_ptr<InputStream>(new StringInputStream(data));
            }
        }

    }
    return true;
}


bool
decodeHttpRsp(shared_ptr<InputStream> in, int &status, map<string, string> &headers, shared_ptr<InputStream> &body) {
    string httpSplit;
    if (!decodeHttpRspLine(in, httpSplit, status)) {
        body = in;
        return false;
    }
    if (!decodeHttpHeaders(in, headers)) {
        body = in;
        return false;
    }
    body = in;
    return true;
}


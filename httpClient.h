#include "allheader.h"
#include "httpMsg.h"
#include "threadpool/threadpool.h"
#include <curl/curl.h>

static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    char *contentsChar = (char *) contents;
    auto body = (BlockQueueInputStream *) userp;
    for (int i = 0; i < size * nmemb; i++) {
        body->push(contentsChar[i]);
    }
//    printf("size=%d\n", (int) realsize);
    fflush(stdout);
    return realsize;
};


static size_t headerCallback(char *buffer, size_t size, size_t nitems, void *userdata) {
//    printf("headerCallback?");
//    fflush(stdout);


    size_t nbytes = size * nitems;
//    ::printf("%s", string(buffer, nbytes).data());
//    fflush(stdout);

    return nbytes;
}


bool httpsRequest(HttpReq &req, HttpRsp &rsp) {
    rsp.body = new BlockQueueInputStream();


    struct Node {
        string url;
        InputStream *rspdata{};
        map<string, string> reqheaders;
    };

    Node *node = new Node;
    node->url = "https://" + req.host + ":" + to_string(req.port) + req.url;
    node->rspdata = rsp.body;
    node->reqheaders = req.headers;

    // req and rsp maybe remove
    submit([](void *args) -> void * {
        auto node = (Node *) args;

        auto body = (BlockQueueInputStream *) node->rspdata;
        auto url = node->url;
        auto reqheaders = node->reqheaders;

        puts(url.data());

        delete node;

        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if (curl) {

            struct curl_slist *chunk = nullptr;
            for (const auto &item: reqheaders) {
                auto headLine = item.first + ":" + item.second;
                chunk = curl_slist_append(chunk, headLine.data());
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.data());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);


            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, body);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

            res = curl_easy_perform(curl);


            if (res != CURLE_OK) {
                printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            body->closePush();
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        return nullptr;
    }, node);

    return true;
}


bool dnsParse(string host, string &parse) {
    struct addrinfo hints{}, *res;
    int status;
    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // 使用IPv4协议
    hints.ai_socktype = SOCK_STREAM; // 使用TCP协议

    if ((status = getaddrinfo(host.data(), nullptr, &hints, &res)) != 0) { // 获取地址信息
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return false;
    }

//    printf("IP addresses for %s:\n\n", host.data());

    for (struct addrinfo *p = res; p != nullptr; p = p->ai_next) {
        void *addr;
        auto *ipv4 = (struct sockaddr_in *) p->ai_addr;
        addr = &(ipv4->sin_addr);

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr); // 将网络字节序的IP地址转换为字符串形式
//        printf("%s\n", ipstr);
        parse = ipstr;
        break;
    }

    freeaddrinfo(res); // 释放getaddrinfo()函数返回的链表内存空间
    return true;
}

bool httpRequest(const HttpReq &req, HttpRsp &rsp) {
    printf("httprequest: host:%s,port:%d,method:%s,url:%s\n",
           req.host.data(), req.port, req.method.data(), req.url.data());
    string host;
    if (!dnsParse(req.host, host)) {
        printf("DNS ERROR");
        return false;
    }
    struct sockaddr_in server{};
    // 创建socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
        return false;
    }
    // 指定GitHub服务器地址和端口号
    if (inet_pton(AF_INET, host.data(), &server.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return false;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(req.port); // HTTP默认端口号为80
    // 连接GitHub服务器
    if (connect(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Connect failed");
        return false;
    }
    // 构造HTTP请求消息
    string requestLine = req.method + " " + req.url + " HTTP/1.1\n";
    string message = requestLine + "Host: github.com\nConnection: close\n\n";
    // 发送HTTP请求消息
    if (send(socket_desc, message.data(), message.size(), 0) < 0) {
        puts("Send failed");
        return false;
    }
    // 接收GitHub服务器响应消息
    rsp.body = new FdInputStream(socket_desc);
    return true;
}

#include "allheader.h"
#include "httpMsg.h"

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

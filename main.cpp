#include <fcntl.h>
#include "allheader.h"
#include "connect.h"
#include "threadpool/threadpool.h"

using namespace std;

static void stop_handler(int sig) { // can be called asynchronously
    puts("\nplease wait..");
    fdstop = true;
}

static void SIGPIPE_handler(int sig) { // can be called asynchronously
    puts("\nSIGPIPE");
}



int main(int argc, char **args) {

//    testBase64();

    threadinit();

    signal(SIGINT, stop_handler);
    signal(SIGPIPE, SIG_IGN);
//    signal(SIGPIPE, SIGPIPE_handler);

    initPdfsSystem(argc, args);


//    write(3, 13, new StringInputStream(string(11, '2')));


    srand(time(0));

    int port = 8050 + (::rand() % 100);
    port = 8080;

    ///定义sockfd
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr{};
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);


    // SO_REUSEADDR 的缺点是什么？
    // SO_REUSEADDR选项可以允许套接字在关闭后立即重新绑定相同的IP地址和端口号，而不管该端口的状态是否处于TIME_WAIT状态，避免了"Address already in use"错误。但是，使用SO_REUSEADDR选项也有以下缺点：
    //
    //可能导致数据包重复传输：由于TCP的TIME_WAIT状态存在的意义是等待所有尚未接收到的数据到达，因此忽略TIME_WAIT状态可能会导致在某些情况下重复发送数据包和重复接收数据包。这可能会导致数据包丢失或重复，影响通信的可靠性。
    //可能会导致安全问题：如果在关闭连接后立即重用其端口，则可能会导致已经建立的连接被其他进程/线程替代，并容易受到中间人攻击等安全问题的影响。
    //不适用于多个客户端/服务器应用程序：SO_REUSEADDR选项只允许一个进程/线程监听特定的IP地址和端口号，这限制了多个客户端和服务器应用程序在同一主机上运行的有效性，因为它们不能共享相同的端口。
    //因此，在一些情况下，使用SO_REUSEADDR选项可能会带来潜在的风险和限制。程序员需要仔细考虑相关的安全性和实现细节，在确保系统稳定性和安全性的前提下选择适当的套接字选项。
    int reuse = 1;
    setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    ///bind，成功返回0，出错返回-1
    if (bind(server_sockfd, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) == -1) {
        port++;
        server_sockaddr.sin_port = htons(port);
    }

    ///listen，成功返回0，出错返回-1
    if (listen(server_sockfd, 20) == -1) {
        perror("listen");
        exit(1);
    }

    printf("PDFS listen at port %d\n", port);
    fflush(stdout);

    while (true) {
        // select
        fd_set fdread;
        FD_ZERO(&fdread);
        FD_SET(server_sockfd, &fdread);

        // select成功时返回就绪（可读、可写和异常）文件描述符的总数。
        // 如果在超时时间内没有任何文件描述符就绪，select将返回0。
        // select失败时返回-1。
        // 如果在select等待期间，程序接收到信号，则select立即返回-1，并设置errno为EINTR。
        int ret = select(server_sockfd + 1, &fdread, nullptr, nullptr, nullptr);
        if (ret == -1 && errno == EINTR) { //信号
            ::puts("STOP accept new connecting");
            fflush(stdout);
            break;
        }
        if (ret == 0) {
            ::puts("accept select ret == 0");
            fflush(stdout);
            break;
        }
        // -1 on error 1 on something to read

        ///客户端套接字
        struct sockaddr_in client_addr{};
        socklen_t length = sizeof(client_addr);
        int fd = accept(server_sockfd, (struct sockaddr *) &client_addr, &length);
        if (fd == -1) {
            perror("connect");
            break;
        }
        printf("connect to fd %d\n", fd);
        fflush(stdout);
        submit(processConnect, (void *) (long(fd)));
    }
    threadstopAndjoin();

    close(server_sockfd);
    puts("\nbye bye\n");

}


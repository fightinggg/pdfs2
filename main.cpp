#include <fcntl.h>
#include "allheader.h"
#include "connect.h"
#include "threadpool/threadpool.h"

using namespace std;

volatile sig_atomic_t stop = 0;

static void stop_handler(int sig) { // can be called asynchronously
    puts("\nplease wait..");
    stop = 1; // set flag
    fdStop = true;
}


int main() {
    threadinit();

    signal(SIGINT, stop_handler);

    initPdfsSystem();


    int port = 8080;

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

    ///客户端套接字
    struct sockaddr_in client_addr{};
    socklen_t length = sizeof(client_addr);

    // 非阻塞
    //第一个参数server_sockfd是要获取标志的套接字文件描述符。
    //第二个参数F_GETFL表示获取文件描述符标志位。
    //第三个参数0表示不需要修改标志位。
    int flags = fcntl(server_sockfd, F_GETFL, 0);
    fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK);

    while (true) {
//        printf("accept");
//        ::fflush(stdout);
        ///成功返回非负描述字，出错返回-1
        int fd = accept(server_sockfd, (struct sockaddr *) &client_addr, &length);
        if (fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 没有客户端连接，请继续轮询
                if (stop) {
                    printf("stop accept new connecting\n");
                    fflush(stdout);
                    break;
                } else {
                    // wait 100ms
                    usleep(100 * 000);
                    continue;
                }
            } else {
                // 其他错误，处理失败情况
                perror("connect");
                break;
            }
        }
        printf("connect to fd %d\n", fd);
        fflush(stdout);
        submit(processConnect, (void *) (long(fd)));
    }
    threadstopAndjoin();

    int closeRet = close(server_sockfd);
    printf("close: %d\n", closeRet);
    puts("\nbye bye\n");
    ::fflush(stdout);


}


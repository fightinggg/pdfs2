#include "allheader.h"
#include "connect.h"

using namespace std;

volatile sig_atomic_t stop = 0;

static void stop_handler(int sig) { // can be called asynchronously
    puts("\nbye bye");
    exit(0);
    stop = 1; // set flag
}

int main() {
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

    ///bind，成功返回0，出错返回-1

    for (int i = 0; i < 10; i++) {
        if (bind(server_sockfd, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) == -1) {
            port++;
            server_sockaddr.sin_port = htons(port);
            if (i == 9) {
                perror("bind");
                exit(1);
            }
        } else {
            break;
        }
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


    while (stop != 1) {
        ///成功返回非负描述字，出错返回-1
        printf("accept:\n");
        int fd = accept(server_sockfd, (struct sockaddr *) &client_addr, &length);
        if (fd < 0) {
            perror("connect");
            break;
        }
        printf("connect to fd %d\n", fd);
        fflush(stdout);
        pthread_t th1;
        pthread_create(&th1, NULL, processConnect, (void *) (fd));
//        pthread_join(th1, NULL);

    }

    close(server_sockfd);
}


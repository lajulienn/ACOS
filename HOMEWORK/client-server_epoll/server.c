#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <utility>

#define MAX_EVENTS 10

void setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    std::map<int, FILE *> conn;
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3000);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(listen_sock, (struct sockaddr *)(&addr), sizeof(addr))) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(listen_sock, 10)) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    int epollfd = epoll_create(2);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;  
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev)) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev)) {
        perror("epoll_ctl: stdin");
        exit(EXIT_FAILURE);
    }
    struct epoll_event events[MAX_EVENTS];
    int conn_id = 0;
    while (1) {
        int events_num = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < events_num; i++) {
            if (events[i].data.fd == listen_sock) {
                printf("ready to accept!\n");
                struct sockaddr local;
                socklen_t addrlen;
                int conn_sock = accept(listen_sock, &local, &addrlen);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                char filename[256];
                sprintf(filename, "%d", conn_id);
                FILE *fd = fopen(filename, "w");

                setnonblocking(conn_sock);
                ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev)) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
                conn.insert(std::make_pair(conn_sock, fd));
                ++conn_id;
            } else if (events[i].data.fd == STDIN_FILENO) {
                char *line = NULL;
                size_t len = 0;
                size_t read = getline(&line, &len, stdin);
                if (read <= 0)
                    break;
                for (auto i : conn) {
                    send(i.first, line, read, 0);
                } 
            } else {
                if (events[i].events & EPOLLIN) {
                    char buf[128] = {0};
                    read(events[i].data.fd, buf, sizeof(buf) - 1);
                    fprintf(conn[events[i].data.fd], "%s", buf);
                    printf("%s", buf);
                } 
                if (events[i].events & EPOLLRDHUP) {
                    
                    fclose(conn[events[i].data.fd]);
                }
            }
        }
    }
    close(epollfd);
    return 0;
}

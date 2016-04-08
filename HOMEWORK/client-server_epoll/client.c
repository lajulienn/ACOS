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
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		fprintf(stderr, "Failed to create socket./n");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Connection failed.\n");
		return 1;
	}
	setnonblocking(sock);

    int epollfd = epoll_create(2);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock;  
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev)) {
        perror("epoll_ctl: sock");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev)) {
        perror("epoll_ctl: STDIN_FILENO");
        exit(EXIT_FAILURE);
    }

    struct epoll_event events[MAX_EVENTS];
	while (1) {
		int events_num = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		for (int i = 0; i < events_num; ++i) {
			if (events[i].data.fd == sock) {
				char buf[128] = {0};
                read(events[i].data.fd, buf, sizeof(buf) - 1);
                printf("%s", buf);
			} else {
				char *line = NULL;
                size_t len = 0;
                size_t read = getline(&line, &len, stdin);
                if (read < 0)
                    break;
				send(sock, line, read, 0);
			}
		}
	} 

	close(sock);

	return 0;
}
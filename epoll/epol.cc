#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/epoll.h>
#define MAX_EVENTS 10

int main() {
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5000);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if (bind(listen_sock, ()&addr, sizeof(addr))) {
		perror("bind_fail\n");
		exit(EXIT_FAILURE);
	}

	int epollfd = epoll_create(2);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
	perror("epoll_ctl: listen_sock");
	exit(EXIT_FAILURE);
	}

	struct epoll_event events[10];

	while(true) {
		int events_num = epoll_wait(epollfd, &ev, MAX_EVENTS, -1);
		for (int i = 0; i < events_num; ++i){
			if (events[i].data.fd == listen_sock){
				printf("ready to accept\n");
				//accept(listen_sock();)
				break;
			}
		}
	}
	close(epollfd);
	return 0;
}
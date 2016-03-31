#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#include <pthread.h>
#include <stdio.h>

void *entry(void *arg) {
	for (int i = 0; i < 1000; ++i) {
		printf("Inside thread %d: %p\n", pthread_self(), arg);
	}
}

int main() {
	int sock, listener;
	struct sockaddr_in addr;
	char buf[256];
	int bytes_read;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		printf("Failed to create listener.");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		printf("Failed to bind.\n");
		return 1;
	}

	listen(listener, 1);

	while (1) {
		sock = accept(listener, NULL, NULL);
		if (sock < 0) {
			printf("Failed to accept.");
			return 1;
		}

		pthread_t thread;
		thread = pthread_create(&thread, NULL, entry, NULL);

		bytes_read = recv(sock, buf, 256, 0);
		if (bytes_read <=0) 
			break;
		printf("%s\n", buf);

		close(sock);
	}
		close(listener);

	return 0;
}
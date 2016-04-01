#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

void *thread_func(int *sock) {
	FILE *fd = fopen("log", "a");
	char c;

	while (1) {
		int bytes_read = recv(*sock, &c, sizeof(char), 0);
		//printf("read %d bytes, %c\n", bytes_read, c);
		if (bytes_read <= 0) {
			//printf("break\n");
			break;
		}
		fputc(c, fd);
	}

	fclose(fd);
	close(*sock);
	free(sock);
}

int main() {
	int listener;
	struct sockaddr_in addr;
	char buf[256];
	int bytes_read;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		fprintf(stderr, "Failed to create listener.");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Failed to bind.\n");
		return 1;	
	}

	if(listen(listener, 1) == -1) {
		fprintf(stderr, "Failed to listen.\n");
		return 1;
	}

	while (1) {
		int *sock = malloc(sizeof(int));
		*sock = accept(listener, NULL, NULL);
		if (sock < 0) {
			fprintf(stderr, "Failed to accept.");
			return 1;
		}
		pthread_t thread;
		thread = pthread_create(&thread, NULL, (void *)thread_func, sock);
	}

	return 0;
}
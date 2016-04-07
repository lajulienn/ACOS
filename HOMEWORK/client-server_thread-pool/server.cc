#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <list>

#define THREAD_N 20

struct connection {
	int socket;
	int id;
};

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
std::list<struct connection *> queue;

void *thread_func(void *) {
	while (1) {
		pthread_mutex_lock(&mutex);
		if (!queue.size()) { //not while but if
    		assert(!pthread_cond_wait(&cond, &mutex));
    	}
    	struct connection *conn = queue.front();
    	queue.pop_front();
    	pthread_mutex_unlock(&mutex);

    	if (!conn->socket) {
    		break;
    	}

    	char filename[256];
		sprintf(filename, "%d", conn->id);
		FILE *fd = fopen(filename, "w");
		char c;

		while (1) {
		int bytes_read = recv(conn->socket, &c, sizeof(char), 0);
		printf("read %d bytes, %c\n", bytes_read, c);
		if (bytes_read <= 0) {
			//printf("break\n");
			break;
		}
		fputc(c, fd);
		}

		fclose(fd);
		close(conn->socket);
		free(conn);
	}
	return NULL;
}

int main() {
	int listener;
	struct sockaddr_in addr;
	char buf[256];
	int bytes_read;
	int conn_id = 0;

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

	pthread_t *pool = new pthread_t[THREAD_N];
	for (int i = 0; i < THREAD_N; i++) {
		pthread_create(pool + i, NULL, thread_func, NULL);
	}

	while (1) {
		struct connection *conn = (struct connection *)malloc(sizeof(struct connection));
		conn->socket = accept(listener, NULL, NULL);
		if (conn->socket < 0) {
			fprintf(stderr, "Failed to accept.");
			return 1;
		}

		pthread_mutex_lock(&mutex);
		queue.push_back(conn);
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);

		conn->id = conn_id;
		++conn_id;
	}
	for (int i = 0; i < THREAD_N; i++) {
		pthread_join(pool[i], NULL);
	}

	return 0;
}
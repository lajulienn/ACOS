#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

struct connection {
	int socket;
	int id;
};

int main() {
	int listener;
	struct connection conn;
	struct sockaddr_in addr;
	char buf[256];
	int bytes_read;
	int conn_id;

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

	conn_id = 0;
	while (1) {
		conn.socket = accept(listener, NULL, NULL);
		if (conn.socket < 0) {
			fprintf(stderr, "Failed to accept.");
			return 1;
		}

		conn.id = conn_id;

		int id = fork();

		if (id == 0) { //in child
		char filename[256];
		sprintf(filename, "%d", conn.id);
		FILE *fd = fopen(filename, "w");
			while (1) {
				char c;
				bytes_read = recv(conn.socket, &c, sizeof(char), 0);
				//printf("read %d bytes, %c\n", bytes_read, c);
				if (bytes_read <= 0) {
					//printf("break\n");
					break;
				}
				fputc(c, fd);
			}
			fclose(fd);
			close(conn.socket);
			return 0;
		} else {
			++conn_id;
			continue;
		}
	}
	return 0;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

char message[256];

int main() {
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		printf("Failed to create socket./n");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3000);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		printf("Connection failed.\n");
		return 1;
	}

	while (1) {
	printf(">");
	scanf("%s", message);
	send(sock, message, sizeof(message), 0);
	}

	close(sock);

	return 0;
}
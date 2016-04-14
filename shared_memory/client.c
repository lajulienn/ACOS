#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#define SHMEM_PATH "/my_shmem"
// 64 KiB
#define SHMEM_SIZE (1 << 16)

int main() {
  	int shmem_fd = shm_open(SHMEM_PATH, O_RDWR | O_CREAT, 0666);
  	if (shmem_fd < 0) {
    	perror("shm_open");
    	exit(1);
  	}

   	int res = ftruncate(shmem_fd, SHMEM_SIZE);
   	if (res) {
    	perror("ftruncate");
    	exit(1);
   	}

   	void *ptr = mmap(NULL, SHMEM_SIZE,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED, shmem_fd, 0);
  	if (!ptr) {
    	perror("mmap");
    	exit(1);
  	}

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

	int value = *(int*)ptr;
	printf("Current value in our shared memory: %d\n", value);

	printf("Please, enter a new value you want to write to shared memory\nand your message to server.\n");
	scanf("%d", &value);
	//changing value
  	*(int*)ptr = value;

	value = *(int*)ptr;
	printf("New value in our shared memory: %d\n", value);
	//printf("Enter your message to server:\n");

	char buf[256];

	//sprintf(buf, "New value: %d", value);
	//send(sock, buf, sizeof(buf), 0);


	fgets(buf, sizeof(buf) / sizeof(char), stdin);
	send(sock, buf, sizeof(buf), 0);

	//res = fscanf(stdin, "%s", buf);
	//printf("Recieved. %d\n", res);
	//char s = fgetc(stdin);
	//if (fgets(buf, sizeof(buf) / sizeof(char), stdin) == NULL) {
	//	printf("No mesage send.\n");
	//}
	//send(sock, buf, sizeof(buf), 0);
	//printf("send\n");

	close(sock);

	return 0;
}
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

  	*(int*)ptr = 42;

	int sock, listener;
	struct sockaddr_in addr;
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
		sock = accept(listener, NULL, NULL);
		if (sock == -1) {
			fprintf(stderr, "Failed to accept.");
			return 1;
		}

	 	FILE *fd = fopen("log", "a");
		char c;

		int value = *(int*)ptr;
		while (1) {
			printf("Current value = %d\n", value);
			char buf[256];
			bytes_read = recv(sock, buf, sizeof(buf) / sizeof(char), 0);
			//printf("read %d bytes, %c\n", bytes_read, c);
			if (bytes_read <= 0) {
					//printf("break\n");
				break;
			}
			fprintf(fd, "%s", buf);
			printf("%s", buf);
			//fputc(c, fd);
			value = *(int*)ptr;
		}

		fclose(fd);
		close(sock);
	}

	res = close(shmem_fd);
  	if (res) {
    	perror("close");
    	exit(1);
  	}

  	res = shm_unlink(SHMEM_PATH);
	if (res) {
	 	perror("shm_unlink");
	  	exit(1);
	}

	return 0;
}
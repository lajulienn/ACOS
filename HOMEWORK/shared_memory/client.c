#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/un.h>

#define SHMEM_PATH "/my_shmem"
// 64 KiB
#define SHMEM_SIZE (1 << 16)

#define SERVER_PATH "server_path"

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
	struct sockaddr_un addr;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket < 0) {
		fprintf(stderr, "Failed to create socket./n");
		return 1;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERVER_PATH);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "Connection failed.\n");
		return 1;
	}

	/////
	while (1) {
		printf("*****\nIf you want to close server enter 'stop'.\nIf you want to close only client enter 'exit'.\nElse enter 'no'.\n*****\n");

		char buf[256];
		//char answer[5];
		fgets(buf, sizeof(buf) / sizeof(char) - 1, stdin);
		buf[strcspn(buf, "\n")] = '\0';
		if (strcmp(buf, "stop") == 0) {
			send(sock, buf, sizeof(buf), 0);
		
			goto end;

		} else if (strcmp(buf, "exit") == 0) {

			goto end;

		} else {
			int value = *(int*)ptr;
			printf("Current value in our shared memory: %d\n", value);

			printf("Please, enter a new value you want to write to shared memory\nand your message to server divided by a space.\n");
			scanf("%d", &value);
			//changing value
	  		*(int*)ptr = value;

			value = *(int*)ptr;
			printf("New value in our shared memory: %d\n", value);
			//printf("Enter your message to server:\n");


			fgets(buf, sizeof(buf) / sizeof(char), stdin);
			send(sock, buf, sizeof(buf), 0);
		}
	} // while

	//res = fscanf(stdin, "%s", buf);
	//printf("Recieved. %d\n", res);
	//char s = fgetc(stdin);
	//if (fgets(buf, sizeof(buf) / sizeof(char), stdin) == NULL) {
	//	printf("No mesage send.\n");
	//}
	//send(sock, buf, sizeof(buf), 0);
	//printf("send\n");

	end:
	close(sock);

	res = close(shmem_fd);
  	if (res) {
    	perror("close");
    	exit(1);
  	}

  	/*res = shm_unlink(SHMEM_PATH);
	if (res) {
	 	perror("shm_unlink");
	  	exit(1);
	}*/

	return 0;
}
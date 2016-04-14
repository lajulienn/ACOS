#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define SHMEM_PATH "/my_shmem"
// 64 KiB
#define SHMEM_SIZE (1 << 16)

int main(int argc, const char** argv) {
  int res, shmem_fd;
  void *ptr;
  shmem_fd = shm_open(SHMEM_PATH, O_RDWR | O_CREAT, 0666);
  if (shmem_fd < 0) {
    perror("shm_open");
    exit(1);
  }

  if (argc == 1) {
    res = ftruncate(shmem_fd, SHMEM_SIZE);
    if (res) {
      perror("ftruncate");
      exit(1);
    }
  }

  ptr = mmap(NULL, SHMEM_SIZE,
             PROT_READ | PROT_WRITE,
             MAP_SHARED, shmem_fd, 0);
  if (!ptr) {
    perror("mmap");
    exit(1);
  }

  // we have SHMEM_SIZE bytes of shared memory starting from ptr
  if (argc > 1) {
    printf("%d\n", *(int*)ptr);
  } else {
    *(int*)ptr = 0xDEADBEEF;
    //sleep(10);
  }

  /*res = munmap(ptr, SHMEM_SIZE);
  if (res) {
    perror("munmap");
    exit(1);
  }*/

  res = close(shmem_fd);
  if (res) {
    perror("close");
    exit(1);
  }

  if (argc == 1) {
	res = shm_unlink(SHMEM_PATH);
	if (res) {
	  perror("shm_unlink");
	  exit(1);
	}
  }

  return 0;
}

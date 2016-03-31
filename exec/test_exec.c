#include <stdio.h>
#include <unistd.h>

int main() {
	execl("/bin/ls", "/bin/ls", NULL);
	printf("Still alive\n");
	return 0;
}
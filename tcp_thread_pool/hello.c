#include <stdio.h>
#include <thread>
#define THREAD_N 20

bool finished;

void entry(void) {
	printf("Hello world\n");
}

int main() {
std::thread** pool = new std::thread[THREAD_N];
for (int i = 0; i < THREAD_N; ++i) {
	auto tptr = new std::thread(entry);
	pool[i] = tptr;
	tptr->detach();
}
std::thread test(entry);
test.join();	
sleep(1);

return 0;
}
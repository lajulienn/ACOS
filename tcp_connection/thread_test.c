#include <pthread.h>
#include <stdio.h>

void *entry(void *arg) {
	for (int i = 0; i < 1000; ++i) {
		printf("Inside thread %d: %p\n", pthread_self(), arg);
	}
}

int main() {
	pthread_t thread;
	thread = pthread_create(&thread, NULL, entry, NULL);
	for (int i = 0; i < 1000; ++i) {
		printf("Main thread %d\n", pthread_self());
	}

	return 0;
}
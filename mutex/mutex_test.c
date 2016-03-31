#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex;

void *entry(void *arg) {
	int *sum = (int *)arg;
	for (int i = 0; i < 1000000; i++) {
		pthread_mutex_lock(&mutex);
		pthread_mutex_lock(&mutex);
		(*sum)++;
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex);
	}
}

int main() {
	pthread_t thread;
	int sum = 0;

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &attr);

	pthread_create(&thread, NULL, entry, &sum);
	for (int i = 0; i < 1000000; ++i) {
		pthread_mutex_lock(&mutex);
		sum++;
		pthread_mutex_unlock(&mutex);
	}
	pthread_join(thread, NULL);
	printf("sum is %d\n", sum);
	return 0;
}
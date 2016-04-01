#include <stdio.h>
#include <pthread.h>
#include <list>

#define THREAD_N 20

bool finished;
std::list<int> queue;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *entry(void *) {
	//while(!finished) {
		pthread_mutex_lock(mutex);
		while (pthread_cond_wait(&cond, &mutex) != 0);
		pthread_mutex_unlock(mutex);
		printf("notified thread\n");
	//}
	return NULL;
}

int main() {
	pthread_t *pool = new std::thread[THREAD_N];
	for (int i = 0; i < THREAD_N; ++i) {
		pthread_create(pool + i, NULL, entry, NULL);
	}
	for (int i = 0; i < THREAD_N; ++i) {
		pthread_cond_signal(&cond);
	}

	//...
	for (int i = 0; i < THREAD_N; ++i); {
		pthread_join(pool[i], NULL);
	}

return 0;
}
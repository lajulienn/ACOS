#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <list>

#define THREAD_N 20

bool finished;
std::list<int> queue;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *entry(void *) {
	//while(!finished) {
		pthread_mutex_lock(&mutex);
		while (pthread_cond_wait(&cond, &mutex) != 0);
		//assert(queue.size() > 0);
		//int sock = queue.front();
		//queue.pop_front();
		pthread_mutex_unlock(&mutex);
		printf("notified thread\n");

		//if (!sock) {
		//	break;
		//}
		//work
	//}
	return NULL;
}

int main() {
	pthread_t *pool = new pthread_t[THREAD_N];
	for (int i = 0; i < THREAD_N; ++i) {
		pthread_create(pool + i, NULL, entry, NULL);
	}
	for (int i = 0; i < THREAD_N; ++i) {
		pthread_cond_signal(&cond);
	}

	//...

	/*while(...) {
		//sock = accept();
		pthread_mutex_lock(&mutex);
		queue.push_back(sock);
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
	}*/

	for (int i = 0; i < THREAD_N; ++i) {
		pthread_join(pool[i], NULL);
	}
	delete[] pool;

return 0;
}
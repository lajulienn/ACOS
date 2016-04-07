#include <assert.h>
#include <stdio.h>
#include <pthread.h>

#include <list>

#define THREAD_N 20

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
std::list<int> queue;

void *entry(void *) {
  while (true) {
    pthread_mutex_lock(&mutex);
    /*
    It is important to note that when pthread_cond_wait() and  pthread_cond_timedwait()
    return  without error, the associated predicate may still be false. Similarly, when
    pthread_cond_timedwait() returns with the timeout error, the  associated  predicate
    may  be  true  due to an unavoidable race between the expiration of the timeout and
    the predicate state change.
    */
    while (!queue.size()) {
      assert(!pthread_cond_wait(&cond, &mutex));
    }
    int sock = queue.front();
    queue.pop_front();
    pthread_mutex_unlock(&mutex);
    
    if (!sock) {
      break;
    }
    // work
  }
  return NULL;
}

int main() {
  pthread_t *pool = new pthread_t[THREAD_N];
  for (int i = 0; i < THREAD_N; i++) {
    pthread_create(pool + i, NULL, entry, NULL);
  }

  while (...) {
	  // sock = accept()
	  pthread_mutex_lock(&mutex);
	  queue.push_back(sock);
	  pthread_mutex_unlock(&mutex);
	  pthread_cond_signal(&cond);
  }

  for (int i = 0; i < THREAD_N; i++) {
    pthread_join(pool[i], NULL);
  }
  delete[] pool;
  return 0;
}

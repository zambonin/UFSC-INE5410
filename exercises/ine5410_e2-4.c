#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 128
#define N 100

static void *increment();

static int global_counter = 0;
static pthread_mutex_t mutex;

int main() {
  pthread_t threads[MAX_THREADS];

  pthread_mutex_init(&mutex, NULL);
  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_create(&threads[i], NULL, increment, NULL);
  }

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("global_counter = %d.\n", global_counter);
  pthread_mutex_destroy(&mutex);
}

void *increment() {
  pthread_mutex_lock(&mutex);
  for (int i = 0; i < N; i++) {
    global_counter++;
  }
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

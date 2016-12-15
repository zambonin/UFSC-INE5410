#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 5

static void *print_thread_id();

int main() {
  pthread_t threads[MAX_THREADS];

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_create(&threads[i], NULL, print_thread_id, NULL);
  }

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_exit(NULL);
}

void *print_thread_id() {
  pthread_t tid = pthread_self();

  printf("New thread created with ID %u!\n", (unsigned int)tid);
  pthread_exit(NULL);
}

#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 128
#define N 100

static void *increment();

static int global_counter = 0;

int main() {
  pthread_t threads[MAX_THREADS];

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_create(&threads[i], NULL, increment, NULL);
  }

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  if (MAX_THREADS * N != global_counter) {
    printf("Race condition! ");
  }
  printf("global_counter = %d.\n", global_counter);

  pthread_exit(NULL);
}

void *increment() {
  for (int i = 0; i < N; i++) {
    global_counter++;
  }

  pthread_exit(NULL);
}

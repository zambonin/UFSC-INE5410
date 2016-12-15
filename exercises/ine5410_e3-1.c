#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX_THREADS 128
#define N 100

static void *func_thread();

static int global_counter = 0;
static sem_t sem;

int main() {
  sem_init(&sem, 0, 1);
  pthread_t threads[MAX_THREADS];

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_create(&threads[i], NULL, func_thread, NULL);
  }

  for (int i = 0; i < MAX_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Main thread: global_counter = %d.\n", global_counter);
  sem_destroy(&sem);

  return 0;
}

void *func_thread() {
  pthread_t tid = pthread_self();

  sem_wait(&sem);
  for (int i = 0; i < N; i++) {
    global_counter++;
  }
  printf("Worker thread %ld: global_counter = %d.\n", tid, global_counter);
  sem_post(&sem);

  return 0;
}
